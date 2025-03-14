/**
* Name: Jephte Pierre
* Date: April 4th, 2024
* Description: Get familiar with cpp IO tools and concurrency
* 
*/ 


#include <iostream>
#include "GZFileExtractor.h"
#include "NiftiFileReader.h"
#include "ShaderFileReader.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <future>
#include <string>
#include <cstdlib>
#include <fstream>



#define VIEWPORTWIDTH 800
#define VIEWPORTHEIGHT 600
#define SLICEINDEX 57

bool fileProcessingComplete = false;
int textureProgram;
GLuint sliceTexture;
GLuint sliceVBO, sliceVAO, sliceEBO;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
float texVertices[] = {
    // Positions         // Texture coordinates
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // Top right
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Bottom right
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Bottom left
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // Top left 
};
unsigned int indices[] = {
    0, 1, 3, // First triangle
    1, 2, 3  // Second triangle
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
std::string removeWordFromString(const std::string& inputString, const std::string& wordToRemove) {
    std::string result = inputString;

    // Find the position of the word in the string
    size_t pos = result.find(wordToRemove);

    // If the word is found, remove it from the string
    if (pos != std::string::npos) {
        // Adjust the position to remove leading/trailing spaces
        while (pos > 0 && result[pos - 1] == ' ') {
            pos--;
        }
        size_t endPoi = pos + wordToRemove.length();
        while (endPoi < result.length() && result[endPoi] == ' ') {
            endPoi++;
        }

        // Erase the word from the string
        result.erase(pos, endPoi - pos);

        // Remove any extra spaces left
        size_t startPos = result.find_first_not_of(" ");
        size_t endPos = result.find_last_not_of(" ");
        if (startPos != std::string::npos && endPos != std::string::npos) {
            result = result.substr(startPos, endPos - startPos + 1);
        }
    }

    return result;
}
GLuint createTexture(int width, int height, const std::vector<float> imageData) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, imageData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void process_file(const std::string& filename, int slice_number, const std::string& destinationFolder) {
    
    // Check if the file needs extraction
    if (filename.size() > 3 && filename.substr(filename.size() - 3) == ".gz") {
        std::cout << "Extracting file: " << filename << std::endl;
        //Due to permission issues, I am putting it on D drive.
        auto extractData = GZFileExtractor::decompressGzipFileAsync(filename, destinationFolder);
        std::cout << "Extraction complete." << std::endl;
        if (extractData.get() != true) {
            std::cout << "Program is not allowed to extract on this location" << std::endl;
            std::cout << "Reading failed !!!";
            return;
        }
    }
    
    // Now start reading the slice data asynchronously
    std::cout << "Reading slice data..." << std::endl;
    std::string destFolder = destinationFolder + filename;
    std::string r  = removeWordFromString(destFolder, ".gz");
    auto future_slice_data = NiftiFileReader::get_slice_data_async<float>(r.c_str(), slice_number);

    // Wait for the slice data to be ready
    std::vector<float> slice_data = future_slice_data.get();
    std::cout << "Slice data read successfully." << std::endl;

    // Render the texture
    std::cout << "Rendering texture..." << std::endl;
    textureProgram = ShaderFileReader::loadShaderProgram("shaders/texture.vs.glsl", "shaders/texture.fs.glsl");
    int width = 240;
    int height = 240;

    sliceTexture = createTexture(width, height, slice_data);

   
    glGenVertexArrays(1, &sliceVAO);
    glGenBuffers(1, &sliceVBO);
    glGenBuffers(1, &sliceEBO);
    glBindVertexArray(sliceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sliceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texVertices), texVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sliceEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    fileProcessingComplete = true;
    std::cout << "Texture rendered." << std::endl;
}
int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
    

    GLFWwindow* window = glfwCreateWindow(VIEWPORTWIDTH, VIEWPORTHEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, VIEWPORTWIDTH, VIEWPORTHEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //This is not a zip file, lets give it a .gz file
    std::string sourceFile = "t1.nii"; // Change this to your NIfTI file
   
    //Doing this so it gives permission error, which will show that is running on the separate thread
    process_file(sourceFile, SLICEINDEX, "D:/extract/");

    //On D it worked file, let see what happens when I put it in C drive
    process_file(sourceFile, SLICEINDEX, "D:/extract/");

    //process_file(sourceFile, SLICEINDEX, "./extract/");
  
    
   
    //// Extract the desired axial slice
    //std::vector<float> sliceData = NiftiFileReader::get_slice_data<float>(sourceFile.c_str(), SLICEINDEX);
    
    int program = ShaderFileReader::loadShaderProgram("shaders/shader.vs.glsl", "shaders/shader.fs.glsl");
  
    glUseProgram(program);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT );
        //rendering here
        if (!fileProcessingComplete) {
            glUseProgram(program);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }else{
            glUseProgram(textureProgram);
            glBindTexture(GL_TEXTURE_2D, sliceTexture);
            glActiveTexture(GL_TEXTURE0);
            GLuint textureLocation = glGetUniformLocation(textureProgram, "textureData");
            glUniform1i(textureLocation, 0);
            glBindVertexArray(sliceVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        //rendering here
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

