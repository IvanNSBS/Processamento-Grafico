#include "Scene.h"
#include <thread>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

Material* findMat(vector<Material*> &mlist, string &name)
{
    for(int i = 0; i < mlist.size(); i++)
    {
        if(mlist[i]->matName == name)
            return mlist[i];
    }
    return nullptr;
}

void readFile(Scene *scene, const std::string &filename, std::string &newfile){
    SceneOptions options;
    Image *im;
    Camera *cam;
    vector<Material*> mlist;

    ifstream file;
    file.open(filename);
    if(!file){
        cout << "Arquivo nao encontrado\n";
        return;
    }

    string str;
    getline(file, str);

    while(file)
    {
        if(str.length() > 0 && str.at(0) != '#')
        {
            std::stringstream stream(str);
            while(stream)
            {
                string next;
                stream >> next;
                if(next == "Canvas"){
                    int w,h;
                    stream >> w >> h;
                    im = new Image(w, h);
                }
                else if(next == "Options"){
                    double tmin, tmax;
                    int rpPixel, depth;
                    stream >> tmin >> tmax >> rpPixel >> depth;
                    options = SceneOptions(tmin, tmax, depth, rpPixel);
                }
                else if(next == "Camera"){
                    double px, py, pz;
                    double tx, ty, tz;
                    double ux, uy, uz;
                    double fov, near;
                    stream >> px >> py >> pz;
                    stream >> tx >> ty >> tz;
                    stream >> ux >> uy >> uz;
                    stream >> fov >> near;
                    cam = new Camera(im, Vector3d(px, py, pz), Vector3d(tx, ty, tz), Vector3d(ux, uy, uz), fov, near);
                    scene->camera = cam;
                    scene->options = options;
                }
                else if(next == "Diffuse"){
                    double r, g, b;
                    double kdx, kdy, kdz;
                    double ksx, ksy, ksz;
                    double alpha;
                    string name;
                    stream >> r >> g >> b >> kdx >> kdy >> kdz >> ksx >> ksy >> ksz >> alpha >> name;
                    mlist.push_back( new Diffuse(Vector3d(r,g,b), Vector3d(kdx,kdy,kdz), Vector3d(ksx,ksy,ksz), alpha, name));
                }
                else if(next == "Conductor"){
                    double r, g, b;
                    double kdx, kdy, kdz;
                    double ksx, ksy, ksz;
                    double alpha;
                    float fuzz;
                    string name;
                    stream >> r >> g >> b >> kdx >> kdy >> kdz >> ksx >> ksy >> ksz >> alpha >> fuzz >> name;
                    mlist.push_back( new Conductor(Vector3d(r,g,b), Vector3d(kdx,kdy,kdz), Vector3d(ksx,ksy,ksz), alpha, fuzz, name));
                }
                else if(next == "Dielectric"){
                    double ior;
                    double r, g, b;
                    float fuzz;
                    string name;
                    stream >> ior >> r >> g >> b >> name;
                    mlist.push_back( new Dielectric(ior, Vector3d(r,g,b), name));
                }
                else if(next == "Light"){
                    double r, g, b;
                    double intensity;
                    string name;
                    stream >> r >> g >> b >> intensity >> name;
                    mlist.push_back( new Light(Vector3d(r,g,b), intensity, name));
                }
                else if(next == "Sphere"){
                    double cx, cy, cz, radius;
                    string name;
                    stream >> cx >> cy >> cz >> radius >> name;
                    scene->add(new Sphere(Vector3d(cx, cy, cz), radius, findMat(mlist, name)));
                }
                else if(next == "Filename"){
                    string fname;
                    stream >> fname;
                    newfile = fname;
                }
            }
        }
        getline(file, str);
    }
    return;
}

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r -std=c++14 -pthread Scene.cpp Object.cpp Ray.cpp Camera.cpp Image.cpp Material.cpp main.cpp
    Scene *scene = new Scene();
    std::string filename;
    readFile(scene, "cornellbox.txt", filename);


    clock_t begin = clock();
    int tnum = 80;
    std::thread *tlist = new std::thread[tnum];
    for(int i = 0; i < tnum; i++)
    {
        tlist[i] = std::thread( &Scene::RenderScene, scene, 0, scene->camera->canvas->height/tnum*i, 
                                scene->camera->canvas->width, scene->camera->canvas->height/tnum*(i+1) );
    }
    for(int i = 0; i < tnum; i++)
    {
        tlist[i].join();
    } 

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "RenderTime with " << tnum << " threads = "<< elapsed_secs << "s" << std::endl;

    scene->camera->canvas->SaveAsPBM("../RenderedImages/", filename);

    return 0; 
}   