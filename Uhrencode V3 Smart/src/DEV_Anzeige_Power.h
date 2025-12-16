struct DEV_Anzeige_Power : Service::LightBulb
{ // Anzeige der Uhr

  // Adafruit_NeoPixel *pixels;      // reference to Neipixels
  SpanCharacteristic *power; // reference to the On Characteristic
  // SpanCharacteristic *brightness; // reference to the Brightness Characteristic
  // SpanCharacteristic *colorTemp;  // reference to the ColorTemperature Characteristic
  // SpanCharacteristic *saturation; // reference to the Saturation Characteristic

  // DEV_Anzeige(Adafruit_NeoPixel *pixels) : Service::LightBulb()
  DEV_Anzeige_Power() : Service::LightBulb()
  {
    power = new Characteristic::On(false, true); // On Characteristic
    // brightness = new Characteristic::Brightness();      // Brightness Characteristic with an initial value of 50
    // brightness->setRange(0, 100, 1);                    // sets the range of the Brightness
    // colorTemp = new Characteristic::ColorTemperature(); // ColorTemperature Characteristic
    // saturation = new Characteristic::Saturation();      // Saturation Characteristic

  } // end constructor

  boolean update()
  { // update() method

    // power characteristic has been updated
    if (power->updated())
    {
      if (power->getNewVal() == 1)
      {
        Serial.println("Turn Anzeige on");
        anzeige.turnOn();
      }
      else
      {
        Serial.println("Turn Anzeige off");
        anzeige.turnOff();
      }
    }

    // brightness characteristic has been updated
    // if (brightness->updated())
    // {
    //   int newBrightness = brightness->getNewVal();
    //   Serial.print("new Brightness: ");
    //   Serial.println(newBrightness);
    //   if (newBrightness > 0)
    //   {
    //     // float newBrightnessFloat = (newBrightness / 100.0) * 255.0;
    //     // Serial.print("new Brightness Float: ");
    //     // Serial.println(newBrightnessFloat);
    //     // anzeige.setBrightness(newBrightness);
    //     anzeige.setMaxBrightness(newBrightness);
    //   }
    // }

    // // colorTemperature characteristic has been updated
    // if (colorTemp->updated())
    // {
    //   // Umrechnung in Kelvin
    //   float kelvin = 1000000.0 / colorTemp->getNewVal();

    //   float red, green, blue;
    //   kelvinToRGB(kelvin, red, green, blue);

    //   anzeige.fillColor(red, green, blue);
    // }

    return (true); // return true

  } // update

  void loop()
  {
    // update time every second if anzeige is on
    if (millis() % 1000 == 0 && anzeige.getAnzeigeIsOn())
    {

      // update time
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo))
      {
        Serial.println("Failed to obtain time");
      }
      else
      {
        int hour = timeinfo.tm_hour;
        int minute = timeinfo.tm_min;

        // check if party mode is not active
        if (anzeige.getPartyModeActive() == false)
        {
          anzeige.setTime(hour, minute);
        }
      }

      // adjust brightness to surrounding light
      if (anzeige.getPartyModeActive() == false)
      {
        anzeige.updateBrightness();
      }
    }
  }
};