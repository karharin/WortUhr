#include <Arduino.h>
#include "HARDWARE.h"
#include "SW_EEPROM_Speicher.h"
#include "SW_Website.h"
#include "HW_Uhrzeit_Anzeige.h"
#include "SW_WIFI_Manager.h"
#include "utils.h"

Adafruit_NeoPixel pixels(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
BH1750 lightSensor;

// Zeiger statt globale Instanzen
SW_EEPROM_Speicher eeprom; // KEIN Zeiger
HW_Anzeige anzeige(&pixels, &lightSensor);
SW_WIFI_Manager wifiManager("MillerWortUhr", &eeprom);
SW_Website website(&anzeige, &eeprom);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starte WortUhr...");

  // Reihenfolge ist wichtig!
  eeprom.begin();

  anzeige.begin();

  anzeige.setMaxBrightness(eeprom.getConfig().maxBrightness);
  anzeige.setMinBrightness(eeprom.getConfig().minBrightness);
  anzeige.setSensitivity(eeprom.getConfig().sensitivity);

  // Farbe aus Kelvin ins RAM (falls so vorgesehen)
  float r, g, b;
  Utils::kelvinToRGB(eeprom.getConfig().colorKelvin, r, g, b);
  anzeige.fillColor(r, g, b);

  // Sichtbar machen:
  anzeige.setBrightness(eeprom.getConfig().minBrightness); // sofort >0
  anzeige.turnOn();
  Utils::updateDisplay(anzeige, eeprom);

  wifiManager.begin();
  while (!wifiManager.isConnected())
  {
    delay(100);
  }

  website.begin();

  Serial.println("Setup abgeschlossen");
}

void loop()
{
  wifiManager.handle();
  Utils::updateDisplay(anzeige, eeprom);
}
