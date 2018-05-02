#include "sphere.h"
#include "OpenGP/Image/Image.h"

using namespace OpenGP;
using Colour = Vec3;

sphere::sphere(){
}

sphere::sphere(float r, bool m, bool g, Vec3 c, Colour col, Colour s, Vec3 a, float sCof, float ref)
{
    radius = r;
    mirror = m;
    glass = g;
    center = c;
    colour = col;
    spec = s;
    ambientCol = a;
    specCof = sCof;
    reflective = ref;
}

float sphere::intersect(Vec3 origin, Vec3 ray){
    float time = 0.0f;
    //ray-sphere intersection
    Vec3 originSubC = origin - center; // camera - sphere center
    // calculate the discriminet to see if the ray hits the sphere
    float disc = std::powf(ray.dot(originSubC), 2) - originSubC.dot(originSubC) + radius*radius;

    if(disc >= 0){
        // Hits the sphere
        time = -(ray.dot(originSubC)) - std::sqrtf(disc);
    }
    return time;
}

Vec3 sphere::get_center(){
    return center;
}

Vec3 sphere::get_ambientCol(){
    return ambientCol;
}


float sphere::get_specCof(){
    return specCof;
}

float sphere::get_reflective(){
    return reflective;
}

//bool sphere::get_mirror(){
//    return mirror;
//}

//bool sphere::get_glass(){
//    return glass;
//}

Colour sphere::get_colour(){
    return colour;
}

Colour sphere::get_spec(){
    return spec;
}

Vec3 sphere::get_normal(Vec3 hitPos){
    return (hitPos - center)/radius;
}
