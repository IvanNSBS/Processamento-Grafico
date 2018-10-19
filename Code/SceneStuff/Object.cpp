#include "Object.h"
#include <iostream>

bool Object::intersect(const Ray& r, double &t0, double &t1)
{
    Vector3d l = this->center - r.getOrigin(); 
    float tca = l.DotProduct(r.getDirection()); 
        if (tca < 0) 
            return false; 

    float d2 = l.DotProduct(l) - tca * tca; 
        if (d2 > radius*radius) 
            return false; 
                
    float thc = sqrt(radius*radius - d2); 

    t0 = tca - thc; 
    t1 = tca + thc;

    return true; 
}

Vector3d Object::getPoint() const
{
    return Vector3d(0);
}