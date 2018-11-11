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
    scene->add(new Sphere(Vector3d(   0, -5004.0, -20),   5000, White));
    //roof
    scene->add(new Sphere(Vector3d(   0, 5018.0, -20),   5000, White));
    //roof light
    scene->add(new Sphere(Vector3d(   0, 117.0, -20),   100, new Light(Vector3d(1.5))));

    //back
    scene->add(new Sphere(Vector3d(   0, 0, -5050),   5000, White));
    
    scene->add(new Sphere(Vector3d(   3015,    -20, -20),   3000, Green));
    scene->add(new Sphere(Vector3d(   -3015,    -20, -20),   3000, Red));


    scene->add(new Sphere(Vector3d(  0,      0, -37),     4, White)); 
    scene->add(new Sphere(Vector3d(  8,      0, -45),     4, Di)); 
    scene->add(new Sphere(Vector3d(  -8,      0, -25),     4, Di2)); 
    scene->add(new Sphere(Vector3d(  12,      0, -25),     4, Chrome)); 

    //scene->add(new Object(Vector3d(  -9,      0, -20),     4, Chrome));
    //scene->add(new Object(Vector3d(   7,      0, -20),     4, PolishedGold)); 
    //scene->add(new Object(Vector3d(-3.7,      1, -12.5),   2, Di)); 
    //scene->add(new Object(Vector3d( 3.7,     -1, -12.5),   2, Di2)); 

    //luz da cena
    //scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1.2)))); 
    //scene->add(new Sphere(Vector3d( -1.0,     9, -18),     2, new Light(Vector3d(1.5))));

    scene->RenderScene(); 
    

    /*
        To do:
        Depth of Field
        + Scene Editor
        Done:
        Perfect reflection AND refraction
        Super Sampling
        Soft Shadows
        Color Bleeding
    */
    return 0; 
}   