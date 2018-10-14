#pragma once

#include "Vector/Vector3.cpp"
#include "float.h"

class Ray
{
private: 
    Vector3d origin, direction;

public:
    mutable double tmin = DBL_EPSILON, tmax = DBL_MAX;

    Ray(Vector3d origin, Vector3d direction);

    Vector3d sample(const double& t) const;

    Vector3d getOrigin() const;

    Vector3d getDirection() const;
};