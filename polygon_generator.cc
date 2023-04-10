#include "polygon_generator.h"

#include <cassert>
#include <cmath>

static double to_rad(double angle_in_deg) {
    static double PI_180 = M_PI / 180;
    return angle_in_deg * PI_180;
}

polygon poly_generate::rectangle(double width, double height, double mass) {
    assert(width > 0);
    assert(height > 0);
    static const double one_over_twelve = 1. / 12;
    return polygon{{0, 0},
        0,
        {{-width / 2, -height / 2},
            {-width / 2, height / 2},
            {width / 2, height / 2},
            {width / 2, -height / 2}},
        one_over_twelve * mass * (width * width + height * height),
        mass};
}

polygon poly_generate::triangle(
    double side1, double side2, double angle, double mass) {
    assert(side1 > 0);
    assert(side2 > 0);
    static const double one_over_36 = 1. / 36;
    double base, height;
    base = side1;
    height = side2 * std::sin(to_rad(angle));
    vec2d points[] = {
        {0, 0}, {side1, 0}, {side2 * std::cos(to_rad(angle)), height}};

    vec2d barycenter = {
        (points[0].x + points[1].x + points[2].x) / 3,
        (points[0].y + points[1].y + points[2].y) / 3,
    };

    for (unsigned int i = 0; i < 3; ++i)
        points[i] -= barycenter;
    return polygon{{0, 0},
        0,
        {std::begin(points), std::end(points)},
        one_over_36 * mass * base * std::pow(height, 3),
        mass};
}
