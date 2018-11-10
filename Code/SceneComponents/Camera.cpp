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
    this->axisZ = target;
    this->axisZ.Normalize();

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

    Vector3d rd = Rand_In_Disk()*2;
    Vector3d offset = axisY*rd.x + axisX*rd.y;

    //Converte o ponto x do canvas para raster space
    //Multiplica pelo aspectratio, pois o canvas pode nao ser quadrado e gerar distorcao
    float Px = (2 * ( (x ) * invWidth) - 1) * angle * aspectratio; 
    float Py = (1 - 2 * ( (y ) * invHeight)) * angle; 


    //Aplica-se a matriz de transformacao camToWorld em rayOrigin
    //pois a camera pode nao estar na origem e nem com seus eixos padrao
    Vector3d rayOrigin(0);
    camToWorld.multVecMatrix(0, rayOrigin);

    //Nao se usa multVecMatrix, pois direçao nao possui posiçao
    Vector3d dir;// = Vector3d(0, 0, -12) - offset - Vector3d(Px, Py, -1);
    camToWorld.multDirMatrix(Vector3d(Px,Py, -1), dir);
    dir.Normalize();
    //rayOrigin = rayOrigin + offset;
    //dir = (dir - rayOrigin).Normalize();
    //dir.Normalize(); //Toda direcao deve ser normalizada


    //teste
    double theta = fov*M_PI/180;
    double half_height = tan(theta*0.5);
    double half_width = aspectratio * half_height;
    Vector3d Origin(0);
    Vector3d Point = Origin - axisY * 14*half_width - axisX*half_height*14 - axisZ*14;
    Vector3d horizontal = axisY*half_width*14*2;
    Vector3d vertical = axisX*half_height*14*2;

    return Ray(Origin + offset, Point + horizontal*x + vertical*y - Origin - offset);
}