
#include "Image.h"
#include <fstream>
#include <math.h>
#include <cmath>
#include <algorithm>

// source channel, target channel, width, height, radius
void gaussBlur_1 ( std::vector<Vector3d> &srcBuffer, std::vector<Vector3d> &result, int w, int h, float r) {
    
    result.clear();
    result.reserve(w*h);

    auto rs = std::ceil(r * 2.57);     // significant radius

    for(auto i=0; i< h; i++){
        for(auto j=0; j < w; j++) {
            float val_x = 0, val_y = 0, val_z = 0, wsum = 0;
            for(int iy = i-rs;  iy < i+rs+1; iy++)
                for(int ix = j-rs; ix<j+rs+1; ix++) {
                    int x = std::min((w-1), std::max(0, ix));
                    int y = std::min((h-1), std::max(0, iy));
                    float dsq = float((ix-j)*(ix-j)+(iy-i)*(iy-i));
                    float wght = std::exp( -dsq / (2.0*r*r) ) / (3.14159265358*2.0*r*r);
                    val_x += (srcBuffer[y*w+x].x * wght);
                    val_y += (srcBuffer[y*w+x].y * wght);
                    val_z += (srcBuffer[y*w+x].z * wght);
                    wsum += wght;
                }
            // cout << "new x = " << val_x << "| " << "wsum = "  << wsum << "\n";
            double help =  val_x/wsum;
            // cout << "new x = " << help << "\n";
            result[i*w+j] = Vector3d ( val_x/wsum, val_y/wsum, val_z/wsum );            
        }
    }
}

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    
    //Seta o tamanho do buffer para o canvas total
    //Necessario, pois pode-se tentar setar um pixel numa posicao
    //ainda nao existente no buffer(ex: buffer tem tamanho 10, mas tentou 
    //setar o pixel na posicao 20)
    this->buffer.reserve(width*height);
}

void Image::SetPixel(int x, int y, const Vector3d& color)
{
    buffer[y * width + x] = color;
}

void Image::SaveAsPBM(const std::string &filepath, const std::string &filename)
{
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs(filepath + filename + ".ppm", std::ios::out | std::ios::binary); 
    ofs << "P6\n" << width << " " << height << "\n255\n"; 
    for (unsigned i = 0; i < width * height; ++i)
    { 
        ofs << (unsigned char)(std::min(double(1), (double)buffer[i].x) * 255.0) << 
               (unsigned char)(std::min(double(1), (double)buffer[i].y) * 255.0) << 
               (unsigned char)(std::min(double(1), (double)buffer[i].z) * 255.0); 
    }
    ofs.close();

    
    std::vector<Vector3d> filtered;
    filtered.reserve(width*height);

    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            filtered[i*width+j] = (buffer[i*width+j].Length() <= 2.25 ? Vector3d(0) : buffer[i*width+j]);

    std::vector<Vector3d> blurred;
    gaussBlur_1(filtered, blurred, this->width, this->height, 4.0f);

    for(int i = 0; i < height; i++)
        for(int j = 0 ; j < width; j++)
            if( blurred[i*width+j].Length() > 0.0 )
                buffer[i*width+j] = blurred[i*width+j] + buffer[i*width+j];

    std::ofstream ofs3(filepath + filename + "_bloom.ppm", std::ios::out | std::ios::binary); 
    ofs3 << "P6\n" << width << " " << height << "\n255\n"; 
    for (unsigned i = 0; i < width * height; ++i)
    { 

        ofs3 << (unsigned char)(std::min(double(1), (double)buffer[i].x) * 255.0) << 
                (unsigned char)(std::min(double(1), (double)buffer[i].y) * 255.0) << 
                (unsigned char)(std::min(double(1), (double)buffer[i].z) * 255.0); 
    }
    ofs3.close();
}