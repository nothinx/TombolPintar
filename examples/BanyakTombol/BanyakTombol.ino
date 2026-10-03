// Banyak tombol dalam array. Setiap tombol hanya memakai 16 byte RAM,
// jadi 10 tombol di Arduino Uno hanya memakai 160 byte (8% RAM).
//
// Sambungan: setiap tombol antara pinnya dan GND.
#include <TombolPintar.h>

TombolPintar tombol[] = {TombolPintar(2), TombolPintar(3), TombolPintar(4), TombolPintar(5)};
const uint8_t JUMLAH = sizeof(tombol) / sizeof(tombol[0]);

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < JUMLAH; i++) tombol[i].mulai();
}

void loop() {
  for (uint8_t i = 0; i < JUMLAH; i++) {
    tombol[i].perbarui();
    if (tombol[i].diklik()) {
      Serial.print("Tombol ");
      Serial.print(i + 1);
      Serial.println(" diklik");
    }
    if (tombol[i].ditekanLama()) {
      Serial.print("Tombol ");
      Serial.print(i + 1);
      Serial.println(" ditekan lama");
    }
  }
}
