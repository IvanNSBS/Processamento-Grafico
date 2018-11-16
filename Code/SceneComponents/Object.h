#pragma once

#ifndef OBJECTH
#define OBJECTH

#include "Material.h"
#include "HitRecord.h"

//O objeto é uma esfera.

class Object;

class Object {
public:
    Material* material;

public:
    // Construtor
    Object() : material(nullptr){}
    Object(Vector3d c, Material* mat): center(c), material(mat) {}
    Vector3d center;

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    virtual bool intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const = 0;
};

class Sphere : public Object
{
public:
    float radius;
    float radius2;

    Sphere() : Object(0, nullptr), radius(0), radius2(0){}
    Sphere(const Vector3d &c, const float &rad, Material* mat): Object(c, mat), radius(rad), radius2(rad*rad) {}

    virtual bool intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const;
};

class XY_Rect : public Object
{
public:
    //k é o onde o plano se encontra no eixo Z
    float x0,x1, y0, y1, k;

    XY_Rect() : Object(0, nullptr){}
    XY_Rect(float xx0, float xx1, float yy0, float yy1, float kk, Material* mat): 
            Object(Vector3d((xx1+xx0)/2, (yy1+yy0)/2, kk), mat), x0(xx0), x1(xx1), y0(yy0), y1(yy1), k(kk) {}

    virtual bool intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const;
};

class XZ_Rect : public Object
{
public:
    //k é o onde o plano se encontra no eixo Z
    float x0,x1, z0, z1, k;

    XZ_Rect() : Object(0, nullptr){}
    XZ_Rect(float xx0, float xx1, float zz0, float zz1, float kk, Material* mat): 
            Object(Vector3d((xx1+xx0)/2, kk, (zz1+zz0)/2), mat), x0(xx0), x1(xx1), z0(zz0), z1(zz1), k(kk) {}

    virtual bool intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const;
};

class YZ_Rect : public Object
{
public:
    //k é o onde o plano se encontra no eixo Z
    float y0, y1, z0, z1, k;

    YZ_Rect() : Object(0, nullptr){}
    YZ_Rect(float yy0, float yy1, float zz0, float zz1, float kk, Material* mat): 
            Object(Vector3d(kk, (yy1+yy0)/2, (zz1+zz0)/2), mat), y0(yy0), y1(yy1), z0(zz0), z1(zz1), k(kk) {}

    virtual bool intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const;
};

#endif