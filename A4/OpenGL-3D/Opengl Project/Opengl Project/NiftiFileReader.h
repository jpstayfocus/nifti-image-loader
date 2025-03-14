#pragma once
#include <iostream>
#include <codecvt>
#include <fstream>
#include <vector>
#include <array>
#include "nifti1_io.h"
#include <future>
struct NIfTIHeader {
    std::array<int16_t, 8> dim;
    int16_t datatype;
    std::array<float, 8> pixdim;
    int16_t qform_code;
    int16_t sform_code;
};
class NiftiFileReader {
public:
           
     static NIfTIHeader readImageHeader(const std::string& filename) {
         NIfTIHeader header;

         std::ifstream file(filename, std::ios::binary);
         if (!file.is_open()) {
             header.dim.fill(0);
             header.datatype = 0;
             header.pixdim.fill(0.0f);
             header.qform_code = 0;
             header.sform_code = 0;
             std::cerr << "Failed to open file for reading: " << filename << std::endl;
             return header; // Return empty header on failure
         }

         // Read header fields
         file.seekg(40);
         file.read(reinterpret_cast<char*>(header.dim.data()), sizeof(int16_t) * 8);
         file.seekg(70);
         file.read(reinterpret_cast<char*>(&header.datatype), sizeof(int16_t));

         // Read pixdim array
         int numPixDims = header.dim[0];
         file.seekg(76);
         file.read(reinterpret_cast<char*>(header.pixdim.data()), sizeof(float) * numPixDims);

         // Ensure pixdim array is filled completely
         for (int i = numPixDims; i < 8; ++i) {
             header.pixdim[i] = 1.0f; // Default value if not provided in the file
         }

         file.seekg(252);
         file.read(reinterpret_cast<char*>(&header.qform_code), sizeof(int16_t));
         file.seekg(254);
         file.read(reinterpret_cast<char*>(&header.sform_code), sizeof(int16_t));

         file.close();
         return header;
     }

     // Function to read a single axial slice from NIfTI image data
     //static std::vector<int16_t> readAxialSlice(const std::string& filename, const NIfTIHeader& header, int sliceIndex = 96) {
     //    std::ifstream file(filename, std::ios::binary);
     //    if (!file.is_open()) {
     //        std::cerr << "Failed to open file for reading: " << filename << std::endl;
     //        return std::vector<int16_t>(); // Return empty vector on failure
     //    }

     //    // Check if the slice index is within the valid range
     //    if (sliceIndex < 0 || sliceIndex >= header.dim[3]) {
     //        std::cerr << "Slice index is out of range." << std::endl;
     //        return std::vector<int16_t>(); // Return empty vector
     //    }

     //    // Calculate the size of a single slice
     //    size_t sliceSize = header.dim[1] * header.dim[2];

     //    // Calculate the offset to the start of the desired slice
     //    size_t sliceOffset = sliceIndex * sliceSize;

     //    // Seek to the start of the desired slice
     //    file.seekg(352 + sliceOffset * sizeof(int16_t));

     //    // Read image data for the desired slice
     //    std::vector<int16_t> sliceData(sliceSize);
     //    file.read(reinterpret_cast<char*>(sliceData.data()), sliceSize * sizeof(int16_t));

     //    file.close();
     //    return sliceData;
     //}

     static std::vector<int16_t> readAxialSlice(const std::string& filename, int sliceIndex) {
         // Load NIfTI image header
         nifti_image* nim = nifti_image_read(filename.c_str(), 1);
         if (!nim) {
             std::cerr << "Failed to open NIfTI file: " << filename << std::endl;
             return std::vector<int16_t>(); // Return empty vector on failure
         }

         // Load image data
         if (nim->data == nullptr) {
             nifti_image_load(nim);
         }

         // Check if the slice index is within the valid range
         if (sliceIndex < 0 || sliceIndex >= nim->nz) {
             std::cerr << "Slice index is out of range." << std::endl;
             nifti_image_free(nim);
             return std::vector<int16_t>(); // Return empty vector
         }

         // Calculate the size of a single slice
         size_t sliceSize = nim->nx * nim->ny;

         // Calculate the offset to the start of the desired slice
         size_t sliceOffset = sliceIndex * sliceSize;

         // Read image data for the desired slice
         std::vector<int16_t> sliceData(sliceSize);
         memcpy(sliceData.data(), reinterpret_cast<int16_t*>(nim->data) + sliceOffset, sliceSize * sizeof(int16_t));

         // Free NIfTI image structure
         nifti_image_free(nim);

         return sliceData;
     }
     template <typename T>
     static std::vector<T> get_slice_data(const char* filename, int slice_number) {
         // Open the NIFTI file
         nifti_image* img = nifti_image_read(filename, 1);
         if (img == nullptr) {
             throw std::runtime_error("Error opening NIFTI file: " + std::string(filename));
         }

         // Get data pointer and dimensions
         void* data = img->data;


         int nx = img->nx;
         int ny = img->ny;
         int nz = img->nz;
         //This will show header information on the screen now.
         std::cout << "Header Information\nImage Dimension: " << img->ndim << std::endl;
         std::cout << "Image Width: " << img->nx<< std::endl;
         std::cout << "Image Height: " << img->ny << std::endl;
         std::cout << "Image Slices: " << img->nz << std::endl;
         std::cout << "Image DataType: " << img->datatype << std::endl;
         std::cout << "Image Voxels: " << img->nvox << std::endl;
         // Check slice number validity (assuming z-axis is for slices)
         if (slice_number < 0 || slice_number >= nz) {
             nifti_image_free(img);
             throw std::out_of_range("Invalid slice number. Valid range: 0 - ");
         }

         // Get data type and create a vector to store the slice data
         size_t data_size = sizeof(T);
         std::vector<T> slice_data(nx * ny);

         // Extract slice data based on data type
         if (img->datatype == NIFTI_TYPE_FLOAT32) {
             float* float_data = static_cast<float*>(data);
             for (int y = 0; y < ny; ++y) {
                 for (int x = 0; x < nx; ++x) {
                     int index = y * nx + x + slice_number * nx * ny;
                     slice_data[y * nx + x] = float_data[index];
                 }
             }
         }
         else if (img->datatype == NIFTI_TYPE_INT16) {
             int16_t* int16_data = static_cast<int16_t*>(data);
             // ... (similar loop to extract data for int16)
         }
         else {
             // Handle other data types as needed
             nifti_image_free(img);
             throw std::runtime_error("Unsupported data type in NIFTI file");
         }

         // Free memory and return the slice data
         nifti_image_free(img);
         return slice_data;
     }

     //This will result in this function running on a separate thread. This gets the slice data 
     template <typename T>
     static  std::future<std::vector<T>> get_slice_data_async(const char* filename, int slice_number) {
         // Start the async operation and return the future
         return std::async(std::launch::async, get_slice_data<T>, filename, slice_number);
     }
};