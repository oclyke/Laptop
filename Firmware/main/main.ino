/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include "arduinoFFT.h"
#include <FastLED.h>
#include "CustomLittBLE.h"

/**CustomLitt BLE**/
CustomLittBLE BLE;

uint8_t brightness = 255;

#define MIC true
#define JACK false

uint8_t avgLowEnd = 3;
uint8_t avgHighEnd = 8;

bool audioSource = MIC;
float audioScale = 0.5;
#define DATA_PIN 14

uint8_t patternNum = 0;

#define X_LEDS 21
#define Y_LEDS 13

String deviceName = "dummy";

#define MISSING0 3
#define MISSING1 0
#define MISSING2 0
#define MISSING3 0
#define MISSING4 0
#define MISSING5 0
#define MISSING6 0
#define MISSING7 0
#define MISSING8 0
#define MISSING9 0
#define MISSING10 1
#define MISSING11 5
#define MISSING12 6

#define MISSING_LEDS MISSING0 + MISSING1 + MISSING2 + MISSING3 + MISSING4 + MISSING5 + MISSING6 + MISSING7 + MISSING8 + MISSING9 + MISSING10 + MISSING11 + MISSING12
#define NUM_LEDS (X_LEDS * Y_LEDS) - MISSING_LEDS

int preArray[Y_LEDS][X_LEDS] = {
  //       0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 |    NUM_MISSING
  /*0*/  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0}, // 3
  /*1*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*2*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*3*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*4*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*5*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*6*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*7*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*8*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*9*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 0
  /*10*/ { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 1
  /*11*/ { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5
  /*12*/ { 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // 6
};

uint8_t singleColorIndex = 0;
uint8_t frameSkip = 1;
uint8_t frameDelay = 0;
CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;
bool audioReaction = true;

uint8_t colorIndex[Y_LEDS][X_LEDS] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

uint8_t WIDTH = 0;
uint8_t LEFT = 0;
uint8_t RIGHT = 0;
uint8_t X_CENTER = 0;

uint8_t HEIGHT = 0;
uint8_t TOP = 0;
uint8_t BOTTOM = 0;
uint8_t Y_CENTER = 0;

CRGB leds[NUM_LEDS];

CRGB customColor = CHSV(0, 255, 255);

int ledArray[Y_LEDS][X_LEDS] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};


/* @brief makeLedArray - populates LED array with proper values, also creates proper value for width and height

*/
void makeLedArray()
{
  int ledCount = 0;
  for (uint8_t y = 0; y < Y_LEDS; y++)
  {
    uint8_t xCount = 0;
    uint8_t rightPlaceholder = 0;
    for (uint8_t x = 0; x < X_LEDS; x++)
    {
      if (preArray[y][x] == 1)
      {
        ledArray[y][x] = ledCount++;
        xCount++;// figure out how many pixels are in this row
        rightPlaceholder = x;
        BOTTOM = y;
      }
    }
    if (xCount != 0) //If the row has pixels, increment our height
    {
      HEIGHT++;
    }
    if (xCount > WIDTH)
    {
      RIGHT = rightPlaceholder;
      WIDTH = xCount;//Set our max width
    }
  }
  LEFT = 1 + RIGHT - WIDTH;
  X_CENTER = round((LEFT + RIGHT) / 2.0);
  TOP = 1 + BOTTOM - HEIGHT;
  Y_CENTER = round((TOP + BOTTOM) / 2.0);
  Serial.print("Top: ");
  Serial.println(TOP);
  Serial.print("Bottom: ");
  Serial.println(BOTTOM);
  Serial.print("Left: ");
  Serial.println(LEFT);
  Serial.print("Right: ");
  Serial.println(RIGHT);
  Serial.print("Height: ");
  Serial.println(HEIGHT);
  Serial.print("Width: ");
  Serial.println(WIDTH);
}

/* @brief fadeAll - used to fade leds
   @param scale - percentage by which leds are faded every step
*/
void fadeAll(uint8_t scale = 250)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(scale);
  }
}

void setup()
{
  Serial.begin(115200);
  makeLedArray();
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  BLE.begin("CustomLitt");
  initializeEEPROM();
  FastLED.setBrightness(brightness);
  initArtnet();
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);

  // set initial bluetooth properties
  BLE.setDisplayBrightness(brightness);
  BLE.setAudioSensitivity(audioScale);
  BLE.setAudioSource((audioSource) ? 1 : 0);
  BLE.setDeviceName(deviceName);
//  BLE.setNetworkSSID(ssid);
//  BLE.setNetworkPassword(password);

  BLE.setPattern(patternNum);
  BLE.setDelay(frameSkip, frameDelay);
  BLE.setAudioReactivity((audioReaction) ? 1 : 0);
  BLE.setFFTBounds(avgLowEnd, avgHighEnd);
  BLE.setColor(customColor.r, customColor.g, customColor.b);
  BLE.setGradient(singleColorIndex, customColor.r, customColor.g, customColor.b);
  BLE.setGradientBlending((currentBlending) ? 1 : 0);
}

void loop()
{
//  //long timerStart = millis();
//  if (SerialBT.available()) {
//    char instructionType = SerialBT.read();
//    bluetoothHandler(instructionType);
//    Serial.write(instructionType);
//  }


  switch (patternNum)
  {
    case 0:
      mirrorFFT();
      break;
    case 1:
      centerFFT();
      break;
    case 2:
      diamond();
      break;
    case 3:
      diagonal(0);
      break;
    case 4:
      diagonal(1);
      break;
    case 5:
      diagonal(2);
      break;
    case 6:
      diagonal(3);
      break;
    case 7:
      audioBuffer();
      break;
    case 8:
      centerAudioBuffer();
      break;
    case 9:
      gradient();
      break;
    case 10:
      audioJump();
      break;
    case 11:
      momentaryAudioRamp();
      break;
    case 12:
      sparkle();
      break;
    case 13:
      rightToLeftFade();
      break;
    case 14:
      leftToRightFade();
      break;
    case 15:
      topToBottomFade();
      break;
    case 16:
      bottomToTopFade();
      break;
    case 17:
      colorSet(customColor);
      break;
    case 18:
      artRead();
      break;
  }
  FastLED.show();
  /*Serial.print("ms: ");
    long timerEnd = millis();
    long fullTimer = timerEnd - timerStart;
    Serial.println(fullTimer);*/
}

/*TODO
  add device name as a setting
  Hardware changes
  X Add sparkle pattern w/ music reaction    
  X add setting sender to send settings to Owen
  X diagonal fade?
  X store color in eeprom
  X store current state in eeprom
  X buffered left to right option that shifts things across display
  X -add middle out
  X Add speed control
  X add blending change option
  X flicker on off to the beat/flicker from one color to another
  X beat detect and jump
  X Add music jumping to simple left/right/top/bottom functions
  X add audio jack functionality to left and right on audio sensitive stuff
*/
