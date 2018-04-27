#ifndef PLANE_H
#define PLANE_H
#include "shape.h"

class plane: public shape
{
protected:
    Vec3 center;
    Vec3 normal;
    Colour colour;
    Colour spec;
    Vec3 ambientCol;
    float specCof;
    float reflective;
public:
    plane();
    plane(Vec3 center, Vec3 normal, Colour colour, Colour spec, Vec3 ambientCol, float specCof, float reflective);
    Vec3 get_center();
    Vec3 get_normal();
    Colour get_colour();
    Colour get_spec();
    Vec3 get_ambientCol();
    float get_specCof();
    float get_reflective();
};

#endif // PLANE_H
