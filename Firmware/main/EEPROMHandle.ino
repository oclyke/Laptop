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

#define AUDIO_SCALE_PS1 201
#define BRIGHTNESS_PS1 202
#define COLOR_PS1 203 //103, 104, 105
#define FRAME_SKIP_PS1 206
#define FRAME_DELAY_PS1 207
#define LOW_FREQUENCY_PS1 208
#define HIGH_FREQUENCY_PS1 209
#define GRADIENT_PS1 210//81-129 110-158
#define AUDIO_SOURCE_PS1 259
#define PATTERN_PS1 260
#define AUDIO_REACTION_PS1 261
#define BLENDING_PS1 262
#define EEPROM_OKAY_PS1 263

#define AUDIO_SCALE_PS2 301
#define BRIGHTNESS_PS2 302
#define COLOR_PS2 303 //103, 104, 105
#define FRAME_SKIP_PS2 306
#define FRAME_DELAY_PS2 307
#define LOW_FREQUENCY_PS2 308
#define HIGH_FREQUENCY_PS2 309
#define GRADIENT_PS2 310//81-129 110-158
#define AUDIO_SOURCE_PS2 359
#define PATTERN_PS2 360
#define AUDIO_REACTION_PS2 361
#define BLENDING_PS2 362
#define EEPROM_OKAY_PS2 363

#define AUDIO_SCALE_PS3 401
#define BRIGHTNESS_PS3 402
#define COLOR_PS3 403 //103, 104, 105
#define FRAME_SKIP_PS3 406
#define FRAME_DELAY_PS3 407
#define LOW_FREQUENCY_PS3 408
#define HIGH_FREQUENCY_PS3 409
#define GRADIENT_PS3 410//81-129 110-158
#define AUDIO_SOURCE_PS3 459
#define PATTERN_PS3 460
#define AUDIO_REACTION_PS3 461
#define BLENDING_PS3 462
#define EEPROM_OKAY_PS3 463

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
