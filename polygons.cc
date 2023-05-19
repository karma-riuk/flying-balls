#include "polygons.h"

#include "balls.h" // just for the restitution_coefficient_get() function,
                   // prolly should be placed in another header file
#include "cairo.h"
#include "collisions.h"
#include "game.h"
#include "gravity.h"
#include "matrix.h"
#include "polygon_generator.h"

#include <algorithm>
#include <iostream>
#include <utility>

polygon* polygons = nullptr;
uint n_polygons = 0;

void polygons_init_state() {
    n_polygons = 20;
    polygons = new polygon[n_polygons];
    int wall_thickness = 50;
    uint n = 0;
    // north wall
    polygons[n++] = poly_generate::rectangle(width, wall_thickness)
                        .set_mass(INFINITY)
                        .set_center({width / 2., -wall_thickness / 2.});
    // south wall
    polygons[n++] = poly_generate::rectangle(width, wall_thickness)
                        .set_mass(INFINITY)
                        .set_center({width / 2., height + wall_thickness / 2.});
    // west wall
    polygons[n++] = poly_generate::rectangle(wall_thickness, height)
                        .set_mass(INFINITY)
                        .set_center({-wall_thickness / 2., height / 2.});
    // east wall
    polygons[n++] = poly_generate::rectangle(wall_thickness, height)
                        .set_mass(INFINITY)
                        .set_center({width + wall_thickness / 2., height / 2.});

    // middle wall
    polygons[n++] = poly_generate::rectangle(50, height / 2.)
                        .set_mass(INFINITY)
                        .set_center({25 + width * 1. / 2, height / 2.})
                        .set_angle(30);

    polygons[n++] = poly_generate::regular(100, 3)
                        .set_center({100, 400})
                        .set_angle(0)
                        .set_speed({200, -10});

    polygons[n++] = poly_generate::square(100)
                        .set_center({600, 400})
                        .set_angle(45)
                        .set_speed({-200, -10});

    polygons[n++] = poly_generate::general(
                        {{0, 0}, {100, 0}, {100, 100}, {50, 150}, {0, 100}}
    )
                        .set_center({200, 600})
                        .set_angle(45)
                        .set_speed({10, 0});

    polygons[n++] = poly_generate::rectangle(100, 150).set_center({600, 200});
    polygons[n++] = poly_generate::regular(50, 5)
                        .set_center({150, 150})
                        .set_speed({50, -50});
    polygons[n++] =
        poly_generate::general({{0, 0}, {50, 80}, {0, 160}, {-50, 80}})
            .set_center({700, 700})
            .set_speed({0, -100});

    assert(n <= n_polygons);
    n_polygons = n;
}

static double to_rad(double angle_in_deg) {
    static double PI_180 = M_PI / 180.;
    return angle_in_deg * PI_180;
}

static double to_deg(double angle_in_rad) {
    static double PI_180 = 180. / M_PI;
    return angle_in_rad * PI_180;
}

static bool is_point_inside_rect(rect rect, vec2d point) {
    vec2d tl = rect.first, br = rect.second;
    return point.x > tl.x && point.x < br.x && point.y > tl.y && point.y < br.y;
}

static bool bounding_rects_collide(rect cur_bound, rect other_bound) {
    vec2d other_tl = other_bound.first, other_br = other_bound.second;
    return is_point_inside_rect(cur_bound, other_tl)
        || is_point_inside_rect(cur_bound, {other_tl.x, other_br.y})
        || is_point_inside_rect(cur_bound, {other_br.x, other_tl.y})
        || is_point_inside_rect(cur_bound, other_br);
}

static double impulse_parameter(
    vec2d v_ab1,
    vec2d n,
    double m_a,
    double m_b,
    vec2d r_ap,
    vec2d r_bp,
    double I_a,
    double I_b,
    double e
) {
    double nominator = -(1 + e) * vec2d::dot(v_ab1, n);

    double r_ap_cross_n = vec2d::cross(r_ap, n);
    double r_bp_cross_n = vec2d::cross(r_bp, n);
    double denominator = 1 / m_a + 1 / m_b + r_ap_cross_n * r_ap_cross_n / I_a
                       + r_bp_cross_n * r_bp_cross_n / I_b;

    return nominator / denominator;
}

static void handle_collision(collision& c, polygon* a, polygon* b) {
    // see https://www.myphysicslab.com/engine2D/collision-en.html for the
    // formulas
    double omega_a1 = to_rad(a->angular_speed);
    double omega_b1 = to_rad(b->angular_speed);

    vec2d r_ap = c.impact_point - a->centroid();
    vec2d v_ap1 = a->speed + vec2d::cross(omega_a1, r_ap);

    vec2d r_bp = c.impact_point - b->centroid();
    vec2d v_bp1 = b->speed + vec2d::cross(omega_b1, r_bp);

    vec2d v_ab1 = v_ap1 - v_bp1;

    if (vec2d::norm(c.overlap) > 10)
        c.overlap = -.1 * vec2d::normalize(c.overlap);
    // std::cout << c.overlap << std::endl;
    if (b->mass == INFINITY)
        a->translate(c.overlap);
    else {
        double ma = a->mass;
        double mb = b->mass;
        double m_total = ma + mb;

        // If b is wall, then mb / m_total = INFINITY / INFINITY = -nan,
        // so we need the if statement above
        a->translate(c.overlap * mb / m_total);
        b->translate(-c.overlap * ma / m_total);
    }

    double I_a = a->inertia, I_b = b->inertia;

    double j = impulse_parameter(
        v_ab1,
        c.n,
        a->mass,
        b->mass,
        r_ap,
        r_bp,
        I_a,
        I_b,
        restitution_coefficient_get()
    );

    vec2d v_a2 = a->speed + j * c.n / a->mass;
    vec2d v_b2 = b->speed - j * c.n / b->mass;

    double omega_a2 = omega_a1 + vec2d::cross(r_ap, j * c.n) / I_a;
    double omega_b2 = omega_b1 - vec2d::cross(r_bp, j * c.n) / I_b;

    a->speed = v_a2;
    a->angular_speed = to_deg(omega_a2);

    b->speed = v_b2;
    b->angular_speed = to_deg(omega_b2);
}

static void check_collisions(polygon* current_p) {
    rect cur_bound = current_p->get_bounding_box();

    collision c;
    polygon* other_p;
    for (other_p = polygons; other_p != polygons + n_polygons; ++other_p) {
        if (other_p == current_p) // polygons don't collide with themselves
            continue;
        rect other_bound = other_p->get_bounding_box();
        if ((bounding_rects_collide(cur_bound, other_bound)
             || bounding_rects_collide(other_bound, cur_bound))
            && (c = collides(*current_p, *other_p)).collides) {
            handle_collision(c, current_p, other_p);
        }
    }
}

void polygons_update_state() {
    for (polygon* p = polygons; p != polygons + n_polygons; ++p) {
        if (p->mass == INFINITY) // immovable objects don't need to be updated
            continue;
        check_collisions(p);

        p->rotate(delta * p->angular_speed);
        p->angle = std::fmod(p->angle, 360);

        p->translate(delta * p->speed);

        vec2d g = gravity_vector(p);
        p->translate(.5 * delta * delta * g);
        p->speed += delta * g;
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
    vec2d tl = bb.first, br = bb.second;

    cairo_line_to(cr, tl.x, tl.y);
    cairo_line_to(cr, tl.x, br.y);
    cairo_line_to(cr, br.x, br.y);
    cairo_line_to(cr, br.x, tl.y);
    cairo_line_to(cr, tl.x, tl.y);
    cairo_stroke(cr);
    cairo_set_dash(cr, 0, 0, 0); // disable dashes
}

static void draw_circle(cairo_t* cr, vec2d p, double radius) {
    cairo_translate(cr, p.x, p.y);
    cairo_arc(cr, 0, 0, radius, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_translate(cr, -p.x, -p.y);
}

void polygon::draw(cairo_t* cr) const {
    // this->draw_bounding_rect(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);

    for (auto& point : this->global_points)
        cairo_line_to(cr, point.x, point.y);

    cairo_line_to(cr, this->global_points[0].x, this->global_points[0].y);
    cairo_stroke(cr);


    // draw centroid
    vec2d centroid = this->centroid();
    draw_circle(cr, centroid, 1);

    // draw speed
    (delta * this->speed).draw(cr, centroid);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}

void polygons_destroy() {
    delete[] (polygons);
}
