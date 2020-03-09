#include "mod_Led.h"

const uint16_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t PixelCount = 24; // make sure to set this to the number of pixels in your strip
const uint16_t AnimCount = 3; // rotate / off /blink-fade  
const uint16_t TailLength = 8; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.1f; // max lightness at the head of the tail (0.5f is full bright)


// Menu callback function
NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma
NeoPixelBrightnessBus <NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object

LedState lastState = off;
uint32_t lastColor = 0;

boolean fadeToColor = true;  // general purpose variable used to store effect state

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};
MyAnimationState blinkAnimationState;

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    randomSeed(seed);
}

void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        strip.RotateRight(1);
    }
}

void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        blinkAnimationState.StartingColor,
        blinkAnimationState.EndingColor,
        param.progress);

    // apply the color to the strip
    for (uint16_t pixel = 0; pixel < strip.PixelCount(); pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void FadeAll(uint8_t darkenBy)
{
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
    {
        color = strip.GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}

void FadeAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        FadeAll(10);
        animations.RestartAnimation(param.index);
    }
}


void DrawTailPixelsFromColor(RgbColor color)
{
    RgbColor black(0, 0, 0);
    for (uint16_t index = 0; index < strip.PixelCount(); index++)
    {
        if(index <= TailLength)
        {
            float progress = index / float(TailLength);
            RgbColor trailColor = RgbColor::LinearBlend(black, color, progress);
            strip.SetPixelColor(index, colorGamma.Correct(trailColor));
        }
        else
        {
            strip.SetPixelColor(index, black);
        }
    }
}

void FadeInFadeOutRinseRepeat(float luminance, uint32_t targetColor)
{
    if (fadeToColor)
    {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HtmlColor(targetColor);

        blinkAnimationState.StartingColor = strip.GetPixelColor(0);
        blinkAnimationState.EndingColor = target;

        animations.StartAnimation(2, 500, BlendAnimUpdate);
    }
    else 
    {
        // fade to black
        blinkAnimationState.StartingColor = strip.GetPixelColor(0);
        blinkAnimationState.EndingColor = RgbColor(0);

        animations.StartAnimation(2, 300, BlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}

void animateLed(uint32_t color, LedState state)
{
    if(lastState == state && lastColor == color)
        return;

    if(state == rotate)
    {
        Serial.println("animateLed rotate");
        animations.StopAnimation(1);
        animations.StopAnimation(2);
        DrawTailPixelsFromColor(HtmlColor(color));
        animations.StartAnimation(0, 66, LoopAnimUpdate); 
    }
    else if(state == off)
    {
        Serial.println("animateLed off");
        animations.StopAnimation(0);
        animations.StopAnimation(2);
        animations.StartAnimation(1, 66, FadeAnimUpdate); 
    }
    else if(state == fill)
    {
        Serial.println("animateLed fill");
        animations.StopAnimation(0);
        animations.StopAnimation(1);
        animations.StopAnimation(2);
        
        strip.ClearTo(colorGamma.Correct(RgbColor(HtmlColor(color))));
    }
    else if(state == blink)
    {
        Serial.println("animateLed blink");
        animations.StopAnimation(0);
        animations.StopAnimation(1);

        FadeInFadeOutRinseRepeat(0.2f, color);
    }
    
    lastState = state;
    lastColor = color;
}

void beginLed()
{
    strip.Begin();
    strip.SetBrightness(5);
    strip.Show();
    SetRandomSeed();
}

void loopLed()
{
    if(lastState == blink && !animations.IsAnimating())
    {
        FadeInFadeOutRinseRepeat(0.2f, lastColor);
    }
    else
    {
        animations.UpdateAnimations();
        strip.Show();
    }
}