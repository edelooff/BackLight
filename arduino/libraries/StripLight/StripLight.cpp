#include <math.h>
#include "StripLight.h"

uint8_t Light::auto_init_count = 0;

Light::Light(): Light(auto_init_count++) {}

Light::Light(uint8_t value) {
    float inv_l;
    if (value <= 20) {
        inv_l = value / linear_divisor;
    } else {
        // inv_l = pow((value / 2.55 + 16) / 116, 3);
        inv_l = pow((value + 41) / 296.0f, 3);
    }
    uint16_t full_lightness = lround(inv_l * lightness_range);
    base = full_lightness >> 2;
    partial = full_lightness & 3;
}

uint8_t Light::outValue(uint8_t index) {
    return 0x80 | (base + (partial > index));
}
