#include "polygon_generator.h"

#include <cassert>
#include <cmath>

#define PI 3.141592653589793238462643323

static double to_rad(double angle_in_deg) {
    static double PI_180 = PI / 180;
    return angle_in_deg * PI_180;
}

polygon poly_generate::rectangle(double width, double height) {
    assert(width > 0);
    assert(height > 0);
    return polygon{
        {0, 0}, 0, {{0, 0}, {width, 0}, {width, height}, {0, height}}};
}

polygon poly_generate::triangle(double side1, double side2, double angle) {
    assert(side1 > 0);
    assert(side2 > 0);
    return polygon{{0, 0},
        0,
        {{0, 0},
            {side1, 0},
            {side2 * std::cos(to_rad(angle)),
                side2 * std::sin(to_rad(angle))}}};
}
