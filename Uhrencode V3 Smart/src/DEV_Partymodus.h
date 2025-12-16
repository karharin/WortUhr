struct DEV_Partymodus : Service::LightBulb
{ // Partymodus der Uhr

    // Adafruit_NeoPixel *pixels;      // reference to Neopixels
    SpanCharacteristic *power;      // reference to the On Characteristic
    SpanCharacteristic *brightness; // reference to the Brightness Characteristic, used for duration
    int duration = 10;
    unsigned long startPartyTime = 0;
    boolean partyActive = false;

    DEV_Partymodus() : Service::LightBulb()
    {
        power = new Characteristic::On(false, true);           // On Characteristic
        brightness = new Characteristic::Brightness(10, true); // Brightness Characteristic (duration)
        brightness->setRange(0, 120, 1);                       // sets the range of the duration
    }

    boolean update()
    { // update() method

        // party modus an/aus
        if (power->updated())
        {
            if (power->getNewVal() == 1)
            {
                Serial.println("Turn partymode on");
                anzeige.turnPartyOn();
                partyActive = true;
                startPartyTime = millis();
            }
            else
            {
                anzeige.turnPartyOff();
                Serial.println("Turn partymode on");
                partyActive = false;
            }
        }

        // brightness characteristic has been updated
        if (brightness->updated())
        {
            duration = brightness->getNewVal();
            Serial.println("Duration: " + duration);
        }

        return (true);

    } // update

    void loop()
    {
        if (partyActive)
        {
            unsigned long currentPartyTime = (millis() - startPartyTime) / 1000;
            if (currentPartyTime >= duration)
            {
                partyActive = false;
                anzeige.turnPartyOff();
                brightness->setVal(0);
                power->setVal(0);
                Serial.println("Turn partymode off");
            }
            else if (millis() % 100 < 10)
            {
                // Serial.print("Party running, time left: ");
                // Serial.println(duration - currentPartyTime);
                brightness->setVal(duration - currentPartyTime);
            }
            anzeige.partyUpdate();
        }
    }
};