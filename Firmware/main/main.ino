/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include "arduinoFFT.h"
#include <FastLED.h>
#include "CustomLittBLE.h"

#include "src/hardware/config.h"
#include "patterns.h"

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

String deviceName = "dummy";
String ssid = "dummy";
String password = "dummy";
bool wifiStatus = false;

float speedFactor = 0.5;
uint8_t paletteIndex = 0;
CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;
bool audioReaction = true;

uint8_t WIDTH = 0;
uint8_t LEFT = 0;
uint8_t RIGHT = 0;
uint8_t X_CENTER = 0;

uint8_t HEIGHT = 0;
uint8_t TOP = 0;
uint8_t BOTTOM = 0;
uint8_t Y_CENTER = 0;

CRGB leds[NUM_LEDS];

animation_t active_ani;

const uint8_t gamma8[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
  5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

void initLedArray()
{
  for (size_t x = 0; x < X_LEDS; x++){
    for (size_t y = 0; y < Y_LEDS; y++){
      ledArray[y][x] = -1; // set a sentinel that indicates this led is not set up
    }
  }
}

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

void setup()
{
  Serial.begin(115200);
  initLedArray();
  makeLedArray();
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  BLE.begin("CustomLitt");
  
  initializeEEPROM();
  
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);

  // set initial bluetooth properties
  BLE.setDisplayBrightness(brightness);
  BLE.setAudioSensitivity(audioScale);
  BLE.setAudioSource((audioSource) ? 1 : 0);
  BLE.setDeviceName(deviceName);
  BLE.setNetworkSSID(ssid);
  BLE.setNetworkPassword(password);
  BLE.setNetworkStatus(wifiStatus);

  BLE.setPattern(patternNum);
  BLE.setSpeedFactor(speedFactor);
  BLE.setAudioReactivity((audioReaction) ? 1 : 0);
  BLE.setFFTBounds(avgLowEnd, avgHighEnd);
  BLE.setGradientIndex(paletteIndex);
  BLE.setGradientBlending((currentBlending) ? 1 : 0);

  CRGB color = currentPalette[paletteIndex];
  BLE.setGradientColor(color.r, color.g, color.b);

  onChangePattern();
}

void loop()
{
  uint32_t now = millis();

  if(active_ani.get_frame){
    active_ani.get_frame(now, (void*)active_ani.arg);
  }else{
    ESP_LOGE("loop", "no current animation");
  }

  CRGB temp;
  for (size_t idx = 0; idx < NUM_LEDS; idx++){
    temp = leds[idx];
    temp.r = gamma8[temp.r];
    temp.g = gamma8[temp.g];
    temp.b = gamma8[temp.b];
    leds[idx] = temp;
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
