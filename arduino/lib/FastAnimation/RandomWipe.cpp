/**
 * @file RandomWipe.cpp
 * @author Brett Lempereur
 *
 * Changes lights from left to right to a randomly generated colour.
 */

#include "FastAnimation.h"

RandomWipe::RandomWipe(uint8_t s, uint8_t v)
{
    _thsv.s = s;
    _thsv.v = v;
}

void RandomWipe::begin()
{
    _thsv.h = random8();
    _trgb = _thsv;
}

void RandomWipe::animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f)
{
    uint8_t k = scale8(f, n);
    now[k] = _trgb;
}

