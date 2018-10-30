#include "Scene.h"

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp
    std::vector<Object*> spheres; 
    Image *im = new Image(320, 240);
    
    //Camera padrao
    Camera *cam = new Camera( im, Vector3d(-1, 0, 0), Vector3d(0, 0, 0.1), Vector3d(0,1,0), 50, 0.5);

    //camera top down
    //Camera *cam = new Camera( im, Vector3d(0, 15, -16.7), Vector3d(0, 5, 1), Vector3d(0,1,0), 60, 1 );
    Scene *scene = new Scene(cam);

    //Posicao, raio, material
    scene->add(new Object(Vector3d( 0.0, -10004, -20), 10000, Silver)); 
    scene->add(new Object(Vector3d( 0.0,      0, -20),     4, Chrome)); 
    scene->add(new Object(Vector3d( 5.0,     -1, -15),     2, Brass)); 
    scene->add(new Object(Vector3d( 5.0,      0, -25),     3, Brass)); 
    scene->add(new Object(Vector3d(-5.5,      0, -15),     3, Chrome));
    scene->add(new Object(Vector3d(-4.5,      -1.5, -11),     1, Silver)); 
    //luz da cena
    scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1)))); 
    scene->RenderScene(); 
    
    return 0; 
}   