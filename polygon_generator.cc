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

static double area_of_triangle(vec2d& a, vec2d& b, vec2d& c) {
    return std::abs(vec2d::cross(c - a, b - a)) / 2;
}

static double area_of_poly(std::vector<vec2d>& points, vec2d& centroid) {
    double area = 0;
    for (int i = 0; i < points.size(); ++i)
        area += area_of_triangle(
            centroid, points[i], points[(i + 1) % points.size()]);
    return area;
}

static double intertia_of_polygon_subtriangle(double total_mass,
    double total_area,
    vec2d& centroid,
    vec2d& p1,
    vec2d& p2) {
    double partial_area = area_of_triangle(centroid, p1, p2);
    std::cout << "partial area: " << partial_area << std::endl;
    double partial_mass = total_mass * partial_area / total_area;

    vec2d CA = p1 - centroid;
    vec2d AB = p2 - p1;

    return partial_mass / 2 *
           (vec2d::norm2(AB) / 3 + vec2d::dot(AB, CA) + vec2d::norm2(CA));
}

static vec2d centroid(std::vector<vec2d>& points) {
    double x = 0, y = 0;
    for (auto& p : points)
        x += p.x, y += p.y;

    return vec2d{x, y} / points.size();
}

polygon poly_generate::general(std::vector<vec2d> points, double mass) {
    double intertia = 0;
    vec2d c = centroid(points);
    double area = area_of_poly(points, c);
    std::cout << "area: " << area << std::endl;

    std::cout << "centroid: " << c << std::endl;
    for (int i = 0; i < points.size(); ++i)
        intertia += intertia_of_polygon_subtriangle(
            mass, area, c, points[i], points[(i + 1) % points.size()]);

    for (auto& p : points) // set the center of the polygon to it's centroid
        p -= c;

    return polygon{{0, 0}, 0, points, intertia, mass};
}
