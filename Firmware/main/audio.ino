#include <driver/adc.h>
arduinoFFT FFT = arduinoFFT();

#define MIC true
#define JACK false

uint8_t avgLowEnd = 3;
uint8_t avgHighEnd = 8;

bool audioSource = MIC;

#define MIC_PIN 34
#define TIP_PIN 32
#define RING_PIN 33
#define LOWEST_HZ_BIN 3

float audioScale = 5.0;
/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
#define MAX_SAMPLES 256
double vReal[MAX_SAMPLES];
double vImag[MAX_SAMPLES];
double vReal1[MAX_SAMPLES];
double vImag1[MAX_SAMPLES];
double vDummy[MAX_SAMPLES];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

/* @brief computeFFT - computes our fourier transform
   @param bool mic - true if we are reading the mic, false if reading the 3.5mm jack
   @param uint16_t sample - number of samples taken to compute FFT, must be a power of 2
*/
void computeFFT (bool source, uint16_t samples = 128)
{
  if (source == MIC)
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
  else if (source == JACK)
  {
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] = map(analogRead(RING_PIN), 1000, 2500, -127, 127);
      vReal1[i] = map(analogRead(TIP_PIN), 1000, 2500, -127, 127);
      vImag[i] = 0.0;
      vImag1[i] = 0.0;
    }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    FFT.Windowing(vReal1, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal1, vImag1, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal1, vImag1, samples); /* Compute magnitudes */
    for (uint16_t i = 0; i < 16; i++)
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
  for (uint8_t i = avgLowEnd; i < avgHighEnd; i++)
  {
    average += vReal[i];
  }
  average /= (avgHighEnd - avgLowEnd);
  constrain(average, 0, 255);
  Serial.println(average);
  return average;
}
