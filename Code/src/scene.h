#pragma once

#ifndef SCENEH
#define SCENEH

#include "object.h"
#include "ray.h"
#include <vector>
#include <limits>
#include "camera.h"
#include <typeinfo>
#include <random>

std::default_random_engine gen; 
std::uniform_real_distribution<float> distr(0, 1);
float random_01() { return distr(gen); }

float clamp(const float& a, const float &min, const float &max)
{
    if(a < min)
        return min;
    else if(a > max)
        return max;

    return a;
}

struct SceneOptions
{
    float tmin, tmax; 
    int rayDepth, raysPerPixel;
    SceneOptions(const float &ttmin = 0.001, 
                const float &ttmax = std::numeric_limits<float>::max(),
                const int &maxDepth = 5, const int &rpPixel = 100):tmin(ttmin), tmax(ttmax), rayDepth(maxDepth), raysPerPixel(rpPixel){}
};

class Scene {
public:
    std::vector<Object*> objects;
    std::vector<Object*> lights;
    Camera *camera;
    SceneOptions options;

public:
    
    Scene(): objects(), camera(nullptr){}
    Scene(Camera* cam, const SceneOptions &opt = 0): objects(), camera(cam), options(opt){}

    //Implementa o modelo de iluminacao escolhido, retornando uma cor
    vec3 trace(const Ray& r, int depth){
        if(depth > options.rayDepth)
            return vec3(1,1,1);

        Object* hitted = NULL;
        HitRecord rec;
        // Encontra o ponto intersecao do raio com a esfera que o raio atingiu primeiro.
        // ponto é salvo em HitRecord rec.
        for (unsigned i = 0; i < objects.size(); ++i)
        { 
            //hitrecord temporario. Salva o hitrecord da esfera atual para verificar qual é o mais proximo
            HitRecord temp_hrec; 
            if ( objects[i]->intersect(r, options.tmin, options.tmax, temp_hrec) ) 
            {  
                if (temp_hrec.t < rec.t) 
                { 
                    hitted = objects[i];
                    rec = temp_hrec;
                } 
            } 
        } 

        //Se nao tem intersecao retorna background color
        if (!hitted)
        {
            return vec3(0,0,0);
            return 1;
            vec3 unit_direction = r.getDirection();
            float t = 0.5 * (unit_direction.y() + 1.0);
            return (vec3(1.0, 1.0, 1.0) * (1.0 - t) ) + (vec3(0.5, 0.7, 1.0)*t);
        }

        vec3 surfaceColor(0,0,0);
        float bias = 1e-4; //Bias para evitar shadow acne

        //Computa o global illumination pelo path tracing
        Ray scattered;
        vec3 attenuation;
        ScatterInfo sinfo;

        if(hitted->material->scatter(r, rec.phit, rec.nhit, sinfo)){
            return hitted->material->emmited() + sinfo.attenuation*trace(sinfo.r1, depth+1);
            //global illumination
            // float obstruction = 0.0f;
            // for (unsigned i = 0; i < lights.size(); ++i) 
            // { 
            //     vec3 lightDirection = (lights[i]->center + random_in_unit_sphere()*dynamic_cast<Sphere*>(lights[i])->radius) - rec.phit;  
            //     lightDirection.make_unit_vector(); 
            //     HitRecord last_rec;
            //     last_rec.t = options.tmax;
            //     for (unsigned j = 0; j < objects.size(); ++j)
            //     { 
            //         if (objects[j] != hitted )
            //         {
            //             Ray n = Ray(rec.phit + rec.nhit * bias, lightDirection);
            //             HitRecord lrec;
            //             if (objects[j]->intersect(n, options.tmin, options.tmax, lrec) )
            //                 if ( lrec.t < last_rec.t )
            //                     last_rec = lrec;
            //         } 
            //     }
            //     if ( last_rec.hitted != nullptr ){
            //         if(last_rec.hitted->material->matType != mat_type::light)
            //             surfaceColor *= (last_rec.t / (rec.phit - lights[i]->center).length());
            //         // else if(last_rec.hitted->material->matType == mat_type::light)
            //             // surfaceColor *= (last_rec.hitted->material->emissionColor * last_rec.hitted->material->lightIntensity);
            //     }         

            // }
        }
        else
            return hitted->material->emmited();
    }


    //Renderiza
    void RenderScene(int minX, int minY, int boundX, int boundY)
    {   
        //Joga raios na cena
        for (unsigned y = minY; y < boundY; ++y)
            for (unsigned x = minX; x < boundX; ++x)
            {
                vec3 col(0,0,0); //cor resultante dos raios

                //Para cada pixel, joga X raios
                //Cada raio possui um offset aleatorio
                //calculando a media dos cem raios acumulados, conseguimos produzir anti-aliasing,
                //além de reduzir o ruido provocado pelo path tracing
                //(quanto mais raios, melhor o AA(super-sampling) e a reduçao de ruido)
                int rayNum = options.raysPerPixel;
                for(int i = 0; i < rayNum; i++){
                    col = col + trace( camera->GetRay( x + random_01(), y + random_01()), 0 );
                }
                float ns = float(rayNum);
                col = vec3 ( sqrt(col.r()/ns), sqrt(col.g()/ns), sqrt(col.b()/ns));
                this->camera->canvas->SetPixel( x, y, col );  //seta a cor do pixel para a coordenada
            }
        
        //Salva o ppm como RenderedCanvas na pasta RenderedImages
    }
    
    //void RenderScene();
    //Adiciona um objeto a cena
    void add(Object* object)
    {
        objects.push_back(object);
        if(object->material->matType == mat_type::light)
            lights.push_back(object);
    }
};

#endif 