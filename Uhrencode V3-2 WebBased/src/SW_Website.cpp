#include "SW_Website.h"
#include "utils.h"

SW_Website::SW_Website(HW_Anzeige *anzeige, SW_EEPROM_Speicher *eeprom) : server(80), anzeige(anzeige), eeprom(eeprom) {}

void SW_Website::begin()
{
  // Power
  server.on("/power", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    bool state = request->getParam("state")->value().toInt();
    this->eeprom->setPower(state);
    this->eeprom->save();
    if (state) {
      this->anzeige->turnOn();
    } else {
      this->anzeige->turnOff();
    }
    request->send(200, "text/plain", state ? "ON" : "OFF"); });

  // Farbton (Kelvin)
  server.on("/color", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    float kelvin = request->getParam("kelvin")->value().toFloat();
    this->eeprom->setColorKelvin(kelvin);
    this->eeprom->save();
    float r, g, b;
    Utils::kelvinToRGB(kelvin, r, g, b);
    this->anzeige->fillColor(r, g, b);
    Serial.println("Farbtemperatur gesetzt: " + String(kelvin) + "K -> RGB(" + String((int)r) + "," + String((int)g) + "," + String((int)b) + ")");
    request->send(200, "text/plain", "OK"); });

  // Max Brightness
  server.on("/brightness/max", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    int val = request->getParam("val")->value().toInt();
    this->eeprom->setMaxBrightness(val);
    this->eeprom->save();
    this->anzeige->setMaxBrightness(val);
    Serial.println("Max Brightness gesetzt: " + String(val));
    request->send(200, "text/plain", "OK"); });

  // Min Brightness
  server.on("/brightness/min", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    int val = request->getParam("val")->value().toInt();
    this->eeprom->setMinBrightness(val);
    this->eeprom->save();
    this->anzeige->setMinBrightness(val);
    Serial.println("Min Brightness gesetzt: " + String(val));
    request->send(200, "text/plain", "OK"); });

  // Sensitivity
  server.on("/sensitivity", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    int val = request->getParam("val")->value().toInt();
    this->eeprom->setSensitivity(val);
    this->eeprom->save();
    this->anzeige->setSensitivity(val);
    Serial.println("Sensitivity gesetzt: " + String(val));
    request->send(200, "text/plain", "OK"); });

  // Party Modus
  server.on("/partymode", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
    if (request->hasParam("state")) {
      bool state = request->getParam("state")->value().toInt();
      this->eeprom->setPartyMode(state);
      this->eeprom->save();
        if (state) {
            this->anzeige->turnPartyOn();
            this->eeprom->getConfig().partyStartTime = millis();
            this->eeprom->save();
        } else {
            this->anzeige->turnPartyOff();
        }
    }
    if (request->hasParam("duration")) {
      int dur = request->getParam("duration")->value().toInt();
      this->eeprom->setPartyDuration(dur);
      this->eeprom->save();
    }
    request->send(200, "text/plain", "OK"); });

  // GET /night?active=0|1&start=22&end=7
  server.on("/night", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
  auto &cfg = this->eeprom->getConfig();

  // active (optional)
  if (request->hasParam("active")) {
    bool active = request->getParam("active")->value().toInt() != 0;
    this->eeprom->setNightActive(active);
  }

  // start (optional, 0..23)
  if (request->hasParam("start")) {
    int startH = request->getParam("start")->value().toInt();
    startH = (startH < 0) ? 0 : (startH > 23 ? 23 : startH);
    this->eeprom->setNightStartHour((uint8_t)startH);
  }

  // end (optional, 0..23)
  if (request->hasParam("end")) {
    int endH = request->getParam("end")->value().toInt();
    endH = (endH < 0) ? 0 : (endH > 23 ? 23 : endH);
    this->eeprom->setNightEndHour((uint8_t)endH);
  }

  this->eeprom->save();

  String resp = "nightActive=" + String(cfg.nightActive ? 1 : 0)
              + ", nightStartHour=" + String(cfg.nightStartHour)
              + ", nightEndHour=" + String(cfg.nightEndHour);
  request->send(200, "text/plain", resp); });

  // --- Aktuelle Konfiguration als JSON (für UI-Initialisierung) ---
  server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
  auto &c = this->eeprom->getConfig();
  // Ohne ArduinoJson: String sicher zusammenbauen
  String json = "{";
  json += "\"minBrightness\":" + String(c.minBrightness) + ",";
  json += "\"maxBrightness\":" + String(c.maxBrightness) + ",";
  json += "\"sensitivity\":" + String(c.sensitivity) + ",";
  json += "\"power\":" + String(c.power ? 1 : 0) + ",";
  json += "\"colorKelvin\":" + String(c.colorKelvin) + ",";
  json += "\"partyMode\":" + String(c.partyMode ? 1 : 0) + ",";
  json += "\"partyDuration\":" + String(c.partyDuration) + ",";
  json += "\"nightActive\":" + String(c.nightActive ? 1 : 0) + ",";
  json += "\"nightStartHour\":" + String(c.nightStartHour) + ",";
  json += "\"nightEndHour\":" + String(c.nightEndHour);
  json += "}";
  request->send(200, "application/json", json); });

  // Root Dashboard
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP32 WortUhr</title>
  <style>
    body { font-family: Arial; text-align:center; margin:30px; background:#111; color:#eee; }
    h1 { color:#0f0; }
    .card { background:#222; padding:20px; margin:20px auto; border-radius:15px; width:320px; box-shadow:0 0 10px #000; }
    input[type=range] { width:100%; }
    button { padding:10px 20px; margin:10px; border:none; border-radius:8px; font-size:16px; }
    .on { background:#0f0; color:#000; }
    .off { background:#f00; color:#fff; }
    .row { display:flex; justify-content:space-between; align-items:center; margin:8px 0; }
    .row label { flex:1; text-align:left; }
    .row input[type=number] { width:80px; padding:6px; border-radius:6px; border:1px solid #444; background:#111; color:#eee; }
    .toggle {
      display:inline-flex; align-items:center; gap:10px; cursor:pointer; user-select:none;
      background:#111; border:1px solid #444; padding:8px 12px; border-radius:999px;
    }
    .toggle input { display:none; }
    .toggle .dot {
      width:18px; height:18px; border-radius:50%; background:#666; transition: all .2s ease;
      box-shadow: 0 0 6px rgba(0,0,0,.6) inset;
    }
    .toggle.active { border-color:#0f0; }
    .toggle.active .dot { background:#0f0; }
    small.hint { color:#aaa; display:block; margin-top:6px; }
  </style>
  <script>
    
  </script>
</head>
<body>
  <h1>WortUhr Steuerung</h1>

  <div class="card">
    <h2>Power</h2>
    <button class="on"  onclick="sendRequest('/power?state=1')">An</button>
    <button class="off" onclick="sendRequest('/power?state=0')">Aus</button>
  </div>

  <div class="card">
    <h2>Farbtemperatur (K)</h2>
    <input id="colorRange" type="range" min="2000" max="6500" value="3000"
      oninput="sendRequest('/color?kelvin=' + this.value); document.getElementById('colorVal').innerText = this.value + 'K';">
    <p id="colorVal">3000K</p>
  </div>

  <div class="card">
    <h2>Helligkeit</h2>
    <label>Max</label>
    <input type="range" min="15" max="180" value="100"
      oninput="sendRequest('/brightness/max?val=' + this.value)">
    <label>Min</label>
    <input type="range" min="15" max="180" value="30"
      oninput="sendRequest('/brightness/min?val=' + this.value)">
  </div>

  <div class="card">
    <h2>Empfindlichkeit</h2>
    <input type="range" min="1" max="100" value="50"
      oninput="sendRequest('/sensitivity?val=' + this.value)">
  </div>

  <div class="card">
    <h2>Party Modus</h2>
    <button class="on" onclick="sendRequest('/partymode?state=1')">Start</button>
    <button class="off" onclick="sendRequest('/partymode?state=0')">Stop</button>
    <p>Dauer einstellen:</p>
    <input type="range" min="10" max="300" value="60"
      oninput="sendRequest('/partymode?duration=' + this.value)">
  </div>

  <!-- ===== Nachtmodus ===== -->
  <div class="card">
    <h2>Nachtmodus</h2>
    <label id="nightToggle" class="toggle">
  <input id="nightActive" type="checkbox" onchange="onNightToggle(this)">
  <span class="dot"></span>
  <span id="nightLabel">Nachtmodus nicht aktiv</span>
</label>


    <div class="row">
      <label for="nightStart">Startstunde</label>
      <input id="nightStart" type="number" min="0" max="23" step="1" value="0" onchange="onNightHourChange()">
    </div>
    <div class="row">
      <label for="nightEnd">Endstunde</label>
      <input id="nightEnd" type="number" min="0" max="23" step="1" value="7" onchange="onNightHourChange()">
    </div>
    <small class="hint">Gültige Werte: 0–23. Der Nachtzeitraum darf über Mitternacht laufen (z.&nbsp;B. 22–7).</small>
  </div>

  <script>
  // --- Hilfen ---
  async function sendRequest(url) {
    try { const r = await fetch(url); return await r.text(); }
    catch(e) { console.error(e); }
  }
  function clampHour(v){
    v = parseInt(v||0,10);
    if (Number.isNaN(v)) v = 0;
    if (v < 0) v = 0;
    if (v > 23) v = 23;
    return v;
  }

  // --- UI-Update für Toggle ---
  function setNightLabel(checked){
    const tg  = document.getElementById('nightToggle');
    const lbl = document.getElementById('nightLabel');
    tg.classList.toggle('active', checked);
    lbl.textContent = checked ? 'Nachtmodus aktiv' : 'Nachtmodus nicht aktiv';
  }

  // EINZIGE Toggle-Funktion (wird vom onchange-Attribut aufgerufen)
  function onNightToggle(el){
    const checked = el.checked;
    setNightLabel(checked);
    sendRequest('/night?active=' + (checked ? 1 : 0));
  }

  // --- Stundenänderung ---
  function onNightHourChange(){
    const st  = document.getElementById('nightStart');
    const en  = document.getElementById('nightEnd');
    st.value = clampHour(st.value);
    en.value = clampHour(en.value);
    sendRequest('/night?start=' + st.value + '&end=' + en.value);
  }

  // --- Initial per /config füllen ---
  async function loadConfig(){
    try {
      const r = await fetch('/config');
      const c = await r.json();

      const chk = document.getElementById('nightActive');
      const st  = document.getElementById('nightStart');
      const en  = document.getElementById('nightEnd');

      chk.checked = !!c.nightActive;
      st.value    = clampHour(c.nightStartHour);
      en.value    = clampHour(c.nightEndHour);

      // Toggle-Optik + Text zum aktuellen Status
      setNightLabel(chk.checked);

      // (Optional) Farb-Range initialisieren
      const colorRange = document.getElementById('colorRange');
      const colorVal   = document.getElementById('colorVal');
      if (colorRange && colorVal && typeof c.colorKelvin === 'number') {
        colorRange.value   = c.colorKelvin;
        colorVal.textContent = c.colorKelvin + 'K';
      }
    } catch(e) {
      console.error('Config load failed', e);
    }
  }

  window.addEventListener('DOMContentLoaded', loadConfig);
</script>



</body>
</html>
)rawliteral";

  request->send(200, "text/html", html); });
  
  server.begin();
  Serial.println("Webserver angefragt /");
}
