/*
Copyright (c) 2020 Owen Lyke
function declarations for the bluetooth interface
*/

#ifndef _BLUETOOTH_INTERFACE_H_
#define _BLUETOOTH_INTERFACE_H_

#include "CustomLittBLE.h" // includes necessary BLE headers

void displayBrightnessCallback(BLECharacteristic* pCharacteristic);
void audioSensitivityCallback(BLECharacteristic* pCharacteristic);
void audioSourceCallback(BLECharacteristic* pCharacteristic);
void deviceNameCallback(BLECharacteristic* pCharacteristic);
void networkSSIDCallback(BLECharacteristic* pCharacteristic);
void networkPasswordCallback(BLECharacteristic* pCharacteristic);
void networkIPAddressCallback(BLECharacteristic* pCharacteristic);
void networkStatusCallback(BLECharacteristic* pCharacteristic);
void resetCallback(BLECharacteristic* pCharacteristic);

void patternCallback(BLECharacteristic* pCharacteristic);
void delayCallback(BLECharacteristic* pCharacteristic);
void audioReactivityCallback(BLECharacteristic* pCharacteristic);
void fftBoundsCallback(BLECharacteristic* pCharacteristic);
void colorCallback(BLECharacteristic* pCharacteristic);
void gradientIndexCallback(BLECharacteristic* pCharacteristic);
void gradientColorCallback(BLECharacteristic* pCharacteristic);
void gradientBlendingCallback(BLECharacteristic* pCharacteristic);

#endif // _BLUETOOTH_INTERFACE_H_
