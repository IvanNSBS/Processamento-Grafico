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
    /*scene->add(new Object(Vector3d( 0.0,   -1004, -20),   1000, Silver)); 
    scene->add(new Object(Vector3d(  -1,      0, -20),     4, Lamb)); 
    scene->add(new Object(Vector3d(  -9,      0, -20),     4, Chrome));
    scene->add(new Object(Vector3d(   7,      0, -20),     4, PolishedGold)); 
    scene->add(new Object(Vector3d(-3.7,      1, -12.5),   2, Di)); 
    scene->add(new Object(Vector3d( 3.7,     -1, -12.5),   2, Di2)); 
    //luz da cena
    scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1.2)))); 
    scene->add(new Object(Vector3d( -1.0,     9, -18),     2, new Light(Vector3d(1.5)))); */

    scene->add(new Object(Vector3d( 0.0,   -504, -20),   500, Silver)); 
    scene->add(new Object(Vector3d(  -2.5,      -1.1, -27),     3.5, Lamb)); 
    scene->add(new Object(Vector3d(   0,        -1.1, -21),     3.5, Di2)); 
    scene->add(new Object(Vector3d(   2.5,      -1.1, -15),     3.5, BasicConductor)); 

    scene->add(new Object(Vector3d(  -8,      -3, -17),     1, Di)); 
    scene->add(new Object(Vector3d(  -8,      -3, -13),     1, Lamb)); 
    scene->add(new Object(Vector3d(  -6,      -3, -15),     1, Chrome)); 
    scene->add(new Object(Vector3d(  -3,      -3, -19),     1, Lamb)); 
    scene->add(new Object(Vector3d(  -8,      -3, -22),     1, Lamb)); 
    scene->add(new Object(Vector3d(  -4,      -3, -28),     1, Lamb)); 
    scene->add(new Object(Vector3d(  -7,      -3, -24),     1, Lamb)); 

    scene->add(new Object(Vector3d(  -2,      -3, -12),     1, Lamb)); 
    scene->add(new Object(Vector3d(   0,      -3, -10),     1, Lamb)); 
    scene->add(new Object(Vector3d(   3,      -3, -15),     1, Lamb)); 
    scene->add(new Object(Vector3d(   2,      -3, -11),     1, Lamb)); 

    scene->add(new Object(Vector3d(   5,      -3, -10),     1, Lamb)); 
    scene->add(new Object(Vector3d(   7,      -3, -13),     1, Lamb)); 
    scene->add(new Object(Vector3d(   8,      -3, -18),     1, Lamb)); 

    scene->add(new Object(Vector3d(   3,      -3, -20),     1, Lamb)); 
    scene->add(new Object(Vector3d(   1,      -3, -12),     1, Lamb)); 
    scene->add(new Object(Vector3d(   5,      -3, -16),     1, Lamb)); 



    scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1.5)))); 

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