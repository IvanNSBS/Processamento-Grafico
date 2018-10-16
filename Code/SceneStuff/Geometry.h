#pragma once

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include <cmath>

class Geometry {
public:
    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    virtual bool intersect(const Ray& r, ObjectIntersection* info = nullptr) const = 0;

    /*
    Retorna um ponto do objeto para ser usado como fonte de luz
    */
    virtual Vector3d getPoint() const = 0;

};

class Sphere : public Geometry {
private:
    Vector3d center;
    double radius;

public:
    Sphere(Vector3d center, double radius);

    bool intersect(const Ray& r, ObjectIntersection* info = nullptr) const override;

    Vector3d getPoint() const override;
};
