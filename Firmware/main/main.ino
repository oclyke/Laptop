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

#define MS_PER_S (1000)

typedef void (*frame_fn_t)(uint32_t time, void* args);
typedef struct _animation_t {
  volatile frame_fn_t get_frame;
  void* arg;
} animation_t;

animation_t current_animation;

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

const char* ANIMATION_TAG = "animation";
const char* OUTPUT_TAG = "output";

uint8_t paletteIndex = 0;
CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;
bool audioReaction = true;

float speedFactorISRVal = 0.0;
float speedFactor = 0.5;

uint8_t WIDTH = 0;
uint8_t LEFT = 0;
uint8_t RIGHT = 0;
uint8_t X_CENTER = 0;

uint8_t HEIGHT = 0;
uint8_t TOP = 0;
uint8_t BOTTOM = 0;
uint8_t Y_CENTER = 0;

CRGB canvas[NUM_LEDS]; // allows for using previous led values
CRGB output[NUM_LEDS]; // gamma corrected output

volatile uint32_t last_ble_op = 0;
volatile bool speed_changed = false;

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

void setup(){
  Serial.begin(115200);
  initLedArray();
  makeLedArray();
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(output, NUM_LEDS);
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

  onPatternChange(patternNum); // set initial animation function
}

void loop()
{
  computeFFT();
  uint32_t now = millis();

  if(current_animation.get_frame != NULL){
    current_animation.get_frame(now, current_animation.arg);
  }else{
    ESP_LOGE(ANIMATION_TAG, "no frame function available");
  }
  if(speed_changed){ // reset speed changed flag
    speedFactor = speedFactorISRVal;
    speed_changed = false;
  }

  if((now - last_ble_op) > 200){ // try just not outputting when ble ops are happening
    gammaCorrectFromInto(canvas, output, NUM_LEDS);
    FastLED.show();
  }
}
