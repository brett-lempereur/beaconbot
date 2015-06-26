/**
 * @file Blink.cpp
 * @author Brett Lempereur
 *
 * Blinking light animation.
 */

#include "FastAnimation.h"

Blink::Blink(uint8_t divisor, CRGB colour)
{
    _divisor = divisor;
    _colour = colour;
}

void Blink::begin()
{

}

void Blink::animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f)
{
    if ((f / _divisor) % 2 == 0) {
        fill_solid(now, n, _colour);
    } else {
        memcpy(now, last, sizeof(CRGB) * n);
    }
}

