#pragma once
#include <Arduino.h>
#include <EEPROM.h>

class SW_EEPROM_Speicher
{
public:
    struct Einstellungen {

         uint32_t magic    = 0x57554C31; // 'WUL1'
        uint16_t version  = 1;

        char ssid[32];
        char password[32];
        int minBrightness = 30;
        int maxBrightness = 120;
        int sensitivity = 50;
        bool power = true;
        int colorKelvin = 3000;
        bool partyMode = false;
        int partyDuration = 60;
        unsigned long partyStartTime = 0;
        bool nightActive = true;
        u_int8_t nightStartHour = 0;
        u_int8_t nightEndHour = 7;
    };

    SW_EEPROM_Speicher(size_t size = 512) : _size(size) {}

    void begin();
    void load();
    void save();
    void setWiFi(const String &ssid, const String &password) { 
        strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid));
        strncpy(config.password, password.c_str(), sizeof(config.password));
    }
    void setMinBrightness(int minBrightness) { config.minBrightness = minBrightness; }
    void setMaxBrightness(int maxBrightness) { config.maxBrightness = maxBrightness; }
    void setSensitivity(int sensitivity) { config.sensitivity = sensitivity; }
    void setPower(bool power) { config.power = power; }
    void setColorKelvin(int colorKelvin) { config.colorKelvin = colorKelvin; }
    void setPartyMode(bool partyMode) { config.partyMode = partyMode; }
    void setPartyDuration(int partyDuration) { config.partyDuration = partyDuration; }
    void setPartyStartTime(unsigned long partyStartTime) { config.partyStartTime = partyStartTime; }
    void setNightActive(bool nightActive) { config.nightActive = nightActive; }
    void setNightStartHour(uint8_t nightStartHour) { config.nightStartHour = nightStartHour; }
    void setNightEndHour(uint8_t nightEndHour) { config.nightEndHour = nightEndHour; }

    Einstellungen& getConfig() { return config; }

private:
    void setDefaults();
    size_t _size;
    Einstellungen config;
};

