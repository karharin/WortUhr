struct DEV_Anzeige_Farbton : Service::LightBulb
{ // Anzeige der Uhr

    // Adafruit_NeoPixel *pixels;      // reference to Neipixels
    SpanCharacteristic *power; // reference to the On Characteristic
    // SpanCharacteristic *brightness; // reference to the Brightness Characteristic
    SpanCharacteristic *colorTemp; // reference to the ColorTemperature Characteristic

    // DEV_Anzeige(Adafruit_NeoPixel *pixels) : Service::LightBulb()
    DEV_Anzeige_Farbton() : Service::LightBulb()
    {
        // second argument: store to nvs
        power = new Characteristic::On(false, true);                 // On Characteristic
        colorTemp = new Characteristic::ColorTemperature(200, true); // ColorTemperature Characteristic

    } // end constructor

    boolean update()
    { // update() method

        // power characteristic has been updated
        // if (power->updated())
        // {
        //     if (power->getNewVal() == 1)
        //     {
        //         Serial.println("Turn Anzeige on");
        //         anzeige.turnOn();
        //     }
        //     else
        //     {
        //         Serial.println("Turn Anzeige off");
        //         anzeige.turnOff();
        //     }
        // }

        // colorTemperature characteristic has been updated
        if (colorTemp->updated())
        {
            // Umrechnung in Kelvin
            float kelvin = 1000000.0 / colorTemp->getNewVal();

            float red, green, blue;
            kelvinToRGB(kelvin, red, green, blue);

            anzeige.fillColor(red, green, blue);
        }

        return (true); // return true

    } // update

    // void loop()
    // {
    //     // update brightness every second
    //     if (millis() % 1000 == 0)
    //     {
    //         // adjust brightess to surrounding light
    //         int brightnessSour = analogRead(34);
    //         Serial.print("Brightness measured: ");
    //         Serial.println(brightnessSour);

    //         // map to percentage in range
    //         brightnessSour = map(brightnessSour, 0, 4095, anzeige.getMinBrightness(), anzeige.getMaxBrightness());
    //         Serial.print("Brightness mapped: ");
    //         Serial.println(brightnessSour);

    //         // set brightness
    //         anzeige.setBrightness((float)brightnessSour / 100 * 255);
    //     }
    // }
};