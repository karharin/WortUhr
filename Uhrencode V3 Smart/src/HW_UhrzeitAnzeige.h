#ifndef HW_UHRZEITANZEIGE_H
#define HW_UHRZEITANZEIGE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "HARDWARE.h"
#include <Wire.h>
#include "BH1750.h"
#include "FastLED.h"

class HW_Anzeige
{

private:
    Adafruit_NeoPixel *pixels;
    BH1750 *lightSensor;
    // contains start and end index of each hour
    int hours[12][2] = {
        {63, 66},   // 1
        {56, 59},   // 2
        {67, 70},   // 3
        {74, 77},   // 4
        {52, 55},   // 5
        {84, 88},   // 6
        {89, 94},   // 7
        {78, 81},   // 8
        {104, 107}, // 9
        {107, 110}, // 10
        {50, 52},   // 11
        {95, 99}    // 12
    };
    // contains start and end index of each minute
    int minutes[11][2] = {
        {8, 11},  // 5
        {19, 22}, // 10
        {27, 33}, // 15
        {12, 18}, // 20
        {0, 0},   // 25
        {45, 48}, // 30
        {0, 0},   // 35
        {12, 18}, // 40
        {23, 33}, // 45
        {19, 22}, // 50
        {8, 11},  // 55
    };

    int points[4] = {111, 112, 113, 114};

    int esIst[2][2] = {
        {1, 2}, // es
        {4, 6}  // ist
    };

    int vor[2] = {42, 44};
    int nach[2] = {34, 37};

    int uhr[2] = {100, 102};

    struct
    {
        boolean partyModeActive;
        boolean AnzeigeOn;
        uint32_t lastColor;
        int currentBrightness;
        int maxBrightness;
        int minBrightness;
        int lastHour;
        int lastMinute;
        int sensitivity;
    } AnzeigeControlStruct;

public:
    HW_Anzeige(Adafruit_NeoPixel *pixels, BH1750 *lightSensor);
    void begin();
    void turnOff();
    void turnOn();
    void setBrightness(int brightness);
    void setColorLetter(int letterNum, uint32_t color);
    void updateAnzeige();
    void fillColor(uint8_t r, uint8_t g, uint8_t b);
    void setTime(int hour, int minute);
    void turnPartyOn();
    void partyUpdate();
    void turnPartyOff();
    boolean getAnzeigeIsOn();
    boolean getPartyModeActive();
    void setMaxBrightness(int maxBrightness);
    void setMinBrightness(int minBrightness);
    int getMaxBrightness();
    int getMinBrightness();
    void setSensitivity(int sensitivity);
    void updateBrightness();
    uint8_t XY(uint8_t x, uint8_t y);
};

#endif /* HW_UHRZEITANZEIGE_H */
