#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoOTA.h>
#include "SW_EEPROM_Speicher.h"

class SW_WIFI_Manager {

    public:
        SW_WIFI_Manager(const char* hostname = "MillerWortUhr", SW_EEPROM_Speicher* eeprom = nullptr);

        void begin();
        void handle();
        bool isConnected();



    private:
        const char* _hostname;
        SW_EEPROM_Speicher* _eeprom;
        char _ssid[32];
        char _password[64];

        AsyncWebServer* _apServer = nullptr;

        void loadCredentials();
        void saveCredentials(const char* ssid, const char* password);
        
        void connectToWiFi();
        void setupAPWebServer();
        void setupOTA();

        void handleConfigPage(AsyncWebServerRequest *request);
        void handleSaveCredentials(AsyncWebServerRequest *request);
};
