#ifndef POLYGON_GENERATOR_H_INCLUDED
#define POLYGON_GENERATOR_H_INCLUDED

#include "polygons.h"

#include <cassert>

namespace poly_generate {

    polygon
    rectangle(double width, double height, std::string label = "rectangle");

    inline polygon square(double width, std::string label = "square") {
        assert(width > 0);
        return rectangle(width, width, label);
    };

    polygon triangle(
        double side1, double side2, double angle, std::string label = "triangle"
    );

    polygon regular(double radius, uint n_sides, std::string label = "regular");

    polygon general(std::vector<vec2d> points, std::string label = "general");

}; // namespace poly_generate
#endif
