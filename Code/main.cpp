#include "SceneStuff/Scene.h"

int main()
{
    //Para compilar: g++ -o r -std=c++14 ../SceneStuf/Scene.cpp ../SceneStuf/Object.cpp ../SceneStuf/Ray.cpp ../SceneStuf/Camera.cpp ../SceneStuf/Image.cpp main.cpp


    std::vector<Object*> spheres; 
    Image *im = new Image(640, 480);

    //camera bugada
    //Camera *cam = new Camera( im, Vector3d(0, 0.9, -0.50), Vector3d(0, 0.5, 0.1), Vector3d(0,1,0), 90, 1 );

    //camera nao bugada para testes
    Camera *cam = new Camera( im, Vector3d(0, 0, 0), Vector3d(0, 0, 0.1), Vector3d(0,1,0), 90, 1 );


    Scene *scene = new Scene(cam);

    // position, radius, surface color, reflectivity, transparency, emission color
    scene->add(new Object(Vector3d( 0.0, -40004, -20), 40000, new Material(Vector3d(0.20, 0.20, 0.20))) ); 
    scene->add(new Object(Vector3d( 0.0,      0, -20),     4, new Material(Vector3d(1.00, 0.32, 0.36))) ); 
    scene->add(new Object(Vector3d( 5.0,     -1, -15),     2, new Material(Vector3d(0.90, 0.76, 0.46))) ); 
    scene->add(new Object(Vector3d( 5.0,      0, -25),     3, new Material(Vector3d(0.65, 0.77, 0.97))) ); 
    scene->add(new Object(Vector3d(-5.5,      0, -15),     3, new Material(Vector3d(0.90, 0.90, 0.90))) ); 
    // light
    scene->add(new Object(Vector3d( 0.0,     30, -30),     0, new Material(Vector3d(0.00, 0.00, 0.00), Vector3d(2.5))) ); 
    scene->RenderScene(); 
    
    return 0; 
}