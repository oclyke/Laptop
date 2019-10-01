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

#define MIC_PIN 34
#define TIP_PIN A1
#define RING_PIN A0

#define DATA_PIN 14

#define X_LEDS 15
#define Y_LEDS 15
#define NUM_LEDS X_LEDS * Y_LEDS

CRGB leds[NUM_LEDS];

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
const double samplingFrequency = 5000;
const uint8_t amplitude = 100;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];
double vReal1[samples];
double vImag1[samples];
double vReal2[samples];
double vImag2[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  Serial.begin(115200);
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop()
{
  /* Build raw data */
  double cycles = (((samples-1) * signalFrequency) / samplingFrequency); //Number of signal cycles that the sampling will read
  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] = map(analogRead(TIP_PIN), 2250, 5250, -127, 127);/* Build data with positive and negative values*/
    vReal1[i] = map(analogRead(MIC_PIN), 1100, 2500, -127, 127);
    vReal2[i] = map(analogRead(RING_PIN), 2250, 5250, -127, 127);
    vImag[i] = 0.0;
    vImag1[i] = 0.0;
    vImag2[i] = 0.0;
  }
  /* Print the results of the simulated sampling according to time */
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  FFT.Windowing(vReal1, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
  FFT.Compute(vReal1, vImag1, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal1, vImag1, samples); /* Compute magnitudes */
  FFT.Windowing(vReal2, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
  FFT.Compute(vReal2, vImag2, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal2, vImag2, samples); /* Compute magnitudes */
  for (int led = 0; led < 30; led++)
  {
    leds[led * 3] = CHSV(vReal1[led + 2] / 2, 255 - (vReal1[led + 2] / 4), vReal1[led + 2] / 2 );
    leds[(led * 3) + 1] = CHSV(vReal1[led + 2] / 2, 255 - (vReal1[led + 2] / 4), vReal1[led + 2] / 2);
    leds[(led * 3) + 2] = CHSV(vReal1[led + 2] / 2 , 255 - (vReal1[led + 2] / 4), vReal1[led + 2] / 2);
  }
  /*for (int led = 13; led < 25; led++)
  {
    leds[led] = CHSV(vReal1[4], 255 - vReal1[10], vReal1[4] / 2);
  }*/
  FastLED.show();
  //while(1); /* Run Once */
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
	break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
