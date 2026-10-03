// Pin sentuh bawaan ESP32 (tanpa modul tambahan) lewat perbarui(bool).
// Tersedia di ESP32 dan ESP32-S3; ESP32-C3 tidak punya pin sentuh.
//
// Sambungan: kabel atau lempeng logam ke GPIO 4 (T0).
// Nilai touchRead() dicetak tiap 0,5 detik. ESP32 klasik: nilai TURUN saat
// disentuh (puluhan). ESP32-S3: nilai NAIK (puluhan ribu). Isi AMBANG dengan
// angka di tengah antara nilai saat disentuh dan tidak disentuh.
#include <TombolPintar.h>

TombolPintar sentuh;

#if defined(ESP32) && SOC_TOUCH_SENSOR_SUPPORTED
const uint8_t PIN_SENTUH = 4;
#if CONFIG_IDF_TARGET_ESP32
const uint32_t AMBANG = 40;
#else
const uint32_t AMBANG = 30000;
#endif

void setup() {
  Serial.begin(115200);
  sentuh.mulai();
}

void loop() {
  uint32_t nilai = touchRead(PIN_SENTUH);
#if CONFIG_IDF_TARGET_ESP32
  sentuh.perbarui(nilai < AMBANG);
#else
  sentuh.perbarui(nilai > AMBANG);
#endif

  static uint32_t terakhir = 0;
  if (millis() - terakhir >= 500) {
    terakhir = millis();
    Serial.print("touchRead: ");
    Serial.println(nilai);
  }

  if (sentuh.diklik()) Serial.println("Sentuh");
  if (sentuh.diklikGanda()) Serial.println("Sentuh ganda");
  if (sentuh.ditekanLama()) Serial.println("Sentuh lama");
}
#else
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Board ini tidak punya pin sentuh bawaan. Pakai ESP32 atau ESP32-S3.");
  delay(2000);
}
#endif
