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
}

CharacteristicParser Parser;





// // global property characteristics
void displayBrightnessCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  brightness = Parser.parseInt();
  FastLED.setBrightness(brightness);
  setCharacteristic(BRIGHTNESS, brightness);
  Parser.flush();
}

void audioSensitivityCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioScale = Parser.parseFloat();
//  char saveSetting = Parser.read();
//  if (saveSetting == 'e')
//  {
//    setFloatCharacteristic(AUDIO_SCALE, audioScale);
//  }
  Parser.flush();
//  Serial.println("audio sensitivity callback");
//  Serial.println(audioScale);
}

void audioSourceCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioSource = Parser.parseInt();
  resetColorIndex();
  setCharacteristic(AUDIO_SOURCE, audioSource);
  Parser.flush();
}

void deviceNameCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempName[32];
  int position = 0;
  while (Parser.available())
  {
    char temp = Parser.read();
    tempName[position++] = temp;
  }
  deviceName = tempName;
  EEPROM.put(DEVICE_NAME_ADDRESS, deviceName);
  Parser.flush();
}

void networkSSIDCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempSsid[32];
  int position = 0;
  while (Parser.available())
  {
    char temp = Parser.read();
    tempSsid[position++] = temp;
  }
  setSSID(tempSsid);
  Parser.flush();
}

void networkPasswordCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char tempPassword[32];
  int position = 0;
  while (Parser.available())
  {
    char temp = Parser.read();
    tempPassword[position++] = temp;
  }
  setPassword(tempPassword);
  Parser.flush();
}

void networkConnectCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  initArtnet();
  Parser.flush();
}

void networkIPAddressCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  // browser does not set ip address
  Parser.flush();
}

void resetCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  resetColorIndex();
  Parser.flush();
}


// // expression specific property characteristics
void patternCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  patternNum = Parser.parseInt();
  setCharacteristic(PATTERN, patternNum);
  Parser.flush();
}

void delayCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char delayType = Parser.read();
  switch (delayType)
  {
    case 's':
      frameSkip = Parser.parseInt();
      break;
    case 'd':
      frameDelay = Parser.parseInt();
      break;
  }
  Parser.flush();
}

void audioReactivityCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  audioReaction = Parser.parseInt();
  setCharacteristic(AUDIO_REACTION, audioReaction);
  Parser.flush();
}

void fftBoundsCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  char frequencyType = Parser.read();
  switch (frequencyType)
  {
    case 'l':
      avgLowEnd = Parser.parseInt();
      break;
    case 'h':
      avgHighEnd = Parser.parseInt();
      break;
  }
  Parser.flush();
}

void colorCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  customColor.red = Parser.parseInt();
  customColor.green = Parser.parseInt();
  customColor.blue = Parser.parseInt();
  setColor(COLOR, customColor);
  Parser.flush();
}

void gradientCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  uint8_t paletteIndex = Parser.parseInt();
  uint8_t red = Parser.parseInt();
  uint8_t green = Parser.parseInt();
  uint8_t blue = Parser.parseInt();
  CRGB paletteColor = CRGB(red, green, blue);
  setColor(GRADIENT + (paletteIndex * 3), paletteColor);
  setPalettePosition(paletteIndex, paletteColor);
  Parser.flush();
}

void gradientBlendingCallback(BLECharacteristic* pCharacteristic){
  Parser.loadCharacteristicValue(pCharacteristic);
  currentBlending = (TBlendType)Parser.parseInt();
  setCharacteristic(BLENDING, currentBlending);
  Parser.flush();
}
