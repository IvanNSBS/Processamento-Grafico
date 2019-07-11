#include "Material.h"
#include <algorithm>
#include <utility>
#include <cmath>

//Calcula o vetor apos a reflexao com a superficie
Vector3d reflect( Vector3d &v, const Vector3d& n) {
     return v - (n * (2*v.DotProduct(n)));
}

double limit(const double& a, const double &min, const double &max)
{
    if(a < min)
        return min;
    else if(a > max)
        return max;

    return a;
}

void fresnel(Vector3d &I, const Vector3d &N, const float &ior, float &kr) 
{ 
    float cosi = limit(I.DotProduct(N),-1, 1); 
    float etai = 1, etat = ior; 
    if (cosi > 0) {  std::swap(etai, etat); } 
    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(std::max(0.f, 1 - (cosi * cosi))); 
    // Total internal reflection
    if (sint >= 1) { 
        kr = 1; 
    } 
    else { 
        float cost = sqrt(std::max(0.f, 1 - (sint * sint))); 
        cosi = fabsf(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
        kr = ((Rs * Rs) + (Rp * Rp)) / 2; 
    } 
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
} 

Vector3d refract(Vector3d &I, const Vector3d &N, const double &ior)
{ 
    float cosi = limit(I.DotProduct(N),-1,1); 
    float etai = 1, etat = ior; 
    Vector3d n = N; 
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= N*-1; } 
    float eta = etai / etat; 
    float k = 1 - eta * eta * (1 - cosi * cosi); 
    return k < 0 ? 0 : (I*eta) + (n*(eta * cosi - sqrtf(k))); 
}