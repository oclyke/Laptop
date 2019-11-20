void mirrorFFT()
{
  computeFFT(MIC);
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    uint8_t xValue = vReal[abs(x - (X_LEDS / 2)) + LOWEST_HZ_BIN];
    uint8_t yValue = map(xValue, 0, 255, 0, Y_LEDS);
    for (uint8_t y = 0; y <= yValue; y++)
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
  computeFFT(MIC);
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    colorIndex[x] += vReal[abs(x - (X_LEDS / 2)) + LOWEST_HZ_BIN];
    for (uint8_t y = 0; y < Y_LEDS; y++)
    {
      leds[ledArray[y][x]] = ColorFromPalette(currentPalette, colorIndex[x], 255, currentBlending);
    }
  }
}

void gradient()
{
  for (uint8_t x = 0; x < X_LEDS; x++)
  {
    colorIndex[x] += animationSpeed;
    CRGB myColor = ColorFromPalette(currentPalette, colorIndex[x], 127, currentBlending);
    for (uint8_t y = 0; y < Y_LEDS; y++)
    {
      leds[ledArray[y][x]] = myColor;
    }
  }
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
    colorIndex[x] = 0;
  }
}

