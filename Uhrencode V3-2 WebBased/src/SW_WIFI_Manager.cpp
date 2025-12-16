#include "SW_WIFI_Manager.h"

SW_WIFI_Manager::SW_WIFI_Manager(const char* hostname, SW_EEPROM_Speicher* eeprom)
    : _hostname(hostname), _eeprom(eeprom) {
    _ssid[0] = '\0';
    _password[0] = '\0';
}

void SW_WIFI_Manager::begin() {
    Serial.begin(115200);
    Serial.println("\nESP32 WortUhr startet...");
    loadCredentials();

    WiFi.setHostname(_hostname);
    connectToWiFi();

    if(WiFi.status() == WL_CONNECTED) {
        setupOTA();
    } 
}

void SW_WIFI_Manager::handle() {
    ArduinoOTA.handle();
}

bool SW_WIFI_Manager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void SW_WIFI_Manager::loadCredentials() {
   if (_eeprom) {
        strncpy(_ssid, _eeprom->getConfig().ssid, sizeof(_ssid));
        _ssid[sizeof(_ssid) - 1] = '\0'; // Sicherheitsabschluss

        strncpy(_password, _eeprom->getConfig().password, sizeof(_password));
        _password[sizeof(_password) - 1] = '\0';

        Serial.println("Geladene WLAN-Daten:");
        Serial.print("SSID: ");
        Serial.println(_ssid);
        Serial.print("Passwort: ");
        Serial.println(_password);
        // Passwort loggen ist optional
    } else {
        Serial.println("⚠️ Kein EEPROM-Objekt übergeben!");
        _ssid[0] = '\0';
        _password[0] = '\0';
    }

    //Serial.println("Gespeicherte WLAN-Daten: " + String(_ssid) + " / " + String(_password));
}

void SW_WIFI_Manager::saveCredentials(const char* ssid, const char* password) {
    _eeprom->setWiFi(String(ssid), String(password));
    _eeprom->save();
}

void SW_WIFI_Manager::connectToWiFi() {
        Serial.println("Verbinde mit WLAN: " + String(_ssid));
        WiFi.begin(_ssid, _password);

        unsigned long startAttemptTime = millis();

        // Warte bis zur Verbindung oder Timeout nach 10 Sekunden
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            Serial.print(".");
            delay(500);
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nVerbunden! IP-Adresse: " + WiFi.localIP().toString());
            configTime(0, 0, "pool.ntp.org", "time.nist.gov");
            setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // Mitteleuropäische Zeit
            tzset();
        } else {
            Serial.println("\nVerbindung fehlgeschlagen. Starte Access Point...");
            setupAPWebServer();
        }
        
}

void SW_WIFI_Manager::setupAPWebServer() {
    WiFi.softAP("MillerWortUhr-Setup", "123456789");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    if (_apServer != nullptr) {
        Serial.println("⚠️ AP Webserver läuft bereits!");
        return;
    }
    _apServer = new AsyncWebServer(80);
    

    _apServer->on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleConfigPage(request);
    });

    _apServer->on("/save", HTTP_POST, [this](AsyncWebServerRequest *request){
        handleSaveCredentials(request);
    });

    _apServer->begin();
}

void SW_WIFI_Manager::setupOTA() {
    ArduinoOTA.setHostname(_hostname);

    ArduinoOTA.onStart([]() {
        Serial.println("OTA Update gestartet...");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA Update abgeschlossen.");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Fortschritt: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Fehler[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("OTA bereit. IP-Adresse: " + WiFi.localIP().toString());
}

void SW_WIFI_Manager::handleConfigPage(AsyncWebServerRequest *request) {
    String html = R"rawliteral(
    <!DOCTYPE HTML><html><head>
      <title>WortUhr WLAN Konfiguration</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; background-color: #f0f0f0; }
        h1 { background-color: #4CAF50; color: white; padding: 20px 0; margin: 0; }
        form { margin: 20px auto; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); max-width: 400px; }
        input[type=text], input[type=password] { width: 100%; padding: 12px; margin: 8px 0; box-sizing: border-box; border: 1px solid #ccc; border-radius: 4px; }
        input[type=submit] { background-color: #4CAF50; color: white; padding: 12px 20px; border: none; border-radius: 4px; cursor: pointer; }
        input[type=submit]:hover { background-color: #45a049; }
      </style>
    </head><body>
      <h1>WortUhr WLAN Konfiguration</h1>
      <form action="/save" method="POST">
        <label for="ssid">SSID:</label><br>
        <input type="text" id="ssid" name="ssid" required><br>
        <label for="password">Passwort:</label><br>
        <input type="password" id="password" name="password"><br><br>
        <input type="submit" value="Speichern & Verbinden">
      </form>
    </body></html>)rawliteral";

    request->send(200, "text/html", html);
}

void SW_WIFI_Manager::handleSaveCredentials(AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();

        saveCredentials(ssid.c_str(), password.c_str());

        String response = "<html><body><h3>Daten gespeichert! Verbinde mit " + ssid + "...</h3></body></html>";
        request->send(200, "text/html", response);

        delay(2000);
        ESP.restart();
    } else {
        request->send(400, "text/plain", "Fehler: SSID und Passwort erforderlich");
    }
}