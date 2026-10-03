// Modul sentuh TTP223: output HIGH saat disentuh, jadi pakai mode AKTIF_HIGH.
// Mode yang sama juga untuk tombol dengan resistor pull-down eksternal.
//
// Sambungan TTP223: VCC -> 3.3V/5V, GND -> GND, I/O -> pin 2.
#include <TombolPintar.h>

TombolPintar sentuh(2, TombolPintar::AKTIF_HIGH);

void setup() {
  Serial.begin(115200);
  sentuh.mulai();
}

void loop() {
  sentuh.perbarui();

  if (sentuh.diklik()) Serial.println("Sentuh");
  if (sentuh.diklikGanda()) Serial.println("Sentuh ganda");
  if (sentuh.ditekanLama()) Serial.println("Sentuh lama");
}
