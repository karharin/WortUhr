// Wandelt eine Temperatur in Kelvin in einen RGB-Wert um
#include <cmath>
#include <algorithm>
#include "utils.h"

void Utils::kelvinToRGB(float kelvin, float &red, float &green, float &blue)
{
  float temp = kelvin / 100.0;
  // float r, g, b;

  if (temp <= 66.0)
  {
    red = 255.0;
    green = 99.4708025861 * log(temp) - 161.1195681661;

    if (temp <= 19.0)
    {
      blue = 0.0;
    }
    else
    {
      blue = 138.5177312231 * log(temp - 10) - 305.0447927307;
    }
  }
  else
  {
    red = 329.698727446 * pow(temp - 60.0, -0.1332047592);
    green = 288.1221695283 * pow(temp - 60.0, -0.0755148492);
    blue = 255.0;
  }

  red = std::clamp(red, 0.0f, 255.0f);
  green = std::clamp(green, 0.0f, 255.0f);
  blue = std::clamp(blue, 0.0f, 255.0f);

  // Ergebniszuweisung als 8-Bit-Werte
  red = (uint8_t)red;
  green = (uint8_t)green;
  blue = (uint8_t)blue;
}

void Utils::updateDisplay(HW_Anzeige anzeige, SW_EEPROM_Speicher eeprom)
{
  if (!eeprom.getConfig().partyMode && eeprom.getConfig().power) // Nur Zeit setzen, wenn Partymodus nicht aktiv ist und uhr an
  {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
      if (eeprom.getConfig().nightActive && ((eeprom.getConfig().nightStartHour > eeprom.getConfig().nightEndHour && (timeinfo.tm_hour >= eeprom.getConfig().nightStartHour || timeinfo.tm_hour < eeprom.getConfig().nightEndHour)) ||
                                             (timeinfo.tm_hour >= eeprom.getConfig().nightStartHour && timeinfo.tm_hour < eeprom.getConfig().nightEndHour)))
      {
        if (anzeige.getAnzeigeIsOn())
          anzeige.turnOff(); // Uhr nachts ausschalten
        return;
      } // in der Zeit nicht updaten
      if (!eeprom.getConfig().nightActive && timeinfo.tm_hour >= eeprom.getConfig().nightEndHour)
        eeprom.setNightActive(true); // Nachtmodus aktivieren, wenn Endstunde erreicht
      if (timeinfo.tm_hour == anzeige.getLastHour() && timeinfo.tm_min == anzeige.getLastMinute())
      {
        return; // Zeit hat sich nicht geändert
      }
      anzeige.setTime(timeinfo.tm_hour, timeinfo.tm_min);
      if (!anzeige.getAnzeigeIsOn() && eeprom.getConfig().power)
        anzeige.turnOn(); // Uhr morgens wieder einschalten
      // Serial.printf("Zeit gesetzt: %02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min);
      anzeige.updateBrightness();
    }
    else
    {
      // Serial.println("Zeit konnte nicht geladen werden!");
    }
  }
  if (eeprom.getConfig().partyMode && eeprom.getConfig().power) // Partymodus update
  {
    unsigned long elapsed = (millis() - eeprom.getConfig().partyStartTime) / 1000;
    if (elapsed >= eeprom.getConfig().partyDuration)
    {
      eeprom.getConfig().partyMode = false;
      eeprom.save();
      anzeige.turnPartyOff();
    }
    else
    {
      anzeige.partyUpdate();
    }
  }
}