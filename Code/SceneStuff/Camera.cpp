#include "Camera.h"

Camera::Camera(Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->position = position;

    this->fov = fov;
    this->near = near;

    this->axisZ = target;
    this->axisZ.Normalize();
    cout << axisZ;

    this->axisY = up - (axisZ * ( (up.DotProduct(axisZ) )/axisZ.DotProduct(axisZ) ) );
    cout << axisY;

    this->axisX = axisY.CrossProduct(axisZ);
    cout << axisX;

    this->axisX.Normalize();
    this->axisY.Normalize();

    camToWorld.m[0][0] = axisX.x; 
    camToWorld.m[0][1] = axisX.y; 
    camToWorld.m[0][2] = axisX.z; 

    camToWorld.m[1][0] = axisY.x; 
    camToWorld.m[1][1] = axisY.y; 
    camToWorld.m[1][2] = axisY.z; 

    camToWorld.m[2][0] = axisZ.x; 
    camToWorld.m[2][1] = axisZ.y; 
    camToWorld.m[2][2] = axisZ.z; 
 
    camToWorld.m[3][0] = position.x; 
    camToWorld.m[3][1] = position.y; 
    camToWorld.m[3][2] = position.z;
}

void Camera::LookAt(const Vector3d& from, const Vector3d& to)
{
    Vector3d forward = to - from;
    forward.Normalize();
    
    Vector3d aux = Vector3d(0, 1, 0);
    aux.Normalize();

    Vector3d right = aux.CrossProduct(forward); 
    Vector3d up = forward.CrossProduct(right); 
 
    camToWorld[0][0] = right.x; 
    camToWorld[0][1] = right.y; 
    camToWorld[0][2] = right.z; 
    camToWorld[1][0] = up.x; 
    camToWorld[1][1] = up.y; 
    camToWorld[1][2] = up.z; 
    camToWorld[2][0] = forward.x; 
    camToWorld[2][1] = forward.y; 
    camToWorld[2][2] = forward.z; 
 
    camToWorld[3][0] = from.x; 
    camToWorld[3][1] = from.y; 
    camToWorld[3][2] = from.z; 
}

Ray Camera::GetRay(double x, double y, int width, int height)
{
        
}