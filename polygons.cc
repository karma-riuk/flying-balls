#include "polygons.h"

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
uint n_polygons = 7;

void polygons_init_state() {
    polygons = new polygon[n_polygons];

    int wall_thickness = 50;
    // north wall
    polygons[0] = poly_generate::rectangle(width, wall_thickness, INFINITY)
                      .set_center({width / 2., -wall_thickness / 2.});
    // south wall
    polygons[1] = poly_generate::rectangle(width, wall_thickness, INFINITY)
                      .set_center({width / 2., height + wall_thickness / 2.});
    // west wall
    polygons[2] = poly_generate::rectangle(wall_thickness, height, INFINITY)
                      .set_center({-wall_thickness / 2., height / 2.});
    // east wall
    polygons[3] = poly_generate::rectangle(wall_thickness, height, INFINITY)
                      .set_center({width + wall_thickness / 2., height / 2.});
    // polygons[0] = poly_generate::triangle(150, 150, 30)
    //                   .set_center({200, 400})
    //                   .set_angle(13)
    //                   .set_angular_speed(10)
    //                   .set_speed({200, 50});
    // polygons[4] = poly_generate::rectangle(50, height / 2., INFINITY)
    //                   .set_center({25 + width * 1. / 2, height / 2.})
    //                   .set_angle(0);

#define PROBLEMATIC_COUPLES 1
#if PROBLEMATIC_COUPLES == 0
    polygons[4] = poly_generate::regular(100, 3)
                      .set_center({100, 400})
                      .set_angle(0)
                      .set_speed({200, -10});

    polygons[5] = poly_generate::square(100)
                      .set_center({600, 400})
                      .set_angle(45)
                      .set_speed({-200, -10});

    // .set_angle(45)
    // .set_center({300, 650})
    // .set_angular_speed(181)
    // .set_speed({200, -10});

    // .set_center({200, 160})
    // .set_angle(-45)
    // .set_speed({200, 10});

    // polygons[6] = poly_generate::regular(100, 5)
    //                   .set_angle(0)
    //                   .set_center({600, 650})
    //                   .set_speed({-200, 200});

    // .set_angle(0)
    // .set_center({600, 650})
    // .set_speed({-200, -10});

    polygons[6] = poly_generate::general(
        {{0, 0}, {100, 0}, {100, 100}, {50, 150}, {0, 100}}, 3)
                      .set_center({400, 400});
#elif PROBLEMATIC_COUPLES == 1
    // for this problematic couple to work, remove the gravity
    n_polygons = 7;
    polygons[4] = poly_generate::regular(100, 3)
                      .set_center({103.91, 684.587})
                      .set_angle(280.108)
                      .set_speed({190.114, 131.983})
                      .set_angular_speed(32.6448);

    polygons[5] = poly_generate::square(100)
                      .set_center({614.338, 514.889})
                      .set_angle(-54.3526)
                      .set_speed({-70.0347, -62.4788})
                      .set_angular_speed(-39.3846);

    polygons[6] = poly_generate::general(
        {{0, 0}, {100, 0}, {100, 100}, {50, 150}, {0, 100}}, 3)
                      .set_center({261.425, 556.613})
                      .set_angle(-122.59)
                      .set_speed({-46.9522, 48.5392})
                      .set_angular_speed(-35.5983);

#endif
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
    return is_point_inside_rect(cur_bound, other_tl) ||
           is_point_inside_rect(cur_bound, {other_tl.x, other_br.y}) ||
           is_point_inside_rect(cur_bound, {other_br.x, other_tl.y}) ||
           is_point_inside_rect(cur_bound, other_br);
}

static double impulse_parameter(vec2d v_ab1,
    vec2d n,
    double m_a,
    double m_b,
    vec2d r_ap,
    vec2d r_bp,
    double I_a,
    double I_b,
    double e) {
    double nominator = -(1 + e) * vec2d::dot(v_ab1, n);

    double r_ap_cross_n = vec2d::cross(r_ap, n);
    double r_bp_cross_n = vec2d::cross(r_bp, n);
    double denominator = 1 / m_a + 1 / m_b + r_ap_cross_n * r_ap_cross_n / I_a +
                         r_bp_cross_n * r_bp_cross_n / I_b;

    return nominator / denominator;
}

static int8_t sign(double d) {
    return d >= 0 ? 1 : -1;
}

static void handle_collision(collision& c, polygon* a, polygon* b) {
    // see https://www.myphysicslab.com/engine2D/collision-en.html for the
    // formulas
    double omega_a1 = to_rad(a->angular_speed);
    double omega_b1 = to_rad(b->angular_speed);

    vec2d r_ap = c.impact_point - a->centroid();
    vec2d v_ap1 = a->speed + vec2d::cross(omega_a1, r_ap);
    // std::cout << "    p = " << c.impact_point << std::endl;
    // std::cout << "    r_ap = " << r_ap << std::endl;
    // std::cout << "    v_ap1 = " << v_ap1 << std::endl;

    vec2d r_bp = c.impact_point - b->centroid();
    vec2d v_bp1 = b->speed + vec2d::cross(omega_b1, r_bp);
    // std::cout << "    r_bp = " << r_bp << std::endl;
    // std::cout << "    v_bp1 = " << v_bp1 << std::endl;

    vec2d v_ab1 = v_ap1 - v_bp1;
    // std::cout << "    v_ab1 = " << v_ab1 << std::endl;

    // avoid the polygons getting stuck if, on the frame after the impact,
    // the polygon a is still inside of b
    // std::cout << "sign(v_a x n) = " <<  << std::endl;
    if (a->collided_with.find(b) != a->collided_with.end())
    // if (vec2d::dot(v_ap1, v_bp1) < 0)
    // might have to tweak this condition
    {
        std::cout << "Lol actually no, nevermind" << std::endl;
        return;
    }

    a->collided_with.insert(b);
    b->collided_with.insert(a);


    double I_a = a->inertia, I_b = b->inertia;
    // std::cout << "  Parameters for j: " << std::endl;
    // std::cout << "    v_ab1 = " << v_ab1 << std::endl;
    // std::cout << "    n = " << c.n << std::endl;
    // std::cout << "    m_a = " << a->mass << std::endl;
    // std::cout << "    m_b = " << b->mass << std::endl;
    // std::cout << "    r_ap = " << r_ap << std::endl;
    // std::cout << "    r_bp = " << r_bp << std::endl;
    // std::cout << "    I_a = " << I_a << std::endl;
    // std::cout << "    I_b = " << I_b << std::endl;

    double j = impulse_parameter(
        v_ab1, c.n, a->mass, b->mass, r_ap, r_bp, I_a, I_b, 1);
    // std::cout << "====> j = " << j << std::endl;

    vec2d v_a2 = a->speed + j * c.n / a->mass;
    vec2d v_b2 = b->speed - j * c.n / b->mass;
    // std::cout << "    v_a2 = " << v_a2 << std::endl;
    // std::cout << "    v_b2 = " << v_b2 << std::endl;

    double omega_a2 = omega_a1 + vec2d::cross(r_ap, j * c.n) / I_a;
    double omega_b2 = omega_b1 - vec2d::cross(r_bp, j * c.n) / I_b;
    // std::cout << "    omega_a2 = " << omega_a2 << std::endl;
    // std::cout << "    omega_b2 = " << omega_b2 << std::endl;

    a->speed = v_a2;
    a->angular_speed = to_deg(omega_a2);

    b->speed = v_b2;
    b->angular_speed = to_deg(omega_b2);
}

collision col; // tbd

static void check_collisions(polygon* current_p) {
    rect cur_bound = current_p->get_bounding_box();

    bool collided_with_something = false;
    for (polygon* other_p = polygons; other_p != polygons + n_polygons;
         ++other_p) {
        if (other_p == current_p) // polygons don't collide with themselves
            continue;
        rect other_bound = other_p->get_bounding_box();
        if (bounding_rects_collide(cur_bound, other_bound) ||
            bounding_rects_collide(other_bound, cur_bound)) {
            // std::cout << "Bounding boxes do collide" << std::endl;
            collision c = collides(*current_p, *other_p);
            if (c.collides) {
                collided_with_something = true;
                col = c;
                std::cout << "Between " << current_p->label << " and "
                          << other_p->label << std::endl;
                std::cout << "----- BEFORE -----" << std::endl;
                std::cout << *current_p << std::endl;
                std::cout << *other_p << std::endl;
                handle_collision(c, current_p, other_p);
                std::cout << "----- AFTER -----" << std::endl;
                std::cout << *current_p << std::endl;
                std::cout << *other_p << std::endl;
            } else if (current_p->collided_with.find(other_p) !=
                       current_p->collided_with.end()) {
                std::cout << "Removing shit" << std::endl;
                current_p->collided_with.erase(other_p);
                other_p->collided_with.erase(current_p);
                //     std::remove(current_p->collided_with.begin(),
                //         current_p->collided_with.end(),
                //         other_p);
                // std::remove(other_p->collided_with.begin(),
                //     other_p->collided_with.end(),
                //     current_p);
            }
        }
    }
    if (!collided_with_something && current_p->collided_with.size() > 0)
        current_p->collided_with.clear();
}

static void check_border_collision(polygon* p) {
    for (auto& point : p->global_points) {
        bool hit_vert_wall = point.x <= 0 || point.x >= width;
        bool hit_hori_wall = point.y <= 0 || point.y >= width;

        if (hit_vert_wall || hit_hori_wall) {
            p->set_angular_speed(-p->angular_speed); // this is not accurate,
                                                     // but avoids bugs for now

            if (hit_vert_wall)
                p->speed.x *= -1;
            if (hit_hori_wall)
                p->speed.y *= -1;

            break;
        }
    }
}

void polygons_update_state() {
    for (polygon* p = polygons; p != polygons + n_polygons; ++p) {
        if (p->mass == INFINITY) // immovable objects don't need to be updated
            continue;
        // check_border_collision(p);
        check_collisions(p);

        p->rotate(delta * p->angular_speed);
        p->angle = std::fmod(p->angle, 360);

        p->translate(delta * p->speed);

        vec2d g = gravity_vector(p);
        p->translate(.5 * delta * delta * g);
        p->speed += delta * g;
        // std::cout << *p << std::endl;
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
    draw_circle(cr, col.impact_point, 3); // tbd
    col.n.draw(cr, col.impact_point);     // tbd

    cairo_set_source_rgb(cr, 1, 1, 1);

    for (auto& point : this->global_points)
        cairo_line_to(cr, point.x, point.y);

    cairo_line_to(cr, this->global_points[0].x, this->global_points[0].y);
    cairo_stroke(cr);


    // draw centroid
    vec2d centroid = this->centroid();
    draw_circle(cr, centroid, 1);

    (10 * delta * this->speed).draw(cr, centroid);
}

void polygons_draw(cairo_t* cr) {
    for (const polygon* p = polygons; p != polygons + n_polygons; ++p)
        p->draw(cr);
}
