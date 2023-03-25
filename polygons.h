#ifndef POLYGONS_H_INCLUDED
#define POLYGONS_H_INCLUDED

#include "vec2d.h"

#include <cmath>
#include <gtk/gtk.h>
#include <utility>
#include <vector>

class polygon {
  private:
    void update_global_points();

  public:
    vec2d center;
    double angle;
    std::vector<vec2d> points;
    std::vector<vec2d> global_points = points;

    vec2d speed, angular_speed;

    void draw(cairo_t* cr) const;
    void draw_bounding_rect(cairo_t* cr) const;

    std::pair<vec2d, vec2d> get_bounding_box() const {
        vec2d min{INFINITY, INFINITY}, max{-INFINITY, -INFINITY};

        for (auto& point : this->global_points) {
            if (point.x < min.x)
                min.x = point.x;
            if (point.y < min.y)
                min.y = point.y;

            if (point.x > max.x)
                max.x = point.x;
            if (point.y > max.y)
                max.y = point.y;
        }
        return {min, max};
    }

    polygon& set_center(vec2d c) {
        center = c;
        update_global_points();
        return *this;
    }

    polygon& set_speed(vec2d s) {
        speed = s;
        return *this;
    }

    polygon& set_angular_speed(vec2d as) {
        angular_speed = as;
        return *this;
    }

    polygon& set_angle(double a) {
        angle = a;
        update_global_points();
        return *this;
    }

    polygon& translate(vec2d delta) {
        center += delta;
        update_global_points();
        return *this;
    }

    polygon& rotate(double delta) {
        angle += delta;
        update_global_points();
        return *this;
    }
};

extern polygon* polygons;
extern uint n_polygons;

extern void polygons_init_state();
extern void polygons_update_state();
extern void polygons_draw(cairo_t* cr);

#endif
