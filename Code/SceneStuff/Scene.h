#pragma once

#include "Object.h"
#include "Ray.h"
#include "../Vector/Vector3.cpp"
#include <vector>
#include "Image.h"
#include "Camera.h"

class Scene {
public:
    //Object *light;
    std::vector<Object*> objects;
    Image *renderedImage;
    Camera *camera;

public:
    
    Scene(): objects(), renderedImage(nullptr){}
    Scene(int width, int height): objects(), renderedImage(new Image(width,height)){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    Vector3d trace(const Ray& r);

    //render
    void render();
    //Adiciona um objeto a cena
    void add(Object* object);
};