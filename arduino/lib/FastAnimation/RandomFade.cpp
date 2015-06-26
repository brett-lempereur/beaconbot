/**
 * @file RandomFade.cpp
 * @author Brett Lempereur
 *
 * Fade all of the lights to random colours.
 */

#include "FastAnimation.h"

RandomFade::RandomFade(const uint8_t count, const uint8_t s, const uint8_t v)
{
    _count = count;
    _s = s;
    _v = v;
    _lights = new CRGB[count];
}

RandomFade::~RandomFade()
{
    delete[] _lights;
}

void RandomFade::begin()
{
    CHSV rhsv(0, _s, _v);

    for (int i = 0; i < _count; i++) {
        rhsv.h = random8();
        _lights[i] = rhsv;
    }
}

void RandomFade::animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f)
{
    for (int i = 0; i < n; i++) {
        now[i].r = lerp8by8(last[i].r, _lights[i].r, f);
        now[i].g = lerp8by8(last[i].g, _lights[i].g, f);
        now[i].b = lerp8by8(last[i].b, _lights[i].b, f);
    }
}

