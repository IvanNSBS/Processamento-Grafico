#include "Scene.h"
#include <algorithm>

#include <cstdlib> 
#include <cstdio> 
#include <cmath> 
#include <fstream> 
#include <vector> 
#include <iostream> 
#include <cassert> 

#define INFINITY 1e8
#define M_PI 3.141592653589793

#define MAX_RAY_DEPTH 5 

Vector3d Scene::trace(const Ray& r) 
{
    float tnear = INFINITY; 
    const Object* sphere = NULL;
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
         return Vector3d(5); 

    Vector3d surfaceColor = 0; // color of the ray/surface of the object intersected by the ray 
    Vector3d phit = r.getOrigin() + r.getDirection() * tnear; // point of intersection 
    Vector3d nhit = phit - sphere->center; // normal at the intersection point 
    nhit.Normalize(); // normalize normal direction 
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4; // add some bias to the point from which we will be tracing 
    //bool inside = false; 
    if (r.getDirection().DotProduct(nhit) > 0) 
    {
        nhit = nhit * -1;
        //inside = true;
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
                    //cout << "ok" << endl;
                    if (objects[j]->intersect(n, t0, t1) )
                    {  
                        transmission = 0; 
                        break; 
                    } 
                } 
            }
            surfaceColor = sphere->material->surfaceColor * transmission * 
            std::max(double(0), nhit.DotProduct(lightDirection)) * objects[i]->material->emissionColor; 
        } 
    } 
    return surfaceColor + sphere->material->emissionColor;  
}

void Scene::add(Object* object)
{
    objects.push_back(object);
}

void Scene::render() 
{   
    unsigned width =  640, height = 480; 
    Vector3d *image = new Vector3d[width * height], *pixel = image; 
    float invWidth = 1 / float(width), invHeight = 1 / float(height); 
    float fov = 60, aspectratio = width / float(height); 
    float angle = tan(M_PI * 0.5 * fov / 180.); 
    // Trace rays
    for (unsigned y = 0; y < height; ++y)
    { 
        for (unsigned x = 0; x < width; ++x, ++pixel)
        { 
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio; 
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle; 
            Vector3d raydir(xx, yy, -1); 
            raydir.Normalize();
            Ray ray = Ray(Vector3d(0), raydir);
            *pixel = trace(ray); 
        } 
    }  
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary); 
    ofs << "P6\n" << width << " " << height << "\n255\n"; 
    for (unsigned i = 0; i < width * height; ++i)
    { 
        ofs << (unsigned char)(std::min(double(1), (double)image[i].x) * 255) << 
               (unsigned char)(std::min(double(1), (double)image[i].y) * 255) << 
               (unsigned char)(std::min(double(1), (double)image[i].z) * 255); 
    }
    ofs.close(); 
    delete [] image; 
} 

int main(int argc, char **argv) 
{ 
    //srand48(13); 
    std::vector<Object*> spheres; 
    Scene *scene = new Scene();
    // position, radius, surface color, reflectivity, transparency, emission color
    scene->objects.push_back(new Object(Vector3d( 0.0, -40004, -20), 40000, new Material(Vector3d(0.20, 0.20, 0.20))) ); 
    scene->objects.push_back(new Object(Vector3d( 0.0,      0, -20),     4, new Material(Vector3d(1.00, 0.32, 0.36))) ); 
    scene->objects.push_back(new Object(Vector3d( 5.0,     -1, -15),     2, new Material(Vector3d(0.90, 0.76, 0.46))) ); 
    scene->objects.push_back(new Object(Vector3d( 5.0,      0, -25),     3, new Material(Vector3d(0.65, 0.77, 0.97))) ); 
    scene->objects.push_back(new Object(Vector3d(-5.5,      0, -15),     3, new Material(Vector3d(0.90, 0.90, 0.90))) ); 
    // light
    scene->objects.push_back(new Object(Vector3d( 0.0,     30, -30),     0, new Material(Vector3d(0.00, 0.00, 0.00), Vector3d(2.5))) ); 
    scene->render(); 
    
    return 0; 
} 