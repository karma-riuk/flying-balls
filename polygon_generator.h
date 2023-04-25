#ifndef POLYGON_GENERATOR_H_INCLUDED
#define POLYGON_GENERATOR_H_INCLUDED

#include "polygons.h"

#include <cassert>

namespace poly_generate {

    polygon rectangle(double width, double height, double mass = 1);

    inline polygon square(double width, double mass = 1) {
        assert(width > 0);
        return rectangle(width, width, mass);
    };

    polygon triangle(double side1, double side2, double angle, double mass = 1);

    polygon regular(double radius, uint n_sides, double mass = 1);

    polygon general(std::vector<vec2d>& points, double mass = 1);

}; // namespace poly_generate
#endif
