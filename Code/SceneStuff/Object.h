#pragma once

#include "../Vector/Vector3.cpp"
#include "Ray.h"

//O objeto é uma esfera.

class Object;

struct Material
{
public:
    Vector3d surfaceColor, emissionColor;
    float Kd; // Diffuse Factor
    float Ks; // Specular Factor
    float alpha; // Coeficiente de rugosidade 

    //float transparency, reflection;

    //Material(): surfaceColor(0), emissionColor(0){}
    //Material(Vector3d sc):surfaceColor(sc), emissionColor(0) {}
    Material(const Vector3d &sc = 0, const Vector3d &ec = 0):surfaceColor(sc), emissionColor(ec) {}
    Material(const float &d = 1, const float &e = 1, const float &a = 1, 
             const Vector3d &sc = 0, const Vector3d &ec = 0):surfaceColor(sc), emissionColor(ec), Kd(d), Ks(e), alpha(a) {}
};

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
    Vector3d getPoint() const;
};