#pragma once

#ifndef OBJECTH
#define OBJECTH

#include "../Vector/Vector3.cpp"
#include "Ray.h"

//O objeto é uma esfera.

class Object;

Vector3d reflect( Vector3d &v, const Vector3d& n);

Vector3d random_in_unit_sphere();

double drand48();


struct Material
{
public:
    Vector3d surfaceColor, emissionColor;
    Vector3d Kd, Ks; //Diffuse Factor, Specular Factor 
    double alpha;
    double Kr, Kf; //Reflection, refraction, respectively

    //Material(): surfaceColor(0), emissionColor(0){}
    //Material(Vector3d sc):surfaceColor(sc), emissionColor(0) {}

    //for light source
    Material();
    Material(const Vector3d & ec): emissionColor(ec){}

    Material(const Vector3d &sc, const Vector3d &ec):surfaceColor(sc), emissionColor(ec), Kd(1), Ks(1), alpha(1) {}

    Material(const Vector3d &sc, const Vector3d &d, 
             const Vector3d &s, const double &a):surfaceColor(sc), Kd(d), Ks(s), alpha(a) {}

    Material(const Vector3d &sc, const Vector3d &d, 
             const Vector3d &s, const double &a, 
             const double &rr, const double &rf):surfaceColor(sc), Kd(d), Ks(s), alpha(a), Kr(rr), Kf(rf) {}

    virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, Vector3d &attenuation, Ray &scattered) const = 0;

};

class Lambertian : public Material {
public:
    using Material::Material;
    Lambertian(const Vector3d& a) : albedo(a) {}
    Lambertian(const Vector3d &sc, const Vector3d &d, 
        const Vector3d &s, const double &a):Material(sc, d, s, a), albedo(sc){}

    virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const  {
        //std::cout <<"inside albedo\n";
        Vector3d target = phit + nhit + random_in_unit_sphere();
        scattered = Ray(phit, target-phit);
        attenuation = albedo;
        return true;
    }

    Vector3d albedo;
};

class Metal : public Material{
    public:
        Metal(const Vector3d& a, double f) : albedo(a), fuzz(f) { if(f < 1) fuzz = f; else fuzz = 1; }
        Metal(const Vector3d &sc, const Vector3d &d, 
        const Vector3d &s, const double &a, 
        double f):Material(sc, d, s, a), albedo(sc), fuzz(f){if(f < 1) fuzz = f; else fuzz = 1;}

        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const  {
            Vector3d reflected = reflect(r_in.getDirection().Normalize(), nhit);
            scattered = Ray(phit, reflected + random_in_unit_sphere()*fuzz);
            attenuation = albedo;
            return ( scattered.getDirection().DotProduct(nhit)  > 0 );
        }
        Vector3d albedo;
        double fuzz;
};

class Light : public Material{
    public:
        Light(const Vector3d &ec):Material(ec){}
        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const 
        { return false; }
};


static Material *Brass = new Lambertian( 
                             Vector3d(0.329412, 0.223529, 0.027451), 
                             Vector3d(0.780392, 0.568627, 0.113725),
                             Vector3d(0.992157, 0.941176, 0.807843), 27.8974);
static Material *Bronze = new Lambertian( 
                             Vector3d(0.2125, 0.1275, 0.054), 
                             Vector3d(0.714, 0.4284, 0.18144), 
                             Vector3d(0.393548, 0.271906, 0.166721), 25.6);
static Material *Chrome = new Metal(Vector3d(0.25), Vector3d(0.4), Vector3d(0.774597), 76.8, 0.0);
static Material *PolishedGold = new Metal( 
                             Vector3d(0.24725, 0.2245, 0.0645), 
                             Vector3d(0.34615, 0.3143, 0.0903),
                             Vector3d(0.797357, 0.723991, 0.208006), 83.2, 0);

static Material *Silver = new Lambertian( Vector3d(0.19225), Vector3d(1), Vector3d(0), 0);


class Object {
public:
    Material* material;
    //Sphere parameters
    Vector3d center;
    int radius;
    int radius2;

public:
    // Construtor
    Object() : material(nullptr), center(0), radius(0), radius2(0){}
    Object(const Vector3d &c, const int &rad, Material* mat): material(mat), center(c), radius(rad), radius2(rad*rad) {}

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    bool intersect(const Ray& r, double &t0, double &t1);
    
    //Vector3d random_in_unit_sphere();
    Vector3d getPoint() const;
};

#endif