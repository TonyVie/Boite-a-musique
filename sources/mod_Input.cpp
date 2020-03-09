#include "mod_Input.h"
#include "mod_Menu.h"

#include <JC_Button.h>

#define encoderPinA A1
#define encoderPinB A2
#define encoderPinButton A3

const unsigned long
    LONG_PRESS(1000);           // we define a "long press" to be 1000 milliseconds.

int rotaryState;
int lastRotaryState;  
int debounceTime = 50;

Button encoderButton(encoderPinButton, debounceTime);

void beginInput()
{
    encoderButton.begin();

    pinMode (encoderPinA,INPUT);
    pinMode (encoderPinB,INPUT);
    lastRotaryState = digitalRead(encoderPinA); 
}

bool pressedForAccepted = false;
void loopLInput()
{
    encoderButton.read();

    if(encoderButton.pressedFor(LONG_PRESS))
    {
        if(pressedForAccepted == false)
            triggerAltSelectMenu();

        pressedForAccepted = true;
    }
    else
    {
        if(encoderButton.wasReleased())
        {
            if(pressedForAccepted == false)
               triggerSelectMenu();

            pressedForAccepted = false;
        }
    }

    rotaryState = digitalRead(encoderPinA); // Reads the "current" state of the encoderPinA
    // If the previous and the current state of the encoderPinA are different, that means a Pulse has occured
    if (rotaryState != lastRotaryState)
    {     
        // If the encoderPinB state is different to the encoderPinA state, that means the encoder is rotating clockwise
        if (digitalRead(encoderPinB) != rotaryState)
        {
            triggerNextMenu();
        }
        else
        {
            triggerPreviousMenu();
        }

        displayMenu();
    }     
    lastRotaryState = rotaryState; // Updates the previous state of the encoderPinA with the current state
}