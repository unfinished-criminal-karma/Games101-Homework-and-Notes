#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    if (splitMethod == SplitMethod::SAH)
    {
        root = recursiveBuildSAH(primitives);
    }
    else
    {
        root = recursiveBuild(primitives);
    }

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Intersection intersection;
    std::array<int,3> dirIsNeg = {ray.direction.x < 0, ray.direction.y < 0,ray.direction.z < 0};
    if (!node)
    {
        return intersection;
    }
    if (!node->bounds.IntersectP(ray,ray.direction_inv,dirIsNeg))return intersection;
    if (!(node->left && node->right))
    {
        return node->object->getIntersection(ray);
    }
    else
    {
        auto hit1 = getIntersection(node->left,ray);
        auto hit2 = getIntersection(node->right,ray);
        return hit1.distance < hit2.distance ? hit1 : hit2;
        
    }
}
BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();
    if (objects.size() == 1)
    {
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    Bounds3 bounds;
    for (size_t i = 0; i < objects.size(); i++)//获取全包围盒表面积
    {
        bounds = Union(bounds, objects[i]->getBounds());
    }
    Bounds3 centroBounds;
    for (size_t i = 0; i < objects.size(); i++)
    {
        centroBounds = Union(centroBounds, objects[i]->getBounds().Centroid());
    }
    int dim = centroBounds.maxExtent();
    
    switch (dim)
    {
    case 0:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
        {
            return f1->getBounds().Centroid().x < f2->getBounds().Centroid().x;
        });
        break;
    case 1:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
       {
           return f1->getBounds().Centroid().y < f2->getBounds().Centroid().y;
       });
        break;
    case 2:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2)
           {
               return f1->getBounds().Centroid().z < f2->getBounds().Centroid().z;
           });
        break;
    }
    const int n = (int)objects.size();
    
    std::vector<Bounds3> rightbounds(n);
    rightbounds[n-1] = objects[n-1]->getBounds();
    for (int k = n - 2; k >= 0 ; k--)
    {
        rightbounds[k] = Union(objects[k]->getBounds(), rightbounds[k+1]);
    }
    
    const float Ctrav = 0.125;
    const float Ciect = 1.0f;
    const float parentArea = bounds.SurfaceArea();
    int bestSplit = -1;
    float bestCost = std::numeric_limits<float>::max();
    
    if (parentArea > 0.0)
    {
        Bounds3 prefixBound;
        for (int i = 1; i < n; i++)
        {
            prefixBound = Union(prefixBound, objects[i-1]->getBounds());
            const int Nl = i;
            const int Nr = n - i;
            
            float cost = Ctrav + prefixBound.SurfaceArea()/parentArea * Nl *Ciect 
            + rightbounds[i].SurfaceArea()/parentArea * Nr * Ciect;
            if (cost < bestCost)
            {
                bestCost = cost;
                bestSplit = i;
            }
        }
    }
    if (bestSplit < 0)
    {
        bestSplit = n/2;
    }
    std::vector<Object*> leftobjects(objects.begin(), objects.begin() + bestSplit);
    std::vector<Object*> rightobjects(objects.begin() + bestSplit, objects.end());
    
    node->left = recursiveBuildSAH(leftobjects);
    node->right = recursiveBuildSAH(rightobjects);
    node->bounds = Union(node->left->bounds, node->right->bounds);
    return node;
}