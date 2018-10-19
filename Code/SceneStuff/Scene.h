#pragma once

#include "Object.h"
#include "Ray.h"
#include "../Vector/Vector3.cpp"
#include <vector>


class Scene {
public:
    std::vector<Object*> objects;

public:
    
    Scene(): objects(){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    Vector3d trace(const Ray& r);

    //render
    void render();
    //Adiciona um objeto a cena
    void add(Object* object);
};