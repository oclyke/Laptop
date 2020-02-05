/*
  Copyright (c) 2019 Andy England
  CustomLitt Laptop
*/

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
      {
        audioScale = SerialBT.parseFloat();
        char saveSetting = SerialBT.read();
        if (saveSetting == 'e')
        {
          setFloatCharacteristic(AUDIO_SCALE, audioScale);
        }
        break;
      }

    /*
      Brightness
      b127
      b0-b255
    */
    case 'b': //brightness
      {
        brightness = SerialBT.parseInt();
        FastLED.setBrightness(brightness);
        setCharacteristic(BRIGHTNESS, brightness);
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
        setColor(COLOR, customColor);
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
        setColor(GRADIENT + (paletteIndex * 3), paletteColor);
        setPalettePosition(paletteIndex, paletteColor);
      }
      break;

    /*
      Change audio source between microphone and audio jack
      mm (switches audio source to microphone) mj switches to audio jack
      m
    */
    case 'm': //audio Source
      {
        char tempSource = SerialBT.read();
        switch (tempSource)
        {
          case 'm':
            audioSource = MIC;
            break;
          case 'j':
            audioSource = JACK;
            break;
        }
        resetColorIndex();
        setCharacteristic(AUDIO_SOURCE, audioSource);
        break;
      }

    /*
      Change pattern utilized
      p3 (changes to the 4th pattern, index at 0)
      p0-12 (currently expanding, check main loop for more accurate pattern count, as well as what pattern is assigned to what number)
    */
    case 'p': //pattern
      patternNum = SerialBT.parseInt();
      setCharacteristic(PATTERN, patternNum);
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
      Sends current settings to the app
      s
      s
    */
    case 's':
      SerialBT.print('D');
      SerialBT.println(deviceName);
      delay(5);
      SerialBT.print('a');
      SerialBT.println(audioScale);
      delay(5);
      SerialBT.print('b');
      SerialBT.println(brightness);
      delay(5);
      SerialBT.print("c");
      SerialBT.print(customColor.red);
      SerialBT.print(" ");
      SerialBT.print(customColor.green);
      SerialBT.print(" ");
      SerialBT.println(customColor.blue);
      delay(5);
      SerialBT.print("ds");
      SerialBT.println(frameSkip);
      delay(5);
      SerialBT.print("dd");
      SerialBT.println(frameDelay);
      delay(5);
      SerialBT.print("fl");
      SerialBT.println(avgLowEnd);
      delay(5);
      SerialBT.print("fh");
      SerialBT.println(avgHighEnd);
      delay(5);
      for (uint8_t index = 0; index < 16; index++)
      {
        SerialBT.print("g");
        SerialBT.print(index);
        SerialBT.print(" ");
        SerialBT.print(currentPalette[index].red);
        SerialBT.print(" ");
        SerialBT.print(currentPalette[index].green);
        SerialBT.print(" ");
        SerialBT.println(currentPalette[index].blue);
        delay(5);
      }
      SerialBT.print('m');
      SerialBT.println(audioSource);
      delay(5);
      SerialBT.print('p');
      SerialBT.println(patternNum);
      delay(5);
      SerialBT.print('A');
      SerialBT.println(audioReaction);
      delay(5);
      SerialBT.print('B');
      SerialBT.println(currentBlending);
      delay(5);
      SerialBT.println("FINISH");
      break;

    /*
      Changes whether or not the pattern is audio reactive
      A
      A
    */
    case 'A':
      {
        audioReaction = SerialBT.parseInt();
        setCharacteristic(AUDIO_REACTION, audioReaction);
        break;
      }

    /*
      Change gradient blending between a linearblend and no blending
      B0 (no blending) B1 (blending)
      B
    */
    case 'B':
      currentBlending = (TBlendType)SerialBT.parseInt();
      setCharacteristic(BLENDING, currentBlending);
      break;

    /*
      Attempt to connect to wifi after changing settings or for any other reason you might want to attempt to reconnect (Still needs backend work for eeprom storage issue)
      C
      C
    */
    case 'C':
      initArtnet();
      break;

    /*
       Change the name of the Device
       DNew Device Name
    */
    case 'D':
      {
        char tempName[32];
        int position = 0;
        while (SerialBT.available())
        {
          char temp = SerialBT.read();
          tempName[position++] = temp;
        }
        deviceName = tempName;
        EEPROM.put(DEVICE_NAME_ADDRESS, deviceName);
        break;
      }

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
          tempSsid[position++] = temp;
        }
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
          tempPassword[position++] = temp;
        }
        setPassword(tempPassword);
        break;
      }
  }
}
