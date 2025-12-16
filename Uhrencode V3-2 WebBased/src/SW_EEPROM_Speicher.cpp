#include "SW_EEPROM_Speicher.h"
#include <EEPROM.h>

static constexpr uint32_t EXPECT_MAGIC = 0x57554C31;
static constexpr uint16_t EXPECT_VER   = 1;


void SW_EEPROM_Speicher::setDefaults() {
    Einstellungen def;    // nutzt die Default Member Initializer
    config = def;
}

void SW_EEPROM_Speicher::begin() {
    // 4-Byte ausrichten (ESP32 EEPROM-Emu schreibt in Blöcken)
    if (_size % 4 != 0) _size += (4 - (_size % 4));
    EEPROM.begin(_size);
    load();
}

void SW_EEPROM_Speicher::load() {
    // Erstmal sichere Defaults im RAM haben
    EEPROM.get(0, config);

    // Uninitialisiert oder falsche Version?
    const bool invalid = (config.magic != EXPECT_MAGIC) || (config.version != EXPECT_VER);
    if (invalid) {
        // Defaults persistent machen
        setDefaults();
        config.magic   = EXPECT_MAGIC;
        config.version = EXPECT_VER;
        save();
        return;
    }

}

void SW_EEPROM_Speicher::save() {
    // Magic/Version immer konsistent halten
    config.magic   = EXPECT_MAGIC;
    config.version = EXPECT_VER;

    EEPROM.put(0, config);
    EEPROM.commit();  // ohne das bleibt es im RAM-Puffer
}
