/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

/*Note that all functions are case sensitive*/

/* Function
   Example string
   Range
*/

#include "RingBuffer.h"

#ifndef PARSER_RINGBUFF_SIZE
#define PARSER_RINGBUFF_SIZE 64
#endif

typedef RingBufferN<PARSER_RINGBUFF_SIZE> ParserRingBuffer;

class CharacteristicParser: public Stream {
public:
  ParserRingBuffer _buffer;

  CharacteristicParser();

  int available(void);
  int peek(void);
  int read(void);

  size_t write(uint8_t c);
  size_t write(const uint8_t *buffer, size_t size);
  void flush(void);

  int loadCharacteristicValue(BLECharacteristic* pCharacteristic);
  
};

CharacteristicParser::CharacteristicParser(){
  
}

int CharacteristicParser::available(void){
  return _buffer.available();
}

int CharacteristicParser::peek(void){
  return _buffer.peek();
}

int CharacteristicParser::read(void){
  return _buffer.read_char();
}

size_t CharacteristicParser::write(uint8_t c){
  return write(&c, 1);
}

size_t CharacteristicParser::write(const uint8_t *buffer, size_t size){
  return 0;
}

void CharacteristicParser::flush( void ){
  while(read() >= 0){};
}

int CharacteristicParser::loadCharacteristicValue(BLECharacteristic* pCharacteristic){
  if(pCharacteristic == NULL){ return 0; }
  std::string value = pCharacteristic->getValue();
  for (int i = 0; i < value.length(); i++){
    _buffer.store_char(value[i]);
  }
  _buffer.store_char(' '); // terminating in a space allows the parser to find the end of an input easily
  return 0;
}

CharacteristicParser Parser;





// // global property characteristics

/*
  Brightness
  127
  0-255
*/
void displayBrightnessCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  brightness = Parser.parseInt();
  FastLED.setBrightness(brightness);
  storeBrightness(brightness);
  Parser.flush();
}

/*
  Audio Sensitivity
  0.69
  0.0-1.0
*/
void audioSensitivityCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioScale = Parser.parseFloat();
  storeAudioScale(audioScale);
  Parser.flush();
}

/*
  Change audio source between microphone and audio jack
  1 [MIC]
  0 [JACK]
*/
void audioSourceCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioSource = Parser.parseInt();
  resetColorIndex();
  storeAudioSource(audioSource);
  Parser.flush();
}

/*
   Change the name of the Device
   String : name (up to 32 char)
*/
void deviceNameCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempName[33];
  uint8_t position = 0;
  while (Parser.available())
  {
    char temp = Parser.read();
    tempName[position++] = temp;
    if(position > 31){ break; }
  }
  tempName[position] = 0; // null terminate
  deviceName = tempName;
  storeDeviceName(deviceName);
  Parser.flush();
}

/*
  Change the SSID to connect to for ArtNet 
  String : SSID (up to 32 characters)
*/
void networkSSIDCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempSsid[33];
  uint8_t position = 0;
  
  Serial.println("Getting SSID");
  Serial.print("Available: "); Serial.println(Parser.available());
  
  
  while (Parser.available() > 1)
  {
    char temp = Parser.read();
    tempSsid[position++] = temp;
    if(position > 31){ break; }

//    Serial.print("0x");
//    if(temp < 0x10){
//      Serial.print("0");
//    }
//    Serial.print(temp, HEX);
//    Serial.print(", ");
  }
  tempSsid[position] = 0; // null terminate
  
  ssid = String(tempSsid);
  storeSSID(ssid);
  Parser.flush();
}

/*
  Change the Password to connect to for ArtNet
  String : password (up to 32 characters)
*/
void networkPasswordCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempPassword[33];
  uint8_t position = 0;
  while (Parser.available() > 1)
  {
    char temp = Parser.read();
    tempPassword[position++] = temp;
    if(position > 31){ break; }
  }
  tempPassword[position] = 0; // null terminate
  password = String(tempPassword);
  storePassword(password);
  Parser.flush();
}

/*
  Read the device's IP Address from this characteristic (read only)
*/
void networkIPAddressCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  // browser does not set ip address
  Parser.flush();
}

/* 
  Write to this characteristic to request a change in network status
  Read this characteristic (or be notified) to determine the actual network status
  1 [network is connected and operational]
  0 [network is disconnected and non-operational]
*/
void networkStatusCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  bool stat = (Parser.parseInt() == 0x00) ? false : true;
  if(stat != 0){
    initArtnet();
  }else{
    deinitArtnet();
  }
  Parser.flush();
}


/*
  Write to this characteristic to reset the color index that has been changed by audio reactive stuff (write only)
  any value
*/
void resetCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  resetColorIndex();
  Parser.flush();
}


// // expression specific property characteristics

/*
  Change pattern utilized
  3 (changes to the 4th pattern, index at 0)
  range determined by number of drawing methods in main loop
*/
void patternCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  patternNum = Parser.parseInt();
  storePatternNum(patternNum);
  onPatternChange(patternNum);
  Parser.flush();
}

/*
  Change delay and how many frames to skip (only works on non audio reactive stuff)
  3 5 [skip 3 frames, delay 5 ms]
  0-255 0-5000?
*/
void speedFactorCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  speedFactor = Parser.parseFloat();
  storeSpeedFactor(speedFactor);
  Parser.flush();
}

/*
  Changes whether or not the pattern is audio reactive
  1 [audio reaction on]
  0 [audio reaction off]
*/
void audioReactivityCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioReaction = Parser.parseInt();
  storeAudioReaction(audioReaction);
  Parser.flush();
}

/*
  change lower and upper frequency bounds on fftAvg function. Controls which frequencies are used to set speeds
  4 12 [sets low bound to 4 and high to 12]
  0-20 0-20
  Make sure that fl is never higher than fh
*/
void fftBoundsCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  avgLowEnd = Parser.parseInt();
  avgHighEnd = Parser.parseInt();
  storeAvgLowEnd(avgLowEnd);
  storeAvgHighEnd(avgHighEnd);
  Parser.flush();
}

/*
  Change Color used in colorSet
  255 0 64 (changes color to all red, no green, and a little blue for a purple, similar use to gradient change)
  0-255 0-255 0-255
*/
void colorCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  customColor.red = Parser.parseInt();
  customColor.green = Parser.parseInt();
  customColor.blue = Parser.parseInt();
  storeCustomColor(customColor);
  Parser.flush();
}


/*
  Change index of current gradient element
  3 (changes the index to the 4th element of the gradient. subsequent operations to the GRADIENT_COLOR characteristic will operate on that element)
  0-15
*/
void gradientIndexCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  paletteIndex = Parser.parseInt();
  storePaletteIndex(paletteIndex);
  CRGB color = currentPalette[paletteIndex];
  BLE.setGradientColor(color.r, color.g, color.b);
  Parser.flush();
}

/*
  Change color of current gradient element
  255 0 64 (changes second color in gradient to purple, first number is index of gradient, followed by the color you would like to change it to)
   0-255 0-255 0-255
*/
void gradientColorCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  uint8_t red = Parser.parseInt();
  uint8_t green = Parser.parseInt();
  uint8_t blue = Parser.parseInt();
  CRGB paletteColor = CRGB(red, green, blue);
  setPalettePosition(paletteIndex, paletteColor);
  storeCurrentPalette(currentPalette);
  Parser.flush();
}

/*
  Change gradient blending between a linearblend and no blending
  1 [blending]
  0 [no blending] 
*/
void gradientBlendingCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  currentBlending = (TBlendType)Parser.parseInt();
  storeGradientBlending(currentBlending);
  Parser.flush();
}
