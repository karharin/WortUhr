#include "HW_Uhrzeit_Anzeige.h"

HW_Anzeige::HW_Anzeige(Adafruit_NeoPixel *pixels, BH1750 *lightSensor)
{
    this->pixels = pixels;
    this->lightSensor = lightSensor;

    AnzeigeControlStruct.partyModeActive = false;
    AnzeigeControlStruct.AnzeigeOn = false;
    AnzeigeControlStruct.lastColor = pixels->Color(255, 255, 255);
    AnzeigeControlStruct.currentBrightness = 50;
}

// begin
// initializes the display
void HW_Anzeige::begin()
{
    pixels->begin();
    pixels->clear();

    Wire.begin(I2C_SDA, I2C_SCL);
    lightSensor->begin();

    AnzeigeControlStruct.partyModeActive = false;
    AnzeigeControlStruct.AnzeigeOn = false;
    AnzeigeControlStruct.lastColor = pixels->Color(255, 255, 255);
    AnzeigeControlStruct.currentBrightness = 0;
    AnzeigeControlStruct.maxBrightness = 255;
    AnzeigeControlStruct.minBrightness = 0;
    AnzeigeControlStruct.sensitivity = 20;
}

// updates the display
void HW_Anzeige::updateAnzeige()
{
    pixels->show();
}

// turns complete display off
void HW_Anzeige::turnOff()
{

    pixels->clear();
    pixels->show();
    AnzeigeControlStruct.AnzeigeOn = false;
}

// returns if the display is on
boolean HW_Anzeige::getAnzeigeIsOn()
{
    return AnzeigeControlStruct.AnzeigeOn;
}

// turns complete display on
void HW_Anzeige::turnOn()
{
    AnzeigeControlStruct.AnzeigeOn = true;
    this->setTime(AnzeigeControlStruct.lastHour, AnzeigeControlStruct.lastMinute);
    Serial.println("Anzeige turned on");
}

// sets the brightness of the display
void HW_Anzeige::setBrightness(int brightness)
{
    pixels->setBrightness(brightness);
    pixels->show();
    AnzeigeControlStruct.currentBrightness = brightness;
}

// sets the color of a specific pixel
void HW_Anzeige::setColorLetter(int letterNum, uint32_t color)
{
    for (int i = 0; i < 4; i++)
    {
        pixels->setPixelColor((letterNum - 1) * 4 + i, color);
    }
    // pixels->show();
}

// fills the whole display with a specific color
void HW_Anzeige::fillColor(uint8_t r, uint8_t g, uint8_t b)
{
    //pixels->fill(pixels->Color(r, g, b));
    //pixels->show();
    AnzeigeControlStruct.lastColor = pixels->Color(r, g, b);
}

// sets the time on the display
void HW_Anzeige::setTime(int hour, int minute)
{
    // clear display
    // pixels->clear();
    // Serial.print("Set time: ");
    // Serial.print(hour);
    // Serial.print(":");
    // Serial.println(minute);

    for (int i = 1; i <= 114; i++)
    {
        this->setColorLetter(i, pixels->Color(0, 0, 0));
    }

    // store time for later use
    AnzeigeControlStruct.lastHour = hour;
    AnzeigeControlStruct.lastMinute = minute;

    // set esIst
    for (int i = esIst[0][0]; i <= esIst[0][1]; i++)
    {
        this->setColorLetter(i, AnzeigeControlStruct.lastColor);
    }
    for (int i = esIst[1][0]; i <= esIst[1][1]; i++)
    {
        this->setColorLetter(i, AnzeigeControlStruct.lastColor);
    }

    // points
    int pointIndex = minute % 5;
    // Serial.print("pointIndex: ");
    // Serial.println(pointIndex);

    for (int i = 0; i < pointIndex; i++)
    {
        this->setColorLetter(points[i], AnzeigeControlStruct.lastColor);
    }

    // Minuten kürzen
    minute -= pointIndex;
    // Serial.print("minute nach Abzug: ");
    // Serial.println(minute);

    // set hour
    // 24 zu 12 Stunden Format
    if (hour > 12)
    {
        hour -= 12;
    }
    // Ab 25 nach heist es vor
    if (minute >= 25)
    {
        hour++;
    }
    if (hour > 12)
    {
        hour = 1;
    }
    else if (hour == 0)
    {
        hour = 12;
    }
    // Serial.print("hour: ");
    // Serial.println(hour);

    for (int i = hours[hour - 1][0]; i <= hours[hour - 1][1]; i++)
    {
        this->setColorLetter(i, AnzeigeControlStruct.lastColor);
    }

    // set minute

    // Sonderfälle
    if (minute == 0)
    {
        // volle Stunde
        for (int i = uhr[0]; i <= uhr[1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        if (hour == 1) {
           this->setColorLetter(hours[hour-1][0], pixels->Color(0, 0, 0));
        }
    }
    else if (minute == 15) 
    {
        //viertel nach
        // nach
        for (int i = nach[0]; i <= nach[1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        //Viertel
        for (int i = minutes[2][0]; i <= minutes[2][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
    }
    else if (minute == 30)
    {
        // halb
        for (int i = minutes[5][0]; i <= minutes[5][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
    }
    else if (minute == 25)
    {
        // 5 vor halb
        // halb
        for (int i = minutes[5][0]; i <= minutes[5][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        // vor
        for (int i = vor[0]; i <= vor[1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        //fünf
        for (int i = minutes[0][0]; i <= minutes[0][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
    }
    else if (minute == 35)
    {
        // 5 nach halb
        // halb
        for (int i = minutes[5][0]; i <= minutes[5][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        // nach
        for (int i = nach[0]; i <= nach[1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        //fünf
        for (int i = minutes[0][0]; i <= minutes[0][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
    }
    else if (minute == 45)
    {
        // Dreiviertel
        for (int i = minutes[8][0]; i <= minutes[8][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
    }
    else
    {
        // normale Minuten
        int minuteIndex = minute / 5 - 1;
        for (int i = minutes[minuteIndex][0]; i <= minutes[minuteIndex][1]; i++)
        {
            this->setColorLetter(i, AnzeigeControlStruct.lastColor);
        }
        // setze vor oder nach
        if (minute < 25)
        {
            for (int i = nach[0]; i <= nach[1]; i++)
            {
                this->setColorLetter(i, AnzeigeControlStruct.lastColor);
            }
        }
        else
        {
            for (int i = vor[0]; i <= vor[1]; i++)
            {
                this->setColorLetter(i, AnzeigeControlStruct.lastColor);
            }
        }
    }
    AnzeigeControlStruct.AnzeigeOn = true;
    this->updateAnzeige();
}

// turns party mode on
void HW_Anzeige::turnPartyOn()
{
    AnzeigeControlStruct.partyModeActive = true;
}

// turns party mode off
void HW_Anzeige::turnPartyOff()
{
    AnzeigeControlStruct.partyModeActive = false;
    this->turnOn();
}

// updates party mode
void HW_Anzeige::partyUpdate()
{
    if (AnzeigeControlStruct.partyModeActive)
    {
        // generate lava lamp effect: iterate over all pixels and set a random color
        // https://pastebin.com/6xT72A46
        // for (int i = 0; i < 114; i++)
        // {
        //     this->setColorLetter(i, pixels->Color(random(255), random(255), random(255)));
        // }
        // random color in corners
        this->setColorLetter(111, pixels->Color(0, 0, 0)); //random(255), random(255), random(255)));
        this->setColorLetter(112, pixels->Color(0, 0, 0)); //random(255), random(255), random(255)));
        this->setColorLetter(113, pixels->Color(0, 0, 0)); //random(255), random(255), random(255)));
        this->setColorLetter(114, pixels->Color(0, 0, 0)); //random(255), random(255), random(255)));

        uint16_t NUM_COLS = 11;
        uint16_t NUM_ROWS = 10;
        uint16_t ms = millis();
        for (int i = 0; i < NUM_COLS; i++)
        {
            for (int j = 0; j < NUM_ROWS; j++)
            {
                uint8_t index = inoise8(i * 41, j * 41, ms / 4); // x and y not change, only change z value with time increment for nice plasma looking noise
                // leds[XY(i, j)] = CHSV((index + ms / 200) % 255, 255, BRIGHTNESS); // within palette
                this->setColorLetter(XY(i, j), pixels->ColorHSV(((index + ms / 200) % 255) * 257, 255, AnzeigeControlStruct.maxBrightness));
            }
        }
        pixels->show();
    }
}

// get party mode status
boolean HW_Anzeige::getPartyModeActive()
{
    return AnzeigeControlStruct.partyModeActive;
}

// set max brightness
// sets the maximum brightness of the display
void HW_Anzeige::setMaxBrightness(int maxBrightness)
{
    AnzeigeControlStruct.maxBrightness = maxBrightness;
}

// set min brightness
// sets the minimum brightness of the display
void HW_Anzeige::setMinBrightness(int minBrightness)
{
    AnzeigeControlStruct.minBrightness = minBrightness;
}

// get max brightness
// returns the maximum brightness of the display
int HW_Anzeige::getMaxBrightness()
{
    return AnzeigeControlStruct.maxBrightness;
}

// get min brightness
// returns the minimum brightness of the display
int HW_Anzeige::getMinBrightness()
{
    return AnzeigeControlStruct.minBrightness;
}

// set sensitivity
// sets the sensitivity of the display
void HW_Anzeige::setSensitivity(int sensitivity)
{
    AnzeigeControlStruct.sensitivity = sensitivity;
}

// update brightness
// updates the brightness of the display
void HW_Anzeige::updateBrightness()
{
    // ---- Tuning-Parameter ----
    static const float     LUX_MIN     = 1.0f;     // alles darunter wie "sehr dunkel" behandeln
    static const float     LUX_MAX     = 300.0f;   // effektiver Maximalwert deiner Umgebung
    static const float     GAMMA       = 2.2f;     // Wahrnehmungskorrektur
    static const uint8_t   DEADBAND    = 2;        // Änderung in Brightness-Schritten, bevor wir updaten
    static const uint32_t  MIN_DT_MS   = 50;       // min. Zeitabstand zwischen Updates (Rate-Limit)

    // ---- interne Zustände (persistieren über Aufrufe) ----
    static uint32_t lastT   = 0;
    static int      lastOut = -1;
    static float    ema     = -1.0f;

    // ---- Rate-Limit ----
    const uint32_t now = millis();
    if (now - lastT < MIN_DT_MS) return;
    lastT = now;

    // ---- Lux lesen & plausibilisieren ----
    float lux = lightSensor->readLightLevel();
    if (!isfinite(lux) || lux < 0.0f) lux = LUX_MIN;
    if (lux < LUX_MIN)  lux = LUX_MIN;
    if (lux > LUX_MAX)  lux = LUX_MAX;

    // ---- Nutzergrenzen robust behandeln ----
    int minB = AnzeigeControlStruct.minBrightness;
    int maxB = AnzeigeControlStruct.maxBrightness;
    if (maxB < minB) { int tmp = maxB; maxB = minB; minB = tmp; }

    // Sensitivität (1..100) auf 0..1
    float sens01 = constrain(AnzeigeControlStruct.sensitivity, 1, 100) / 100.0f;

    // ---- Normierung + nichtlineare Kurve (log + Gamma) ----
    float x = (lux - LUX_MIN) / (LUX_MAX - LUX_MIN);  // 0..1
    x = constrain(x, 0.0f, 1.0f);

    // log-Betoning für Auflösung im Dunkeln
    float x_log   = logf(1.0f + 9.0f * x) / logf(10.0f); // 0..1
    float x_gamma = powf(x_log, 1.0f / GAMMA);           // 0..1

    // Sensitivität mischt die Kurvencharakteristik
    float curve   = (1.0f - sens01) * x_log + sens01 * x_gamma;

    // Zielhelligkeit im Nutzerbereich
    float targetF = minB + curve * float(maxB - minB);

    // ---- EMA-Glättung ----
    float alpha = 0.12f + 0.38f * sens01;     // ~0.12..0.5, höher = reaktiver
    if (ema < 0.0f) ema = targetF;            // Initialisierung
    else            ema = ema + alpha * (targetF - ema);

    int out = int(lroundf(ema));
    out = constrain(out, minB, maxB);

    // ---- Deadband: nur bei spürbarer Änderung schreiben ----
    if (lastOut >= 0 && abs(out - lastOut) < DEADBAND) return;

    // setBrightness() ruft bei dir bereits pixels->show() auf
    this->setBrightness(out);
    AnzeigeControlStruct.currentBrightness = out;
    lastOut = out;
}


uint8_t HW_Anzeige::XY(uint8_t x, uint8_t y)
{
    uint16_t kMatrixWidth = 11;
    uint16_t kMatrixHeight = 10;
    uint16_t LAST_VISIBLE_LED = 109;
    // any out of bounds address maps to the first hidden pixel
    if ((x >= kMatrixWidth) || (y >= kMatrixHeight))
    {
        return (LAST_VISIBLE_LED + 1);
    }

    const uint8_t XYTable[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33,
        44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
        65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55,
        66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
        87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,
        88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
        109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99};

    uint8_t i = (y * kMatrixWidth) + x;
    uint8_t j = XYTable[i];
    return j + 1;
}