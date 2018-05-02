#ifndef SHAPE_H
#define SHAPE_H

#include "OpenGP/Image/Image.h"

using namespace OpenGP;
using Colour = Vec3;
class shape{
public:
    shape();
    virtual float intersect(Vec3 origin, Vec3 ray) = 0;
    virtual Vec3 get_center() = 0;
    virtual Vec3 get_normal(Vec3 hitPos) = 0;
    virtual Colour get_colour() = 0;
    virtual Colour get_spec() = 0;
    virtual Vec3 get_ambientCol() = 0;
    virtual float get_specCof() = 0;
    virtual float get_reflective() = 0;
};

#endif // SHAPE_H
