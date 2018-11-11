#ifndef HITRECORDH
#define HITRECORDH

#include "../Vector/Vector3.cpp"

class Material;

struct HitRecord
{
    Vector3d nhit, phit;
    double t;
    Material* mat;
};

#endif