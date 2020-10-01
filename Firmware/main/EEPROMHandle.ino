/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <Preferences.h>
#include "EEPROMHandle.h"

Preferences preferences;
                              //".......|......." // max key length 15 chars, unique
#define PREF_NAMESPACE          "customlitt"
#define PREF_KEY_SSID           "ssid"
#define PREF_KEY_PASSWORD       "pass"
#define PREF_KEY_DEVICE_NAME    "name"
#define PREF_KEY_AUDIO_SOURCE   "source"
#define PREF_KEY_AUDIO_SCALE    "scale"
#define PREF_KEY_BRIGHTNESS     "bright"
#define PREF_KEY_COLOR          "color"
#define PREF_KEY_FRAME_SKIP     "skip"
#define PREF_KEY_FRAME_DELAY    "delay"
#define PREF_KEY_LOW_FREQUENCY  "low"
#define PREF_KEY_HIGH_FREQUENCY "high"
#define PREF_KEY_GRADIENT       "gradient"
#define PREF_KEY_PALETTE_INDEX  "index"
#define PREF_KEY_PATTERN        "pattern"
#define PREF_KEY_AUDIO_REACTION "react"
#define PREF_KEY_BLENDING       "blend"
#define PREF_KEY_WIFI_CONFIG_OK "wifiok"
#define PREF_KEY_EEPROM_OKAY    "valid"

#define EEPROM_STATUS_OKAY (0xE5)

#define RECOVERY_STRAPPING_PIN (0)

void initializeEEPROM()
{
  preferences.begin(PREF_NAMESPACE, false); // RW mode

  pinMode(RECOVERY_STRAPPING_PIN, INPUT_PULLUP);
  if(!digitalRead(RECOVERY_STRAPPING_PIN)){
    preferences.clear();
  }
  
  uint8_t status = getEEPROMStatus();
  if(status == EEPROM_STATUS_OKAY){
    Serial.println("EEPROM was already initialized");
    ssid = getSSID();
    password = getPassword();
    deviceName = getDeviceName();
    audioSource = getAudioSource();
    audioScale = getAudioScale();
    brightness = getBrightness();
    customColor = getCustomColor();
    frameSkip = getFrameSkip();
    frameDelay = getFrameDelay();
    avgLowEnd = getAvgLowEnd();
    avgHighEnd = getAvgHighEnd();
    currentPalette = getCurrentPalette();
    patternNum = getPatternNum();
    audioReaction = getAudioReaction();
    currentBlending = getGradientBlending();
    wifiStatus = getWiFiStatus();

//    if(wifiStatus){
//      initArtnet();
//    }

    wifiStatus = false;
    storeWiFiStatus(wifiStatus);
    
  }else{
    Serial.println("Configuring EEPROM for the first time");
    storeEEPROMStatus(EEPROM_STATUS_OKAY);
    storeSSID(ssid);
    storePassword(password);
    storeDeviceName(deviceName);
    storeAudioSource(audioSource);
    storeAudioScale(audioScale);
    storeBrightness(brightness);
    storeCustomColor(customColor);
    storeFrameSkip(frameSkip);
    storeFrameDelay(frameDelay);
    storeAvgLowEnd(avgLowEnd);
    storeAvgHighEnd(avgHighEnd);
    storeCurrentPalette(currentPalette);
    storePatternNum(patternNum);
    storeAudioReaction(audioReaction);
    storeGradientBlending(currentBlending);
    storeWiFiStatus(wifiStatus);
  }
}

//
// getters / store-ers

//
// SSID
String getSSID( void ){
  return preferences.getString(PREF_KEY_SSID, ssid);
}
size_t storeSSID(String val){
  return preferences.putString(PREF_KEY_SSID, val);
}

//
// Password
String getPassword( void ){
  return preferences.getString(PREF_KEY_PASSWORD, password);
}
size_t storePassword(String val){
  return preferences.putString(PREF_KEY_PASSWORD, val);
}

//
// Device Name
String getDeviceName( void ){
  return preferences.getString(PREF_KEY_DEVICE_NAME, deviceName);
}
size_t storeDeviceName(String val){
  return preferences.putString(PREF_KEY_DEVICE_NAME, val);
}

//
// Audio Source
bool getAudioSource( void ){
  return (preferences.getUChar(PREF_KEY_AUDIO_SOURCE, (uint8_t)audioSource)) ? true : false;
}
size_t storeAudioSource(bool val){
  return preferences.putUChar(PREF_KEY_AUDIO_SOURCE, (val) ? 0x01 : 0x00);
}

//
// Audio Scale
float getAudioScale( void ){
  return preferences.getFloat(PREF_KEY_AUDIO_SCALE, audioScale);
}
size_t storeAudioScale(float val){
  return preferences.putFloat(PREF_KEY_AUDIO_SCALE, val);
}

//
// Brightness
uint8_t getBrightness( void ){
  return preferences.getUChar(PREF_KEY_BRIGHTNESS, brightness);
}
size_t storeBrightness(uint8_t val){
  return preferences.putUChar(PREF_KEY_BRIGHTNESS, val);
}

//
// Custom Color
typedef struct _color_t {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;

CRGB getCustomColor( void ){
  CRGB retval;
  color_t color;
  size_t got = preferences.getBytes(PREF_KEY_COLOR, (void*)&color, (sizeof(color_t)/sizeof(uint8_t)));
  if(!got){
    retval.r = customColor.r;
    retval.g = customColor.g;
    retval.b = customColor.b;
    return retval;
  }
  retval.r = color.r;
  retval.g = color.g;
  retval.b = color.b;
  return retval;
}
size_t storeCustomColor(CRGB val){
  color_t color;
  color.r = val.r;
  color.g = val.g;
  color.b = val.b;
  return preferences.putBytes(PREF_KEY_COLOR, (const void*)&color, (sizeof(color_t)/sizeof(uint8_t)));
}

//
// Frame Skip
uint8_t getFrameSkip( void ){
  return preferences.getUChar(PREF_KEY_FRAME_SKIP, frameSkip);
}
size_t storeFrameSkip(uint8_t val){
  return preferences.putUChar(PREF_KEY_FRAME_SKIP, val);
}

//
// Frame Delay
uint8_t getFrameDelay( void ){
  return preferences.getUChar(PREF_KEY_FRAME_DELAY, frameDelay);
}
size_t storeFrameDelay(uint8_t val){
  return preferences.putUChar(PREF_KEY_FRAME_DELAY, val);
}

//
// Avg Low End
uint8_t getAvgLowEnd( void ){
  return preferences.getUChar(PREF_KEY_LOW_FREQUENCY, avgLowEnd);
}
size_t storeAvgLowEnd(uint8_t val){
  return preferences.putUChar(PREF_KEY_LOW_FREQUENCY, val);
}

//
// Avg High End
uint8_t getAvgHighEnd( void ){
  return preferences.getUChar(PREF_KEY_HIGH_FREQUENCY, avgLowEnd);
}
size_t storeAvgHighEnd(uint8_t val){
  return preferences.putUChar(PREF_KEY_HIGH_FREQUENCY, val);
}

//
// Current Palette
typedef struct _gradient_t {
  color_t c0;
  color_t c1;
  color_t c2;
  color_t c3;
  color_t c4;
  color_t c5;
  color_t c6;
  color_t c7;
  color_t c8;
  color_t c9;
  color_t c10;
  color_t c11;
  color_t c12;
  color_t c13;
  color_t c14;
  color_t c15;
} gradient_t;

CRGBPalette16 getCurrentPalette( void ){
  CRGBPalette16 retval;
  gradient_t gradient;
  size_t got = preferences.getBytes(PREF_KEY_GRADIENT, (void*)&gradient, (sizeof(gradient_t)/sizeof(uint8_t)));
  if(!got){
    return currentPalette;
  }
  color_t* base = (color_t*)&gradient;
  color_t* color = NULL;
  for(int idx = 0; idx < 16; idx++){
    color = (base + idx);
    retval[idx].r = color->r;
    retval[idx].g = color->g;
    retval[idx].b = color->b;
  }
  return retval;
}
size_t storeCurrentPalette(CRGBPalette16 val){
  gradient_t gradient;
  color_t* base = (color_t*)&gradient;
  color_t* color = NULL;
  for(int idx = 0; idx < 16; idx++){
    color = (base + idx);
    color->r = val[idx].r;
    color->g = val[idx].g;
    color->b = val[idx].b;
  }
  return preferences.putBytes(PREF_KEY_GRADIENT, (const void*)&gradient, (sizeof(gradient_t)/sizeof(uint8_t)));
}

// Palette Index
uint8_t getPaletteIndex( void ){
  return preferences.getUChar(PREF_KEY_PALETTE_INDEX, patternNum);
}
size_t storePaletteIndex(uint8_t val){
  return preferences.putUChar(PREF_KEY_PALETTE_INDEX, val);
}

//
// Pattern Num
uint8_t getPatternNum( void ){
  return preferences.getUChar(PREF_KEY_PATTERN, patternNum);
}
size_t storePatternNum(uint8_t val){
  return preferences.putUChar(PREF_KEY_PATTERN, val);
}

//
// Audio Reaction
bool getAudioReaction( void ){
  return (preferences.getUChar(PREF_KEY_AUDIO_REACTION, (uint8_t)audioReaction)) ? true : false;
}
size_t storeAudioReaction(bool val){
  return preferences.putUChar(PREF_KEY_AUDIO_REACTION, (val) ? 0x01 : 0x00);
}

//
// Gradient Blending
TBlendType getGradientBlending( void ){
  return (preferences.getUChar(PREF_KEY_BLENDING, (uint8_t)audioReaction)) ? LINEARBLEND : NOBLEND;
}
size_t storeGradientBlending(TBlendType val){
  return preferences.putUChar(PREF_KEY_BLENDING, (val == LINEARBLEND) ? 0x01 : 0x00);
}

//
// WiFi Configured OK
bool   getWiFiStatus( void ){
  uint8_t got = preferences.getUChar(PREF_KEY_WIFI_CONFIG_OK, wifiStatus);
  
  Serial.print("getting wifiStatus: ");
  Serial.println(got);
  
  return (got) ? true : false; 
}
size_t storeWiFiStatus(bool val){
  
  Serial.print("Storing wifiStatus: ");
  Serial.println(val);
  
  return preferences.putUChar(PREF_KEY_WIFI_CONFIG_OK, (val) ? 0x01 : 0x00);
}

//
// EEPROM Status
uint8_t getEEPROMStatus( void ){
  return preferences.getUChar(PREF_KEY_EEPROM_OKAY, 0);
}
size_t storeEEPROMStatus(uint8_t val){
  return preferences.putUChar(PREF_KEY_EEPROM_OKAY, val);
}
