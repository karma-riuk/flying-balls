#ifndef COLLISIONS_H_INCLUDED
#define COLLISIONS_H_INCLUDED

#include "polygons.h"
#include "vec2d.h"

struct collision {
    bool collides;
    vec2d n; // minimum push vector
    vec2d impact_point;
};

extern collision collides(polygon& p, polygon& q);

#endif
