/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/
void setPalettePosition(uint8_t paletteIndex, CRGB color)
{
  currentPalette[paletteIndex] = color;
  // experimentally determined that the flashing comes from 
  // timing disruptions (due to BLE interrupts / processing)
  // rather than from updating the palette mid-animation
}
