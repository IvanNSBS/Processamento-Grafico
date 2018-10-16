#include "Scene.h"
 
bool Scene::intersect(const Ray& r, ObjectIntersection* info = nullptr) const {

}

Vector3d Scene::trace(const Ray& r, int recursiveLevel = 0) const {

}

void Scene::add(Object* object) {
    objects.push_back(object);
}