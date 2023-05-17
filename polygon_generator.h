#ifndef POLYGON_GENERATOR_H_INCLUDED
#define POLYGON_GENERATOR_H_INCLUDED

#include "polygons.h"

#include <cassert>

namespace poly_generate {

    polygon rectangle(double width,
        double height,
        double mass = 1,
        std::string label = "rectangle");

    inline polygon square(
        double width, double mass = 1, std::string label = "square") {
        assert(width > 0);
        return rectangle(width, width, mass, label);
    };

    polygon triangle(double side1,
        double side2,
        double angle,
        double mass = 1,
        std::string label = "triangle");

    polygon regular(double radius,
        uint n_sides,
        double mass = 1,
        std::string label = "regular");

    polygon general(std::vector<vec2d> points,
        double mass = 1,
        std::string label = "general");

}; // namespace poly_generate
#endif
