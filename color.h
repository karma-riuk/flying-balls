#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <cmath>

struct color_t {
    double red, green, blue;
};

struct hsv_t {
    double hue, sat, val;
};

extern hsv_t rgb2hsv(color_t in);
extern color_t hsv2rgb(hsv_t in);

#endif
