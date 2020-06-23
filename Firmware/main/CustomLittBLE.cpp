/*
Copyright (c) 2020 Owen Lyke
Header: CustomLitt_BLE.h
*/

#include "CustomLittBLE.h"

#define SERVICE_UUID_CUSTOMLITT                         "9F170100-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CL_ID                       "9F170101-899F-4F35-AD8A-85DF24B12007"

#define SERVICE_UUID_GLOBAL_PROPERTIES                  "9F170200-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_DISPLAY_BRIGHTNESS     "9F170201-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_AUDIO_SENSITIVITY      "9F170202-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_AUDIO_SOURCE           "9F170203-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_DEVICE_NAME            "9F170204-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_NETWORK_SSID           "9F170205-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_NETWORK_PASSWORD       "9F170206-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_NETWORK_CONNECT        "9F170207-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_NETWORK_IP_ADDRESS     "9F170208-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLGP_RESET                  "9F170209-899F-4F35-AD8A-85DF24B12007"

#define SERVICE_UUID_EXPRESSION_PROPERTIES              "9F170300-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_PATTERN                "9F170301-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_DELAY                  "9F170302-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_AUDIO_REACTIVITY       "9F170303-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_FFT_BOUNDS             "9F170304-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_COLOR                  "9F170305-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_GRADIENT               "9F170306-899F-4F35-AD8A-85DF24B12007"
#define CHARACTERISTIC_UUID_CLEP_GRADIENT_BLENDING      "9F170307-899F-4F35-AD8A-85DF24B12007"

#define PROPERTY_RWN (BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY)

void setCharacteristic(BLECharacteristic *pCharacteristic, const uint8_t *buffer, size_t size){
  size_t remaining = size;
  size_t sent = 0;
  while( remaining > ESP_GATT_MAX_ATTR_LEN){
    pCharacteristic->setValue((uint8_t*)(buffer + sent), ESP_GATT_MAX_ATTR_LEN);
    pCharacteristic->notify();
    remaining -= ESP_GATT_MAX_ATTR_LEN;
    sent += ESP_GATT_MAX_ATTR_LEN;
  }
  pCharacteristic->setValue((uint8_t*)(buffer + sent), remaining);
  pCharacteristic->notify();
}

void defaultCallback(BLECharacteristic* pCharacteristic){
  Serial.print("callback for characteristic: 0x");
  Serial.println((uint32_t)pCharacteristic, HEX);
}

void CustomLittBLECharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
  std::string val = pCharacteristic->getValue();
  if(_p_customlittble != NULL){
    if(_callback != NULL){
      _callback(pCharacteristic);
    }
  }
}


//
// Constructor
CustomLittBLE::CustomLittBLE(void){

}

//
// Destructor
CustomLittBLE::~CustomLittBLE(void){

}

//
// begin
bool CustomLittBLE::begin(const char* localName="CustomLittBLE Server"){
  // Create the BLE Device
  BLEDevice::init(localName);

  // Create a server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new CustomLittBLEServerCallbacks());

  // Create services
  pCustomLittService = pServer->createService(BLEUUID(SERVICE_UUID_CUSTOMLITT));
  pGlobalPropertiesService = pServer->createService(BLEUUID(SERVICE_UUID_GLOBAL_PROPERTIES), 30);
  pExpressionPropertiesService = pServer->createService(BLEUUID(SERVICE_UUID_EXPRESSION_PROPERTIES), 30);

  // Create characteristics
  // // ID Service
  pCustomLittIdCharacteristic = pCustomLittService->createCharacteristic(
                    CHARACTERISTIC_UUID_CL_ID,
                    BLECharacteristic::PROPERTY_READ
                  );

  // // Global Properties Service
  pDisplayBrightnessChar =  pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_DISPLAY_BRIGHTNESS),    PROPERTY_RWN);
  pAudioSensitivityChar =   pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_AUDIO_SENSITIVITY),     PROPERTY_RWN);
  pAudioSourceChar =        pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_AUDIO_SOURCE),          PROPERTY_RWN);
  pDeviceNameChar =         pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_DEVICE_NAME),           PROPERTY_RWN);
  pNetworkSSIDChar =        pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_NETWORK_SSID),          PROPERTY_RWN);
  pNetworkPasswordChar =    pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_NETWORK_PASSWORD),      PROPERTY_RWN);
  pNetworkConnectChar =     pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_NETWORK_CONNECT),       PROPERTY_RWN);
  pNetworkIPAddressChar =   pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_NETWORK_IP_ADDRESS),    PROPERTY_RWN);
  pResetChar =              pGlobalPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLGP_RESET),                 PROPERTY_RWN);

  pDisplayBrightnessChar->addDescriptor(new BLE2902());
  pAudioSensitivityChar->addDescriptor(new BLE2902());
  pAudioSourceChar->addDescriptor(new BLE2902());
  pDeviceNameChar->addDescriptor(new BLE2902());
  pNetworkSSIDChar->addDescriptor(new BLE2902());
  pNetworkPasswordChar->addDescriptor(new BLE2902());
  pNetworkConnectChar->addDescriptor(new BLE2902());
  pNetworkIPAddressChar->addDescriptor(new BLE2902());
  pResetChar->addDescriptor(new BLE2902());

  pDisplayBrightnessChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &displayBrightnessCallback));
  pAudioSensitivityChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &audioSensitivityCallback));
  pAudioSourceChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &audioSourceCallback));
  pDeviceNameChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &deviceNameCallback));
  pNetworkSSIDChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &networkSSIDCallback));
  pNetworkPasswordChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &networkPasswordCallback));
  pNetworkConnectChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &networkConnectCallback));
  pNetworkIPAddressChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &networkIPAddressCallback));
  pResetChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &resetCallback));

  // // Expression Properties Service
  pPatternChar =            pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_PATTERN),            PROPERTY_RWN);
  pDelayChar =              pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_DELAY),              PROPERTY_RWN);
  pAudioReactivityChar =    pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_AUDIO_REACTIVITY),   PROPERTY_RWN);
  pFFTBoundsChar =          pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_FFT_BOUNDS),         PROPERTY_RWN);
  pColorChar =              pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_COLOR),              PROPERTY_RWN);
  pGradientChar =           pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_GRADIENT),           PROPERTY_RWN);
  pGradientBlendingChar =   pExpressionPropertiesService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_CLEP_GRADIENT_BLENDING),  PROPERTY_RWN);

  pPatternChar->addDescriptor(new BLE2902());
  pDelayChar->addDescriptor(new BLE2902());
  pAudioReactivityChar->addDescriptor(new BLE2902());
  pFFTBoundsChar->addDescriptor(new BLE2902());
  pColorChar->addDescriptor(new BLE2902());
  pGradientChar->addDescriptor(new BLE2902());
  pGradientBlendingChar->addDescriptor(new BLE2902());

  pPatternChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &patternCallback));
  pDelayChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &delayCallback));
  pAudioReactivityChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &audioReactivityCallback));
  pFFTBoundsChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &fftBoundsCallback));
  pColorChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &colorCallback));
  pGradientChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &gradientCallback));
  pGradientBlendingChar->setCallbacks(new CustomLittBLECharacteristicCallbacks(this, &gradientBlendingCallback));

  // Start the service(s)
  pCustomLittService->start();
  pGlobalPropertiesService->start();
  pExpressionPropertiesService->start();

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID_CUSTOMLITT);
  pServer->getAdvertising()->start();
  return true;
}


void CustomLittBLE::setIPAddress(IPAddress address){  
  const unsigned int tempIPArrayLen = 24;
  char tempIPArray[tempIPArrayLen] = {0};
  String LocalIP = address.toString();
  LocalIP.toCharArray(tempIPArray, tempIPArrayLen);
  setCharacteristic(pNetworkIPAddressChar, (uint8_t*)tempIPArray, LocalIP.length());
}
