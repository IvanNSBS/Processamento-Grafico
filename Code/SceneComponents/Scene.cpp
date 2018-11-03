#include "Scene.h"
#include "Material.h"
#include <algorithm>
#include <stdlib.h> 
#include <cmath> 

#define MAX_RAY_DEPTH 5  

double mix(const double &a, const double &b, double &mix) 
{ 
    return b * mix + a * (1 - mix); 
}

double clamp(const double& a, const double &min, const double &max)
{
    if(a < min)
        return min;
    else if(a > max)
        return max;

    return a;
}

void Scene::add(Object* object)
{
    objects.push_back(object);
}

Vector3d Scene::trace(const Ray& r, int depth) 
{
    float tnear = INFINITY; 
    Object* sphere = NULL;
    // Encontra a intersecao do raio com a esfera mais 'anterior' ao raio(que o raio atingiu primeiro)
    for (unsigned i = 0; i < objects.size(); ++i)
    { 
        double t0 = INFINITY, t1 = INFINITY; 
        if ( objects[i]->intersect(r,  t0, t1) ) 
        { 
            if (t0 < 0) 
                t0 = t1; 
            if (t0 < tnear) 
            { 
                tnear = t0; 
                sphere = objects[i];
            } 
        } 
    } 

    //Se nao tem intersecao retorna background color
    if (!sphere)
    {
        Vector3d unit_direction = r.getDirection().Normalize();
        double t = 0.5 * (unit_direction.y + 1.0);
        return (Vector3d(0.65, 0.85, 1.0) * (1.0 - t) ) + Vector3d(0.5, 0.7, 1.0)*t;
    }

    Vector3d surfaceColor = 0; //Cor do raio/superficie do objeto intersectado pelo raio 
    Vector3d phit = r.getOrigin() + r.getDirection() * tnear; //Ponto de intersecao
    Vector3d nhit = phit - sphere->center; //Normal do ponto de intersecao
    nhit.Normalize(); //Normaliza a normal

    float bias = 1e-4; //Bias para evitar shadow acne

    if(depth > MAX_RAY_DEPTH)
        return 0;

    switch (sphere->material->matType)
    {
        case mat_type::lambertian:{
            Ray scattered;
            Vector3d attenuation;
            if(sphere->material->scatter(r, phit, nhit, attenuation, scattered))
                surfaceColor = attenuation*(trace(scattered, depth+1));
            else
                return Vector3d(0, 0, 0);

                for (unsigned i = 0; i < objects.size(); ++i) 
                { 
                    if (objects[i]->material->emissionColor.x > 0)
                    { 
                        //Caso o objeto seja uma fonte de luz
                        Vector3d transmission = 1; 
                        Vector3d lightDirection = objects[i]->center - phit; 
                        lightDirection.Normalize(); 
                        for (unsigned j = 0; j < objects.size(); ++j)
                        { 
                            if (i != j)
                            { 
                                double t0, t1;
                                Ray n = Ray(phit + nhit * bias, lightDirection);
                                if (objects[j]->intersect(n, t0, t1) )
                                {  
                                    transmission = sphere->material->surfaceColor * 0.95; 
                                    break; 
                                } 
                            } 
                        }
                        //Vetor da reflexao da luz com a superficie
                        Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

                        //Fator difuso pelo modelo de iluminacao de Phong
                        Vector3d diffuse = transmission * 
                        sphere->material->surfaceColor * objects[i]->material->emissionColor * 
                        std::max(double(0), nhit.DotProduct(lightDirection));

                        //Fator especular pelo modelo de iluminacao de Phong
                        Vector3d specular = transmission * objects[i]->material->emissionColor * 
                        pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), sphere->material->alpha);
                            
                        //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                        surfaceColor = surfaceColor + (diffuse * sphere->material->Kd) + (specular * sphere->material->Ks);
                    } 
                }
                break;
        }

        case mat_type::conductor:{
            ScatterInfo sinfo;
            sphere->material->scatter(r, phit, nhit, sinfo);
            surfaceColor = sphere->material->surfaceColor*trace(sinfo.r1, depth + 1); 
            for (unsigned i = 0; i < objects.size(); ++i) 
            { 
                if (objects[i]->material->emissionColor.x > 0)
                { 
                    //Caso o objeto seja uma fonte de luz
                    Vector3d transmission = 1; 
                    Vector3d lightDirection = objects[i]->center - phit; 
                    lightDirection.Normalize(); 
                    for (unsigned j = 0; j < objects.size(); ++j)
                    { 
                        if (i != j)
                        { 
                            double t0, t1;
                            Ray n = Ray(phit + nhit * bias, lightDirection);
                            if (objects[j]->intersect(n, t0, t1) )
                            {  
                                transmission = sphere->material->surfaceColor * 0.95; 
                                break; 
                            } 
                        } 
                    }
                    //Vetor da reflexao da luz com a superficie
                    Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

                    //Fator difuso pelo modelo de iluminacao de Phong
                    Vector3d diffuse = transmission * 
                    sphere->material->surfaceColor * objects[i]->material->emissionColor * 
                    std::max(double(0), nhit.DotProduct(lightDirection));

                    //Fator especular pelo modelo de iluminacao de Phong
                    Vector3d specular = transmission * objects[i]->material->emissionColor * 
                    pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), sphere->material->alpha);
                        
                    //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                    surfaceColor = surfaceColor + (diffuse * sphere->material->Kd) + (specular * sphere->material->Ks);
                } 
            }
            break;
        } 

        case mat_type::dielectric:{
            ScatterInfo sinfo;
            sphere->material->scatter(r, phit, nhit, sinfo);
            Vector3d reflectionColor = trace(sinfo.r1, depth + 1); 
            Vector3d refractionColor = trace(sinfo.r2, depth + 1);
            surfaceColor = reflectionColor * sinfo.kr + refractionColor * (1 - sinfo.kr); 

            
            break; 
        }
    }
    //Delimita o valor da cor entre 0 e 1
    return Vector3d( (clamp(surfaceColor.x, 0.0, 1.0)), (clamp(surfaceColor.y, 0.0, 1.0)), 
                     (clamp(surfaceColor.z, 0.0, 1.0)) ) + sphere->material->emissionColor;
    //return Vector3d( sqrt(surfaceColor.x), sqrt(surfaceColor.y), sqrt(surfaceColor.z));  
}

void Scene::RenderScene() 
{   
    //Joga raios na cena
    for (unsigned y = 0; y < camera->canvas->height; ++y)
        for (unsigned x = 0; x < camera->canvas->width; ++x)
        {
            Vector3d col = 0; //cor resultante dos raios

            //Para cada pixel, joga 20 raios
            //Cada raio possui um offset aleatorio
            //calculando a media dos cem raios acumulados, conseguimos produzir anti-aliasing
            for(int i = 0; i < 100; i++)
                col = col + trace( camera->GetRay( x + drand48() , y + drand48() ), 0 );
            double ns = double(100);
            col = Vector3d (col.x/ns, col.y/ns, col.z/ns);
            //col = Vector3d ( sqrt(col.x), sqrt(col.y), sqrt(col.z));
            this->camera->canvas->SetPixel( x, y, col);  //seta a cor do pixel para a coordenada
        }
    
    //Salva o ppm como RenderedCanvas na pasta RenderedImages
    this->camera->canvas->SaveAsPBM("../RenderedImages/", "RenderedCanvas5");
}

/*
            for (unsigned i = 0; i < lights.size(); ++i) 
            {
                Vector3d transmission = 1; 
                Vector3d lightDirection = lights[i]->center - phit; 
                lightDirection.Normalize(); 
                for (unsigned j = 0; j < objects.size(); ++j)
                { 
                    double t0, t1;
                    Ray n = Ray(phit + nhit * bias, lightDirection);
                    if (objects[j]->intersect(n, t0, t1))
                    {  
                        transmission = sphere->material->surfaceColor * 0.95; 
                        break; 
                    }  
                }
                //Vetor da reflexao da luz com a superficie
                Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

                //Fator difuso pelo modelo de iluminacao de Phong
                Vector3d diffuse = transmission * 
                sphere->material->surfaceColor * objects[i]->material->emissionColor * 
                std::max(double(0), nhit.DotProduct(lightDirection));

                //Fator especular pelo modelo de iluminacao de Phong
                Vector3d specular = transmission * objects[i]->material->emissionColor * 
                pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), sphere->material->alpha);
                        
                //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                surfaceColor = surfaceColor + (diffuse * sphere->material->Kd) + (specular * sphere->material->Ks); 
            }
*/