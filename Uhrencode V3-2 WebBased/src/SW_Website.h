#pragma once
#include <ESPAsyncWebServer.h>
#include "HW_Uhrzeit_ANzeige.h"
#include "SW_EEPROM_Speicher.h"

class SW_Website {
public:
    SW_Website(HW_Anzeige* anzeige, SW_EEPROM_Speicher* eeprom);
    void begin();

private:
    AsyncWebServer server;
    HW_Anzeige* anzeige;
    SW_EEPROM_Speicher* eeprom;
};
