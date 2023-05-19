#include "collisions.h"

#include "game.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

struct vertex {
    vec2d v, p1, p2;
};

typedef std::pair<vec2d, vec2d> segment;

enum Orientation {
    COLINEAR,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
};

static std::vector<vertex> vertices_of(polygon& p) {
    std::vector<vertex> vertices;
    vertices.reserve(p.global_points.size());

    // i = 1 and <= points.size() cuz { -1 % n = -1 } et c'est chiant
    // so start from 1, "overflow" with i = points.size() and gg
    for (uint i = 1; i <= p.global_points.size(); ++i)
        vertices.push_back({
            p.global_points[i % p.points.size()],
            p.global_points[(i + 1) % p.points.size()],
            p.global_points[(i - 1) % p.points.size()],
        });
    return vertices;
}

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
static bool on_segment(vec2d& q, segment& pr) {
    return q.x <= std::max(pr.first.x, pr.second.x) &&
           q.x >= std::min(pr.first.x, pr.second.x) &&
           q.y <= std::max(pr.first.y, pr.second.y) &&
           q.y >= std::min(pr.first.y, pr.second.y);
}

static Orientation orientation(vec2d& p, vec2d& q, vec2d& r) {
    int v = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (v == 0)
        return COLINEAR;
    return v > 0 ? CLOCKWISE : COUNTER_CLOCKWISE;
}

static bool do_intersect(segment s1, segment s2) {
    // Find the four orientations needed for general and
    // special cases
    Orientation o1 = orientation(s1.first, s1.second, s2.first);
    Orientation o2 = orientation(s1.first, s1.second, s2.second);
    Orientation o3 = orientation(s2.first, s2.second, s1.first);
    Orientation o4 = orientation(s2.first, s2.second, s1.second);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == COLINEAR && on_segment(s2.first, s1))
        return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == COLINEAR && on_segment(s2.second, s1))
        return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == COLINEAR && on_segment(s1.first, s2))
        return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == COLINEAR && on_segment(s1.second, s2))
        return true;

    return false;
}

static std::vector<segment> edges_of(polygon& p) {
    std::vector<segment> ret;
    ret.reserve(p.points.size());
    for (uint i = 0; i < p.points.size(); ++i)
        ret.push_back(
            {p.global_points[i], p.global_points[(i + 1) % p.points.size()]});
    return ret;
}

static collision penetration(segment& edge, vertex& vertex, vec2d& d) {
    collision ret{true};
    ret.impact_point = vertex.v;

    vec2d n = (edge.second - edge.first).orthogonal();
    ret.n = vec2d::normalize(n);


    if (vec2d::dot(n, d) > 0)
        ret.n *= -1;

    vec2d temp = vertex.v - edge.first;
    ret.overlap = vec2d::dot(temp, ret.n) * -ret.n;
    ret.overlap += .1 * delta * -ret.n;
    // std::cout << "-------------- Impact: penetration --------------"
    //           << std::endl;
    return ret;
}

static collision parallel(segment edge_p, segment edge_q, vec2d d) {
    collision ret{true};

    vec2d line_start = edge_p.first;
    vec2d base = vec2d::normalize(edge_p.second - line_start);

    std::pair<double, vec2d> proj_p1, proj_p2, proj_q1, proj_q2;
    proj_p1 = {0, edge_p.first};
    proj_p2 = {vec2d::dot(edge_p.second - line_start, base), edge_p.second};
    proj_q1 = {vec2d::dot(edge_q.first - line_start, base), edge_q.first};
    proj_q2 = {vec2d::dot(edge_q.second - line_start, base), edge_q.second};

    std::pair<double, vec2d>*p_min, *q_min, *p_max, *q_max;
    if (proj_p1.first < proj_p2.first) {
        p_min = &proj_p1;
        p_max = &proj_p2;
    } else {
        p_min = &proj_p2;
        p_max = &proj_p1;
    }

    if (proj_q1.first < proj_q2.first) {
        q_min = &proj_q1;
        q_max = &proj_q2;
    } else {
        q_min = &proj_q2;
        q_max = &proj_q1;
    }

    vec2d min = p_min->first < q_min->first ? q_min->second : p_min->second;
    vec2d max = p_max->first < q_max->first ? p_max->second : q_max->second;

    ret.impact_point = (min + max) / 2;
    ret.n = base.orthogonal();
    if (vec2d::dot(ret.n, d) > 0)
        ret.n *= -1;

    vec2d temp = ret.impact_point - edge_p.first;
    ret.overlap = vec2d::dot(temp, ret.n) * -ret.n;
    ret.overlap += .1 * delta * -ret.n;

    // std::cout << "-------------- Impact: parallel --------------" <<
    // std::endl;
    return ret;
}

static bool are_vecs_parallel(vec2d s1, vec2d s2) {
    return std::abs(vec2d::dot(vec2d::normalize(s1), vec2d::normalize(s2))) >
           .99;
}

static double distance_between_parallel_segments(segment s1, segment s2) {
    double area = vec2d::cross(s1.first - s2.first, s2.second - s2.first);
    double base = vec2d::norm(s2.second - s2.first);
    return std::abs(area / base);
}

#define SMALLEST_DIST 3

static bool are_edges_colinear(segment& e1, segment& e2) {
    vec2d e1_vec = e1.second - e1.first;
    vec2d e2_vec = e2.second - e2.first;
    return are_vecs_parallel(e1_vec, e2_vec) &&
           distance_between_parallel_segments(e1, e2) < SMALLEST_DIST;
}

static collision vertex_edge_collision(polygon& p, polygon& q) {
    std::vector<vertex> vertices_p = vertices_of(p);
    std::vector<segment> edges_q = edges_of(q);
    vec2d d = q.centroid() - p.centroid();

    segment edge_p1, edge_p2;
    bool col1, col2;
    for (auto& vertex : vertices_p)
        for (auto& edge_q : edges_q) {
            edge_p1 = {vertex.v, vertex.p1};
            edge_p2 = {vertex.v, vertex.p2};
            col1 = do_intersect(edge_q, edge_p1);
            col2 = do_intersect(edge_q, edge_p2);

            if (col1 || col2) {
                if (are_edges_colinear(edge_q, edge_p1))
                    return parallel(edge_q, edge_p1, d);

                if (are_edges_colinear(edge_q, edge_p2))
                    return parallel(edge_q, edge_p2, d);

                if (col1 && col2)
                    return penetration(edge_q, vertex, d);
            }
        }
    return {false};
}

static collision vertex_vertex_collision(polygon& p, polygon& q) {
    std::vector<vertex> vertices_p = vertices_of(p);
    std::vector<segment> edges_q = edges_of(q);
    vec2d d = q.centroid() - p.centroid();

    for (auto& vertex : vertices_p)
        for (auto& edge_q : edges_q) {
            if (do_intersect(edge_q, {vertex.v, vertex.p1})) {
                vec2d edge_q_vec = edge_q.second - edge_q.first;
                vec2d n = vec2d::normalize(edge_q_vec.orthogonal());

                if (vec2d::dot(n, d) > 0)
                    n *= -1;

                vec2d temp = vertex.v - edge_q.first;
                vec2d overlap = vec2d::dot(temp, n) * -n;
                overlap += .1 * delta * -n;

                // std::cout
                //     << "-------------- Impact: angle in angle --------------"
                //     << std::endl;
                return {true, n, vertex.v, overlap};
            }
        }
    return {false};
}

static collision convex_collides(polygon& p, polygon& q) {
    collision ret;

    if ((ret = vertex_edge_collision(p, q)).collides)
        return ret;

    if ((ret = vertex_edge_collision(q, p)).collides) {
        ret.n *= -1;
        ret.overlap *= -1;
        return ret;
    }

    if ((ret = vertex_vertex_collision(p, q)).collides)
        return ret;

    if ((ret = vertex_vertex_collision(q, p)).collides) {
        ret.n *= -1;
        ret.overlap *= -1;
    }
    return ret;
}

collision collides(polygon& p, polygon& q) {
    return convex_collides(p, q);
}
