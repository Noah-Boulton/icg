#ifndef SPHERE_H
#define SPHERE_H
#include "shape.h"

class sphere: public shape{
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
    virtual Vec3 get_center();
    virtual Vec3 get_ambientCol();
    virtual float get_specCof();
    virtual float get_reflective();
    virtual Colour get_colour();
    virtual Colour get_spec();
    virtual float intersect(Vec3 origin, Vec3 ray);
    virtual Vec3 get_normal(Vec3 hitPos);
    //    bool get_mirror();
    //    bool get_glass();
};

#endif // SPHERE_H
