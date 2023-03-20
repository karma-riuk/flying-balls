#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "vec2d.h"

class matrix {
  public:
    vec2d b1, b2;
};

inline vec2d operator*(matrix m, vec2d v) {
    return vec2d{v.x * m.b1.x + v.y * m.b2.x, v.x * m.b1.y + v.y * m.b2.y};
}

#endif
