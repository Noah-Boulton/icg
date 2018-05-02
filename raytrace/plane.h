#ifndef PLANE_H
#define PLANE_H
#include "shape.h"

class plane: public shape{
protected:
    Vec3 center;
    Vec3 normal;
    Colour colour;
    Colour spec;
    Vec3 ambientCol;
    float specCof;
    float reflective;
    bool mirror;
    bool glass;

public:
    plane();
    plane(Vec3 center, Vec3 normal, Colour colour, Colour spec, Vec3 ambientCol, float specCof, float reflective, bool m, bool g);
    virtual Vec3 get_center();
    virtual Vec3 get_normal(Vec3 hitPos);
    virtual Colour get_colour();
    virtual Colour get_spec();
    virtual Vec3 get_ambientCol();
    virtual float get_specCof();
    virtual float get_reflective();
    virtual float intersect(Vec3 origin, Vec3 ray);
    virtual bool get_mirror();
    virtual bool get_glass();
};

#endif // PLANE_H
