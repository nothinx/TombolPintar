// ditekan() vs diklik():
// - ditekan() langsung true saat tombol disentuh, tanpa jeda. Pakai untuk aksi
//   yang harus cepat: tombol game, bel, tombol darurat.
// - diklik() menunggu jeda klik (default 300 ms) untuk memastikan bukan
//   klik ganda. Pakai jika tombol punya beberapa fungsi.
//
// Sambungan: tombol antara pin 2 dan GND, LED bawaan board sebagai penanda.
#include <TombolPintar.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DevKit: LED biru di GPIO 2
#endif

TombolPintar tombol(2);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  tombol.mulai();
}

void loop() {
  tombol.perbarui();

  if (tombol.ditekan()) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("ditekan (langsung)");
  }
  if (tombol.dilepas()) digitalWrite(LED_BUILTIN, LOW);
  if (tombol.diklik()) Serial.println("diklik (setelah 300 ms)");
}
