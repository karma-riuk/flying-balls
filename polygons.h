#ifndef POLYGONS_H_INCLUDED
#define POLYGONS_H_INCLUDED

// extern ball spaceship;

#include "vec2d.h"

#include <gtk/gtk.h>
#include <vector>

class polygon {
  public:
    vec2d center;
    double angle;
    std::vector<vec2d> points;

    void draw(cairo_t* cr) const;
};

extern polygon* polygons;
extern uint n_polygons;

extern void polygons_init_state();
extern void polygons_update_state();
// extern void polygon_control (double dx, double dy);
extern void polygons_draw(cairo_t* cr);

#endif
