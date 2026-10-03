// Klik N kali: satu tombol untuk banyak perintah.
// Contoh: 1 klik = nyala/mati, 2 klik = mode, 3 klik = info, 5 klik = reset.
//
// Sambungan: tombol antara pin 2 dan GND.
#include <TombolPintar.h>

TombolPintar tombol(2);

void setup() {
  Serial.begin(115200);
  tombol.mulai();
  tombol.aturJedaKlik(400); // beri waktu lebih untuk klik beruntun
}

void loop() {
  tombol.perbarui();

  uint8_t n = tombol.jumlahKlik();
  if (n == 0) return;

  Serial.print(n);
  Serial.print(" klik: ");
  switch (n) {
    case 1: Serial.println("nyala/mati"); break;
    case 2: Serial.println("ganti mode"); break;
    case 3: Serial.println("tampilkan info"); break;
    case 5: Serial.println("reset!"); break;
    default: Serial.println("tidak ada perintah"); break;
  }
}
