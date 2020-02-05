/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <EEPROM.h>

//Wildly inefficient use of EEPROM, for the love of god pare this down when you have time
#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32
#define DEVICE_NAME_ADDRESS 64
#define AUDIO_SOURCE_ADDRESS 96
#define SSID_OKAY 97
#define SSID_SIZE 98
#define PASSWORD_OKAY 99
#define PASSWORD_SIZE 100
#define AUDIO_SCALE 101
#define BRIGHTNESS 102
#define COLOR 103 //103, 104, 105
#define FRAME_SKIP 106
#define FRAME_DELAY 107
#define LOW_FREQUENCY 108
#define HIGH_FREQUENCY 109
#define GRADIENT 110//81-129 110-158
#define AUDIO_SOURCE 159
#define PATTERN 160
#define AUDIO_REACTION 161
#define BLENDING 162
#define EEPROM_OKAY 163

void initializeEEPROM()
{
  EEPROM.begin(512);
  uint8_t eepromOkay;
  EEPROM.get(EEPROM_OKAY, eepromOkay);
  if (eepromOkay == 70)
  {
    EEPROM.get(DEVICE_NAME_ADDRESS, deviceName);
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
    for (uint8_t address = 0; address < 48; address += 3)
    {
      EEPROM.get(GRADIENT + address, currentPalette[address].red);
      EEPROM.get(GRADIENT + address + 1, currentPalette[address].green);
      EEPROM.get(GRADIENT + address + 2, currentPalette[address].blue);
    }
    Serial.println("got");
  }
  else
  {
    EEPROM.put(DEVICE_NAME_ADDRESS, "Custom Litt Laptop");
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
    for (uint8_t address = 0; address < 48; address += 3)
    {
      EEPROM.put(GRADIENT + address, currentPalette[address].red);
      EEPROM.put(GRADIENT + address + 1, currentPalette[address].green);
      EEPROM.put(GRADIENT + address + 2, currentPalette[address].blue);
    }
    Serial.println("set");
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

void setName(String tempName)
{
  Serial.println(tempName);
  EEPROM.writeString(DEVICE_NAME_ADDRESS, tempName);
  EEPROM.commit();
}

String getName()
{
  return EEPROM.readString(DEVICE_NAME_ADDRESS);
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
