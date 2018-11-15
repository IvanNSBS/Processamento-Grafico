#include "Scene.h"
#include "Material.h"
#include <thread>

#define MAX_RAY_DEPTH 5

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
    Object* hitted = NULL;
    // Encontra a intersecao do raio com a esfera mais 'anterior' ao raio(que o raio atingiu primeiro)
    HitRecord rec;
    for (unsigned i = 0; i < objects.size(); ++i)
    { 
        HitRecord arec;
        double t0, t1; 
        if ( objects[i]->intersect(r,  t0, t1, arec) ) 
        {  
            if (arec.t < tnear) 
            { 
                tnear = arec.t; 
                hitted = objects[i];
                rec = arec;
            } 
        } 
    } 

    //Se nao tem intersecao retorna background color
    if (!hitted)
    {
        Vector3d unit_direction = r.getDirection();
        double t = 0.5 * (unit_direction.y + 1.0);
        return (Vector3d(1.0, 1.0, 1.0) * (1.0 - t) ) + (Vector3d(0.5, 0.7, 1.0)*t);
    }
    
    if(depth > MAX_RAY_DEPTH)
        return 1;

    Vector3d surfaceColor = 0; //Cor do raio/superficie do objeto intersectado pelo raio 
    //Vector3d phit = r.getOrigin() + r.getDirection() * tnear;
    //Vector3d nhit = phit - hitted->center; //Normal do ponto de intersecao
    //nhit.Normalize(); //Normaliza a normal
    Vector3d phit = rec.phit;
    Vector3d nhit = rec.nhit; //Normal do ponto de intersecao
    //nhit.Normalize(); //Normaliza a normal

    float bias = 1e-4; //Bias para evitar shadow acne

    switch (hitted->material->matType)
    {
        case mat_type::lambertian:{  
            for (unsigned i = 0; i < objects.size(); ++i) 
            { 
                if (objects[i]->material->emissionColor.x > 0)
                { 
                    //Caso o objeto seja uma fonte de luz
                    Vector3d transmission = 1; 
                    Vector3d lightDirection = (objects[i]->center+random_in_unit_sphere()*dynamic_cast<Sphere*>(objects[i])->radius) - phit; 
                    lightDirection.Normalize(); 
                    for (unsigned j = 0; j < objects.size(); ++j)
                    { 
                        if (i != j)
                        { 
                            double t0, t1;
                            Ray n = Ray(phit + nhit * bias, lightDirection);
                            HitRecord lrec;
                            if (objects[j]->intersect(n, t0, t1, lrec) )
                            {  
                                double dist = t0 > t1 ? t1 : t0;
                                transmission = dist/((objects[i]->center)-objects[j]->center).Length(); 
                                break; 
                            } 
                        } 
                    }

                    //Vetor da reflexao da luz com a superficie
                    Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

                    //Fator difuso pelo modelo de iluminacao de Phong
                    Vector3d diffuse = transmission *   
                    hitted->material->surfaceColor * objects[i]->material->emissionColor * 
                    std::max(double(0), nhit.DotProduct(lightDirection));

                    //Fator especular pelo modelo de iluminacao de Phong
                    Vector3d specular = transmission * objects[i]->material->emissionColor * 
                    pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), hitted->material->alpha);
                            
                    //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                    surfaceColor += (diffuse * hitted->material->Kd) + (specular * hitted->material->Ks);
                    //in a weekend method
                    Ray scattered;
                    Vector3d attenuation;
                    if(hitted->material->scatter(r, phit, nhit, attenuation, scattered))
                        surfaceColor += attenuation*trace(scattered, depth+1);

                } 
            }
            break;
        }

        case mat_type::conductor:{
            ScatterInfo sinfo;
            hitted->material->scatter(r, phit, nhit, sinfo);
            surfaceColor = hitted->material->surfaceColor*trace(sinfo.r1, depth + 1); 
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
                            HitRecord lrec;
                            if (objects[j]->intersect(n, t0, t1, lrec))
                            {  
                                double dist = t0 > t1 ? t1 : t0;
                                transmission = dist/((objects[i]->center)-objects[j]->center).Length(); 
                                break; 
                            } 
                        } 
                    }
                    //Vetor da reflexao da luz com a superficie
                    Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

                    //Fator difuso pelo modelo de iluminacao de Phong
                    Vector3d diffuse = transmission * 
                    hitted->material->surfaceColor * objects[i]->material->emissionColor * 
                    std::max(double(0), nhit.DotProduct(lightDirection));
                        
                    //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                    surfaceColor += (diffuse * hitted->material->Kd);
                } 
            }
            break;
        } 

        case mat_type::dielectric:{
            ScatterInfo sinfo;
            hitted->material->scatter(r, phit, nhit, sinfo);
            Vector3d reflectionColor = trace(sinfo.r1, depth + 1); 
            Vector3d refractionColor = trace(sinfo.r2, depth + 1);
            surfaceColor = (reflectionColor * sinfo.kr) + (refractionColor * (1 - sinfo.kr));// * hitted->material->surfaceColor; 
            break; 
        }
    }
    //Delimita o valor da cor entre 0 e 1
    return Vector3d( clamp(surfaceColor.x, 0.0, 1.0), clamp(surfaceColor.y, 0.0, 1.0), 
                     clamp(surfaceColor.z, 0.0, 1.0) ) + hitted->material->emissionColor;
}

void Scene::RenderScene() 
{   
    //Joga raios na cena
    for (unsigned y = 0; y < camera->canvas->height; ++y)
        for (unsigned x = 0; x < camera->canvas->width; ++x)
        {
            Vector3d col = 0; //cor resultante dos raios

            //Para cada pixel, joga 100 raios
            //Cada raio possui um offset aleatorio
            //calculando a media dos cem raios acumulados, conseguimos produzir anti-aliasing
            int rayNum = 5;
            for(int i = 0; i < rayNum; i++){
                col = col + trace( camera->GetRay( x + drand48() , y + drand48() ), 0 );
            }
            double ns = double(rayNum);
            col = Vector3d (col.x/ns, col.y/ns, col.z/ns);
            this->camera->canvas->SetPixel( x, y, col);  //seta a cor do pixel para a coordenada
        }
    
    //Salva o ppm como RenderedCanvas na pasta RenderedImages
    this->camera->canvas->SaveAsPBM("../RenderedImages/", "test2");
}