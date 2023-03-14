#ifndef POLYGON_GENERATOR_H_INCLUDED
#define POLYGON_GENERATOR_H_INCLUDED

#include "polygons.h"

#include <cassert>

namespace poly_generate {

    polygon rectangle(double width, double height);

    inline polygon square(double width) {
        assert(width > 0);
        return rectangle(width, width);
    };

    polygon triangle(double side1, double side2, double angle);

}; // namespace poly_generate
#endif
