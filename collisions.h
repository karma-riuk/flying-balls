#ifndef COLLISIONS_H_INCLUDED
#define COLLISIONS_H_INCLUDED

#include "polygons.h"
#include "vec2d.h"

struct collision {
    bool collides = false;
    vec2d n;
    vec2d impact_point;
    vec2d overlap; // minimum push vector
};

extern collision collides(polygon& p, polygon& q);

#endif
