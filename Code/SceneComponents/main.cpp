#include "Scene.h"
#include <thread>
#include <future>
#include <ctime>

using namespace std;

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 -pthread Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp Material.cpp main.cpp
    std::vector<Object*> spheres; 
    Image *im = new Image(640, 480);
    
    //Camera padrao
    Vector3d lookFrom(278, 278, -800);
    Vector3d lookAt(278, 278,0);
    Vector3d dir = (lookFrom - lookAt).Normalize();
    //Camera *cam = new Camera( im, lookFrom, dir, Vector3d(0,1,0), 40.0, 1);

    //camera top down
    Camera *cam = new Camera( im, Vector3d(0, 7.3, 50), Vector3d(0, 0, 1), Vector3d(0,1,0), 20, 1 );
    Scene *scene = new Scene(cam);

    //ground
    scene->add(new Sphere(Vector3d(   0, -5004.0, -0),   5000, White));
    //roof
    scene->add(new Sphere(Vector3d(   0, 5018.0, -0),   5000, White));
    //back
    scene->add(new Sphere(Vector3d(   0, 0, -550),   500, White));
    //sides
    scene->add(new Sphere(Vector3d(   5015,    -20, -0),   5000, Green));
    scene->add(new Sphere(Vector3d(   -5015,    -20, -0),   5000, Red));

    //roof light
    //scene->add(new Sphere(Vector3d(   0, 538, -25),   520, new Light(Vector3d(1.5))));
    //scene->add(new Sphere(Vector3d(   0, 136.5, -20),   120, new Light(Vector3d(1.5))));
    
    scene->add(new Sphere(Vector3d(   0, 17.8, -23),   2, new Light(Vector3d(4))));
    scene->add(new Sphere(Vector3d(   12, 17.8, -23),   2, new Light(Vector3d(4))));
    scene->add(new Sphere(Vector3d(   -12, 17.8, -23),   2, new Light(Vector3d(4))));

    scene->add(new Sphere(Vector3d(  0,      1.8, -31),     5.5, White)); 
    scene->add(new Sphere(Vector3d(  8,      0, -45),     4, Di)); 
    scene->add(new Sphere(Vector3d(  -11,      0, -20),     4, Di2)); 
    scene->add(new Sphere(Vector3d(  11,      0, -20),     4, PolishedChrome));

    /*scene->add(new YZ_Rect(0,555,0,555,555, Green));
    scene->add(new YZ_Rect(0,555,0,555,0, Red));
    //scene->add(new XZ_Rect(213,343,227,332,554, new Light(Vector3d(1.5))));
    scene->add(new XZ_Rect(0,555,0,555,0, White));
    scene->add(new XZ_Rect(0,555,0,555,555, White));
    scene->add(new XY_Rect(0,555,0,555,555, White));*/
    //scene->add(new Sphere(Vector3d(277.5, 555, 416.25),   138.75, new Light(Vector3d(2))));

    clock_t begin = clock();
    int tnum = 4;
    std::thread *tlist = new std::thread[tnum];
    for(int i = 0; i < tnum; i++)
    {
        tlist[i] = std::thread( &Scene::RenderScene, scene, 0, scene->camera->canvas->height/tnum*i, 
                                scene->camera->canvas->width, scene->camera->canvas->height/tnum*(i+1) );
    }
    for(int i = 0; i < tnum; i++)
    {
        tlist[i].join();
    } 

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "RenderTime with " << tnum << " threads = "<< elapsed_secs << "s" << std::endl;

    scene->camera->canvas->SaveAsPBM("../RenderedImages/", "test4");

    return 0; 
}   