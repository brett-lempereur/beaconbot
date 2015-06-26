/**
 * @file RandomSolidFade.cpp
 * @author Brett Lempereur
 *
 * Fade all of the lights to a single random colour.
 */

#include "FastAnimation.h"

RandomSolidFade::RandomSolidFade(uint8_t s, uint8_t v)
{
    _thsv.s = s;
    _thsv.v = v;
}

void RandomSolidFade::begin()
{
    _thsv.h = random8();
    _trgb = _thsv;
}

void RandomSolidFade::animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f)
{
    CRGB c;

    c.r = lerp8by8(last[0].r, _trgb.r, f);
    c.g = lerp8by8(last[0].g, _trgb.g, f);
    c.b = lerp8by8(last[0].b, _trgb.b, f);
    fill_solid(now, n, c);
}

