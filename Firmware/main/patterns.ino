void mirrorFFT()
{
  uint8_t fadeValue;
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x < RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    switch (audioSource)
    {
      case MIC:
        fadeValue = 245;
        xValue = vReal[abs(x - X_CENTER) + LOWEST_HZ_BIN];
        break;
      case JACK:
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

void centerFFT()
{
  uint8_t fadeValue;
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t xValue;
    uint8_t yValue;
    switch (audioSource)
    {
      case MIC:
        xValue = vReal[abs(x - X_CENTER) + LOWEST_HZ_BIN];
        fadeValue = 245;
        break;
      case JACK:
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

void diamond()
{
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - X_CENTER);
    switch (audioSource)
    {
      case MIC:
        memcpy(vDummy, vReal, sizeof(vReal));
        break;
      case JACK:
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
        break;
    }
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      if (audioReaction)
      {
        colorIndex[y][x] += vDummy[((absX + abs(y - (Y_LEDS / 2))) / 2) + LOWEST_HZ_BIN];
      }
      else
      {
        colorIndex[y][x] += frameSkip;
      }
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
    }
  }
}

void rightToLeftFade()
{
  for (int x = LEFT; x <= RIGHT; x++)
  {
    uint8_t adjustedIndex = singleColorIndex + map(x, LEFT, RIGHT, 0, 255);
    for (int y = TOP; y <= BOTTOM; y++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    singleColorIndex += fftAvg() / 4;
  }
  else
  {
    singleColorIndex += frameSkip;
  }
  delay(frameDelay);//Delay
}

void leftToRightFade()
{
  for (int x = RIGHT; x >= LEFT; x--)
  {
    uint8_t adjustedIndex = singleColorIndex + map(x, RIGHT, LEFT, 0, 255);
    for (int y = TOP; y <= BOTTOM; y++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    singleColorIndex += fftAvg() / 4;
  }
  else
  {
    singleColorIndex += frameSkip;
  }
  delay(frameDelay);//Delay
}

void bottomToTopFade()
{
  for (int y = TOP; y <= BOTTOM; y++)
  {
    uint8_t adjustedIndex = singleColorIndex + map(y, TOP, BOTTOM, 0, 255);
    for (int x = LEFT; x <= RIGHT; x++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    singleColorIndex += fftAvg() / 4;
  }
  else
  {
    singleColorIndex += frameSkip;
  }
  delay(frameDelay);//Delay
}

void topToBottomFade()
{
  for (int y = BOTTOM; y >= TOP; y--)
  {
    uint8_t adjustedIndex = singleColorIndex + map(y, BOTTOM, TOP, 0, 255);
    for (int x = LEFT; x <= RIGHT; x++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, adjustedIndex, 255, currentBlending);
    }
  }
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    singleColorIndex += fftAvg() / 4;
  }
  else
  {
    singleColorIndex += frameSkip;
  }
  delay(frameDelay);//Delay
}

void gradient()
{
  computeFFT(audioSource);
  if (audioReaction == true)
  {
    singleColorIndex += fftAvg() / 4;
  }
  else
  {
    singleColorIndex += frameSkip;
  }
  colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
}

void momentaryAudioRamp()
{
  computeFFT(audioSource);
  uint8_t avg = fftAvg();
  if (singleColorIndex < avg)
  {
    singleColorIndex = avg;
  }
  colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
  singleColorIndex = (singleColorIndex * 235) / 256;
}

void audioJump()
{
  computeFFT(audioSource);
  if (fftAvg() > 200)
  {
    singleColorIndex += 16;
    colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
    FastLED.show();
    delay(75);
  }
  colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
}

void sparkle()
{
  int randomMax;
  uint8_t randomVal;
  if (audioReaction == true)
  {
    computeFFT(audioSource);
    randomMax = 255 - (fftAvg() * 2);//vReal[LOWEST_HZ_BIN];
    if (randomMax <= 1)
    {
      randomMax = 1;
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
    if (randomVal == 1)
    {
      leds[led] = ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending);
    }
  }
  singleColorIndex++;
  fadeAll(196);
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
