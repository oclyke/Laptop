/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include "patterns.h"

uint8_t colorBuffer[X_LEDS][3];

double getPhase(uint32_t now, double min_hz, double max_hz){
  double freq_hz = (max_hz - min_hz)*speedFactor + min_hz;
  const double seconds = (double)now / MS_PER_S;
  const double phase = fmod(seconds*freq_hz, 1.0);
  if(phase > 1.0){ return 1.0; }
  if(phase < 0.0){ return 0.0; }
  return phase;
}

/* @brief fadeAll - used to fade leds
   @param scale - percentage by which leds are faded every step
*/
void fadeAll(uint8_t scale = 250)
{
  CRGB* leds = activeLEDs();
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(scale);
  }
}

void mirrorFFT(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t fadeValue = 0;
  for (uint8_t x = LEFT; x < RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    if(audioSource == MIC){
        fadeValue = 245;
        xValue = u8FFT(abs(x - X_CENTER), audioSource);
    }else{
        fadeValue = 235;
        if (x < X_CENTER)
        {
          xValue = u8FFT((X_CENTER - x), true);
        }
        else if (x == X_CENTER)
        {
          xValue = (u8FFT(0, true) + u8FFT(0, false)) / 2;
        }
        else
        {
          xValue = u8FFT((x - X_CENTER), false);
        }
    }
    yValue = map(xValue, 0, 255, BOTTOM, TOP);
    for (int y = BOTTOM; y >= yValue; y--)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[ledArray[y][x]].getLuma())
      {
        leds[ledArray[y][x]] = myColor;
      }
    }
  }
  fadeAll(fadeValue);
}

void centerFFT(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t fadeValue;
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    if(audioSource == MIC){
        xValue = u8FFT(abs(x - X_CENTER), audioSource);
        fadeValue = 245;
    }else{
        fadeValue = 235;
        if (x < X_CENTER)
        {
          xValue = u8FFT((X_CENTER - x), true);
        }
        else if (x == X_CENTER)
        {
          xValue = (u8FFT(0, true) + u8FFT(0, false)) / 2;
        }
        else
        {
          xValue = u8FFT((x - X_CENTER), false);
        }
        break;
    }
    yValue = round(map(xValue, 0, 255, TOP, BOTTOM) / 2.0);
    for (int y = Y_CENTER; y >= Y_CENTER - yValue; y--)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[ledArray[y][x]].getLuma())
      {
        leds[ledArray[y][x]] = myColor;
      }
    }
    for (int y = Y_CENTER; y <= Y_CENTER + yValue; y++)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[ledArray[y][x]].getLuma())
      {
        leds[ledArray[y][x]] = myColor;
      }
    }
  }
  fadeAll(fadeValue);
}

void diamond(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.2;
  const float max_freq_hz = 1.0;

  static double phase = 0;

  CRGB* leds = activeLEDs();
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - X_CENTER);
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      if (audioReaction)
      {
        phase = 0.0;
        size_t bin = ((absX + abs(y - (Y_LEDS / 2))) / 2);
        if(audioSource == MIC){
          colorIndex[y][x] = u8FFT(bin, true);
        }else{
          if (x < X_CENTER)
          {
            colorIndex[y][x] = u8FFT(bin, true);
          }
          else if (x == X_CENTER)
          {
            colorIndex[y][x] = (u8FFT(bin, true) + u8FFT(bin, false)) / 2;
          }
          else
          {
            colorIndex[y][x] = u8FFT(bin, false);
          }
        }
      }
      else
      {
        phase = getPhase(now, min_freq_hz, max_freq_hz);
      }
      uint8_t offset = colorIndex[y][x]; // existing amount that the color is advanced
      uint8_t basis = phase * 255;
      uint8_t index = basis + offset;
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, index, 255, currentBlending);
    }
  }
}

void diagonal(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t corner = (uint32_t)arg;

  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.5;

  static uint8_t offset = 0;
  offset = 0;

  uint8_t xVal = 0;
  uint8_t yVal = 0;
  switch (corner)
  {
    case 0: //Topleft origin
      xVal = 0;
      yVal = 0;
      break;
    case 1:
      xVal = WIDTH;
      yVal = 0;
      break;
    case 2:
      xVal = WIDTH;
      yVal = HEIGHT;
      break;
    case 3:
      xVal = 0;
      yVal = HEIGHT;
  }
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - xVal);
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      uint8_t absY = abs(y - yVal);
      if (audioReaction)
      {
        colorIndex[y][x] += u8FFT(((absX + absY) / 2), audioSource);
      }
      else
      {
        offset = 255 *  getPhase(now, min_freq_hz, max_freq_hz);
        colorIndex[y][x] = offset + ((absX + absY) / 2);
      }
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
    }
  }
}

void audioBuffer(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t temp = u8FFT(0, audioSource);
  colorBuffer[0][0] = temp;
  colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  for (int x = LEFT; x <= RIGHT; x++)
  {
    for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[x][1]; y--)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
    }
    for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[x][1]; y++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
    }
  }
  for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  {
    colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
    colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  }
  fadeAll(220);
}

void centerAudioBuffer(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t temp = u8FFT(0, audioSource);
  colorBuffer[0][0] = temp;
  colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  for (int x = LEFT; x <= RIGHT; x++)
  {
    uint8_t colorPosition = abs(x - X_CENTER);//rename this variable
    for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[colorPosition][1]; y--)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
    }
    for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[colorPosition][1]; y++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
    }
  }
  for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  {
    colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
    colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  }
  fadeAll(220);
}

void linearFade(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();
  uint8_t mode = (uint32_t)arg;

  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.8;

  bool axis = false; // true: up/down, false: left/right
  bool dir = false; // true: L->R or T->B, false: R->L or B->T

  switch (mode)
  {
    case 0: // L->R
      axis = false;
      dir = true;
      break;
    case 1: // R->L
      axis = false;
      dir = false;
      break;
    case 2: // T->B
      axis = true;
      dir = true;
      break;
    case 3: // B->T
      axis = true;
      dir = false;
      break;
  }

  static uint8_t index = 0;

  if (audioReaction == true)
  {
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  CRGB color;
  if(axis){
    for (size_t y = 0; y < HEIGHT; y++){
      uint8_t adj_index = index + map(y, (dir) ? BOTTOM : TOP, (dir) ? TOP : BOTTOM, 0, 255);
      color = ColorFromPalette(currentPalette, adj_index, 255, currentBlending);
      for (size_t x = 0; x < WIDTH; x++){
        leds[ledArray[y][x]] = color;
      }
    }
  }else{
    for (size_t x = 0; x < WIDTH; x++){
      uint8_t adj_index = index + map(x, (dir) ? LEFT : RIGHT, (dir) ? RIGHT : LEFT, 0, 255);
      color = ColorFromPalette(currentPalette, adj_index, 255, currentBlending);
      for (size_t y = 0; y < HEIGHT; y++){
        leds[ledArray[y][x]] = color;
      }
    }
  }
}

void gradient(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.5;

  static uint8_t index = 0;

  if (audioReaction == true)
  {
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  CRGB color = ColorFromPalette(currentPalette, index, 255, currentBlending);
  colorSet(now, &color);
}

void momentaryAudioRamp(uint32_t now, void* arg)
{
  uint8_t avg = fftAvg();
  static uint8_t index = 0;
  static uint32_t prev = 0;
  if (index < avg)
  {
    index = avg;
  }
  CRGB color = ColorFromPalette(currentPalette, index, 255, currentBlending);
  colorSet(now, &color);
  if((now - prev) > 5){ // constrain the decay to real time
    prev = now;
    index = (index * 245) / 256;
  }
}

void audioJump(uint32_t now, void* arg)
{
  CRGB color;
  static uint8_t index = 0;
  static uint32_t prev = 0;
  if((now - prev) > 75){  // debounce
    prev = now;
    if (fftAvg() > 150)
    {
      index += 16;
    }
  }
  color = ColorFromPalette(currentPalette, index, 255, currentBlending);
  colorSet(now, &color);
}

void sparkle(uint32_t now, void* arg)
{
  CRGB* leds = activeLEDs();

  const float min_freq_hz = 0.1;
  const float max_freq_hz = 2.0;
  uint8_t index = 255 * getPhase(now, min_freq_hz, max_freq_hz);

  int randomMax;
  int randomVal;
  if (audioReaction == true)
  {
    randomMax = 255 - (fftAvg() * 2);
    if (randomMax <= 48)
    {
      randomMax = 5;
    }
    //randomMax /= 4;
  }
  else
  {
    randomMax = 20;
  }
  for (int led = 0; led < NUM_LEDS; led++)
  {
    randomVal = random(randomMax);
    if (randomVal == 0)
    {
      leds[led] = ColorFromPalette(currentPalette, index, 255, currentBlending);
    }
  }

  fadeAll(196);
}

void solidColor(uint32_t now, void* arg){
  colorSet(now, &currentPalette[0]);
}

void colorSet(uint32_t now, void* arg)
{
  CRGB color = *((CRGB*)arg);
  CRGB* leds = activeLEDs();
  for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++)
  {
    leds[ledNum] = color;
  }
}

void resetColorIndex()
{
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    for (uint8_t y = 0; y < Y_LEDS; y++)
    {
      colorIndex[y][x] = 0;
    }
  }
}

void onPatternChange(uint8_t val){
  // todo: perhaps clear the led buffer when changing patterns
  switch (val)
  {
    case 0:
      current_animation.get_frame = mirrorFFT;
      current_animation.arg = NULL;
      break;
    case 1:
      current_animation.get_frame = centerFFT;
      current_animation.arg = NULL;
      break;
    case 2:
      current_animation.get_frame = diamond;
      current_animation.arg = NULL;
      break;
    case 3:
      current_animation.get_frame = diagonal;
      current_animation.arg = (void*)0;
      break;
    case 4:
      current_animation.get_frame = diagonal;
      current_animation.arg = (void*)1;
      break;
    case 5:
      current_animation.get_frame = diagonal;
      current_animation.arg = (void*)2;
      break;
    case 6:
      current_animation.get_frame = diagonal;
      current_animation.arg = (void*)3;
      break;
    case 7:
      current_animation.get_frame = audioBuffer;
      current_animation.arg = NULL;
      break;
    case 8:
      current_animation.get_frame = centerAudioBuffer;
      current_animation.arg = NULL;
      break;
    case 9:
      current_animation.get_frame = gradient;
      current_animation.arg = NULL;
      break;
    case 10:
      current_animation.get_frame = audioJump;
      current_animation.arg = NULL;
      break;
    case 11:
      current_animation.get_frame = momentaryAudioRamp;
      current_animation.arg = NULL;
      break;
    case 12:
      current_animation.get_frame = sparkle;
      current_animation.arg = NULL;
      break;
    case 13:
      current_animation.get_frame = linearFade;
      current_animation.arg = (void*)0;
      break;
    case 14:
      current_animation.get_frame = linearFade;
      current_animation.arg = (void*)1;
      break;
    case 15:
      current_animation.get_frame = linearFade;
      current_animation.arg = (void*)2;
      break;
    case 16:
      current_animation.get_frame = linearFade;
      current_animation.arg = (void*)3;
      break;
    case 17:
      current_animation.get_frame = solidColor;
      current_animation.arg = NULL;
      break;
    case 18:
      current_animation.get_frame = artRead;
      current_animation.arg = NULL;
      break;
  }
}
