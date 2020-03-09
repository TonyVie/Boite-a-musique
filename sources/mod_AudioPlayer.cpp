#include "mod_AudioPlayer.h"

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

SoftwareSerial mp3PlayerSerial(11, 10); // RX, TX
DFPlayerMini_Fast mp3Player;

uint8_t currentVolume = 20;

void beginAudioPlayer()
{
    mp3PlayerSerial.begin(9600);
    mp3Player.begin(mp3PlayerSerial);
    
    mp3Player.volume(currentVolume);
    mp3Player.EQSelect(EQ_POP);
}

void loopAudioPlayer()
{
    int sensorValue = analogRead(A0);
    uint8_t newVolume = (sensorValue / 1023.0 * 45.0); //30*1.5

    if(abs(newVolume - currentVolume) >= 2) //if(newVolume != currentVolume)
    {
        currentVolume = newVolume;
        mp3Player.volume(currentVolume);
    }
}

void setAudioPlayerPlaying(bool shouldPlay)
{
    if(shouldPlay)
        playAudioPlayer();
    else
        pauseAudioPlayer();
}

void playAudioPlayer()
{
    Serial.println("playAudioPlayer");
    mp3Player.resume();
}

void pauseAudioPlayer()
{
    Serial.println("pauseAudioPlayer");
    mp3Player.pause();
}

void nextAudioPlayer()
{
    Serial.println("nextAudioPlayer");
    mp3Player.playNext();
}

void previousAudioPlayer()
{
    Serial.println("previousAudioPlayer");
    mp3Player.playPrevious();
}

int currentFolderIndex = -1;
void loadAudioPlayerFolder(int folderIndex)
{
    if(folderIndex == currentFolderIndex)
        return;
    Serial.print("loadAudioPlayerFolder: ");
    Serial.println(folderIndex);
    currentFolderIndex = folderIndex;
    mp3Player.repeatFolder(currentFolderIndex);
}
