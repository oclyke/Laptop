#include <EEPROM.h>

#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32
#define AUDIO_SOURCE_ADDRESS 33

void setSSID(char *tempSsid)
{
  Serial.println(tempSsid);
  EEPROM.put(SSID_ADDRESS, tempSsid);
}

char * getSSID()
{
  char *tempSsid;
  return EEPROM.get(SSID_ADDRESS, tempSsid);
}

void setPassword(char *tempPassword)
{
  EEPROM.put(PASSWORD_ADDRESS, tempPassword);
}

char * getPassword()
{
  char *tempPassword;
  return EEPROM.get(PASSWORD_ADDRESS, tempPassword);
}

void setAudioSource (bool tempAudioSource)
{
  EEPROM.put(AUDIO_SOURCE_ADDRESS, tempAudioSource);
}

bool getAudioSource ()
{
  bool tempAudioSource;
  return EEPROM.get(AUDIO_SOURCE_ADDRESS, tempAudioSource); 
}
