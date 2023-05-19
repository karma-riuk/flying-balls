#ifndef POLYGONS_H_INCLUDED
#define POLYGONS_H_INCLUDED

#include "vec2d.h"

#include <cmath>
#include <gtk/gtk.h>
#include <utility>
#include <vector>

typedef std::pair<vec2d, vec2d> rect;

class polygon {
  private:
    void update_global_points();

  public:
    vec2d center;
    double angle;
    std::vector<vec2d> points;
    double inertia;
    double mass;
    std::string label;

    std::vector<vec2d> global_points = points;

    vec2d speed;
    double angular_speed;

    void draw(cairo_t* cr) const;
    void draw_bounding_rect(cairo_t* cr) const;

    rect get_bounding_box() const {
        vec2d tl{INFINITY, INFINITY}, tr{-INFINITY, -INFINITY};

        for (auto& point : this->global_points) {
            if (point.x < tl.x)
                tl.x = point.x;
            if (point.y < tl.y)
                tl.y = point.y;

            if (point.x > tr.x)
                tr.x = point.x;
            if (point.y > tr.y)
                tr.y = point.y;
        }
        return {tl, tr};
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

    polygon& set_angular_speed(double as) {
        angular_speed = as;
        return *this;
    }

    polygon& set_angle(double a) {
        angle = a;
        update_global_points();
        return *this;
    }

    polygon& set_mass(double m) {
        mass = m;
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

    vec2d centroid() const {
        double x = 0, y = 0;
        for (auto& p : global_points)
            x += p.x, y += p.y;

        return vec2d{x, y} / points.size();
    }

    friend std::ostream& operator<<(std::ostream& os, polygon& p) {
        vec2d c = p.centroid();
        os << p.label << ": " << std::endl;
        os << "    mass: " << p.mass << std::endl;
        os << "    position: " << c << std::endl;
        os << "    angle: " << p.angle << std::endl;
        os << "    speed: " << p.speed << std::endl;
        os << "    angular speed: " << p.angular_speed << std::endl;
        return os;
    }
};

extern polygon* polygons;
extern uint n_polygons;

extern void polygons_init_state();
extern void polygons_update_state();
extern void polygons_draw(cairo_t* cr);
extern void polygons_destroy();

#endif
