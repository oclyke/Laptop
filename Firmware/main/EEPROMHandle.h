/*
Copyright (c) 2020 Owen Lyke
*/

#ifndef _EEPROMHANDLE_H_
#define _EEPROMHANDLE_H_

#include "Arduino.h"

String        getSSID( void );
String        getPassword( void );
String        getDeviceName( void );
bool          getAudioSource( void );
float         getAudioScale( void );
uint8_t       getBrightness( void );
CRGB          getCustomColor( void );
uint8_t       getFrameSkip( void );
uint8_t       getFrameDelay( void );
uint8_t       getAvgLowEnd( void );
uint8_t       getAvgHighEnd( void );
CRGBPalette16 getCurrentPalette( void );
uint8_t       getPaletteIndex( void );
uint8_t       getPatternNum( void );
bool          getAudioReaction( void );
TBlendType    getGradientBlending( void );
bool          getWiFiStatus( void );
uint8_t       getEEPROMStatus( void );

size_t storeSSID(String val);
size_t storePassword(String val);
size_t storeDeviceName(String val);
size_t storeAudioSource(bool val);
size_t storeAudioScale(float val);
size_t storeBrightness(uint8_t val);
size_t storeCustomColor(CRGB val);
size_t storeFrameSkip(uint8_t val);
size_t storeFrameDelay(uint8_t val);
size_t storeAvgLowEnd(uint8_t val);
size_t storeAvgHighEnd(uint8_t val);
size_t storeCurrentPalette(CRGBPalette16 val);
size_t storePaletteIndex(uint8_t val);
size_t storePatternNum(uint8_t val);
size_t storeAudioReaction(bool val);
size_t storeGradientBlending(TBlendType val);
size_t storeWiFiStatus(bool val);
size_t storeEEPROMStatus(uint8_t val);

#endif // _EEPROMHANDLE_H_
