#include "scene.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <thread>

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
                    float tmin, tmax;
                    int rpPixel, depth;
                    stream >> tmin >> tmax >> rpPixel >> depth;
                    options = SceneOptions(tmin, tmax, depth, rpPixel);
                }
                else if(next == "Camera"){
                    float px, py, pz;
                    float tx, ty, tz;
                    float ux, uy, uz;
                    float fov, near;
                    stream >> px >> py >> pz;
                    stream >> tx >> ty >> tz;
                    stream >> ux >> uy >> uz;
                    stream >> fov >> near;
                    cam = new Camera(im, vec3(px, py, pz), vec3(tx, ty, tz), vec3(ux, uy, uz), fov, near);
                    scene->camera = cam;
                    scene->options = options;
                }
                else if(next == "Diffuse"){
                    float r, g, b;
                    float kdx, kdy, kdz;
                    float ksx, ksy, ksz;
                    float alpha;
                    string name;
                    stream >> r >> g >> b >> kdx >> kdy >> kdz >> ksx >> ksy >> ksz >> alpha >> name;
                    mlist.push_back( new Diffuse(vec3(r,g,b), vec3(kdx,kdy,kdz), vec3(ksx,ksy,ksz), alpha, name));
                }
                else if(next == "Conductor"){
                    float r, g, b;
                    float kdx, kdy, kdz;
                    float ksx, ksy, ksz;
                    float alpha;
                    float fuzz;
                    string name;
                    stream >> r >> g >> b >> kdx >> kdy >> kdz >> ksx >> ksy >> ksz >> alpha >> fuzz >> name;
                    mlist.push_back( new Conductor(vec3(r,g,b), vec3(kdx,kdy,kdz), vec3(ksx,ksy,ksz), alpha, fuzz, name));
                }
                else if(next == "Dielectric"){
                    float ior;
                    float r, g, b;
                    float fuzz;
                    string name;
                    stream >> ior >> r >> g >> b >> name;
                    mlist.push_back( new Dielectric(ior, vec3(r,g,b), name));
                }
                else if(next == "Light"){
                    float r, g, b;
                    float intensity;
                    string name;
                    stream >> r >> g >> b >> intensity >> name;
                    mlist.push_back( new Light(vec3(r,g,b), intensity, name));
                }
                else if(next == "Sphere"){
                    float cx, cy, cz, radius;
                    string name;
                    stream >> cx >> cy >> cz >> radius >> name;
                    scene->add(new Sphere(vec3(cx, cy, cz), radius, findMat(mlist, name)));
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
    // 80043298769
    string name1 = "White";
    string name2 = "WhiteFloor";
    string name3 = "Red";
    string name4 = "Green";
    string blue = "Blue";
    string light = "LSource";
    // scene->add( new Mesh( vec3(0.0, -7.7, -31.0), vec3(6,6,6), vec3(34.6f, -50.1f, -4.56f), findMat(mlist, name ), "./monkey_smooth.obj"));
    // scene->add( new Mesh( vec3(0.0, -0.7, -31.0), vec3(6,6,6), vec3(0), findMat(mlist, name3 ), "./monkey_smooth.obj"));
    // scene->add( new Mesh( vec3(0.6, 0.3, -0.2), vec3(0.3, 0.3, 0.3), vec3(34.6f, -50.1f, -4.56f), findMat(mlist, name3 ), "./monkey_smooth.obj"));
    // scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name1 ), "./back.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name1 ), "../objs/ceiling.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name1 ), "../objs/back.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name2 ), "../objs/ground.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name4 ), "../objs/right_wall.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name3 ), "../objs/left_wall.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, name1 ), "../objs/box_flat.obj"));
    scene->add( new Mesh( vec3(0, 0, 0), vec3(1,1,1), vec3(0), findMat(mlist, light ), "../objs/light.obj"));
    return;
}

int main(int argc, char **argv) 
{ 
    //Para compilar: g++ -o r.exe -std=c++14 -pthread main.cpp
    Scene *scene = new Scene();
    std::string filename;
    readFile(scene, "../scenes/cornellbox_objs.txt", filename);

    clock_t begin = clock();
    int tnum = 16;
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
    float elapsed_secs = float(end - begin) / CLOCKS_PER_SEC;
    std::cout << "RenderTime with " << tnum << " threads = "<< elapsed_secs << "s" << std::endl;

    scene->camera->canvas->SaveAsPBM("../images_new/", filename);

    return 0; 
}   