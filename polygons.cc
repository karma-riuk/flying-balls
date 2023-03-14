#include "polygons.h"

#include <iostream>

polygon* polygons = nullptr;
uint n_polygons = 1;

void polygons_init_state() {
    polygons = new polygon[n_polygons];

    polygon poly =
        polygon{{400, 400}, 0, {{-50, -50}, {50, -50}, {50, 50}, {-50, 50}}};
    polygons[0] = poly;
}

void polygon::draw(cairo_t* cr) const {
    polygon& poly = polygons[0];
    vec2d& center = poly.center;
    cairo_set_source_rgb(cr, 1, 1, 1);

    for (auto& point : poly.points)
        cairo_line_to(cr, center.x + point.x, center.y + point.y);
    cairo_line_to(cr, center.x + poly.points[0].x, center.y + poly.points[0].y);
    cairo_stroke(cr);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}
