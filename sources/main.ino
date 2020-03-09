
#include "mod_Led.h"
#include "mod_Input.h"
#include "mod_Menu.h"
#include "mod_AudioPlayer.h"

void setup() 
{ 
    Serial.begin (9600);
    
    beginInput();
    beginMenu();
    beginLed();
    beginAudioPlayer();

    Serial.println("setup done");
} 

 void loop() 
 { 
    loopMenu();
    loopLInput();
    loopLed();
    loopAudioPlayer();
 }
