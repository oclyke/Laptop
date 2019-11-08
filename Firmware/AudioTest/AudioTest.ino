/*

	Example of use of the FFT libray
        Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
  In this example, the Arduino simulates the sampling of a sinusoidal 1000 Hz
  signal with an amplitude of 100, sampled at 5000 Hz. Samples are stored
  inside the vReal array. The samples are windowed according to Hamming
  function. The FFT is computed using the windowed samples. Then the magnitudes
  of each of the frequencies that compose the signal are calculated. Finally,
  the frequency with the highest peak is obtained, being that the main frequency
  present in the signal.
*/

#include "arduinoFFT.h"
#include <FastLED.h>

#define MIC true
#define JACK false

#define MIC_PIN 34
#define TIP_PIN A1
#define RING_PIN A0

#define DATA_PIN 14

#define X_LEDS 21
#define Y_LEDS 13
#define MISSING_LEDS 16
#define NUM_LEDS (X_LEDS * Y_LEDS) - MISSING_LEDS

CRGB leds[NUM_LEDS];

int ledArray[Y_LEDS][X_LEDS] = {
  //       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20
  /*0*/  { -1,  0, -1, -1, -1, -1, -1, -1, -1,  1,  2, -1, -1, -1, -1, -1, -1, -1, -1,  3, -1},
  /*1*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1,  6},
  /*2*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  7, -1,  8,  9, -1, -1, -1, -1, -1, -1, -1},
  /*3*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*4*/  { -1, -1, -1, -1, -1, 11, -1, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*5*/  { -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*6*/  { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
  /*7*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, 36, 37, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*8*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*9*/  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  /*10*/ { -1, 41, -1, -1, -1, -1, -1, -1, -1, -1, 42, -1, -1, 43, -1, -1, -1, -1, -1, -1, -1},
  /*11*/ { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 44, -1, -1, -1, 45, -1, -1, -1, -1, -1, 46},
  /*12*/ { -1, -1, -1, -1, -1, 47, -1, -1, -1, -1, 48, -1, -1, -1, -1, -1, -1, -1, -1, 49, -1}
};

arduinoFFT FFT = arduinoFFT();
/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
#define MAX_SAMPLES 2048
double vReal[MAX_SAMPLES];
double vImag[MAX_SAMPLES];
double vReal1[MAX_SAMPLES];
double vImag1[MAX_SAMPLES];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

/*fadeAll - used to fade leds
   scale - percentage by which leds are faded every step
*/
void fadeAll(uint8_t scale = 250)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(scale);
  }
}

/*ComputeFFT - computes our fourier transform
  bool mic - true if we are reading the mic, false if reading the 3.5mm jack
  uint16_t sample - number of samples taken to compute FFT, must be a power of 2
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
      constrain(vReal[i], 0, 255);
      constrain(vReal1[i], 0, 255);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
}

void loop()
{
  computeFFT(MIC);
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    for (uint8_t y = 0; y < Y_LEDS; y++)
    {
      CRGB myColor = CHSV(vReal[x + 6], 255 - vReal[x + 6] / 2, vReal[x + 6]);
      if (myColor.getLuma() > leds[ledArray[y][x]].getLuma())
      {
        leds[ledArray[y][x]] = myColor;
      }
    }
  }
  fadeAll(230);
  FastLED.show();
  //while(1); /* Run Once */
}
