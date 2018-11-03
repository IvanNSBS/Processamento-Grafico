#include "Scene.h"

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp Material.cpp main.cpp
    std::vector<Object*> spheres; 
    Image *im = new Image(320, 240);
    
    //Camera padrao
    Camera *cam = new Camera( im, Vector3d(-1, 0, 0), Vector3d(0, 0, 1), Vector3d(0,1,0), 50, 1);

    //camera top down
    //Camera *cam = new Camera( im, Vector3d(0, 15, -16.7), Vector3d(0, 5, 1), Vector3d(0,1,0), 60, 1 );
    Scene *scene = new Scene(cam);

    //Posicao, raio, material
    scene->add(new Object(Vector3d( 0.0,   -1004, -20),   1000, Silver)); 
    scene->add(new Object(Vector3d(  -1,      0, -20),     4, Lamb)); 
    scene->add(new Object(Vector3d(  -9,      0, -20),     4, Basic));
    scene->add(new Object(Vector3d(   7,      0, -20),     4, Basic2)); 
    scene->add(new Object(Vector3d(-3.7,      1, -12.5),   2, Di)); 
    scene->add(new Object(Vector3d( 3.7,     -1, -12.5),   2, Di2)); 
    //luz da cena
    scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1.2)))); 
    //scene->add(new Object(Vector3d( 3,     1, -15),     2, new Light(Vector3d(1.2)))); 
    printf("%d", scene->lights.size());
    scene->RenderScene(); 
    
    return 0; 
}   