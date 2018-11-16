#pragma once

#ifndef SCENEH
#define SCENEH

#include "Object.h"
#include "Ray.h"
#include "../Vector/Vector3.cpp"
#include <vector>
#include <limits>
#include "Camera.h"

struct SceneOptions
{
    double nearClip = 0.0001;
    double farClip = std::numeric_limits<double>::max();
    int maxRayDepth = 5;
    int raysPerPixel = 100;
};

class Scene {
public:
    std::vector<Object*> objects;
    std::vector<Object*> lights;
    Camera *camera;

public:
    
    Scene(): objects(), camera(nullptr){}
    Scene(Camera* cam): objects(), camera(cam){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    Vector3d trace(const Ray& r, int depth);

    //Renderiza
    void RenderScene(int minX, int minY, int boundX, int boundY);
    //void RenderScene();
    //Adiciona um objeto a cena
    void add(Object* object);
};

#endif 