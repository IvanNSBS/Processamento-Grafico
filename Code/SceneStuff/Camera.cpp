#include "Camera.h"

Camera::Camera(Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->position = position;

    this->fov = fov;
    this->near = near;

    this->axisX = target;
    
    this->axisY = axisX - ((axisX.DotProduct(up))/(up.DotProduct(up)));
    this->axisZ = axisX.CrossProduct(axisY);
    this->axisX.Normalize();
    this->axisY.Normalize();
    this->axisZ.Normalize();
}

Ray Camera::GetRay(double x, double y, int width, int height)
{
        
}