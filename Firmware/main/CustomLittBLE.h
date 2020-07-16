/*
Copyright (c) 2020 Owen Lyke

CustomLittBLE

Direct characteristic mapping of CustomLitt properties (global and expression-specific)

Built on top of the BLE utilities by Neil Kolban
https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
*/

/* todo:
- separate the bluetooth device and server - BLESerial should be added to an existing server before advertisement begins
- fix lorem-ipsum partial send (quits at ~231 characters). Buffer length? Or other problem?
*/


#ifndef _CUSTOMLITTBLE_H_
#define _CUSTOMLITTBLE_H_

#include "Arduino.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <FastLED.h>

#include "bluetoothInterface.h"

class CustomLittBLE; // forward declaration

class CustomLittBLEServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // deviceConnected = true;
    // // code here for when device connects
  };

  void onDisconnect(BLEServer* pServer) {
    // deviceConnected = false;
    // // code here for when device disconnects
  }
};

typedef void (*CustomLittBLECallback_fn)(BLECharacteristic* pCharacteristic);

class CustomLittBLECharacteristicCallbacks: public BLECharacteristicCallbacks {
public:
  CustomLittBLECharacteristicCallbacks(CustomLittBLE* p_customlittble, CustomLittBLECallback_fn callback){
    _p_customlittble = p_customlittble;
    _callback = callback;
  }
  
  CustomLittBLE* _p_customlittble = NULL;
  CustomLittBLECallback_fn _callback = NULL;

  void onWrite(BLECharacteristic *pCharacteristic);
};

class CustomLittBLE {
public:

  CustomLittBLE();
  ~CustomLittBLE();

  bool begin(const char* localName);

  void setDisplayBrightness(uint8_t brightness);
  void setAudioSensitivity(float sensitivity);
  void setAudioSource(uint8_t source);
  void setDeviceName(String name);
  void setNetworkSSID(String ssid);
  void setNetworkPassword(String password);
  void setIPAddress(IPAddress address);
  void setNetworkStatus(bool status);

  void setPattern(uint8_t pattern);
  void setDelay(uint8_t frames, uint16_t ms);
  void setAudioReactivity(uint8_t reactivity);
  void setFFTBounds(uint8_t low, uint8_t high);
  void setColor(uint8_t r, uint8_t g, uint8_t b);
  void setGradientIndex(uint8_t index);
  void setGradientColor(uint8_t r, uint8_t g, uint8_t b);
  void setGradientBlending(uint8_t blending);

  void setGradient(CRGBPalette16 palette);

  
  
  
  

private:
//  String local_name;

  BLEServer *pServer = NULL;
  
  BLEService *pCustomLittService = NULL;
  BLECharacteristic * pCustomLittIdCharacteristic = NULL;

  BLEService *pGlobalPropertiesService = NULL;
  BLECharacteristic * pDisplayBrightnessChar = NULL;
  BLECharacteristic * pAudioSensitivityChar = NULL;
  BLECharacteristic * pAudioSourceChar = NULL;
  BLECharacteristic * pDeviceNameChar = NULL;
  BLECharacteristic * pNetworkSSIDChar = NULL;
  BLECharacteristic * pNetworkPasswordChar = NULL;
  BLECharacteristic * pNetworkConnectChar = NULL;
  BLECharacteristic * pNetworkIPAddressChar = NULL;
  BLECharacteristic * pNetworkStatusChar = NULL;
  BLECharacteristic * pResetChar = NULL;
  
  BLEService *pExpressionPropertiesService = NULL;
  BLECharacteristic * pPatternChar = NULL;
  BLECharacteristic * pDelayChar = NULL;
  BLECharacteristic * pAudioReactivityChar = NULL;
  BLECharacteristic * pFFTBoundsChar = NULL;
  BLECharacteristic * pColorChar = NULL;
  BLECharacteristic * pGradientIndexChar = NULL;
  BLECharacteristic * pGradientColorChar = NULL;
  BLECharacteristic * pGradientBlendingChar = NULL;
};

#endif // _CUSTOMLITTBLE_H_
