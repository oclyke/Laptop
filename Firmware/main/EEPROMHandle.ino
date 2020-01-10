/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <EEPROM.h>

//Wildly inefficient use of EEPROM, for the love of god pare this down when you have time
#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32
#define AUDIO_SOURCE_ADDRESS 64
#define SSID_OKAY 65
#define SSID_SIZE 66
#define PASSWORD_OKAY 67
#define PASSWORD_SIZE 68
#define AUDIO_SCALE 69
#define BRIGHTNESS 73
#define COLOR 74 //74,75,76
#define FRAME_SKIP 77
#define FRAME_DELAY 78
#define LOW_FREQUENCY 79
#define HIGH_FREQUENCY 80
#define GRADIENT 81 //81-95
#define AUDIO_SOURCE 96
#define PATTERN 97
#define AUDIO_REACTION 98
#define BLENDING 99
#define EEPROM_OKAY 100

void initializeEEPROM()
{
  EEPROM.begin(512);
  uint8_t eepromOkay;
  EEPROM.get(EEPROM_OKAY, eepromOkay);
  if (eepromOkay == 69)
  {
    EEPROM.get(AUDIO_SCALE, audioScale);
    EEPROM.get(BRIGHTNESS, brightness);
    EEPROM.get(FRAME_SKIP, frameSkip);
    EEPROM.get(FRAME_DELAY, frameDelay);
    EEPROM.get(LOW_FREQUENCY, avgLowEnd);
    EEPROM.get(HIGH_FREQUENCY, avgHighEnd);
    EEPROM.get(AUDIO_SOURCE, audioSource);
    EEPROM.get(PATTERN, patternNum);
    EEPROM.get(AUDIO_REACTION, audioReaction);
    EEPROM.get(BLENDING, currentBlending);
    Serial.print("got");
  }
  else
  {
    EEPROM.put(AUDIO_SCALE, audioScale);
    EEPROM.put(BRIGHTNESS, brightness);
    EEPROM.put(FRAME_SKIP, frameSkip);
    EEPROM.put(FRAME_DELAY, frameDelay);
    EEPROM.put(LOW_FREQUENCY, avgLowEnd);
    EEPROM.put(HIGH_FREQUENCY, avgHighEnd);
    EEPROM.put(AUDIO_SOURCE, audioSource);
    EEPROM.put(PATTERN, patternNum);
    EEPROM.put(AUDIO_REACTION, audioReaction);
    EEPROM.put(BLENDING, currentBlending);
    EEPROM.put(EEPROM_OKAY, 69);
    Serial.print("set");
  }
}

void setCharacteristic(uint8_t address, uint8_t value)
{
  EEPROM.put(address, value);
  EEPROM.commit();
}

void setFloatCharacteristic(uint8_t address, float value)
{
  EEPROM.put(address, value);
  EEPROM.commit();
}

void setColor(uint8_t address, CRGB color)
{
  EEPROM.put(address, color.red);
  EEPROM.put(address + 1, color.green);
  EEPROM.put(address + 2, color.blue);
  EEPROM.commit();
}

uint8_t getCharacteristic(uint8_t address)
{
  uint8_t value;
  EEPROM.get(address, value);
  return value;
}

float getFloatCharacteristic(uint8_t address)
{
  float value;
  EEPROM.get(address, value);
  return value;
}

CRGB getColor(uint8_t address)
{
  CRGB tempColor;
  for (uint8_t colorToWrite = 0; colorToWrite < 3; colorToWrite++)
  {
    uint8_t temp;
    EEPROM.get(address + colorToWrite, temp);
    tempColor[colorToWrite] = temp;
  }
  return tempColor;
}

void setSSID(String tempSsid)
{
  Serial.println(tempSsid);
  EEPROM.writeString(SSID_ADDRESS, tempSsid);
  EEPROM.put(SSID_OKAY, true);
  EEPROM.commit();
}

String getSSID()
{
  return EEPROM.readString(SSID_ADDRESS);
}

void setPassword(String tempPassword)
{
  Serial.println(tempPassword);
  EEPROM.writeString(PASSWORD_ADDRESS, tempPassword);
  EEPROM.put(PASSWORD_OKAY, true);
  EEPROM.commit();
}

String getPassword()
{
  return EEPROM.readString(PASSWORD_ADDRESS);
}

void setAudioSource (bool tempAudioSource)
{
  EEPROM.put(AUDIO_SOURCE_ADDRESS, tempAudioSource);
  EEPROM.commit();
}

bool getAudioSource ()
{
  bool tempAudioSource;
  return EEPROM.get(AUDIO_SOURCE_ADDRESS, tempAudioSource);
}

bool getWiFiOkay()
{
  bool ssidOkay = false;
  bool passwordOkay = false;
  EEPROM.get(SSID_OKAY, ssidOkay);
  EEPROM.get(PASSWORD_OKAY, passwordOkay);
  EEPROM.commit();
  Serial.println(ssidOkay & passwordOkay);
  Serial.println(passwordOkay);
  return ssidOkay && passwordOkay;
}
