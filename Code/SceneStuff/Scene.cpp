#include "Scene.h"
#include <algorithm>
#include <stdlib.h> 
#include <cmath> 

#define MAX_RAY_DEPTH 25 

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

Vector3d Scene::trace(const Ray& r, int depth) 
{
    float tnear = INFINITY; 
    Object* sphere = NULL;
    // Encontra a intersecao do ray com a esfera mais 'anterior' ao raio(que o raio atingiu primeiro)
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

    // if there's no intersection return black or background color
    if (!sphere)
         return Vector3d(0.498, 0.69, 1); 

    Vector3d surfaceColor = 0; // color of the ray/surface of the object intersected by the ray 
    Vector3d phit = r.getOrigin() + r.getDirection() * tnear; // point of intersection 
    Vector3d nhit = phit - sphere->center; // normal at the intersection point 
    nhit.Normalize(); // normalize normal direction 
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.

    //difuso bounce hit
    if(sphere){
        Ray scattered;
        Vector3d attenuation;
        if(depth < MAX_RAY_DEPTH && sphere->material->scatter(r, phit, nhit, attenuation, scattered))
            surfaceColor = attenuation*(trace(scattered, depth+1));
    }

    float bias = 1e-4; // add some bias to the point from which we will be tracing 
    bool inside = false; 
    if (r.getDirection().DotProduct(nhit) > 0) 
    {
        nhit = nhit * -1;
        inside = true;
    }

    //Eh difuso
    for (unsigned i = 0; i < objects.size(); ++i) 
    { 
        if (objects[i]->material->emissionColor.x > 0)
        { 
            // this is a light
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
                        transmission = sphere->material->surfaceColor * 0.85; 
                        break; 
                    } 
                } 
            }
            //vetor da reflexao da luz com a superficie
            Vector3d R = (nhit * (2*lightDirection.DotProduct(nhit))) - lightDirection;

            Vector3d diffuse = transmission * 
            sphere->material->surfaceColor * objects[i]->material->emissionColor * 
            std::max(double(0), nhit.DotProduct(lightDirection));

            Vector3d specular = transmission * objects[i]->material->emissionColor * 
            pow(std::max(double(0), R.DotProduct(r.getDirection()*-1)), sphere->material->alpha);
             
            surfaceColor = surfaceColor + (diffuse * sphere->material->Kd) + (specular * sphere->material->Ks);
        } 
    } 
    return Vector3d( (clamp(surfaceColor.x, 0.0, 1.0)), (clamp(surfaceColor.y, 0.0, 1.0)), 
    (clamp(surfaceColor.z, 0.0, 1.0)) );  
}

void Scene::add(Object* object)
{
    objects.push_back(object);
}

void Scene::RenderScene() 
{   
    // Trace rays
    for (unsigned y = 0; y < camera->canvas->height; ++y)
        for (unsigned x = 0; x < camera->canvas->width; ++x)
        {
            Vector3d col = 0;
            for(int i = 0; i < 100; i++)
                col = col + trace( camera->GetRay( x + drand48() , y + drand48() ), 0 );
            double ns = double(100);
            col = Vector3d (col.x/ns, col.y/ns, col.z/ns);
            this->camera->canvas->SetPixel( x, y, col ); 
        }
    
    //Salva o ppm como RenderedCanvas na pasta RenderedImages
    this->camera->canvas->SaveAsPBM("../RenderedImages/", "RenderedCanvas3");
} 

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp
    std::vector<Object*> spheres; 
    Image *im = new Image(320, 240);
    
    //camera nao bugada para testes
    Camera *cam = new Camera( im, Vector3d(-1, 0, 0), Vector3d(0, 0, 0.1), Vector3d(0,1,0), 50, 1 );

    //camera bugada
    //Camera *cam = new Camera( im, Vector3d(0, 15, -16.7), Vector3d(0, 5, 1), Vector3d(0,1,0), 60, 1 );
    Scene *scene = new Scene(cam);

    // position, radius, surface color, reflectivity, transparency, emission color
    scene->add(new Object(Vector3d( 0.0, -10004, -20), 10000, Silver)); 
    scene->add(new Object(Vector3d( 0.0,      0, -20),     4, Chrome)); 
    scene->add(new Object(Vector3d( 5.0,     -1, -15),     2, Brass)); 
    scene->add(new Object(Vector3d( 5.0,      0, -25),     3, Brass)); 
    scene->add(new Object(Vector3d(-5.5,      0, -15),     3, Chrome));
    scene->add(new Object(Vector3d(-4.5,      -1.5, -11),     1, Silver)); 
    // light
    scene->add(new Object(Vector3d( 0.0,     70, -30),     0, new Light(Vector3d(1)))); 
    scene->RenderScene(); 
    
    return 0; 
}   