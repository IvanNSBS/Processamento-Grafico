#ifndef MATERIALH
#define MATERIALH

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include <iostream>
#include <random>
#include <string>

Vector3d random_in_unit_sphere();

// double drand48();

Vector3d reflect( Vector3d &v, const Vector3d& n);

double limit(const double& a, const double &min, const double &max);

Vector3d refract(Vector3d &I, const Vector3d &N, const double &ior);

void fresnel(Vector3d &I, const Vector3d &N, const float &ior, float &kr);

enum mat_type{ lambertian, conductor, dielectric, light };

struct ScatterInfo
{
    Ray r1;//reflection
    Ray r2;//refraction
    Vector3d attenuation;
    float ior;
    float kr;
};

struct Material
{
    public:
    Vector3d surfaceColor, emissionColor = 0;
    Vector3d Kd, Ks; //Fator Difuso, Fator Especular, respectivamente (cor) 
    double ior, alpha;
    mat_type matType;
    std::string matName;
    double lightIntensity;

    Material();
    //Construtor para fonte de luz
    Material(const Vector3d & ec): emissionColor(ec){}

    //Construtor padrao de material 
    Material(mat_type mt, const Vector3d &sc = 0, const Vector3d &d = 0, 
             const Vector3d &s = 0, const double &a = 1, 
             const std::string &name = ""):surfaceColor(sc), Kd(d), Ks(s), alpha(a), matType(mt), matName(name) {}

    //Funcao que calcula o raio resultante da reflexao/refracao
    //virtual, pois cada material tem suas regras de reflexao/refracao
    virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const = 0;

};


class Diffuse : public Material {
    public:
    using Material::Material;
    Diffuse(const Vector3d &sc, const Vector3d &d, 
            const Vector3d &s, const double &a, const std::string &name = ""):Material(mat_type::lambertian, sc, d, s, a, name), albedo(sc){}
    Diffuse(const Vector3d &sc, const std::string &name):Material(mat_type::lambertian, sc, sc, Vector3d(0), 1, name), albedo(sc){}

    //Diffuse eh o material padrao para objetos difusos
    //scatter joga um raio em uma direcao aleatoria
    //baseada no local de impacto do raio incidente
    virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const  {
        Vector3d target = phit + nhit + random_in_unit_sphere();
        sinfo.r1 = Ray(phit, target-phit);
        sinfo.attenuation = albedo;
        return true;
    }
    
    Vector3d albedo = Vector3d(0);
};

class Conductor : public Material{
    public:
        Conductor(const Vector3d& a, double f) : albedo(a), fuzz(f) { if(f < 1) fuzz = f; else fuzz = 1; }
        Conductor(const Vector3d &sc, const Vector3d &d, 
        const Vector3d &s, const double &a, 
        double f, const std::string &name = ""):Material(mat_type::conductor, sc, d, s, a, name), albedo(sc), fuzz(f){if(f < 1) fuzz = f; else fuzz = 1;}

        virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const
        {
            float bias = 1e-4;
            Vector3d rdir = r_in.getDirection();
            Vector3d reflectionDirection = reflect(rdir, nhit); 
            Vector3d reflectionRayOrig = (reflectionDirection.DotProduct(nhit) < 0) ? 
                phit + nhit * bias : 
                phit - nhit * bias; 
            sinfo.r1 = (Ray(reflectionRayOrig, reflectionDirection + random_in_unit_sphere()*fuzz)); 
            sinfo.r2 = Ray(0,0);
            sinfo.attenuation = albedo;
            // sinfo.kr = 1;
            // sinfo.ior = 1;
            return true;
        }

        Vector3d albedo = Vector3d(0);
        double fuzz;
};

class Dielectric : public Material{
    public:
    //Dielectric(double ri) : Material(), ref_idx(ri) {}
    Dielectric(const double &ri = 1.5, 
               const Vector3d &sc = 1,
               const std::string &name = ""):Material(mat_type::dielectric, sc, 1, 0, 1, name), ref_idx(ri){}
        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const  {
            float bias = 1e-4;
            Vector3d dir = r_in.getDirection();

            float kr; 
            fresnel(dir, nhit, ref_idx, kr); 
            bool outside = dir.DotProduct(nhit) < 0;
            Vector3d vbias = nhit * bias;
            if(kr < 1)
            {
                Vector3d refractionDirection = refract(dir, nhit, ref_idx).Normalize(); 
                Vector3d refractionRayOrig = (outside) ? 
                    phit - vbias : 
                    phit + vbias; 
                sinfo.r2 = Ray(refractionRayOrig, refractionDirection);
            }

            Vector3d reflectionDirection = reflect(dir, nhit).Normalize(); 
            Vector3d reflectionRayOrig = (reflectionDirection.DotProduct(nhit) < 0) ? 
                phit - vbias : 
                phit + vbias; 

            sinfo.ior = ref_idx;
            sinfo.kr = kr;
            sinfo.r1 = Ray(reflectionRayOrig, reflectionDirection);
            return true;
        }

        double ref_idx;
};

class Light : public Material{
    public:

        Light(const Vector3d &ec, const double &str = 1,
              const std::string &name = ""):Material(ec){
                  surfaceColor = ec; matType = mat_type::light; matName = name; 
                  lightIntensity = str;}
        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const 
        { return false; }
};

static Material *Chrome = new Conductor(Vector3d(0.25), Vector3d(0.4), Vector3d(0.774597), 76.8, 0.3);
static Material *PolishedChrome = new Conductor(Vector3d(0.25), Vector3d(0.4), Vector3d(0.774597), 76.8, 0.03);

static Material *Red = new Diffuse(Vector3d(0.65, 0.03, 0.03), Vector3d(0.9, 0.03, 0.03), Vector3d(0.2, 0.4, 0.1), 1);
static Material *Green = new Diffuse(Vector3d(0.12, 0.45, 0.15), Vector3d(0.12, 0.75, 0.15), Vector3d(0.4, 0.5, 0), 1);
static Material *White = new Diffuse(Vector3d(0.73, 0.73, 0.73), Vector3d(0.93, 0.93, 0.93), Vector3d(0), 1);
static Material *Di = new Dielectric( 2.437);
static Material *Di2 = new Dielectric( 1.5);

#endif