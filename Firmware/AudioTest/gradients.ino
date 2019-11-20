CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;

void setPalettePosition(uint8_t paletteIndex, CRGB color)
{
  currentPalette[paletteIndex] = color;
}

