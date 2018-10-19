#pragma once

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include <cmath>

class Camera
{
private: 
    Vector3d position, axisX, axisY, axisZ;
    double fov, near;

    Camera (Vector3d position, Vector3d target, Vector3d up, double fov, double near);

    Ray GetRay(double x, double y, int width, int height);
};