/**
 * @file RandomAnimator.cpp
 * @author Brett Lempereur
 * 
 * Randomly animate lights.
 */

#include "FastAnimation.h"

RandomAnimator::RandomAnimator(Animation* animations[], const uint8_t count)
{
    _animations = animations;
    _count = count;
    _frame = 255;
    _current = 0;
}

void RandomAnimator::update(CRGB* now, CRGB* last, const uint8_t n)
{
    _animations[_current]->animate(now, last, n, _frame++);
    if (_frame == 255) {
        memcpy(last, now, sizeof(CRGB) * n);
        _frame = 0;
        _current = random8(_count);
        _animations[_current]->begin();
    }
}

