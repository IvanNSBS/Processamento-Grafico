#pragma once

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include "../Vector/Matrix.cpp"
#include <cmath>

class Camera
{
public: 
    Vector3d position, axisX, axisY, axisZ;
    Matrix44d camToWorld;
    double fov, near;

    Camera (Vector3d position, Vector3d target, Vector3d up, double fov, double near);
    void LookAt(const Vector3d& from, const Vector3d& to);

    Ray GetRay(double x, double y, int width, int height);
};