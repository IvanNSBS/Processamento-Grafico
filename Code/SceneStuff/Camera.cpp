#include "Camera.h"

Camera::Camera(Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->position = position;

    this->fov = fov;
    this->near = near;

    this->axisX = target;
    this->axisY = up;
    this->axisZ = axisX.CrossProduct(axisY);
}

Ray Camera::GetRay(double x, double y, int width, int height)
{
        
}