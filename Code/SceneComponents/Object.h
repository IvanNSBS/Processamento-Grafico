#pragma once

#ifndef OBJECTH
#define OBJECTH

#include "Material.h"

//O objeto é uma esfera.

class Object;

class Object {
public:
    Material* material;
    //Sphere parameters
    Vector3d center;
    int radius;
    int radius2;

public:
    // Construtor
    Object() : material(nullptr), center(0), radius(0), radius2(0){}
    Object(const Vector3d &c, const int &rad, Material* mat): material(mat), center(c), radius(rad), radius2(rad*rad) {}

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    bool intersect(const Ray& r, double &t0, double &t1);
    
    //Vector3d random_in_unit_sphere();
    Vector3d getPoint() const;
};

#endif