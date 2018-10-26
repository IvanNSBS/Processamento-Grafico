#include "Object.h"
#include <stdlib.h>
#include <iostream>

Vector3d reflect( Vector3d &v, const Vector3d& n) {
     return v - (n * (2*v.DotProduct(n)));
}

Vector3d random_in_unit_sphere() {
    Vector3d p;
    do {
        double drand48 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        p = (Vector3d(drand48, drand48, drand48) * 2.0 )- Vector3d(1,1,1);
    } while (p.Length_Squared() >= 1.0);
    return p;
}

double drand48()
{
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

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