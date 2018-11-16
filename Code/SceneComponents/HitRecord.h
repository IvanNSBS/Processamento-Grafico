#ifndef HITRECORDH
#define HITRECORDH

#include "../Vector/Vector3.cpp"
#include <limits>

class Material;
class Object;

struct HitRecord
{
    Vector3d nhit, phit;
    double t = std::numeric_limits<double>::max();
    Material* mat;
    Object* hitted; 
};

#endif