#include "HW_Uhrzeit_Anzeige.h"
#include "SW_EEPROM_Speicher.h"

class Utils {
public:
    static void kelvinToRGB(float kelvin, float &red, float &green, float &blue);
    static void updateDisplay(HW_Anzeige display, SW_EEPROM_Speicher eeprom);
};