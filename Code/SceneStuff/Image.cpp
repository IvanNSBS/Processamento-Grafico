#pragma once

#include "Image.h"
#include <fstream>
#include <algorithm>

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    this->buffer.reserve(width*height);
}

void Image::SetPixel(int x, int y, const Vector3d& color)
{
    int size = x * y;
    buffer[y * width + x] = color;
}

void Image::SaveAsPBM()
{
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary); 
    ofs << "P6\n" << width << " " << height << "\n255\n"; 
    for (unsigned i = 0; i < width * height; ++i)
    { 
        ofs << (unsigned char)(std::min(double(1), (double)buffer[i].x) * 255) << 
               (unsigned char)(std::min(double(1), (double)buffer[i].y) * 255) << 
               (unsigned char)(std::min(double(1), (double)buffer[i].z) * 255); 
    }
    ofs.close();
}