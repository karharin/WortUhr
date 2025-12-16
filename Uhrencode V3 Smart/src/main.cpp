#include <Arduino.h>
#include <HomeSpan.h>
#include "HARDWARE.h"
#include "HW_UhrzeitAnzeige.h"

Adafruit_NeoPixel pixels(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
BH1750 lightSensor;

HW_Anzeige anzeige(&pixels, &lightSensor);

void kelvinToRGB(float kelvin, float &r, float &g, float &b);

#include "DEV_Anzeige_Power.h"
#include "DEV_Anzeige_Farbton.h"
#include "DEV_Partymodus.h"
#include "DEV_Max_Helligkeit.h"
#include "DEV_Min_Helligkeit.h"
#include "DEV_Sensitivity_Helligkeit.h"

struct statusLEDFunk : Blinkable
{ // create a child class derived from Blinkable

  int LEDNumbers[4] = {38, 39, 40, 41};
  // int LEDNumbers[4] = {0, 1, 2, 3};

  statusLEDFunk() {}

  void on() override
  {
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        pixels.setPixelColor((LEDNumbers[i] - 1) * 4 + j, pixels.Color(0, 255, 0));
        // Serial.println("LED: " + String(LEDNumbers[i] * 4 + j) + " on");
      }
    }
    pixels.show();
  }
  void off() override
  {
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        pixels.setPixelColor((LEDNumbers[i] - 1) * 4 + j, pixels.Color(0, 0, 0));
      }
    }
    pixels.show();
  }
  int getPin() override { return (pixels.getPin()); } // required function getPin() - returns pin number
};

void setup()
{

  Serial.begin(115200);

  Serial.println("Starting...");

  // Button
  // pinMode(BUTTON_PIN_GND, OUTPUT);
  // digitalWrite(BUTTON_PIN_GND, LOW);

  // homeSpan.setControlPin(BUTTON_PIN_SIG, PushButton::TRIGGER_ON_LOW);

  homeSpan.setApSSID("MillerWortUhr-Setup");
  homeSpan.setApPassword("MillerWortUhr-Setup");
  homeSpan.enableAutoStartAP();

  // homeSpan.setStatusPixel(LED_DATA_PIN, 120, 100, 100);
  homeSpan.setStatusDevice(new statusLEDFunk());

  // Einstellungszustand speichern
  // --> erledigt in DEV Klassen

  // enable OTA updates
  // https://github.com/HomeSpan/HomeSpan/blob/master/docs/OTA.md
  homeSpan.enableOTA(false);

  homeSpan.begin(Category::Bridges, "MillerWortuhrBridge");

  homeSpan.enableWebLog(10, "pool.ntp.org", "CET-1CEST,M3.5.0,M10.5.0/3", "UhrenLog");

  // // "Bridge" Accessory

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();

  // // Anzeige

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new DEV_Anzeige_Power();
  new Characteristic::ConfiguredName("Power");
  new DEV_Partymodus();
  new Characteristic::ConfiguredName("Partymodus");
  new DEV_Max_Helligkeit();
  new Characteristic::ConfiguredName("HelligkeitMax");
  new DEV_Anzeige_Farbton();
  new Characteristic::ConfiguredName("Farbton");
  new DEV_Min_Helligkeit();
  new Characteristic::ConfiguredName("HelligkeitMin");
  new DEV_Sensitivity_Helligkeit();
  new Characteristic::ConfiguredName("Sensitivity");

  Serial.println("Begin of Anzeige...");
  anzeige.begin();

  //   pixels.setBrightness(120);

  //   // // Test: turn all LEDs on
  //   for (int i = 0; i < LED_COUNT; i++)
  //   {
  //     pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  //   }
  //   pixels.show();
}

void loop()
{
  homeSpan.poll();
}

// Wandelt eine Temperatur in Kelvin in einen RGB-Wert um
void kelvinToRGB(float kelvin, float &r, float &g, float &b)
{
  // float temp = kelvin / 100.0;
  // float red, green, blue;

  // if (temp <= 66)
  // {
  //   red = 255;
  //   green = 99.4708025861 * log(temp) - 161.1195681661;
  //   if (temp <= 19)
  //   {
  //     blue = 0;
  //   }
  //   else
  //   {
  //     blue = 138.5177312231 * log(temp - 10) - 305.0447927307;
  //   }
  // }
  // else
  // {
  //   red = 329.698727446 * pow((temp - 60) / 100.0, -0.1332047592);
  //   green = 288.1221695283 * pow((temp - 60) / 100.0, -0.0755148492);
  //   blue = 255;
  // }

  // // Werte begrenzen
  // r = constrain((int)red, 0, 255);
  // g = constrain((int)green, 0, 255);
  // b = constrain((int)blue, 0, 255);

  float temp = kelvin / 100.0;
  // float r, g, b;

  // Berechne Red:
  if (temp <= 66.0)
  {
    r = 255.0;
  }
  else
  {
    r = temp - 60.0;
    r = 329.698727446 * pow(r, -0.1332047592);
    if (r < 0)
      r = 0;
    if (r > 255)
      r = 255;
  }

  // Berechne Green:
  if (temp <= 66.0)
  {
    g = temp;
    g = 99.4708025861 * log(g) - 161.1195681661;
    if (g < 0)
      g = 0;
    if (g > 255)
      g = 255;
  }
  else
  {
    g = temp - 60.0;
    g = 288.1221695283 * pow(g, -0.0755148492);
    if (g < 0)
      g = 0;
    if (g > 255)
      g = 255;
  }

  // Berechne Blue:
  if (temp >= 66.0)
  {
    b = 255.0;
  }
  else
  {
    if (temp <= 19.0)
    {
      b = 0.0;
    }
    else
    {
      b = temp - 10.0;
      b = 138.5177312231 * log(b) - 305.0447927307;
      if (b < 0)
        b = 0;
      if (b > 255)
        b = 255;
    }
  }

  // Ergebniszuweisung als 8-Bit-Werte
  r = (uint8_t)r;
  g = (uint8_t)g;
  b = (uint8_t)b;
}
