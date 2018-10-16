#pragma once
#include "../Vector/Vector3.cpp"

class Object;

struct ObjectIntersection
{
    double t;
    Vector3d p, n;
    Object *o;
};