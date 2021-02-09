/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include "patterns.h"

uint8_t colorBuffer[X_LEDS][3];

uint8_t frameSkip = 1; // temporary - silences compiler
int frameDelay = 15; // temporary

#define MS_PER_S (1000)
double getPhase(uint32_t now, double min_hz, double max_hz){
  double freq_hz = (max_hz - min_hz)*speedFactor + min_hz;
  const double seconds = (double)now / MS_PER_S;
  const double phase = fmod(seconds*freq_hz, 1.0);
  if(phase > 1.0){ return 1.0; }
  if(phase < 0.0){ return 0.0; }
  return phase;
}

inline size_t indexByXY(uint8_t x, uint8_t y){
  size_t index = ledArray[y][x];
  if(index > NUM_LEDS){
    return NUM_LEDS;
  }
  return index;
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

void mirrorFFT(uint32_t now, void* arg)
{
  uint8_t fadeValue = 0;
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x < RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    if(audioSource == MIC){
        fadeValue = 245;
        xValue = vReal[abs(x - X_CENTER) + LOWEST_HZ_BIN];
    }else{
        fadeValue = 235;
        if (x < X_CENTER)
        {
          xValue = vReal[(X_CENTER - x) + LOWEST_HZ_BIN];
        }
        else if (x == X_CENTER)
        {
          xValue = (vReal[LOWEST_HZ_BIN] + vReal1[LOWEST_HZ_BIN]) / 2;
        }
        else
        {
          xValue = vReal1[(x - X_CENTER) + LOWEST_HZ_BIN];
        }
    }
    yValue = map(xValue, 0, 255, BOTTOM, TOP);
    for (int y = BOTTOM; y >= yValue; y--)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[indexByXY(x, y)].getLuma())
      {
        leds[indexByXY(x, y)] = myColor;
      }
    }
  }
  fadeAll(fadeValue);
}

void centerFFT(uint32_t now, void* arg)
{
  uint8_t fadeValue;
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    if(audioSource == MIC){
        xValue = vReal[abs(x - X_CENTER) + LOWEST_HZ_BIN];
        fadeValue = 245;
    }else{
        fadeValue = 235;
        if (x < X_CENTER)
        {
          xValue = vReal[(X_CENTER - x) + LOWEST_HZ_BIN];
        }
        else if (x == X_CENTER)
        {
          xValue = (vReal[LOWEST_HZ_BIN] + vReal1[LOWEST_HZ_BIN]) / 2;
        }
        else
        {
          xValue = vReal1[(x - X_CENTER) + LOWEST_HZ_BIN];
        }
        break;
    }
    yValue = round(map(xValue, 0, 255, TOP, BOTTOM) / 2.0);
    for (int y = Y_CENTER; y >= Y_CENTER - yValue; y--)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[indexByXY(x, y)].getLuma())
      {
        leds[indexByXY(x, y)] = myColor;
      }
    }
    for (int y = Y_CENTER; y <= Y_CENTER + yValue; y++)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[indexByXY(x, y)].getLuma())
      {
        leds[indexByXY(x, y)] = myColor;
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

  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - X_CENTER);
    if (audioSource == MIC){
        memcpy(vDummy, vReal, sizeof(vReal));
    }else{
        if (x < X_CENTER)
        {
          memcpy(vDummy, vReal, sizeof(vReal));
        }
        else if (x == X_CENTER)
        {
          vDummy[LOWEST_HZ_BIN] = (vReal1[LOWEST_HZ_BIN] + vReal[LOWEST_HZ_BIN]) / 2;
        }
        else
        {
          memcpy(vDummy, vReal1, sizeof(vReal1));
        }
    }
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      if (audioReaction)
      {
        colorIndex[y][x] += vDummy[((absX + abs(y - (Y_LEDS / 2))) / 2) + LOWEST_HZ_BIN];
      }
      else
      {
        phase = getPhase(now, min_freq_hz, max_freq_hz);
      }
      uint8_t offset = colorIndex[y][x]; // existing amount that the color is advanced
      uint8_t basis = phase * 255;
      uint8_t index = basis + offset;
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, index, 255, currentBlending);
    }
  }
}

void diagonal(uint32_t now, void* arg)
{
  uint8_t corner = (uint32_t)arg;

  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.5;

  static uint8_t singleColorIndex = 0;
  static uint8_t offset = 0;
  offset = 0;

  computeFFT(audioSource);
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
        colorIndex[y][x] += vReal[((absX + absY) / 2) + LOWEST_HZ_BIN];
      }
      else
      {
        offset = 255 *  getPhase(now, min_freq_hz, max_freq_hz);
        colorIndex[y][x] = offset + ((absX + absY) / 2);
      }
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
    }
  }
  singleColorIndex++;
}

void audioBuffer(uint32_t now, void* arg)
{
  computeFFT(audioSource, 128);
  uint8_t temp = vReal[LOWEST_HZ_BIN];
  colorBuffer[0][0] = temp;
  colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  for (int x = LEFT; x <= RIGHT; x++)
  {
    for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[x][1]; y--)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
    }
    for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[x][1]; y++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
    }
  }
  for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  {
    colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
    colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  }
  fadeAll(127);
}

void centerAudioBuffer(uint32_t now, void* arg)
{
  computeFFT(audioSource);
  uint8_t temp = vReal[LOWEST_HZ_BIN];
  colorBuffer[0][0] = temp;
  colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  for (int x = LEFT; x <= RIGHT; x++)
  {
    uint8_t colorPosition = abs(x - X_CENTER);//rename this variable
    for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[colorPosition][1]; y--)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
    }
    for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[colorPosition][1]; y++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
    }
  }
  for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  {
    colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
    colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  }
  fadeAll(127);
}

void rightToLeftFade(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.8;

  static uint8_t index = 0;

  for (int x = LEFT; x <= RIGHT; x++)
  {
    uint8_t adjustedIndex = index + map(x, LEFT, RIGHT, 0, 255);
    for (int y = TOP; y <= BOTTOM; y++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  delay(frameDelay);//Delay
}

void leftToRightFade(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.8;

  static uint8_t index = 0;

  for (int x = RIGHT; x >= LEFT; x--)
  {
    uint8_t adjustedIndex = index + map(x, RIGHT, LEFT, 0, 255);
    for (int y = TOP; y <= BOTTOM; y++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  delay(frameDelay);//Delay
}

void bottomToTopFade(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.8;

  static uint8_t index = 0;

  for (int y = TOP; y <= BOTTOM; y++)
  {
    uint8_t adjustedIndex = index + map(y, TOP, BOTTOM, 0, 255);
    for (int x = LEFT; x <= RIGHT; x++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  delay(frameDelay);//Delay
}

void topToBottomFade(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.8;

  static uint8_t index = 0;

  for (int y = BOTTOM; y >= TOP; y--)
  {
    uint8_t adjustedIndex = index + map(y, BOTTOM, TOP, 0, 255);
    for (int x = LEFT; x <= RIGHT; x++)
    {
      leds[indexByXY(x, y)] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  delay(frameDelay);//Delay
}

void gradient(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 0.5;

  static uint8_t index = 0;

  computeFFT(audioSource);
  if (audioReaction == true)
  {
    index += fftAvg() / 4;
  }
  else
  {
    index = 255 * getPhase(now, min_freq_hz, max_freq_hz);
  }
  colorSet(ColorFromPalette(currentPalette, index, 255, currentBlending));
}

void momentaryAudioRamp(uint32_t now, void* arg)
{
  static uint8_t index = 0;
  computeFFT(audioSource);
  uint8_t avg = fftAvg();
  if (index < avg)
  {
    index = avg;
  }
  colorSet(ColorFromPalette(currentPalette, index, 255, currentBlending));
  index = (index * 235) / 256;
}

void audioJump(uint32_t now, void* arg)
{
  static uint8_t index = 0;
  computeFFT(audioSource);
  if (fftAvg() > 200)
  {
    index += 16;
    colorSet(ColorFromPalette(currentPalette, index, 255, currentBlending));
    FastLED.show();
    delay(75);
  }
  colorSet(ColorFromPalette(currentPalette, index, 255, currentBlending));
}

void sparkle(uint32_t now, void* arg)
{
  const float min_freq_hz = 0.1;
  const float max_freq_hz = 2.0;
  uint8_t index = 255 * getPhase(now, min_freq_hz, max_freq_hz);

  int randomMax;
  int randomVal;
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    randomMax = 255 - (fftAvg() * 2);//vReal[LOWEST_HZ_BIN];
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
  index++;
  fadeAll(196);
}

void solidColor(unsigned int, void*){
  colorSet(currentPalette[0]);
}

void colorSet(CRGB color)
{
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

void onChangePattern( void ){
  ani_frame_fn fn = NULL;
  void* arg = NULL;

  switch (patternNum){
    case 0:
      fn = mirrorFFT;
      break;
    case 1:
      fn = centerFFT;
      break;
    case 2:
      fn = diamond;
      break;
    case 3:
      fn = diagonal;
      arg = (void*)0;
      break;
    case 4:
      fn = diagonal;
      arg = (void*)1;
      break;
    case 5:
      fn = diagonal;
      arg = (void*)2;
      break;
    case 6:
      fn = diagonal;
      arg = (void*)3;
      break;
    case 7:
      fn = audioBuffer;
      break;
    case 8:
      fn = centerAudioBuffer;
      break;
    case 9:
      fn = gradient;
      break;
    case 10:
      fn = audioJump;
      break;
    case 11:
      fn = momentaryAudioRamp;
      break;
    case 12:
      fn = sparkle;
      break;
    case 13:
      fn = rightToLeftFade;
      break;
    case 14:
      fn = leftToRightFade;
      break;
    case 15:
      fn = topToBottomFade;
      break;
    case 16:
      fn = bottomToTopFade;
      break;
    case 17:
      fn = solidColor;
      break;
    // case 18:
    //   fn = artRead;
    //   break;
    default:
      break;
  }

  assert(fn);

  active_ani.get_frame = fn;
  active_ani.arg = arg;
}
