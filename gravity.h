#ifndef GRAVITY_H_INCLUDED
#define GRAVITY_H_INCLUDED

#include "balls.h"
#include "polygons.h"

extern void gravity_constant_field(double x, double y);
extern void gravity_newton_field(double r, double g);

extern vec2d gravity_vector(const ball* b);
extern vec2d gravity_vector(const polygon* b);

extern void gravity_draw_visible_field(cairo_t* cr);

extern void gravity_draw(cairo_t* cr);
extern void gravity_change(double dx, double dy);

extern void gravity_collisions(ball* begin, ball* end);

#endif
