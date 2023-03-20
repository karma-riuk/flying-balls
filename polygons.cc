#include "polygons.h"

#include "matrix.h"
#include "polygon_generator.h"

#include <cmath>
#include <iostream>

polygon* polygons = nullptr;
uint n_polygons = 3;

void polygons_init_state() {
    polygons = new polygon[n_polygons];

    polygons[0] = poly_generate::triangle(150, 150, 30)
                      .set_center({400, 300})
                      .set_angle(90);
    polygons[1] = poly_generate::square(200).set_center({200, 600});
    polygons[2] = poly_generate::rectangle(200, 100).set_center({600, 600});
}

static double to_rad(double angle_in_deg) {
    static double PI_180 = M_PI / 180;
    return angle_in_deg * PI_180;
}

void polygons_update_state() {
    static double angle = 0;
    for (polygon* p = polygons; p != polygons + n_polygons; ++p) {
        p->rotate(1);
        p->translate({2 * std::cos(angle), 0});
    }
    angle -= .03;
}

void polygon::draw(cairo_t* cr) const {
    const vec2d& center = this->center;
    cairo_set_source_rgb(cr, 1, 1, 1);

    double cos_theta = std::cos(to_rad(this->angle));
    double sin_theta = std::sin(to_rad(this->angle));

    matrix rotation = matrix{{cos_theta, sin_theta}, {-sin_theta, cos_theta}};

    vec2d final_point;
    for (auto& point : this->points) {
        final_point = rotation * point;
        cairo_line_to(cr, center.x + final_point.x, center.y + final_point.y);
    }
    final_point = rotation * this->points[0];
    cairo_line_to(cr, center.x + final_point.x, center.y + final_point.y);
    cairo_stroke(cr);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}
