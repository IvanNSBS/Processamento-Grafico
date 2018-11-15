#ifndef MATERIALH
#define MATERIALH

#include "../Vector/Vector3.cpp"
#include "Ray.h"
#include <iostream>
#include <random>

Vector3d random_in_unit_sphere();

double drand48();

Vector3d reflect( Vector3d &v, const Vector3d& n);

double limit(const double& a, const double &min, const double &max);

Vector3d refract(Vector3d &I, const Vector3d &N, const double &ior);

void fresnel(Vector3d &I, const Vector3d &N, const float &ior, float &kr);

enum mat_type{ lambertian, conductor, dielectric };

struct ScatterInfo
{
    Ray r1;//reflection
    Ray r2;//refraction
    float ior;
    float kr;
};

struct Material
{
public:
    Vector3d surfaceColor, emissionColor;
    Vector3d Kd, Ks; //Fator Difuso, Fator Especular, respectivamente (cor) 
    double ior;
    double alpha;
    mat_type matType;

    Material();
    //Construtor para fonte de luz
    Material(const Vector3d & ec): emissionColor(ec){}

    //Construtor padrao de material 
    Material(const Vector3d &sc, const Vector3d &d, 
             const Vector3d &s, const double &a, mat_type mt):surfaceColor(sc), Kd(d), Ks(s), alpha(a), matType(mt) {}

    //Funcao que calcula o raio resultante da reflexao/refracao
    //virtual, pois cada material tem suas regras de reflexao/refracao
    virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, Vector3d &attenuation, Ray &scattered) const = 0;
    virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const = 0;

};


class Diffuse : public Material {
public:
    using Material::Material;
    Diffuse(const Vector3d &sc, const Vector3d &d, 
            const Vector3d &s, const double &a):Material(sc, d, s, a, mat_type::lambertian), albedo(sc){}
    Diffuse(const Vector3d &sc):Material(sc, sc, Vector3d(0), 1, mat_type::lambertian), albedo(sc){}

    //Diffuse eh o material padrao para objetos difusos
    //scatter joga um raio em uma direcao aleatoria
    //baseada no local de impacto do raio incidente
    virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const
    {return false;}

    virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const  {
        Vector3d target = phit + nhit + random_in_unit_sphere();
        scattered = Ray(phit, target-phit);
        attenuation = albedo;
        return true;
    }
    
    /*virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const  {
        std::default_random_engine generator; 
        std::uniform_real_distribution<float> distributor(0, 1); 
        float bias = 1e-4;
        Vector3d Nt, Nb; 
        createCoordinateSystem(nhit, Nt, Nb); 
        //float pdf = 1 / (2 * M_PI); 
        float r1 = distributor(generator); 
        float r2 = distributor(generator); 
        Vector3d sample = uniformSampleHemisphere(r1, r2); 
        Vector3d sampleWorld( 
                sample.x * Nb.x + sample.y * nhit.x + sample.z * Nt.x, 
                sample.x * Nb.y + sample.y * nhit.y + sample.z * Nt.y, 
                sample.x * Nb.z + sample.y * nhit.z + sample.z * Nt.z); 
        // don't forget to divide by PDF and multiply by cos(theta)
        scattered = Ray(nhit + sampleWorld * bias, sampleWorld);
        attenuation = albedo; 
        return true;
    }*/
    
    Vector3d albedo = Vector3d(0);
};

class Conductor : public Material{
    public:
        Conductor(const Vector3d& a, double f) : albedo(a), fuzz(f) { if(f < 1) fuzz = f; else fuzz = 1; }
        Conductor(const Vector3d &sc, const Vector3d &d, 
        const Vector3d &s, const double &a, 
        double f):Material(sc, d, s, a, mat_type::conductor), albedo(sc), fuzz(f){if(f < 1) fuzz = f; else fuzz = 1;}

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
            sinfo.kr = 1;
            sinfo.ior = 1;
            return true;
        }


        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const  {
            Vector3d reflected = reflect(r_in.getDirection().Normalize(), nhit);
            scattered = Ray(phit, reflected + random_in_unit_sphere()*fuzz);
            attenuation = 1;
            return ( scattered.getDirection().DotProduct(nhit)  > 0 );
        }
        Vector3d albedo = Vector3d(0);
        double fuzz;
};

class Dielectric : public Material{
    public:
    Dielectric(double ri) : Material(), ref_idx(ri) {}
    Dielectric(const Vector3d &sc, const Vector3d &d, 
               const Vector3d &s, const double &a, 
               double ri):Material(sc, d, s, a, mat_type::dielectric), ref_idx(ri){}
        virtual bool scatter(const Ray &r_in, Vector3d &phit, Vector3d &nhit, Vector3d &attenuation, Ray &scattered) const
        {attenuation = 1; return false;}
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
        /*virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const
        {
            float kr; 
            fresnel(r_in.getDirection(), nhit, ior, kr); 
            bool outside = r_in.getDirection().DotProduct(nhit) < 0; 
            Vector3d bias = nhit*bias; 
                // compute refraction if it is not a case of total internal reflection
            if (kr < 1) { 
                Vector3d refractionDirection = refract(dir, hitNormal, isect.hitObject->ior).normalize(); 
                Vector3d refractionRayOrig = outside ? hitPoint - bias : hitPoint + bias; 
            } 
 
            Vector3d reflectionDirection = reflect(dir, hitNormal).normalize(); 
            Vector3d reflectionRayOrig = outside ? hitPoint + bias : hitPoint - bias; 
            return true;
        }*/

        double ref_idx;
};

class Light : public Material{
    public:
        Light(const Vector3d &ec):Material(ec){surfaceColor = ec;}
        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, Vector3d& attenuation, Ray& scattered) const 
        { attenuation = 1; return false; }
        virtual bool scatter(const Ray& r_in, Vector3d &phit, Vector3d &nhit, ScatterInfo &sinfo) const 
        { return false; }
};


static Material *Brass = new Diffuse( 
                             Vector3d(0.329412, 0.223529, 0.027451), 
                             Vector3d(0.780392, 0.568627, 0.113725),
                             Vector3d(0.992157, 0.941176, 0.807843), 27.8974);
static Material *Bronze = new Diffuse( 
                             Vector3d(0.2125, 0.1275, 0.054), 
                             Vector3d(0.714, 0.4284, 0.18144), 
                             Vector3d(0.393548, 0.271906, 0.166721), 25.6);
static Material *Chrome = new Conductor(Vector3d(0.25), Vector3d(0.4), Vector3d(0.774597), 76.8, 0.3);
static Material *PolishedChrome = new Conductor(Vector3d(0.25), Vector3d(0.4), Vector3d(0.774597), 76.8, 0.03);
static Material *PolishedGold = new Conductor( 
                             Vector3d(0.24725, 0.2245, 0.0645), 
                             Vector3d(0.34615, 0.3143, 0.0903),
                             Vector3d(0.797357, 0.723991, 0.208006), 83.2, 0.25);

static Material *Silver = new Diffuse( Vector3d(0.19225), Vector3d(1), Vector3d(0), 0);

static Material *Lamb = new Diffuse(Vector3d(0.8, 0.3, 0.3), Vector3d(0.8, 0.3, 0.3), Vector3d(0), 1);
static Material *Red = new Diffuse(Vector3d(0.65, 0.03, 0.03), Vector3d(0.65, 0.03, 0.03), Vector3d(0), 1);
static Material *Green = new Diffuse(Vector3d(0.12, 0.45, 0.15), Vector3d(0.12, 0.45, 0.15), Vector3d(0), 1);
static Material *White = new Diffuse(Vector3d(0.73, 0.73, 0.73), Vector3d(0.73, 0.73, 0.73), Vector3d(0), 1);
static Material *BasicConductor = new Conductor(Vector3d(0.8), Vector3d(0.8), Vector3d(0.9), 76.8, 0.03);
static Material *Basic2 = new Conductor( Vector3d(0.8, 0.6, 0.2), Vector3d(0.8,0.6,0.2), Vector3d(0.8,0.6,0.2), 30.8, 0.2);
static Material *Di = new Dielectric( Vector3d(1.0), Vector3d(1.0), Vector3d(0.0), 0, 2.437);
static Material *Di2 = new Dielectric( Vector3d(1.0), Vector3d(1.0), Vector3d(0.0), 0, 1.5);

#endif