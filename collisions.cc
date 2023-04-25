#include "collisions.h"

#include <algorithm>
#include <iostream>
#include <vector>

static std::vector<vec2d> edges_of(polygon& p) {
    std::vector<vec2d> edges;
    edges.reserve(p.global_points.size());

    for (uint i = 0; i < p.global_points.size(); ++i)
        edges.push_back(p.global_points[(i + 1) % p.global_points.size()] -
                        p.global_points[i]);
    return edges;
}

struct projection_t {
    double min_proj, max_proj;
    vec2d min_point, max_point;
};

static projection_t project(polygon& p, vec2d axis) {
    projection_t ret{INFINITY, -INFINITY};

    double proj;
    for (auto& point : p.global_points) {
        proj = vec2d::dot(point, axis);

        if (proj < ret.min_proj) {
            ret.min_proj = proj;
            ret.min_point = point;
        }

        if (proj > ret.max_proj) {
            ret.max_proj = proj;
            ret.max_point = point;
        }
    }

    return ret;
}

static collision convex_collides(polygon& p, polygon& q) {
    collision ret{false};

    std::vector<vec2d> edges_p = edges_of(p);
    std::vector<vec2d> edges_q = edges_of(q);

    std::vector<vec2d> edges;
    edges.reserve(edges_p.size() + edges_q.size());
    edges.insert(edges.end(), edges_p.begin(), edges_p.end());
    edges.insert(edges.end(), edges_q.begin(), edges_q.end());

    std::vector<vec2d> orthogonals;
    orthogonals.reserve(edges.size());
    for (auto& v : edges)
        orthogonals.push_back(v.orthogonal());

    std::vector<collision> candidate_collisions;
    candidate_collisions.reserve(orthogonals.size());

    vec2d d = q.centroid() - p.centroid();

    double min_overlap = INFINITY;
    for (auto& o : orthogonals) {
        vec2d axis = vec2d::normalize(o);
        if (vec2d::dot(d, axis) > 0)
            axis *= -1;


        projection_t p_proj = project(p, axis);
        projection_t q_proj = project(q, axis);
        if (p_proj.max_proj < q_proj.min_proj ||
            q_proj.max_proj < p_proj.min_proj)
            // the axis is separating (the projections don't overlap)
            return ret;


        double overlap = std::min(p_proj.max_proj, q_proj.max_proj) -
                         std::max(p_proj.min_proj, q_proj.min_proj);

        if (overlap < min_overlap) {
            min_overlap = overlap;
            ret.n = axis;

            ret.impact_point = p_proj.max_proj > q_proj.min_proj
                                   ? p_proj.min_point
                                   : q_proj.max_point;

            // if (vec2d::dot(axis, d) > 0) {
            //     ret.impact_point = p_proj.max_proj > q_proj.min_proj
            //                            ? q_proj.min_point
            //                            : q_proj.max_point;
            // } else {
            //     ret.impact_point = p_proj.max_proj > q_proj.min_proj
            //                            ? p_proj.min_point
            //                            : p_proj.max_point;
            // }
        }
    }
    // if no axis is sperating, then they must be colliding
    ret.collides = true;

    // if (vec2d::dot(d, ret.n) > 0)
    //     ret.n *= -1;
    return ret;
}

collision collides(polygon& p, polygon& q) {
    return convex_collides(p, q);
}
