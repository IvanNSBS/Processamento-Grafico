#include "Camera.h"

#define M_PI 3.141592653589793

double random()
{
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

Vector3d Rand_In_Disk()
{
    Vector3d p;
    do{
        
        p = (Vector3d(random(), random(), 0) * 2) - Vector3d(1.0, 1.0, 0.0);
    }while(p.DotProduct(p) >= 1.0);

    return p;
}

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
    //Todos os eixos devem ser normalizados
    this->axisZ = (position-target).Normalize();

    //Garante que o vetor up(axisY) eh ortogonal ao eixo z
    //pois a entrada pode ser um vetor nao ortogonal
    //Isso eh feito utilizando o metodo Gram Schmidt 
    this->axisY = up - (axisZ * ( (up.DotProduct(axisZ) )/axisZ.DotProduct(axisZ) ) );
    this->axisY.Normalize();

    //Eixo x deve ser ortogonal ao eixo y e ao eixo z
    //Para isso, realiza o produto vetorial entre y e z, que ja sao ortogonais entre si
    this->axisX = axisY.CrossProduct(axisZ);
    this->axisX.Normalize();

    //Seta matriz de transformacao afim para ser capaz de movimentar a camera.
    //Parte linear da transformacao
    camToWorld.m[0][0] = axisX.x; 
    camToWorld.m[0][1] = axisX.y; 
    camToWorld.m[0][2] = axisX.z; 

    camToWorld.m[1][0] = axisY.x; 
    camToWorld.m[1][1] = axisY.y; 
    camToWorld.m[1][2] = axisY.z; 

    camToWorld.m[2][0] = axisZ.x; 
    camToWorld.m[2][1] = axisZ.y; 
    camToWorld.m[2][2] = axisZ.z; 
    
    //Parte nao linear (translacao)
    camToWorld.m[3][0] = position.x; 
    camToWorld.m[3][1] = position.y; 
    camToWorld.m[3][2] = position.z;
}

Ray Camera::GetRay(double x, double y)
{

    float invWidth = 1 / float(canvas->width), invHeight = 1 / float(canvas->height); 
    float aspectratio = canvas->width / float(canvas->height); 
    //Angulo de abertura da camera
    float angle = tan((this->fov * 0.5) * (M_PI / 180)) * near ; //Multiplica pelo near (zoom)
    float half_width = angle * aspectratio;
    float half_height = angle;

    //Converte o ponto x do canvas para raster space
    //Multiplica pelo aspectratio, pois o canvas pode nao ser quadrado e gerar distorcao
    float Px = (2 * ( (x ) * invWidth) - 1) * half_width; 
    float Py = (1 - 2 * ( (y ) * invHeight)) * half_height; 

    Vector3d origin = 0;
    Vector3d dir = Vector3d(Px, Py, -1) - origin;
    camToWorld.multVecMatrix(origin, origin);
    camToWorld.multDirMatrix(dir, dir);
    dir.Normalize();
    //dir.Normalize();
    return Ray(origin , dir);
}