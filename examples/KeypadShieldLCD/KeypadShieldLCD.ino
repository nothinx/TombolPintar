// Lima tombol LCD Keypad Shield yang semuanya terbaca di satu pin analog (A0).
// Pin A0 dibaca sekali, lalu hasilnya dibagikan ke lima tombol lewat
// perbarui(bool). Cara yang sama berlaku untuk sumber apa pun: IO expander,
// shift register, atau tangga resistor buatan sendiri.
//
// Nilai di bawah untuk shield DFRobot di board 5V (ADC 10 bit).
// Shield lain atau board 3.3V: cek nilai A0 tiap tombol di Serial Monitor,
// lalu sesuaikan batasnya.
#include <TombolPintar.h>

TombolPintar kanan, atas, bawah, kiri, pilih;

void setup() {
  Serial.begin(115200);
  kanan.mulai();
  atas.mulai();
  bawah.mulai();
  kiri.mulai();
  pilih.mulai();
}

void loop() {
  int a = analogRead(A0);
  kanan.perbarui(a < 60);
  atas.perbarui(a >= 60 && a < 200);
  bawah.perbarui(a >= 200 && a < 400);
  kiri.perbarui(a >= 400 && a < 600);
  pilih.perbarui(a >= 600 && a < 800);

  if (kanan.diklik()) Serial.println("Kanan");
  if (atas.diklik() || atas.berulang()) Serial.println("Atas");
  if (bawah.diklik() || bawah.berulang()) Serial.println("Bawah");
  if (kiri.diklik()) Serial.println("Kiri");
  if (pilih.diklik()) Serial.println("Pilih");
  if (pilih.ditekanLama()) Serial.println("Pilih (lama): kembali ke menu");
}
