#pragma once

#ifndef SCENEH
#define SCENEH

#include "Object.h"
#include "Ray.h"
#include "../Vector/Vector3.cpp"
#include <vector>
#include "Camera.h"

class Scene {
public:
    //Object *light;
    std::vector<Object*> objects;
    Camera *camera;

public:
    
    Scene(): objects(), camera(nullptr){}
    Scene(Camera* cam): objects(), camera(cam){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    Vector3d trace(const Ray& r, int depth);

    //render
    void RenderScene();
    //Adiciona um objeto a cena
    void add(Object* object);
};

#endif 