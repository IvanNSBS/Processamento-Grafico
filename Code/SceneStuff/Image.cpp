#pragma once

#include "Image.h"
#include <fstream>

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

void SaveAsPBM(const std::string &filePath)
{
    
}