#include "Ray.h"

Ray::Ray(Vector3d origin, Vector3d direction)
{
    this->origin = origin;
    this->direction = direction;
}

Vector3d Ray::sample(const double& t)  const
{
    return this->origin + (this->direction * t);
}