#include "color.h"

// taken from
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
hsv_t rgb2hsv(color_t in) {
    hsv_t out;
    double min, max, delta;

    min = in.red < in.green ? in.red : in.green;
    min = min < in.blue ? min : in.blue;

    max = in.red > in.green ? in.red : in.green;
    max = max > in.blue ? max : in.blue;

    out.val = max; // v
    delta = max - min;
    if (delta < 0.00001) {
        out.sat = 0;
        out.hue = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.sat = (delta / max); // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.sat = 0.0;
        out.hue = NAN; // its now undefined
        return out;
    }
    if (in.red >= max) // > is bogus, just keeps compilor happy
        out.hue = (in.green - in.blue) / delta; // between yellow & magenta
    else if (in.green >= max)
        out.hue = 2.0 + (in.blue - in.red) / delta; // between cyan & yellow
    else
        out.hue = 4.0 + (in.red - in.green) / delta; // between magenta & cyan

    out.hue *= 60.0; // degrees

    if (out.hue < 0.0)
        out.hue += 360.0;

    return out;
}

color_t hsv2rgb(hsv_t in) {
    double hh, p, q, t, ff;
    long i;
    color_t out;

    if (in.sat <= 0.0) { // < is bogus, just shuts up warnings
        out.red = in.val;
        out.green = in.val;
        out.blue = in.val;
        return out;
    }
    hh = in.hue;
    if (hh >= 360.0)
        hh = 0.0;
    hh /= 60.0;
    i = (long) hh;
    ff = hh - i;
    p = in.val * (1.0 - in.sat);
    q = in.val * (1.0 - (in.sat * ff));
    t = in.val * (1.0 - (in.sat * (1.0 - ff)));

    switch (i) {
    case 0:
        out.red = in.val;
        out.green = t;
        out.blue = p;
        break;
    case 1:
        out.red = q;
        out.green = in.val;
        out.blue = p;
        break;
    case 2:
        out.red = p;
        out.green = in.val;
        out.blue = t;
        break;

    case 3:
        out.red = p;
        out.green = q;
        out.blue = in.val;
        break;
    case 4:
        out.red = t;
        out.green = p;
        out.blue = in.val;
        break;
    case 5:
    default:
        out.red = in.val;
        out.green = p;
        out.blue = q;
        break;
    }
    return out;
}
