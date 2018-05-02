#include "plane.h"
#include "OpenGP/Image/Image.h"

using namespace OpenGP;
using Colour = Vec3;

plane::plane(){
}

plane::plane(Vec3 c, Vec3 n, Colour col, Colour s, Vec3 a, float sCof, float r){
    center = c;
    normal = n;
    colour = col;
    spec = s;
    ambientCol = a;
    specCof = sCof;
    reflective = r;
}

float plane::intersect(Vec3 origin, Vec3 ray){
    float time = -1.0f;
    if((center - origin).dot(normal) != 0){
        time = ((center - origin).dot(normal))/(ray.dot(normal));
    }
    return time;
}

Vec3 plane::get_center(){
    return center;
}

Vec3 plane::get_normal(Vec3 hitPos){
    return normal;
}

Colour plane::get_colour(){
    return colour;
}

Colour plane::get_spec(){
    return spec;
}

Vec3 plane::get_ambientCol(){
    return ambientCol;
}

float plane::get_specCof(){
    return specCof;
}

float plane::get_reflective(){
    return reflective;
}
