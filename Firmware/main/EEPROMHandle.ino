#include <EEPROM.h>

#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32
#define AUDIO_SOURCE_ADDRESS 64
#define SSID_OKAY 65
#define PASSWORD_OKAY 66
#define SSID_SIZE 67
#define PASSWORD_SIZE 68

void setSSID(char tempSSID[], uint8_t stringSize)
{
  EEPROM.begin(512);
  for (uint8_t i = 0; i < stringSize; i++)
  {
    EEPROM.write(SSID_ADDRESS + i, tempSSID[i]);
  }
  EEPROM.write(SSID_SIZE, stringSize);
  EEPROM.write(SSID_OKAY, true);
  EEPROM.commit();
  EEPROM.end();
}

void getSSID(char tempSSID[])
{
  EEPROM.begin(512);
  uint8_t stringSize = EEPROM.read(SSID_SIZE);
  for (uint8_t i = 0; i < stringSize; i++)
  {
    char temp = EEPROM.read(SSID_ADDRESS + i);
    tempSSID[i] = temp;
  }
  EEPROM.end();
  //tempSSID[stringSize] = '\0';
}

void setPassword(char tempPassword[], uint8_t stringSize)
{
  EEPROM.begin(512);
  for (uint8_t i = 0; i < stringSize; i++)
  {
    EEPROM.put(PASSWORD_ADDRESS + i, tempPassword[i]);
  }
  EEPROM.put(PASSWORD_SIZE, stringSize);
  EEPROM.put(PASSWORD_OKAY, true);
  EEPROM.commit();
  EEPROM.end();
}

void getPassword(char tempPassword[])
{
  EEPROM.begin(512);
  uint8_t stringSize;
  EEPROM.get(PASSWORD_SIZE, stringSize);
  for (uint8_t i = 0; i < stringSize; i++)
  {
    char temp;
    EEPROM.get(PASSWORD_ADDRESS + i, temp);
    tempPassword[i] = temp;
  }
  EEPROM.end();
  tempPassword[stringSize] = '\0';
}

void setAudioSource (bool tempAudioSource)
{
  EEPROM.begin(512);
  EEPROM.put(AUDIO_SOURCE_ADDRESS, tempAudioSource);
  EEPROM.commit();
  EEPROM.end();
}

bool getAudioSource ()
{
  bool tempAudioSource;
  EEPROM.begin(512);
  EEPROM.get(AUDIO_SOURCE_ADDRESS, tempAudioSource);
  EEPROM.end();
  return tempAudioSource;
}

bool getWiFiOkay()
{
  bool ssidOkay = false;
  bool passwordOkay = false;
  EEPROM.begin(512);
  EEPROM.get(SSID_OKAY, ssidOkay);
  EEPROM.get(PASSWORD_OKAY, passwordOkay);
  EEPROM.end();
  return ssidOkay && passwordOkay;
}

