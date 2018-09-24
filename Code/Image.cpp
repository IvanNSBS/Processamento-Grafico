#pragma once

#include <vector>
#include <string>
#include "Vector3.cpp"

class Image
{
    private:
    int width, height;
    std::vector<Vector3d> buffer;

    public:
    Image(int width, int height);

    void SetPixel(int x, int y, const Vector3d& color);

    void SaveAsPBM(const std::string &filePath);
};

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