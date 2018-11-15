#include "Scene.h"

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp Material.cpp main.cpp
    std::vector<Object*> spheres; 
    Image *im = new Image(320, 240);
    
    //Camera padrao
    Camera *cam = new Camera( im, Vector3d(0, 7.3, 50), Vector3d(0, 0, 1), Vector3d(0,1,0), 20.0, 1.0);

    //camera top down
    //Camera *cam = new Camera( im, Vector3d(0, 15, -16.7), Vector3d(0, 5, 1), Vector3d(0,1,0), 60, 1 );
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
    //scene->add(new Sphere(Vector3d(   8, 536.95, -25),   520, new Light(Vector3d(1.5))));
    //scene->add(new Sphere(Vector3d(   0, 136.5, -20),   120, new Light(Vector3d(1.5))));
    
    scene->add(new Sphere(Vector3d(   0, 17.8, -20),   2, new Light(Vector3d(1.3))));
    //scene->add(new Sphere(Vector3d(   8, 17.8, -20),   2, new Light(Vector3d(1.3))));
    //scene->add(new Sphere(Vector3d(   -8, 17.8, -20),   2, new Light(Vector3d(1.3))));
    //scene->add(new Sphere(Vector3d(   -8, 17.8, -20),   2, new Light(Vector3d(1.3))));
    //scene->add(new Sphere(Vector3d(   0, 2, -10),   2, new Light(Vector3d(1.3))));
    //scene->add(new Sphere(Vector3d(   0, 70, -145),   90, new Light(Vector3d(2.5))));


    scene->add(new Sphere(Vector3d(  0,      1.8, -31),     5.5, Lamb)); 
    //scene->add(new Sphere(Vector3d(  8,      0, -45),     4, Di)); 
    scene->add(new Sphere(Vector3d(  -11,      0, -20),     4, Di2)); 
    scene->add(new Sphere(Vector3d(  11,      0, -20),     4, PolishedChrome));

    scene->RenderScene(); 
    return 0; 
}   