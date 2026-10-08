//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)      
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection intersection;
    intersection = intersect(ray);
    if (!intersection.happened)return Vector3f(0,0,0);
    Vector3f N;
    float pdf_light;
    Vector3f wi = normalize(ray.direction);
    Vector3f wo;
    Vector3f p = intersection.coords;
    Material *m = intersection.m;
    if (m->hasEmission())
    {
        return m->getEmission();
    }
    N = normalize(intersection.normal);
    Intersection inter;
    sampleLight(inter,pdf_light);
    
    
    Vector3f x = inter.coords;
    Vector3f ws = normalize(x-p);
    Vector3f NN = inter.normal;
    Vector3f emit = inter.emit;
    Vector3f L_dir = 0 ;
    Vector3f L_indir = 0;
    
    Ray ws_ray = Ray(p, ws);
    Intersection ws_inter = intersect(ws_ray);
    if (ws_inter.happened && ws_inter.distance - (p-x).norm() > -0.01)
    {
        L_dir = emit * m->eval(wo,ws,N) * 
            dotProduct(ws,N) * dotProduct(-ws,NN) / 
                dotProduct(x-p,x-p) / pdf_light;
    }
    
    if (depth > 0)
        {
            float random = get_random_float();
            if (random > RussianRoulette)
            {
                return L_dir;
            }
        }
    
    wi = m->sample(wo,N);
    
    Ray r = Ray(p, wi);
    Intersection r_intersection = intersect(r);
    
    if (r_intersection.happened && r_intersection.m->hasEmission())
    {
        return L_dir;
    }
    else if (r_intersection.happened && !r_intersection.m->hasEmission())
    {
        Vector3f q = r_intersection.coords;
        L_indir = castRay(r,depth+1) * m->eval(wo,wi,N) * 
            dotProduct(wi,N) / m->pdf(wo,wi,N) / RussianRoulette;
    }
    else
    {
        return L_dir;
    }
    return  L_dir + L_indir;
    
    
    
    
}