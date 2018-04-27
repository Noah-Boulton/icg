#ifndef SPHERE_H
#define SPHERE_H
#include "shape.h"

class sphere: public shape
{
protected:
    Vec3 center;
    Vec3 ambientCol;
    Colour colour;
    Colour spec;
    float specCof;
    float reflective;
    float radius;
    bool mirror;
    bool glass;
public:
    sphere();
    sphere(float r, bool m, bool g, Vec3 c, Colour col, Colour s, Vec3 a, float sCof, float ref);
    Vec3 get_center();
    Vec3 get_ambientCol();
    float get_radius();
    float get_specCof();
    float get_reflective();
    bool get_mirror();
    bool get_glass();
    Colour get_colour();
    Colour get_spec();
};

#endif // SPHERE_H
