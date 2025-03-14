#pragma once
#include <iostream>
#include "zconf.h"
#include "zlib.h"
#include <future>
#define CHUNK_SIZE 16384
class GZFileExtractor {
public:
    static bool decompressGzipFile(const std::string& sourceFile, const std::string& destinationFolder) {
        gzFile file = gzopen(sourceFile.c_str(), "rb");
        if (!file) {
            std::cerr << "Error: Cannot open file " << sourceFile << std::endl;
            return false;
        }

        std::string outFileName = destinationFolder + "/" + sourceFile.substr(sourceFile.find_last_of('/') + 1);
        size_t lastDotPos = outFileName.find_last_of('.');
        if (lastDotPos != std::string::npos && outFileName.substr(lastDotPos) == ".gz") {
            outFileName = outFileName.substr(0, lastDotPos); // Remove the .gz extension
        }

        FILE* outFile = nullptr;
        if (fopen_s(&outFile, outFileName.c_str(), "wb") != 0 || !outFile) {
            std::cerr << "Error: Cannot create output file" << std::endl;
            gzclose(file);
            return false;
        }

        char buffer[CHUNK_SIZE];
        int bytes_read;
        while ((bytes_read = gzread(file, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes_read, outFile);
        }

        gzclose(file);
        fclose(outFile);

        std::cout << "File decompressed successfully: " << sourceFile << " -> " << outFileName << std::endl;

        return true;
    }
    //This extracts the data 
    static std::future<bool> decompressGzipFileAsync(const std::string& sourceFile, const std::string& destinationFolder) {
        // Start the async operation and return the future
        return std::async(std::launch::async, decompressGzipFile, sourceFile, destinationFolder);
    }
};