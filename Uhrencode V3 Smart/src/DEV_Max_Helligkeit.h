struct DEV_Max_Helligkeit : Service::LightBulb
{ // Anzeige der Uhr

    // Adafruit_NeoPixel *pixels;      // reference to Neipixels
    SpanCharacteristic *power;      // reference to the On Characteristic
    SpanCharacteristic *brightness; // reference to the Brightness Characteristic

    // DEV_Anzeige(Adafruit_NeoPixel *pixels) : Service::LightBulb()
    DEV_Max_Helligkeit() : Service::LightBulb()
    {
        power = new Characteristic::On(false, true);          // On Characteristic
        brightness = new Characteristic::Brightness(0, true); // Brightness Characteristic with an initial value of 50
        brightness->setRange(0, 100, 1);                      // sets the range of the Brightness

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

        // brightness characteristic has been updated
        if (brightness->updated())
        {
            int newBrightness = brightness->getNewVal();
            Serial.print("new Max Brightness: ");
            Serial.println(newBrightness);
            if (newBrightness > 0)
            {
                // float newBrightnessFloat = (newBrightness / 100.0) * 255.0;
                // Serial.print("new Brightness Float: ");
                // Serial.println(newBrightnessFloat);
                // anzeige.setBrightness(newBrightness);
                anzeige.setMaxBrightness(newBrightness);
            }
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