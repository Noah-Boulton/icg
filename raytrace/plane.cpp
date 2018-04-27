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

Vec3 plane::get_center(){
    return center;
}

Vec3 plane::get_normal(){
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
