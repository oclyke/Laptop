/*
Copyright (c) 2019 Owen Lyke
Header: BLESerial.h
*/

#include "BLESerial.h"

#define SERVICE_UUID_CUSTOMLITT   "9F170001-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CL_ID "9F170002-899F-4F35-AD8A-85DF24B12007"

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


void NUSCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
  std::string rxValue = pCharacteristic->getValue();
  if(_p_bleserial != NULL){
    if (rxValue.length() > 0) {
      for (int i = 0; i < rxValue.length(); i++){
        _p_bleserial->_rx_buffer.store_char(rxValue[i]);
      }
    }
  }
}


//
// Constructor
BLESerial::BLESerial(void){

}

//
// Destructor
BLESerial::~BLESerial(void){

}

//
// begin
bool BLESerial::begin(const char* localName="UART Service"){
  // Create the BLE Device
  BLEDevice::init(localName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLESerialServerCallbacks());

  // Create the BLE Service
  pCustomLittService = pServer->createService(SERVICE_UUID_CUSTOMLITT);
  pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCustomLittIdCharacteristic = pCustomLittService->createCharacteristic(
                    CHARACTERISTIC_UUID_CL_ID,
                    BLECharacteristic::PROPERTY_READ
                  );
                  
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
  pTxCharacteristic->addDescriptor(new BLE2902());

  pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);
  pRxCharacteristic->setCallbacks(new NUSCallbacks(this));

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID_CUSTOMLITT);
  pServer->getAdvertising()->start();
  return true;
}


int BLESerial::available(void){
  return _rx_buffer.available();
}

int BLESerial::peek(void){
  return _rx_buffer.peek();
}

bool BLESerial::hasClient(void){
  // Serial.println("BLESerial: Not Implemented");
}

int BLESerial::read(void){
  return _rx_buffer.read_char();
}

size_t BLESerial::write(uint8_t c){
  return write(&c, 1);
}

size_t BLESerial::write(const uint8_t *buffer, size_t size){
  size_t remaining = size;
  size_t sent = 0;
  while( remaining > ESP_GATT_MAX_ATTR_LEN){
    pTxCharacteristic->setValue((uint8_t*)(buffer + sent), ESP_GATT_MAX_ATTR_LEN);
    pTxCharacteristic->notify();
    remaining -= ESP_GATT_MAX_ATTR_LEN;
    sent += ESP_GATT_MAX_ATTR_LEN;
  }
  pTxCharacteristic->setValue((uint8_t*)(buffer + sent), remaining);
  pTxCharacteristic->notify();
}

void BLESerial::flush(){
  while(read() >= 0){};
}

void BLESerial::end(void){
  // Serial.println("BLESerial: Not Implemented");
}

// esp_err_t register_callback(esp_spp_cb_t * callback){
//   Serial.println("BLESerial: Not Implemented");
// }
