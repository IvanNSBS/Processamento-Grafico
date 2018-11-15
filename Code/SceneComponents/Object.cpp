#include "Object.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <random>

std::default_random_engine generator; 
std::uniform_real_distribution<float> distributor(0, 1);
//Calcula uma direcao aleatoria dentro de uma esfera
Vector3d random_in_unit_sphere() {
    Vector3d p;
    do {
        double drand49 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        p = ( Vector3d( distributor(generator), distributor(generator), distributor(generator) ) * 2.0 )- Vector3d(1,1,1);
    } while (p.Length_Squared() >= 1.0);
    return p;
}

double drand48()
{
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

//Calcula os dois pontos em que um raio intersecta uma esfera
bool Sphere::intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const
{
    Vector3d nhit = this->center - r.getOrigin(); 
    float tca = nhit.DotProduct(r.getDirection()); 
        if (tca < 0) 
            return false; 

    float d2 = nhit.DotProduct(nhit) - tca * tca; 
        if (d2 > radius*radius) 
            return false; 
                
    float thc = sqrt(radius*radius - d2); 

    t0 = tca - thc; 
    t1 = tca + thc;

    float tnear;
    if(t0 < 0)
        tnear = t1;
    else
        tnear = t0 < t1 ? t0 : t1;
    rec.t = tnear;
    rec.phit = r.getOrigin() + r.getDirection() * tnear;
    rec.nhit = rec.phit - center;
    rec.nhit.Normalize();
    rec.mat = this->material;
    return true; 
}

bool XZ_Rect::intersect(const Ray& r, double &t0, double &t1, HitRecord& rec) const
{
    float tnear = (k-r.getOrigin().y) / r.getDirection().y;
    //if(tnear < tmin || tnear > tmax)
    //    return false;
    float x = r.getOrigin().x + r.getDirection().x * tnear;
    float y = r.getOrigin().z + r.getDirection().z * tnear;
    if(x < x0 || x > x1 || y < y0 || y > y1)
        return false;

    t0 = t1 = tnear;
    rec.t = tnear;
    rec.mat = this->material;
    rec.phit = r.getOrigin() + r.getDirection()*tnear;
    rec.nhit = Vector3d(0, 1, 0); 
    return true; 
}