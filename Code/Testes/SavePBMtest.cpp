#include <cstdlib> 
#include <cstdio> 
#include <cmath> 
#include <fstream> 
#include <vector> 
#include <iostream> 
#include <cassert>

struct Color
{
public:
    int r, g, b;

    Color() : r(255), g(255), b(255){}
    Color(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}
};

int main()
{
    int w, h;
    int r;

    std::cin >> w;
    std::cin >> h;
    std::cin >> r;

    Color red(255, 0, 0);

    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary); 
    ofs << "P3\n" << w << " " << h << "\n255\n"; 
    for (unsigned i = 0; i < w; ++i) 
    { 
        for(int j = 0; j < h; ++j)
        {
            std::cout << "passing" << std::endl;
            if( sqrt(((i - (w/2) ) * (i - (w/2) )) + ((j - (h/2)) * (j - (h/2) ) )) < r)
            {
                ofs <<  ("255 ") << 
                        ("0 ") << 
                        ("0   ");    
            }
            else
            {
                ofs << ("255 ") <<
                       ("255 ") << 
                       ("255   ");
            }
        }
        ofs << "\n";
    } 
    ofs.close(); 
}