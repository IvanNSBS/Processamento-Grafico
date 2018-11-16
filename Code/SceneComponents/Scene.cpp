#include "Scene.h"
#include "Material.h"
#include <thread>

#define MAX_RAY_DEPTH 10

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
    if(object->material->matType == mat_type::light)
        lights.push_back(object);
}

Vector3d Scene::trace(const Ray& r, int depth) 
{
    if(depth > MAX_RAY_DEPTH)
        return 1;

    Object* hitted = NULL;
    HitRecord rec;
    // Encontra o ponto intersecao do raio com a esfera que o raio atingiu primeiro.
    // ponto é salvo em HitRecord rec.
    for (unsigned i = 0; i < objects.size(); ++i)
    { 
        //hitrecord temporario. Salva o hitrecord da esfera atual para verificar qual é o mais proximo
        HitRecord temp_hrec;
        double t0, t1; 
        if ( objects[i]->intersect(r, t0, t1, temp_hrec) ) 
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
        Vector3d unit_direction = r.getDirection();
        double t = 0.5 * (unit_direction.y + 1.0);
        return (Vector3d(1.0, 1.0, 1.0) * (1.0 - t) ) + (Vector3d(0.5, 0.7, 1.0)*t);
    }

    Vector3d surfaceColor = 0;
    float bias = 1e-4; //Bias para evitar shadow acne

    switch (hitted->material->matType)
    {
        case mat_type::lambertian:{  
            //Computa o global illumination pelo path tracing
            Ray scattered;
            Vector3d attenuation;
            if(hitted->material->scatter(r, rec.phit, rec.nhit, attenuation, scattered))
                surfaceColor += attenuation*trace(scattered, depth+1);
            for (unsigned i = 0; i < lights.size(); ++i) 
            { 
                Vector3d transmission = 1; 
                Vector3d lightDirection = (lights[i]->center + random_in_unit_sphere()*dynamic_cast<Sphere*>(lights[i])->radius) - rec.phit;  
                //Vector3d lightDirection = lights[i]->center - rec.phit;  
                lightDirection.Normalize(); 
                for (unsigned j = 0; j < objects.size(); ++j)
                { 
                    if (objects[j]->material->matType != mat_type::light)
                    { 
                        double t0, t1;
                        Ray n = Ray(rec.phit + rec.nhit * bias, lightDirection);
                        HitRecord lrec;
                        if (objects[j]->intersect(n, t0, t1, lrec) )
                        {  
                            transmission = (lrec.t)/(lights[i]->center - rec.phit).Length(); 
                            transmission = 0.0; 
                            break; 
                        } 
                    } 
                }

                //Vetor da reflexao da luz com a superficie
                Vector3d R = reflect(lightDirection, rec.nhit);

                //Fator difuso pelo modelo de iluminacao de Phong
                Vector3d diffuse = transmission *   
                hitted->material->surfaceColor * lights[i]->material->emissionColor * 
                std::max(double(0), rec.nhit.DotProduct(lightDirection));

                //Fator especular pelo modelo de iluminacao de Phong
                Vector3d specular = transmission * lights[i]->material->emissionColor * 
                pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), hitted->material->alpha);
                            
                //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                surfaceColor += (diffuse * hitted->material->Kd) + (specular * hitted->material->Ks);
            }
            break;
        }

        case mat_type::conductor:{
            ScatterInfo sinfo;
            hitted->material->scatter(r, rec.phit, rec.nhit, sinfo);
            surfaceColor = hitted->material->surfaceColor*trace(sinfo.r1, depth + 1); 
            for (unsigned i = 0; i < lights.size(); i++) 
            { 
                Vector3d transmission = 1; 
                Vector3d lightDirection = (lights[i]->center + random_in_unit_sphere()*dynamic_cast<Sphere*>(lights[i])->radius) - rec.phit;  
                //Vector3d lightDirection = lights[i]->center - rec.phit;  
                lightDirection.Normalize(); 
                for (unsigned j = 0; j < objects.size(); j++)
                { 
                    if (objects[j]->material->matType != mat_type::light)
                    { 
                        double t0, t1;
                        Ray n = Ray(rec.phit + rec.nhit * bias, lightDirection);
                        HitRecord lrec;
                        if (objects[j]->intersect(n, t0, t1, lrec) )
                        {  
                            transmission = (lrec.t)/(lights[i]->center - rec.phit).Length(); 
                            transmission = 0.0;
                            break; 
                        } 
                    } 
                }
                //Vetor da reflexao da luz com a superficie
                Vector3d R = reflect(lightDirection, rec.nhit);

                //Fator difuso pelo modelo de iluminacao de Phong
                Vector3d diffuse = transmission *   
                hitted->material->surfaceColor * lights[i]->material->emissionColor * 
                std::max(double(0), rec.nhit.DotProduct(lightDirection));
                            
                //Aplica o modelo de iluminacao de Phong completo na surfaceColor
                surfaceColor += (diffuse * hitted->material->Kd);
            }
            break;
        } 

        case mat_type::dielectric:{
            ScatterInfo sinfo;
            hitted->material->scatter(r, rec.phit, rec.nhit, sinfo);
            Vector3d reflectionColor = trace(sinfo.r1, depth + 1); 
            Vector3d refractionColor = trace(sinfo.r2, depth + 1);
            surfaceColor = (reflectionColor * sinfo.kr) + (refractionColor * (1 - sinfo.kr));// * hitted->material->surfaceColor; 
            break; 
        }

        case mat_type::light :{
            break;
        }
    }
    //Delimita o valor da cor entre 0 e 1
    return Vector3d( clamp(surfaceColor.x, 0.0, 1.0), clamp(surfaceColor.y, 0.0, 1.0), 
                     clamp(surfaceColor.z, 0.0, 1.0) ) + hitted->material->emissionColor;
}

void Scene::RenderScene(int minX, int minY, int boundX, int boundY) 
{   
    //Joga raios na cena
    //std::cout << "Lights size: " << lights.size() << std::endl;
    for (unsigned y = minY; y < boundY; ++y)
        for (unsigned x = minX; x < boundX; ++x)
        {
            Vector3d col = 0; //cor resultante dos raios

            //Para cada pixel, joga X raios
            //Cada raio possui um offset aleatorio
            //calculando a media dos cem raios acumulados, conseguimos produzir anti-aliasing,
            //além de reduzir o ruido provocado pelo path tracing
            //(quanto mais raios, melhor o AA(super-sampling) e a reduçao de ruido)
            int rayNum = 100;
            for(int i = 0; i < rayNum; i++){
                col = col + trace( camera->GetRay( x + drand48() , y + drand48() ), 0 );
            }
            double ns = double(rayNum);
            col = Vector3d (col.x/ns, col.y/ns, col.z/ns);
            this->camera->canvas->SetPixel( x, y, col);  //seta a cor do pixel para a coordenada
        }
    
    //Salva o ppm como RenderedCanvas na pasta RenderedImages
}