/*
Copyright (c) 2019 Owen Lyke

BLESerial

An Arduino "Serial" compatible implementation of BLE Nordic Uart Service (NUS) on ESP32

Built on top of the BLE utilities by Neil Kolban
https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
*/

/* todo:
- separate the bluetooth device and server - BLESerial should be added to an existing server before advertisement begins
- fix lorem-ipsum partial send (quits at ~231 characters). Buffer length? Or other problem?
*/


#ifndef _BLESERIAL_H_
#define _BLESERIAL_H_

#include "Arduino.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "RingBuffer.h"

#ifndef BLESERIAL_RINGBUFF_SIZE
#define BLESERIAL_RINGBUFF_SIZE 512
#endif

typedef RingBufferN<BLESERIAL_RINGBUFF_SIZE> BLESerialRingBuffer;

class BLESerial; // forward declaration

class BLESerialServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // deviceConnected = true;
    // // code here for when device connects
  };

  void onDisconnect(BLEServer* pServer) {
    // deviceConnected = false;
    // // code here for when device disconnects
  }
};

class NUSCallbacks: public BLECharacteristicCallbacks {
public:
  NUSCallbacks(BLESerial* bleserial){
    _p_bleserial = bleserial;
  }
  BLESerial* _p_bleserial;

  void onWrite(BLECharacteristic *pCharacteristic);
};

class BLESerial: public Stream
{
public:

  BLESerialRingBuffer _rx_buffer;

  BLESerial(void);
  ~BLESerial(void);

  bool begin(const char* localName);
  int available(void);
  int peek(void);
  bool hasClient(void);
  int read(void);
  size_t write(uint8_t c);
  size_t write(const uint8_t *buffer, size_t size);
  void flush();
  void end(void);
  // esp_err_t register_callback(esp_spp_cb_t * callback);

  // void enableSSP();
  // bool setPin(const char *pin);
  // bool connect(String remoteName);
  // bool connect(uint8_t remoteAddress[]);
  // bool connect();
  // bool connected(int timeout=0);
  // bool isReady(bool checkMaster=false, int timeout=0);
  // bool disconnect();
  // bool unpairDevice(uint8_t remoteAddress[]);

private:
  String local_name;

  BLEServer *pServer = NULL;
  
  BLEService *pCustomLittService = NULL;
  BLECharacteristic * pCustomLittIdCharacteristic = NULL;
  
  BLEService *pService = NULL;
  BLECharacteristic * pTxCharacteristic = NULL;
  BLECharacteristic * pRxCharacteristic = NULL;

};

#endif