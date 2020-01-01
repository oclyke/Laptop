/*Note that all functions are case sensitive*/

/* Function
   Example string
   Range
*/
void bluetoothHandler(char instructionType)
{
  switch (instructionType)
  {
    /*
      Audio Sensitivity
      a5.6 (Doesn't need to have a decimal place)
      a0-a100
    */
    case 'a': //audio scaling
      audioScale = SerialBT.parseFloat();
      break;

    /*
      Brightness
      b127
      b0-b255
    */
    case 'b': //brightness
      {
        uint8_t tempBrightness = SerialBT.parseInt();
        FastLED.setBrightness(tempBrightness);
      }
      break;

    /*
      Change Color used in colorSet
      c255 0 64 (changes color to all red, no green, and a little blue for a purple, similar use to gradient change)
      c0-255 0-255 0-255
    */
    case 'c': //color
      {
        customColor.red = SerialBT.parseInt();
        customColor.green = SerialBT.parseInt();
        customColor.blue = SerialBT.parseInt();
      }
      break;

    /*
      Change delay and how many frames to skip (only works on non audio reactive stuff)
      ds3 (changes so we skip three frames of our animation)
      dd5 (change our delay to 5 ms)
      ds0-255
      dd0-5000? maybe a custom field to input a number
    */
    case 'd':
      {
        char delayType = SerialBT.read();
        switch (delayType)
        {
          case 's':
            frameSkip = SerialBT.parseInt();
            break;
          case 'd':
            frameDelay = SerialBT.parseInt();
            break;
        }
      }
      break;

    /*
      change lower and upper frequency bounds on fftAvg function. Controls which frequencies are used to set speeds
      fh12
      fl4
      fl0-20 (that is a lowercase L)
      fh0-20
      Make sure that fl is never higher than fh
    */
    case 'f':
      {
        char frequencyType = SerialBT.read();
        switch (frequencyType)
        {
          case 'l':
            avgLowEnd = SerialBT.parseInt();
            break;
          case 'h':
            avgHighEnd = SerialBT.parseInt();
            break;
        }
        break;
      }

    /*
      Change colors of gradient currently utilized
      g1 255 0 64 (changes second color in gradient to purple, first number is index of gradient, followed by the color you would like to change it to)
      g0-15 0-255 0-255 0-255
    */
    case 'g'://Set indivudal gradient colors
      {
        uint8_t paletteIndex = SerialBT.parseInt();
        uint8_t red = SerialBT.parseInt();
        uint8_t green = SerialBT.parseInt();
        uint8_t blue = SerialBT.parseInt();
        CRGB paletteColor = CRGB(red, green, blue);
        setPalettePosition(paletteIndex, paletteColor);
      }
      break;

    /*
      Change audio source between microphone and audio jack
      m (switches audio source)
      m
    */
    case 'm': //audio Source
      if (audioSource == MIC)
      {
        audioSource = JACK;
      }
      else
      {
        audioSource = MIC;
      }
      break;

    /*
      Change pattern utilized
      p3 (changes to the 4th pattern, index at 0)
      p0-12 (currently expanding, check main loop for more accurate pattern count, as well as what pattern is assigned to what number)
    */
    case 'p': //pattern
      patternNum = SerialBT.parseInt();
      break;

    /*
      Resets color index that has been changed by audio reactive stuff (just used in testing)
      r
      r
    */
    case 'r': //reset
      resetColorIndex();
      break;

    /*
      Changes whether or not the pattern is audio reactive
      A
      A
    */
    case 'A':
      if (audioReaction == true)
      {
        audioReaction = false;
      }
      else
      {
        audioReaction = true;
      }
      break;
    /*
      Change gradient blending between a linearblend and no blending
      B
      B
    */
    case 'B':
      if (currentBlending == NOBLEND)      {
        currentBlending = LINEARBLEND;
      }
      else
      {
        currentBlending = NOBLEND;
      }
      break;

    /*
      Attempt to connect to wifi after changing settings or for any other reason you might want to attempt to reconnect (Still needs backend work for eeprom storage issue)
      C
      C
    */
    case 'C':
      connectWifi();
      break;

    /*
      Change the SSID to connect to for ArtNet (still needs backend work for eeprom storage issue)
      SBlack Panther (My home SSID is Black Panther)
      SSSIDString (SSIDString may be anywhere up to 32 characters)
    */
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
    /*
      Change the Password to connect to for ArtNet (still needs backend work for eeprom storage issue)
      Pfigureitoutdude (My password is figureitoutdude)
      Pfigureitoutdude (Pfigureitoutdude may be anywhere up to 32 characters)
    */
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
