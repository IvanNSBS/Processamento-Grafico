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
    double tmin, tmax; 
    int rayDepth, raysPerPixel;
    SceneOptions(const double &ttmin = 0.001, 
                const double &ttmax = std::numeric_limits<double>::max(),
                const int &maxDepth = 5, const int &rpPixel = 100):tmin(ttmin), tmax(ttmax), rayDepth(maxDepth), raysPerPixel(rpPixel){}
};

class Scene {
public:
    std::vector<Object*> objects;
    std::vector<Object*> lights;
    Camera *camera;
    SceneOptions options;

public:
    
    Scene(): objects(), camera(nullptr){}
    Scene(Camera* cam, const SceneOptions &opt = 0): objects(), camera(cam), options(opt){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    Vector3d trace(const Ray& r, int depth);

    //Renderiza
    void RenderScene(int minX, int minY, int boundX, int boundY);
    //void RenderScene();
    //Adiciona um objeto a cena
    void add(Object* object);
};

#endif 