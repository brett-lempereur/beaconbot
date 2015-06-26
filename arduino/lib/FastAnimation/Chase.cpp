/**
 * @file Chase.cpp
 * @author Brett Lempereur
 *
 * Chase a colour along the lights.
 */

#include "FastAnimation.h"

Chase::Chase(CRGB colour)
{
    _colour = colour;
}

void Chase::begin()
{

}

void Chase::animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f)
{
    uint8_t k = scale8(f, n);

    // Ensure that the last light is always enabled at the end of the
    // animation.
    if (f == 255) {
        memcpy(now, last, sizeof(CRGB) * n);
    } else {
        now[k-1] = last[k-1];
        now[k] = _colour;
    }
}

