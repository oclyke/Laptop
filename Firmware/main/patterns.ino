uint8_t frameSkip = 1;
uint8_t frameDelay = 0;

void mirrorFFT()
{
  computeFFT(audioSource);
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    uint8_t xValue = vReal[abs(x - (X_LEDS / 2)) + LOWEST_HZ_BIN];
    uint8_t yValue = map(xValue, 0, 255, BOTTOM, TOP);
    for (int y = BOTTOM; y >= yValue; y--)
    {
      CRGB myColor = ColorFromPalette(currentPalette, xValue, xValue, currentBlending);
      if (myColor.getLuma() > leds[ledArray[y][x]].getLuma())
      {
        leds[ledArray[y][x]] = myColor;
      }
    }
  }
  fadeAll(230);
}

void centerFFT()
{
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t xValue = vReal[abs(x - X_CENTER) + LOWEST_HZ_BIN];
    uint8_t yValue = round(map(xValue, 0, 255, TOP, BOTTOM) / 2.0);
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
  fadeAll(230);
}

void gradientAudio()
{
  computeFFT(audioSource);
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    uint8_t absX = abs(x - (X_LEDS / 2));
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      colorIndex[y][x] += vReal[((absX + abs(y - (Y_LEDS / 2))) / 2) + LOWEST_HZ_BIN];
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
    }
  }
}

void gradient()
{
  for (uint8_t x = LEFT; x <= RIGHT; x++)
  {
    for (uint8_t y = TOP; y <= BOTTOM; y++)
    {
      colorIndex[y][x] += animationSpeed;
      CRGB myColor = ColorFromPalette(currentPalette, colorIndex[y][x], 255, currentBlending);
      leds[ledArray[y][x]] = myColor;
    }
  }
}

void simpleFade()
{
  colorSet(ColorFromPalette(currentPalette, singleColorIndex++, 255, currentBlending));
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
  singleColorIndex += frameSkip;//Speed Control
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
  singleColorIndex += frameSkip;//Speed Control
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
  singleColorIndex += frameSkip;//Speed Control
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
  singleColorIndex += frameSkip;//Speed Control
  delay(frameDelay);//Delay
}

void audioRamp()
{
  computeFFT(audioSource);
  singleColorIndex += vReal[LOWEST_HZ_BIN];
  colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
}

void momentaryAudioRamp()
{
  computeFFT(audioSource);
  uint8_t avg = fftAvg();
  if (singleColorIndex < avg)
  {
    singleColorIndex = avg;
    colorSet(ColorFromPalette(currentPalette, singleColorIndex, 255, currentBlending));
  }
  fadeAll(230);
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
