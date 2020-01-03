/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <EEPROM.h>

#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32
#define AUDIO_SOURCE_ADDRESS 64
#define SSID_OKAY 65
#define SSID_SIZE 66
#define PASSWORD_OKAY 67
#define PASSWORD_SIZE 68

void initializeEEPROM()
{
  EEPROM.begin(512);
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
