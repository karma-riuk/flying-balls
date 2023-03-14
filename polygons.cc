#include "polygons.h"

#include "polygon_generator.h"

#include <iostream>

polygon* polygons = nullptr;
uint n_polygons = 3;

void polygons_init_state() {
    polygons = new polygon[n_polygons];

    polygons[0] = poly_generate::triangle(200, 200, 30).set_center({300, 200});
    polygons[1] = poly_generate::square(200).set_center({100, 400});
    polygons[2] = poly_generate::rectangle(200, 100).set_center({400, 400});
}

void polygon::draw(cairo_t* cr) const {
    const vec2d& center = this->center;
    cairo_set_source_rgb(cr, 1, 1, 1);

    for (auto& point : this->points)
        cairo_line_to(cr, center.x + point.x, center.y + point.y);
    cairo_line_to(
        cr, center.x + this->points[0].x, center.y + this->points[0].y);
    cairo_stroke(cr);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}
