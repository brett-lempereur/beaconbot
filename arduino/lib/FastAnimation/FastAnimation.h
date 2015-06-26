/**
 * @file FastAnimation.h
 * @author Brett Lempereur
 *
 * A collection of fast RGB lighting animation routines that can be integrated
 * into existing main loops.
 */

#ifndef FAST_ANIMATION_H
#define FAST_ANIMATION_H

#include <FastLED.h>

/**
 * Animation function interface.
 */
class Animation {

public:
    
    /**
     * Begin a new animation.
     */
    virtual void begin() = 0;

    /**
     * Compute and update the current state of the lights based on the frame
     * number.
     *
     * @param[in] now   current state of the lights.
     * @param[in] last  state of the lights at the start of the animation.
     * @param[in] n     number of lights.
     * @param[in] f     frame number.
     */
    virtual void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f) = 0;

};

/**
 * Blink lights.
 */
class Blink : public Animation {

    uint8_t _divisor;
    CRGB _colour;

public:

    /**
     * Construct a new blinking lights animation.
     *
     * @param[in] divisor   number of frames between blinks.
     * @param[in] colour    colour to blink the lights.
     */
    Blink(uint8_t divisor, CRGB colour);

    void begin();

    void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f);

};

/**
 * Chase a colour along the lights.
 */
class Chase : public Animation {

    CRGB _colour;

public:

    /**
     * Construct a new chase animation.
     *
     * @param[in] colour    the colour to chase along the lights.
     */
    Chase(const CRGB colour);

    void begin();

    void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f);

};

/**
 * Fade all of the lights to a single random colour.
 */
class RandomSolidFade : public Animation {

    CRGB _trgb;
    CHSV _thsv;

public:

    /**
     * Construct a new random solid fade animation.
     *
     * @param[in] s     saturation of the randomly selected colour.
     * @param[in] v     brightness of the randomly selected colour.
     */
    RandomSolidFade(uint8_t s, uint8_t v);

    void begin();

    void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f);

};

/**
 * Fade all of the lights to random colours.
 */
class RandomFade : public Animation {

    uint8_t _count;
    uint8_t _s, _v;
    CRGB* _lights;

public:

    /**
     * Construct a new random fade animation.
     *
     * This constructor will allocate a buffer large enough to hold RGB values
     * for each of the lights on the heap, which fill be released by its
     * destructor.
     *
     * @param[in] count number of lights.
     * @param[in] s     saturation of the randomly selected colour.
     * @param[in] v     brightness of the randomly selected colour.
     */
    RandomFade(const uint8_t count, const uint8_t s, const uint8_t v);

    /**
     * Release the light buffer.
     */
    ~RandomFade();

    void begin();

    void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f);

};

/**
 * Changes lights from left to right to a randomly generated colour.
 */
class RandomWipe : public Animation {

    CRGB _trgb;
    CHSV _thsv;

public:

    /**
     * Construct a new random wipe.
     *
     * @param[in] s     saturation of the randomly selected colour.
     * @param[in] v     brightness of the randomly selected colour.
     */
    RandomWipe(const uint8_t s, const uint8_t v);

    void begin();

    void animate(CRGB* now, const CRGB* last, const uint8_t n, const uint8_t f);

};

/**
 * Randomly animate lights.
 */
class RandomAnimator {

    Animation** _animations;
    uint8_t _count;
    uint8_t _current, _frame;

public:

    /**
     * Construct a new random animation controller.
     *
     * @param[in] animations    array of animation instances to select from.
     * @param[in] count         number
     */
    RandomAnimator(Animation* animations[], const uint8_t count);

    /**
     * Apply an animation to the lights.
     *
     * @param[in] now   current state of the lights.
     * @param[in] last  state of the lights at the start of the animation.
     * @param[in] n     number of lights.
     */
    void update(CRGB* now, CRGB* last, const uint8_t n);

};

#endif
