void bluetoothHandler(char instructionType)
{
  switch (instructionType)
  {
    case 'a': //audio scaling
      audioScale = SerialBT.parseFloat();
      break;
    case 'b': //brightness
      {
        uint8_t tempBrightness = SerialBT.parseInt();
        FastLED.setBrightness(tempBrightness);
      }
      break;
    case 'c': //color
      {
        char colorType = SerialBT.read();
        uint8_t colorValue = SerialBT.parseInt();
        switch (colorType)
        {
          case 'r':
            customColor.r = colorValue;
            break;
          case 'g':
            customColor.g = colorValue;
            break;
          case 'b':
            customColor.b = colorValue;
            break;
        }
      }
      break;
    case 'g'://Set indivudal gradient colors
      {
        uint8_t paletteIndex = SerialBT.parseInt();
        uint8_t red = SerialBT.parseInt();
        uint8_t green = SerialBT.parseInt();
        uint8_t blue = SerialBT.parseInt();
        Serial.println(paletteIndex);
        Serial.println(red);
        Serial.println(green);
        Serial.println(blue);
        CRGB paletteColor = CRGB(red, green, blue);
        setPalettePosition(paletteIndex, paletteColor);
      }
      break;
    case 'm': //audio Source
      audioSource != audioSource;
    case 'p': //pattern
      patternNum = SerialBT.parseInt();
      break;
    case 'r': //reset
      resetColorIndex();
      break;
    case 'C':
      connectWifi();
      break;
    case 'S': //SSIDSet
      {
        char tempSsid[32];
        int position = 0;
        while (SerialBT.available())
        {
          char temp = SerialBT.read();
          if (temp != 10)
          {
            tempSsid[position++] = temp;
          }
          else
          {
            tempSsid[position] = '\0';
            break;
          }
        }
        Serial.print("TempSSID: ");
        Serial.println(tempSsid);
        setSSID(tempSsid);
        break;
      }
    case 'P': //PasswordSet
      {
        char tempPassword[32];
        int position = 0;
        while (SerialBT.available())
        {
          char temp = SerialBT.read();
          if (temp != 10)
          {
            tempPassword[position++] = temp;
          }
          else
          {
            tempPassword[position] = '\0';
            break;
          }
        }
        Serial.print("bluetoothpass: ");
        Serial.println(tempPassword);
        setPassword(tempPassword);
        break;
      }
  }
}
