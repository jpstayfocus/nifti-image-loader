#version 330 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D textureData;

void main()
{
    // Sample texture
    vec4 texColor = texture(textureData, texCoord);
    
    // Convert RGB to grayscale using luminance calculation
    float grayscale = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    
    // Set fragment color with grayscale value
    FragColor = vec4(grayscale, grayscale, grayscale, 1.0);
}