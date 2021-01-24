/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <driver/adc.h>
arduinoFFT FFT = arduinoFFT();


#define MIC_PIN 34
#define TIP_PIN 32
#define RING_PIN 33
#define LOWEST_HZ_BIN 3

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
void computeFFT (uint16_t samples)
{
  if (audioSource == MIC)
  {
    for (uint16_t i = 0; i < samples; i++)
    {
      vReal[i] = map(analogRead(MIC_PIN), 1100, 2500, -127, 127);
      vImag[i] = 0.0;
    }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    for (uint16_t i = 0; i < 20; i++)
    {
      
      vReal[i] *= i;
      vReal[i] /= 100 * (1.0001 - audioScale);//pow(10.0, 1 - audioScale);
      if(vReal[i] > 255.0){
        vReal[i] = 255.0;
      }
      if(vReal[i] < 0.0){
        vReal[i] = 0.0;
      }
    }
  }
  else if (audioSource == JACK)
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
      if(vReal[i] > 255.0){
        vReal[i] = 255.0;
      }
      if(vReal[i] < 0.0){
        vReal[i] = 0.0;
      }
      vReal1[i] *= i;
      vReal1[i] /= audioScale;
      if(vReal1[i] > 255.0){
        vReal1[i] = 255.0;
      }
      if(vReal1[i] < 0.0){
        vReal1[i] = 0.0;
      }
    }
  }
}
void computeFFT (void){
  computeFFT(256);
}

uint8_t strengthMajorPeak()
{
  return 0;
}

uint8_t fftAvg()
{
  int average = 0;
  for (uint8_t i = avgLowEnd; i < avgHighEnd; i++)
  {
    average += u8FFT(i, audioSource);
  }
  average /= (avgHighEnd - avgLowEnd);
  if(average > 255.0){ average = 255.0; }
  if(average < 0.0){ average = 0.0; }
  return average;
}

uint8_t u8FFT(uint8_t idx, bool Lr) {
  size_t bin = LOWEST_HZ_BIN + idx;
  float src;
  if(Lr){
    src = vReal[bin];
  }else{
    src = vReal1[bin];
  }
  float scaled = src * (3.0/(1.0 - audioScale)); // increase denominator for more low-level capability, decrease for more sensitivity at high levels
  uint8_t clamped = scaled;
  if(scaled < 0){ clamped = 0; }
  if(scaled > 255){ clamped = 255; }
  return clamped;
}
