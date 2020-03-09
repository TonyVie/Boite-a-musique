#define NEOPIXEBUS_NO_STL 1

#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include <NeoPixelSegmentBus.h>


enum LedState
{
    off,
    fill,
    rotate,
    blink,
};

void beginLed();
void animateLed(uint32_t color, LedState state);
void loopLed();