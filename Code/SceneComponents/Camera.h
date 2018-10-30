#pragma once

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include "../Vector/Matrix.cpp"
#include "Image.h"
#include <cmath>

class Camera
{
public: 
    Vector3d position, axisX, axisY, axisZ;
    Matrix44d camToWorld;   //matriz para posicionar a camera. Setada no construtor
    Image* canvas;          //Imagem renderizada pela camera. É setada no RenderScene() do Scene.h  
    double fov, near;

public:

    //construtor sem setar Canvas
    Camera (Vector3d position, Vector3d target, Vector3d up, double fov, double near);
    //construtor setando canvas
    Camera (Image *im, Vector3d position, Vector3d target, Vector3d up, double fov, double near);

    //GetRay retorna o raio jogado no centro do pixel a partir da camera.
    //Leva em consideração posição da camera e seus eixos
    //Ler comentarios na implementaçao para mais detalhes de como as coisas funcionam
    Ray GetRay(double x, double y);

    //faz a mesma coisa que o construtor antes fazia, 
    //mas como agora existem 2, essa funçao deixa mais organizado as coisas
    void SetCamAxisAndMatrix(Vector3d position, Vector3d target, Vector3d up);
};