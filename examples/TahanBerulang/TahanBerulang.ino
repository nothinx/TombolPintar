// Atur angka dengan dua tombol: klik = naik/turun 1, tahan = naik/turun terus
// seperti tombol volume. Cocok untuk mengatur suhu, timer, kecepatan, dll.
//
// Sambungan: tombol NAIK antara pin 2 dan GND, tombol TURUN antara pin 3 dan GND.
#include <TombolPintar.h>

TombolPintar naik(2);
TombolPintar turun(3);
int nilai = 50;

void setup() {
  Serial.begin(115200);
  naik.mulai();
  turun.mulai();
  // Satu klik langsung diproses saat dilepas, tanpa menunggu klik ganda.
  naik.aturJedaKlik(0);
  turun.aturJedaKlik(0);
  // Mulai berulang setelah ditahan 500 ms, lalu tiap 100 ms.
  naik.aturTekanLama(500);
  turun.aturTekanLama(500);
  naik.aturUlang(100);
  turun.aturUlang(100);
}

void loop() {
  naik.perbarui();
  turun.perbarui();

  int lama = nilai;
  if (naik.diklik() || naik.berulang()) nilai++;
  if (turun.diklik() || turun.berulang()) nilai--;
  nilai = constrain(nilai, 0, 100);

  if (nilai != lama) {
    Serial.print("Nilai: ");
    Serial.println(nilai);
  }
}
