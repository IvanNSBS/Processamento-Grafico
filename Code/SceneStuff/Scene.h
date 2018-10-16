#pragma once

#include "Object.h"
#include "ObjectIntersection.h"
#include "Ray.h"
#include "../Vector/Vector3.cpp"
#include <vector>

class Scene {
private:
    std::vector<Object*> objects;

public:

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    bool intersect(const Ray& r, ObjectIntersection* info = nullptr) const;

    /*
    Implementa o modelo de iluminacao escolhido, retornando uma cor
    */
    Vector3d trace(const Ray& r, int recursiveLevel = 0) const;

    /*
    Adiciona um objeto a cena
    */
    void add(Object* object);
};