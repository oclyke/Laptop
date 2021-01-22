/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

#include "patterns.h"

void animation_task(void* args){
  uint32_t now = 0;
  while(1){
    now = millis();

    static uint32_t next_animation = 0;
    if(now >= next_animation){
      next_animation = now + (MS_PER_S/ANIMATION_RATE_HZ);
      computeFFT();
      run_animation();
    }
  }
}

void run_animation( void ){
  if(pdTRUE != xSemaphoreTake(output_lock, 0)){
    return; // just bail - we can afford to lose some frames
  }
  if(current_animation.get_frame != NULL){
    uint32_t now = millis();
    current_animation.get_frame(now, current_animation.arg);
  }else{
    ESP_LOGE(ANIMATION_TAG, "no frame function available");
  }
  xSemaphoreGive(output_lock);
}

uint8_t colorBuffer[X_LEDS][3];

/* @brief fadeAll - used to fade leds
   @param scale - percentage by which leds are faded every step
*/
void fadeAll(uint8_t scale = 250)
{
  // this fn should be called while in control of the output_lock mutex
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

  const float freq = (max_freq_hz - min_freq_hz)*speedFactor + min_freq_hz;

  static uint32_t prev = 0;
  float delta = (now - prev);
  float cycles = freq*delta / 1000;
  uint8_t advance = (uint8_t)((uint32_t)(cycles*255) % 255);

  prev = now;

  CRGB* leds = activeLEDs();
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - X_CENTER);
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      if (audioReaction)
      {
        size_t bin = ((absX + abs(y - (Y_LEDS / 2))) / 2);
        if(audioSource == MIC){
          colorIndex[y][x] += u8FFT(bin, true);
        }else{
          if (x < X_CENTER)
          {
            colorIndex[y][x] += u8FFT(bin, true);
          }
          else if (x == X_CENTER)
          {
            colorIndex[y][x] += (u8FFT(bin, true) + u8FFT(bin, false)) / 2;
          }
          else
          {
            colorIndex[y][x] += u8FFT(bin, false);
          }
        }
      }
      else
      {
        colorIndex[y][x] += advance;
      }
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
    }
  }
}

void diagonal(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // uint8_t corner = (uint32_t)arg;
  // uint8_t xVal = 0;
  // uint8_t yVal = 0;
  // switch (corner)
  // {
  //   case 0: //Topleft origin
  //     xVal = 0;
  //     yVal = 0;
  //     break;
  //   case 1:
  //     xVal = WIDTH;
  //     yVal = 0;
  //     break;
  //   case 2:
  //     xVal = WIDTH;
  //     yVal = HEIGHT;
  //     break;
  //   case 3:
  //     xVal = 0;
  //     yVal = HEIGHT;
  // }
  // for (uint8_t x = LEFT; x <= RIGHT; x++)
  // {
  //   uint8_t absX = abs(x - xVal);
  //   for (uint8_t y = TOP; y <= BOTTOM; y++)
  //   {
  //     uint8_t absY = abs(y - yVal);
  //     if (audioReaction)
  //     {
  //       colorIndex[y][x] += vReal[((absX + absY) / 2) + LOWEST_HZ_BIN];
  //     }
  //     else
  //     {
  //       colorIndex[y][x] = singleColorIndex + ((absX + absY) / 2);
  //     }
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
  //   }
  // }
  // singleColorIndex++;
}

void audioBuffer(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // uint8_t temp = vReal[LOWEST_HZ_BIN];
  // colorBuffer[0][0] = temp;
  // colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  // for (int x = LEFT; x <= RIGHT; x++)
  // {
  //   for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[x][1]; y--)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
  //   }
  //   for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[x][1]; y++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[x][0], colorBuffer[x][0], currentBlending);
  //   }
  // }
  // for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  // {
  //   colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
  //   colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  // }
  // fadeAll(127);
}

void centerAudioBuffer(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // uint8_t temp = vReal[LOWEST_HZ_BIN];
  // colorBuffer[0][0] = temp;
  // colorBuffer[0][1] = round(map(temp, 0, 255, TOP, BOTTOM) / 2.0);
  // for (int x = LEFT; x <= RIGHT; x++)
  // {
  //   uint8_t colorPosition = abs(x - X_CENTER);//rename this variable
  //   for (int y = Y_CENTER; y >= Y_CENTER - colorBuffer[colorPosition][1]; y--)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
  //   }
  //   for (int y = Y_CENTER; y <= Y_CENTER + colorBuffer[colorPosition][1]; y++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorBuffer[colorPosition][0], colorBuffer[colorPosition][0], currentBlending);
  //   }
  // }
  // for (int bufferPosition = RIGHT; bufferPosition >= LEFT; bufferPosition--)
  // {
  //   colorBuffer[bufferPosition][0] = colorBuffer[bufferPosition - 1][0];
  //   colorBuffer[bufferPosition][1] = colorBuffer[bufferPosition - 1][1];
  // }
  // fadeAll(127);
}

void rightToLeftFade(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // for (int x = LEFT; x <= RIGHT; x++)
  // {
  //   uint8_t adjustedIndex = singleColorIndex + map(x, LEFT, RIGHT, 0, 255);
  //   for (int y = TOP; y <= BOTTOM; y++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
  //   }
  // }
  // if (audioReaction == true)
  // {
  //   singleColorIndex += fftAvg() / 4;
  // }
  // else
  // {
  //   singleColorIndex += frameSkip;
  // }
  // delay(frameDelay);//Delay
}

void leftToRightFade(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // for (int x = RIGHT; x >= LEFT; x--)
  // {
  //   uint8_t adjustedIndex = singleColorIndex + map(x, RIGHT, LEFT, 0, 255);
  //   for (int y = TOP; y <= BOTTOM; y++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
  //   }
  // }
  // if (audioReaction == true)
  // {
  //   singleColorIndex += fftAvg() / 4;
  // }
  // else
  // {
  //   singleColorIndex += frameSkip;
  // }
  // delay(frameDelay);//Delay
}

void bottomToTopFade(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // for (int y = TOP; y <= BOTTOM; y++)
  // {
  //   uint8_t adjustedIndex = singleColorIndex + map(y, TOP, BOTTOM, 0, 255);
  //   for (int x = LEFT; x <= RIGHT; x++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
  //   }
  // }
  // if (audioReaction == true)
  // {
  //   singleColorIndex += fftAvg() / 4;
  // }
  // else
  // {
  //   singleColorIndex += frameSkip;
  // }
  // delay(frameDelay);//Delay
}

void topToBottomFade(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // for (int y = BOTTOM; y >= TOP; y--)
  // {
  //   uint8_t adjustedIndex = singleColorIndex + map(y, BOTTOM, TOP, 0, 255);
  //   for (int x = LEFT; x <= RIGHT; x++)
  //   {
  //     leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
  //   }
  // }
  // if (audioReaction == true)
  // {
  //   singleColorIndex += fftAvg() / 4;
  // }
  // else
  // {
  //   singleColorIndex += frameSkip;
  // }
  // delay(frameDelay);//Delay
}

void gradient(uint32_t now, void* arg)
{
  // if (audioReaction == true)
  // {
  //   singleColorIndex += fftAvg() / 4;
  // }
  // else
  // {
  //   singleColorIndex += frameSkip;
  // }
  // CRGB color = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
  // colorSet(now, &color);
}

void momentaryAudioRamp(uint32_t now, void* arg)
{
  // uint8_t avg = fftAvg();
  // if (singleColorIndex < avg)
  // {
  //   singleColorIndex = avg;
  // }
  // CRGB color = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
  // colorSet(now, &color);
  // singleColorIndex = (singleColorIndex * 235) / 256;
}

void audioJump(uint32_t now, void* arg)
{
  // CRGB color;
  // if (fftAvg() > 200)
  // {
  //   singleColorIndex += 16;
  //   color = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
  //   colorSet(now, &color);
  //   FastLED.show();
  //   delay(75);
  // }
  // color = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
  // colorSet(now, &color);
}

void sparkle(uint32_t now, void* arg)
{
  // CRGB* leds = activeLEDs();
  // int randomMax;
  // int randomVal;
  // if (audioReaction == true)
  // {
  //   randomMax = 255 - (fftAvg() * 2);//vReal[LOWEST_HZ_BIN];
  //   if (randomMax <= 48)
  //   {
  //     randomMax = 5;
  //   }
  //   //randomMax /= 4;
  // }
  // else
  // {
  //   randomMax = 20;
  // }
  // for (int led = 0; led < NUM_LEDS; led++)
  // {
  //   randomVal = random(randomMax);
  //   if (randomVal == 0)
  //   {
  //     leds[led] = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
  //   }
  // }
  // singleColorIndex++;
  // fadeAll(196);
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
      current_animation.get_frame = rightToLeftFade;
      current_animation.arg = NULL;
      break;
    case 14:
      current_animation.get_frame = leftToRightFade;
      current_animation.arg = NULL;
      break;
    case 15:
      current_animation.get_frame = topToBottomFade;
      current_animation.arg = NULL;
      break;
    case 16:
      current_animation.get_frame = bottomToTopFade;
      current_animation.arg = NULL;
      break;
    case 17:
      current_animation.get_frame = colorSet;
      current_animation.arg = (void*)&customColor;
      break;
    case 18:
      current_animation.get_frame = artRead;
      current_animation.arg = NULL;
      break;
  }
}
