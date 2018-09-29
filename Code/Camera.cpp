#pragma once

#include "Vector3.cpp"
#include "Ray.cpp"
#include <cmath>

class Camera
{
private: 
    Vector3d position, axisX, axisY, axisZ;
    double fov, near;

    Camera (Vector3d position, Vector3d target, Vector3d up, double fov, double near);

    Ray GetRay(double x, double y, int width, int height);
};

Camera::Camera(Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->position = position;

    this->fov = fov;
    this->near = near;

    this->axisX = target;
    this->axisY = up;
    this->axisZ = axisX.CrossProduct(axisY);
}