#ifndef POLYGONS_H_INCLUDED
#define POLYGONS_H_INCLUDED

#include "vec2d.h"

#include <gtk/gtk.h>
#include <vector>

class polygon {
  public:
    vec2d center;
    double angle;
    std::vector<vec2d> points;

    void draw(cairo_t* cr) const;

    polygon& set_center(vec2d c) {
        center = c;
        return *this;
    }

    polygon& set_angle(double a) {
        angle = a;
        return *this;
    }

    polygon& translate(vec2d delta) {
        center += delta;
        return *this;
    }

    polygon& rotate(double delta) {
        angle += delta;
        return *this;
    }
};

extern polygon* polygons;
extern uint n_polygons;

extern void polygons_init_state();
extern void polygons_update_state();
extern void polygons_draw(cairo_t* cr);

#endif
