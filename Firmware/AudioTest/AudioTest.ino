#include "arduinoFFT.h"
#include <FastLED.h>
#include "BluetoothSerial.h"

/**Serial Bluetooth**/
BluetoothSerial SerialBT;

uint8_t patternNum = 0;

#define MIC true
#define JACK false

#define MIC_PIN 34
#define TIP_PIN A1
#define RING_PIN A0

#define DATA_PIN 14
#define LOWEST_HZ_BIN 3

#define X_LEDS 21
#define Y_LEDS 13
#define MISSING_LEDS 14
#define NUM_LEDS (X_LEDS * Y_LEDS) - MISSING_LEDS

uint8_t singleColorIndex = 0;

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

uint8_t animationSpeed = 1;
float audioScale = 5.0;
uint8_t myDelay = 5;

CRGB customColor = CHSV(0, 255, 255);

//int ledArray[Y_LEDS][X_LEDS] = {
//0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20
/*0*/  //{ -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1},
/*1*/  //{ 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38},
/*2*/  //{ 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59},
/*3*/  //{ 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80},
/*4*/  //{ 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101},
/*5*/  //{102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122},
/*6*/  //{123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143},
/*7*/  //{144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164},
/*8*/  //{165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185},
/*9*/  //{186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206},
/*10*/ //{ -1, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226},
/*11*/ //{ -1, -1, -1, -1, -1, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242},
/*12*/ //{ -1, -1, -1, -1, -1, 243, 244, 245, 246, -1, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, -1}
//};

int preArray[Y_LEDS][X_LEDS] = {
  //       0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
  /*0*/  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
  /*1*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*4*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*6*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*7*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*8*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*9*/  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*10*/ { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*11*/ { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  /*12*/ { 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

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

arduinoFFT FFT = arduinoFFT();

/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
#define MAX_SAMPLES 512
double vReal[MAX_SAMPLES];
double vImag[MAX_SAMPLES];
double vReal1[MAX_SAMPLES];
double vImag1[MAX_SAMPLES];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

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

/* @brief computeFFT - computes our fourier transform
   @param bool mic - true if we are reading the mic, false if reading the 3.5mm jack
   @param uint16_t sample - number of samples taken to compute FFT, must be a power of 2
*/
void computeFFT (bool mic, uint16_t samples = 512)
{
  if (mic)
  {
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] = map(analogRead(MIC_PIN), 1100, 2500, -127, 127);
      vImag[i] = 0.0;
    }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] *= i;
      vReal[i] /= audioScale;
      constrain(vReal[i], 0, 255);
    }
  }
  else
  {
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] = map(analogRead(TIP_PIN), 2250, 5250, -127, 127);/* Build data with positive and negative values*/
      vReal1[i] = map(analogRead(RING_PIN), 2250, 5250, -127, 127);
      vImag[i] = 0.0;
      vImag1[i] = 0.0;
    }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    FFT.Windowing(vReal1, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal1, vImag1, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal1, vImag1, samples); /* Compute magnitudes */
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] *= i;
      vReal[i] /= audioScale;
      constrain(vReal[i], 0, 255);
      vReal1[i] *= i;
      vReal1[i] /= audioScale;
      constrain(vReal1[i], 0, 255);
    }
  }
}

uint8_t fftAvg()
{
  int average = 0;
  for (uint8_t i = 0; i < 16; i++)
  {
    average += vReal[i];
  }
  average /= 16;
  return average;
}

void setup()
{
  Serial.begin(115200);
  makeLedArray();
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  SerialBT.begin("CustomLitt"); //Bluetooth device name
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
}

void loop()
{
  //long timerStart = millis();
  if (SerialBT.available()) {
    char instructionType = SerialBT.read();
    bluetoothHandler(instructionType);
  }
  switch (patternNum)
  {
    case 0:
      mirrorFFT();
      break;
    case 1:
      centerFFT();
      break;
    case 2:
      gradientAudio();
      break;
    case 3:
      audioRamp();
      break;
    case 4:
      momentaryAudioRamp();
      break;
    case 5:
      gradient();
      break;
    case 6:
      rightToLeftFade();
      break;
    case 7:
      leftToRightFade();
      break;
    case 8:
      topToBottomFade();
      break;
    case 9:
      bottomToTopFade();
      break;
    case 10:
      colorSet(customColor);
      break;
    case 11:
      simpleFade();
      break;
  }
  FastLED.show();
  /*Serial.print("ms: ");
    long timerEnd = millis();
    long fullTimer = timerEnd - timerStart;
    Serial.println(fullTimer);*/
}
