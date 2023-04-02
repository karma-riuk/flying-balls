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

static bool separating_axis(
    vec2d& axis, polygon& p, polygon& q, vec2d* pv, vec2d* impact_point) {
    double min_p, max_p, min_q, max_q;
    min_p = min_q = INFINITY;
    max_p = max_q = -INFINITY;

    double projection;
    vec2d min_p_point, max_p_point;
    for (auto& point : p.global_points) {
        projection = vec2d::dot(point, axis);

        if (projection < min_p) {
            min_p = projection;
            min_p_point = point;
        }

        if (projection > max_p) {
            max_p = projection;
            max_p_point = point;
        }
    }

    for (auto& point : q.global_points) {
        projection = vec2d::dot(point, axis);

        min_q = std::min(min_q, projection);
        max_q = std::max(max_q, projection);
    }

    if (max_p >= min_q && max_q >= min_p) {
        double d;
        if (max_q - min_p < max_p - min_q) {
            d = max_q - min_p;
            *impact_point = min_p_point;
        } else {
            d = max_p - min_q;
            *impact_point = max_p_point;
        }
        // push a bit more than needed so the shapes do not overlap in future
        // tests due to float precision
        double d_over_o_squared = d / vec2d::dot(axis, axis) + 1e-10;
        *pv = d_over_o_squared * axis;
        return false;
    }

    return true;
}

static uint get_smallest_vec_index(std::vector<vec2d> vs) {
    uint ret, i = 0;
    double min = INFINITY;
    for (auto& v : vs) {
        double dot = vec2d::dot(v, v);
        if (dot < min) {
            ret = i;
            min = dot;
        }
        i++;
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

    std::vector<vec2d> push_vectors;
    push_vectors.reserve(orthogonals.size());
    std::vector<vec2d> impact_points;
    push_vectors.reserve(orthogonals.size());

    vec2d push_vector;
    vec2d impact_point;
    for (auto& o : orthogonals) {
        if (separating_axis(o, p, q, &push_vector, &impact_point))
            // the axis is separating (the projections don't overlap)
            return ret;
        push_vectors.push_back(push_vector);
        impact_points.push_back(impact_point);
    }
    // if no axis is sperating, then they must be colliding
    ret.collides = true;
    uint i = get_smallest_vec_index(push_vectors);
    ret.n = vec2d::normalize(push_vectors[i]);
    ret.impact_point = impact_points[i];

    // assert that p pushes away from q
    vec2d d = q.centroid() - p.centroid();
    if (vec2d::dot(ret.n, d) > 0)
        ret.n *= -1;

    return ret;
}

collision collides(polygon& p, polygon& q) {
    return convex_collides(p, q);
}
