#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "vec3.h"


// source channel, target channel, width, height, radius
void gaussBlur_1 ( std::vector<vec3> &srcBuffer, std::vector<vec3> &result, int w, int h, float r) {
    
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
                    val_x += (srcBuffer[y*w+x].x() * wght);
                    val_y += (srcBuffer[y*w+x].y() * wght);
                    val_z += (srcBuffer[y*w+x].z() * wght);
                    wsum += wght;
                }
            // cout << "new x = " << val_x << "| " << "wsum = "  << wsum << "\n";
            float help =  val_x/wsum;
            // cout << "new x = " << help << "\n";
            result[i*w+j] = vec3 ( val_x/wsum, val_y/wsum, val_z/wsum );            
        }
    }
}

class Image
{
public:
    int width, height;

private:
    std::vector<vec3> buffer;

public:
    Image(int width, int height){
        this->width = width;
        this->height = height;
    
        //Seta o tamanho do buffer para o canvas total
        //Necessario, pois pode-se tentar setar um pixel numa posicao
        //ainda nao existente no buffer(ex: buffer tem tamanho 10, mas tentou 
        //setar o pixel na posicao 20)
        this->buffer.reserve(width*height);
    }

    void SetPixel(int x, int y, const vec3& color){
        buffer[y * width + x] = color;
    }

    void SaveAsPBM(const std::string &filepath, const std::string &filename)
    {

        // Save result to a PPM image (keep these flags if you compile under Windows)
        std::ofstream ofs(filepath + filename + ".ppm", std::ios::out | std::ios::binary); 
        ofs << "P6\n" << width << " " << height << "\n255\n"; 
        for (unsigned i = 0; i < width * height; ++i)
        { 
            ofs <<  (unsigned char)(std::min(float(1), (float)buffer[i].x()) * 255.99 ) << 
                    (unsigned char)(std::min(float(1), (float)buffer[i].y()) * 255.99 ) << 
                    (unsigned char)(std::min(float(1), (float)buffer[i].z()) * 255.99 ); 
        }
        ofs.close();



        /*
            0.2126*x' + 0.7152*y' + 0.0722*z' = 3
            0.2126*x1 + 0.7152*y1 + 0.0722*z1 = 0.6

            0.5008856
            0.2126x1 = Lo*0.2126x'/L
            x1 = Lo*x/L
            (0.2126*x1 + 0.7152*y1 + 0.0722*z1) = 0.6*(0.2126*x + 0.7152*y + 0.0722*z)/3
        */

        // for (int i = 0; i < height; i++){
        //     for(int j = 0; j < width; j++){
        //         float L = buffer[i*width+j].get_luminance();
        //         float Lo = L / (L+1.0f);
        //         buffer[i*width+j] = (buffer[i*width+j]*Lo)/L;
        //     }
        // }    

        std::vector<vec3> filtered;
        filtered.reserve(width*height);

        for(int i = 0; i < height; i++)
            for(int j = 0; j < width; j++)
                filtered[i*width+j] = (buffer[i*width+j].get_luminance() <= 1.0f ? vec3(0,0,0) : buffer[i*width+j]);

        std::vector<vec3> blurred;
        gaussBlur_1(filtered, blurred, this->width, this->height, 3.0f);

        // Save result to a PPM image (keep these flags if you compile under Windows)
        // std::ofstream ofs2(filepath + filename + "_filtered.ppm", std::ios::out | std::ios::binary); 
        // ofs2 << "P6\n" << width << " " << height << "\n255\n"; 
        // for (unsigned i = 0; i < width * height; ++i)
        // { 
        //     ofs2 << (unsigned char)(std::min(float(1), (float)filtered[i].x()) * 255.99 ) << 
        //             (unsigned char)(std::min(float(1), (float)filtered[i].y()) * 255.99 ) << 
        //             (unsigned char)(std::min(float(1), (float)filtered[i].z()) * 255.99 ); 
        // }
        // ofs2.close();


        // std::ofstream ofs_blur(filepath + filename + "_blurred.ppm", std::ios::out | std::ios::binary); 
        // ofs_blur << "P6\n" << width << " " << height << "\n255\n"; 
        // for (unsigned i = 0; i < width * height; ++i)
        // { 
        //     ofs_blur << (unsigned char)(std::min(float(1), (float)blurred[i].x()) * 255.99 ) << 
        //                 (unsigned char)(std::min(float(1), (float)blurred[i].y()) * 255.99 ) << 
        //                 (unsigned char)(std::min(float(1), (float)blurred[i].z()) * 255.99 ); 
        // }
        // ofs_blur.close();


        for(int i = 0; i < height; i++)
            for(int j = 0 ; j < width; j++)
                if( blurred[i*width+j].length() > 0.001f ){
                    buffer[i*width+j] = (blurred[i*width+j] + buffer[i*width+j]);
                    // float Lo = (buffer[i*width+j].get_luminance()/(buffer[i*width+j].get_luminance() + 1.0));
                    // float L = buffer[i*width+j].get_luminance();
                    // // std::cout << "L = " << buffer[i].get_luminance() << "\t";
                    // // std::cout << "Lo = " << (buffer[i].get_luminance()/(buffer[i].get_luminance() + 3.0)) << "\n";
                    // // std::cout << "Col = " << buffer[i] << "\n";
                    // buffer[i*width+j] = vec3( (buffer[i*width+j].x() * Lo)/(L), (buffer[i*width+j].y() * Lo)/(L), (buffer[i*width+j].z() * Lo)/(L));
                }

        /*
        L = 0.262826    Lo = 0.208125
        Col = ( 0.241027 , 0.272793 , 0.228281 )

        tnCol = ( 0.241027 , 0.272793 , 0.228281 )
        */
        // std::cout << bX << " | " << bY << " | " << bZ << "\n";
        std::ofstream ofs3(filepath + filename + "_bloom.ppm", std::ios::out | std::ios::binary); 
        ofs3 << "P6\n" << width << " " << height << "\n255\n"; 
        for (unsigned i = 0; i < width * height; ++i)
        { 
            // std::cout << "tnCol = " << (buffer[i]) << "\n";
            ofs3 << (unsigned char)(std::min(float(1), (float)buffer[i].x()) * 255) << 
                    (unsigned char)(std::min(float(1), (float)buffer[i].y()) * 255) << 
                    (unsigned char)(std::min(float(1), (float)buffer[i].z()) * 255); 
        }
        ofs3.close();
    }
};