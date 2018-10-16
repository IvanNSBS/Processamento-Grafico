#pragma once

#include "Geometry.h"
#include "Material.h"
#include "ObjectIntersection.h"

class Object {
public:
    Geometry* geometry;
    Material* material;

public:
    Object(Geometry* geometry, Material* material);

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    bool intersect(const Ray& r, ObjectIntersection* info = nullptr) const;
    
};