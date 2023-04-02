#ifndef VEC2D_H_INCLUDED
#define VEC2D_H_INCLUDED

#include <cmath>
#include <ostream>

class vec2d {
  public:
    double x;
    double y;

    vec2d& operator=(const vec2d& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    vec2d& operator+=(const vec2d& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2d& operator-=(const vec2d& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2d& operator*=(double l) {
        x *= l;
        y *= l;
        return *this;
    }

    vec2d operator+(const vec2d& other) const {
        return vec2d{x + other.x, y + other.y};
    }

    vec2d operator-(const vec2d& other) const {
        return vec2d{x - other.x, y - other.y};
    }

    vec2d operator-() const {
        return vec2d{-x, -y};
    }

    vec2d operator*(double l) const {
        return vec2d{x * l, y * l};
    }

    vec2d operator/(double a) const {
        return vec2d{x / a, y / a};
    }

    vec2d& rotate(double angle) {
        double sin_a = sin(angle);
        double cos_a = cos(angle);
        double x1 = x;
        double y1 = y;
        x = cos_a * x1 - sin_a * y1;
        y = sin_a * x1 + cos_a * y1;
        return *this;
    }

    vec2d orthogonal() {
        return {-y, x};
    }

    static double dot(const vec2d& a, const vec2d& b) {
        return a.x * b.x + a.y * b.y;
    }

    static double cross(const vec2d& a, const vec2d& b) {
        return a.x * b.y - a.y * b.x;
    }

    static vec2d cross(const double omega, const vec2d& v) {
        return {-omega * v.y, omega * v.x};
    }

    static double norm(const vec2d& v) {
        return std::sqrt(dot(v, v));
    }

    static vec2d normalize(const vec2d& v) {
        return v / norm(v);
    }
};

static inline std::ostream& operator<<(std::ostream& os, vec2d& p) {
    return os << '(' << p.x << ", " << p.y << ')';
}

static inline vec2d operator*(double l, const vec2d& v) {
    return vec2d{v.x * l, v.y * l};
}

#endif
