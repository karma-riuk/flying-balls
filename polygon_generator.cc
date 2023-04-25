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

polygon poly_generate::regular(double radius, uint n_sides, double mass) {
    assert(n_sides > 2);
    std::vector<vec2d> points;
    points.reserve(n_sides);
    double theta = 2 * M_PI / n_sides;

    for (uint i = 0; i < n_sides; i++)
        points.push_back({radius * cos(i * theta), radius * sin(i * theta)});

    double l = vec2d::norm(points[1] - points[0]);

    double sin_, cos_;
    sincos(M_PI / n_sides, &sin_, &cos_);
    double cot = cos_ / sin_;

    double inertia = mass * l * l / 24 * (1 + 3 * cot * cot);
    return polygon{{0, 0}, 0, points, inertia, mass};
}

static double intertia_of_polygon_subtriangle(
    vec2d& centroid, vec2d& p1, vec2d& p2) {
    double base, height;
    if (vec2d::norm(p1 - centroid) > vec2d::norm(p2 - centroid))
        base = vec2d::norm(p1 - centroid);
}

polygon poly_generate::general(std::vector<vec2d>& points, double mass) {
    double intertia = 0;
    return polygon{{0, 0}, 0, points, intertia, mass};
}
