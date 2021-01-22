/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
#include <driver/adc.h>

#define MIC_PIN 34
#define TIP_PIN 32
#define RING_PIN 33

#define SAMPLING_FREQ_HZ (40000)
#define SAMPLING_PERIOD_US (1000000 / SAMPLING_FREQ_HZ)

/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
#define FFT_SAMPLES (1024)

uint16_t raw_l[FFT_SAMPLES];
uint16_t raw_r[FFT_SAMPLES];
volatile size_t raw_wh = 0;

double real_l[FFT_SAMPLES];
double real_r[FFT_SAMPLES];
double imag_l[FFT_SAMPLES];
double imag_r[FFT_SAMPLES];

#define TOTAL_BINS (20)
int bins_l[TOTAL_BINS];
int bins_r[TOTAL_BINS];

uint8_t results_l[TOTAL_BINS];
uint8_t results_r[TOTAL_BINS];

arduinoFFT FFT_l = arduinoFFT(real_l, imag_l, FFT_SAMPLES, SAMPLING_FREQ_HZ);
arduinoFFT FFT_r = arduinoFFT(real_r, imag_r, FFT_SAMPLES, SAMPLING_FREQ_HZ);

void computeFFT ( void )
{
  if(audioSource == MIC){
    for(size_t idx = 0; idx < FFT_SAMPLES; idx++){
      uint32_t mark = micros();
      real_l[idx] = map(analogRead(MIC_PIN), 1100, 2500, -127, 127);
      // real_l[idx] = analogRead(MIC_PIN);
      imag_l[idx] = 0.0;
      while(micros() < (mark + SAMPLING_PERIOD_US)){ /* wait for next sample */ }
    }

    FFT_l.DCRemoval();
    FFT_l.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_l.Compute(FFT_FORWARD);
    FFT_l.ComplexToMagnitude();
  }else{
    for(size_t idx = 0; idx < FFT_SAMPLES; idx++){
      uint32_t mark = micros();
      real_l[idx] = map(analogRead(RING_PIN), 1000, 2500, -127, 127);
      real_r[idx] = map(analogRead(TIP_PIN), 1000, 2500, -127, 127);
      // real_l[idx] = analogRead(RING_PIN);
      // real_r[idx] = analogRead(TIP_PIN);
      imag_l[idx] = 0.0;
      imag_r[idx] = 0.0;
      while(micros() < (mark + SAMPLING_PERIOD_US)){ /* wait for next sample */ }
    }

    FFT_l.DCRemoval();
    FFT_l.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_l.Compute(FFT_FORWARD);
    FFT_l.ComplexToMagnitude();

    FFT_r.DCRemoval();
    FFT_r.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT_r.Compute(FFT_FORWARD);
    FFT_r.ComplexToMagnitude();
  }

  // Reset bandValues[]
  for (int i = 0; i< TOTAL_BINS; i++){
    bins_l[i] = 0;
    bins_r[i] = 0;
  }

  // Analyse FFT results
  for (int i = 2; i < (FFT_SAMPLES/2); i++){      // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    // if (real_l[i] > 500) {                     // Add a crude noise filter
      
      if (i<=2 )          {  bins_l[0]  += (int)real_l[i];  bins_r[0] += (int)real_r[i];  }
      if (i>2   && i<=3  ){  bins_l[1]  += (int)real_l[i];  bins_r[1] += (int)real_r[i];  }
      if (i>3   && i<=4  ){  bins_l[2]  += (int)real_l[i];  bins_r[2] += (int)real_r[i];  }
      if (i>4   && i<=5  ){  bins_l[3]  += (int)real_l[i];  bins_r[3] += (int)real_r[i];  }
      if (i>5   && i<=7  ){  bins_l[4]  += (int)real_l[i];  bins_r[4] += (int)real_r[i];  }
      if (i>7   && i<=10 ){  bins_l[5]  += (int)real_l[i];  bins_r[5] += (int)real_r[i];  }
      if (i>10  && i<=13 ){  bins_l[6]  += (int)real_l[i];  bins_r[6] += (int)real_r[i];  }
      if (i>13  && i<=17 ){  bins_l[7]  += (int)real_l[i];  bins_r[7] += (int)real_r[i];  }
      if (i>17  && i<=22 ){  bins_l[8]  += (int)real_l[i];  bins_r[8] += (int)real_r[i];  }
      if (i>22  && i<=29 ){  bins_l[9]  += (int)real_l[i];  bins_r[9] += (int)real_r[i];  }
      if (i>29  && i<=39 ){  bins_l[10] += (int)real_l[i];  bins_r[10] += (int)real_r[i]; }
      if (i>39  && i<=51 ){  bins_l[11] += (int)real_l[i];  bins_r[11] += (int)real_r[i]; }
      if (i>51  && i<=68) {  bins_l[12] += (int)real_l[i];  bins_r[12] += (int)real_r[i]; }
      if (i>68  && i<=89) {  bins_l[13] += (int)real_l[i];  bins_r[13] += (int)real_r[i]; }
      if (i>89  && i<=118){  bins_l[14] += (int)real_l[i];  bins_r[14] += (int)real_r[i]; }
      if (i>118 && i<=156){  bins_l[15] += (int)real_l[i];  bins_r[15] += (int)real_r[i]; }
      if (i>156 && i<=206){  bins_l[16] += (int)real_l[i];  bins_r[16] += (int)real_r[i]; }
      if (i>206 && i<=272){  bins_l[17] += (int)real_l[i];  bins_r[17] += (int)real_r[i]; }
      if (i>272 && i<=360){  bins_l[18] += (int)real_l[i];  bins_r[18] += (int)real_r[i]; }
      if (i>360 && i<=475){  bins_l[19] += (int)real_l[i];  bins_r[19] += (int)real_r[i]; }
    // }
  }

  // scale results down
  for (int i = 0; i< TOTAL_BINS; i++){
    bins_l[i] /= (100.0 * 0.5);
    bins_r[i] /= (100.0 * 0.5);

    results_l[i] = bins_l[i];
    if(bins_l[i] > 255){ results_l[i] = 255; }
    if(bins_l[i] < 0){ results_l[i] = 0; }

    results_r[i] = bins_r[i];
    if(bins_r[i] > 255){ results_r[i] = 255; }
    if(bins_r[i] < 0){ results_r[i] = 0; }
  }
}

uint8_t strengthMajorPeak()
{
  return 0;
}

uint8_t fftAvg()
{
  int average = 0;
  for (int i = 0; i < TOTAL_BINS; i++)
  {
    average += (audioSource == MIC) ? u8FFT(i, true) : ((u8FFT(i, true) + u8FFT(i, false))/2);
  }
  average /= (avgHighEnd - avgLowEnd);
  if(average > 255.0){ average = 255.0; }
  if(average < 0.0){ average = 0.0; }
  // Serial.print("fft avg: ");
  // Serial.println(average);
  return average;
}

uint8_t u8FFT(size_t bin, bool Lr){
  return (Lr) ? results_l[bin] : results_r[bin];
}
