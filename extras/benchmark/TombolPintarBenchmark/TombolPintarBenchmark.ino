// Benchmark TombolPintar di ATmega328P 16 MHz (simavr). Cara menjalankan dan
// angka hasilnya: README bagian "Kecepatan & memori".
// Siklus.h: Timer1 tanpa prescaler, UKUR(nama, ulang, kode) mencetak
// "BENCH nama siklus_per_panggilan". millis() berhenti selama UKUR, jadi tiap
// skenario mengukur satu keadaan yang disiapkan sebelumnya.
#include <TombolPintar.h>
#include "Siklus.h"

const uint8_t PIN = 2;
TombolPintar tombol(PIN);  // perbarui(): termasuk digitalRead()
TombolPintar virt;         // perbarui(bool): biaya library murni
TombolPintar tepi;         // debounce 0: setiap panggilan adalah tepi baru
volatile bool masukan, hasil;

// Tekan = pin dipaksa LOW (OUTPUT), lepas = kembali ke pull-up.
void tekan(bool t) {
  masukan = t;
  if (t) { pinMode(PIN, OUTPUT); digitalWrite(PIN, LOW); }
  else pinMode(PIN, INPUT_PULLUP);
}

void jalankan(uint16_t ms) {
  uint32_t t = millis();
  while (millis() - t < ms) { tombol.perbarui(); virt.perbarui(masukan); }
}

void setup() {
  Serial.begin(115200);
  tombol.mulai();
  virt.mulai();
  tepi.mulai();
  tepi.aturDebounce(0);
  Serial.print(F("BENCH sizeof "));
  Serial.println(sizeof(TombolPintar));

  UKUR("digitalRead", 1000, hasil = digitalRead(PIN));

  jalankan(50);
  UKUR("perbarui_diam", 1000, hasil = tombol.perbarui());
  UKUR("perbarui_bool_diam", 1000, hasil = virt.perbarui(masukan));

  tekan(true);
  jalankan(100);
  UKUR("perbarui_ditahan", 1000, hasil = tombol.perbarui());
  UKUR("perbarui_bool_ditahan", 1000, hasil = virt.perbarui(masukan));

  tekan(false);
  jalankan(50);  // masih di dalam jeda klik 300 ms
  UKUR("perbarui_jeda_klik", 1000, hasil = tombol.perbarui());
  UKUR("perbarui_bool_jeda_klik", 1000, hasil = virt.perbarui(masukan));

  jalankan(400);
  tekan(true);
  jalankan(1100);  // lewat batas tekan lama: jalur berulang()
  UKUR("perbarui_berulang", 1000, hasil = tombol.perbarui());
  UKUR("perbarui_bool_berulang", 1000, hasil = virt.perbarui(masukan));

  UKUR("perbarui_bool_tepi", 1000, hasil = tepi.perbarui(_i & 1));
  selesai();
}

void loop() {}
