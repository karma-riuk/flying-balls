#include "polygons.h"

#include "cairo.h"
#include "game.h"
#include "matrix.h"
#include "polygon_generator.h"

#include <iostream>

polygon* polygons = nullptr;
uint n_polygons = 1;

void polygons_init_state() {
    polygons = new polygon[n_polygons];

    std::cout << "width" << width << std::endl;
    // polygons[0] = poly_generate::square(200)
    //                   .set_center({400, 600})
    //                   .set_angle(37)
    //                   .set_speed({1, -1});
    polygons[0] = poly_generate::triangle(150, 150, 30)
                      .set_center({400, 300})
                      .set_angle(45)
                      .set_speed({3, 2});
    // polygons[2] = poly_generate::rectangle(200, 100).set_center({600, 600});
}

static double to_rad(double angle_in_deg) {
    static double PI_180 = M_PI / 180;
    return angle_in_deg * PI_180;
}

static void check_border_collision(polygon* p) {
    for (auto& point : p->global_points) {
        if (point.x <= 0 || point.x >= width) {
            p->speed.x *= -1;
            break;
        }
        if (point.y <= 0 || point.y >= height) {
            p->speed.y *= -1;
            break;
        }
    }
}

void polygons_update_state() {
    for (polygon* p = polygons; p != polygons + n_polygons; ++p) {
        p->rotate(1);
        check_border_collision(p);
        p->translate(p->speed);
    }
}

void polygon::update_global_points() {
    double cos_theta = std::cos(to_rad(this->angle));
    double sin_theta = std::sin(to_rad(this->angle));

    matrix rotation = matrix{{cos_theta, sin_theta}, {-sin_theta, cos_theta}};

    for (uint i = 0; i < this->points.size(); ++i)
        this->global_points[i] = rotation * this->points[i] + this->center;
}

void polygon::draw_bounding_rect(cairo_t* cr) const {
    cairo_set_source_rgb(cr, .7, .7, .7);
    double dashes[] = {5, 10};
    cairo_set_dash(cr, dashes, 2, 0);

    auto bb = this->get_bounding_box();
    vec2d min = bb.first, max = bb.second;

    cairo_line_to(cr, min.x, min.y);
    cairo_line_to(cr, min.x, max.y);
    cairo_line_to(cr, max.x, max.y);
    cairo_line_to(cr, max.x, min.y);
    cairo_line_to(cr, min.x, min.y);
    cairo_stroke(cr);
    cairo_set_dash(cr, 0, 0, 0); // disable dashes
}

void polygon::draw(cairo_t* cr) const {
    this->draw_bounding_rect(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);

    for (auto& point : this->global_points)
        cairo_line_to(cr, point.x, point.y);

    cairo_line_to(cr, this->global_points[0].x, this->global_points[0].y);
    cairo_stroke(cr);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}
