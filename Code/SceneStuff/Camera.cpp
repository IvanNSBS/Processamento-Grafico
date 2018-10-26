#include "Camera.h"

#define M_PI 3.141592653589793

Camera::Camera(Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->position = position;

    this->fov = fov;
    this->near = near;
    SetCamAxisAndMatrix(position, target, up);
}

Camera::Camera(Image* im, Vector3d position, Vector3d target, Vector3d up, double fov, double near)
{
    this->canvas = im;
    this->position = position;

    this->fov = fov;
    this->near = near;
    SetCamAxisAndMatrix(position, target, up);
}

void Camera::SetCamAxisAndMatrix(Vector3d position, Vector3d target, Vector3d up)
{
    this->axisZ = target;
    this->axisZ.Normalize();

    this->axisY = up - (axisZ * ( (up.DotProduct(axisZ) )/axisZ.DotProduct(axisZ) ) );
    this->axisY.Normalize();

    this->axisX = axisY.CrossProduct(axisZ);
    this->axisX.Normalize();


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

Ray Camera::GetRay(double x, double y)
{

    float invWidth = 1 / float(canvas->width), invHeight = 1 / float(canvas->height); 
    float aspectratio = canvas->width / float(canvas->height); 
    float angle = tan(M_PI * 0.5 * this->fov / 180);

    float Px = (2 * ((x) * invWidth) - 1) * angle * aspectratio; 
    float Py = (1 - 2 * ((y) * invHeight)) * angle; 

    Vector3d rayOrigin(0);
    camToWorld.multVecMatrix(Vector3d(0), rayOrigin);
    Vector3d dir;
    camToWorld.multDirMatrix(Vector3d(Px, Py, -1), dir);
    dir.Normalize();
    return Ray(rayOrigin, dir);
}