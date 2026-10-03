// Contoh dasar: klik, klik ganda, dan tekan lama di Serial Monitor (115200).
//
// Sambungan: satu kaki tombol ke pin 2, kaki lainnya ke GND.
// Tidak perlu resistor, pull-up internal dinyalakan oleh mulai().
#include <TombolPintar.h>

TombolPintar tombol(2);

void setup() {
  Serial.begin(115200);
  tombol.mulai();
  Serial.println("Coba klik, klik dua kali, atau tahan tombol.");
}

void loop() {
  tombol.perbarui();

  if (tombol.diklik()) Serial.println("Klik");
  if (tombol.diklikGanda()) Serial.println("Klik ganda");
  if (tombol.ditekanLama()) Serial.println("Tekan lama");
}
