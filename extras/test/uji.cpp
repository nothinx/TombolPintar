// Uji logika TombolPintar di PC:
//   g++ -std=c++11 -Wall -Wextra -I. -I../../src uji.cpp ../../src/TombolPintar.cpp -o uji && ./uji
#include <assert.h>
#include <stdio.h>
#include "TombolPintar.h"

uint32_t waktuPalsu = 1000;
int pinPalsu = HIGH; // pull-up: HIGH = dilepas

// Jalankan perbarui() tiap 1 ms selama ms, dengan tombol ditekan/dilepas.
// Mengembalikan event terakhir yang muncul lewat fungsi cek.
static int jalan(TombolPintar &t, bool ditekan, uint32_t ms, bool (TombolPintar::*cek)() const = nullptr) {
  int jumlah = 0;
  for (uint32_t i = 0; i < ms; i++) {
    t.perbarui(ditekan);
    if (cek && (t.*cek)()) jumlah++;
    waktuPalsu++;
  }
  return jumlah;
}

static void klik(TombolPintar &t) { jalan(t, true, 50); jalan(t, false, 50); }

int main() {
  assert(sizeof(TombolPintar) <= 16);

  { // ditekan() instan, getaran kontak tidak menghasilkan event ganda
    TombolPintar t;
    t.mulai();
    t.perbarui(true);
    assert(t.ditekan());
    int tekan = 0;
    for (int i = 0; i < 15; i++) { waktuPalsu++; t.perbarui(i % 2); tekan += t.ditekan(); }
    assert(tekan == 0);
  }
  { // satu klik dilaporkan setelah jeda, tepat sekali
    TombolPintar t;
    t.mulai();
    jalan(t, true, 50);
    assert(jalan(t, false, 299, &TombolPintar::diklik) == 0);
    assert(jalan(t, false, 100, &TombolPintar::diklik) == 1);
  }
  { // klik ganda dan klik tiga kali
    TombolPintar t;
    t.mulai();
    klik(t);
    klik(t);
    assert(jalan(t, false, 400, &TombolPintar::diklikGanda) == 1);
    klik(t); klik(t); klik(t);
    uint8_t n = 0;
    for (int i = 0; i < 400; i++) { t.perbarui(false); if (t.jumlahKlik()) n = t.jumlahKlik(); waktuPalsu++; }
    assert(n == 3);
  }
  { // jeda 0: klik langsung saat dilepas
    TombolPintar t;
    t.mulai();
    t.aturJedaKlik(0);
    jalan(t, true, 50);
    t.perbarui(false);
    assert(t.diklik() && t.dilepas());
  }
  { // tekan lama sekali, berulang tiap 200 ms, lepas bukan klik
    TombolPintar t;
    t.mulai();
    assert(jalan(t, true, 999, &TombolPintar::ditekanLama) == 0);
    assert(jalan(t, true, 2, &TombolPintar::ditekanLama) == 1);
    assert(jalan(t, true, 1000, &TombolPintar::berulang) == 5);
    assert(t.durasiTekan() >= 2000);
    assert(jalan(t, false, 1000, &TombolPintar::diklik) == 0);
  }
  { // ulang 0: berulang() hanya sekali saat tekan lama
    TombolPintar t;
    t.mulai();
    t.aturUlang(0);
    assert(jalan(t, true, 3000, &TombolPintar::berulang) == 1);
  }
  { // berulang tetap jalan setelah hitungan 8 bit meluap (> 255 kali)
    TombolPintar t;
    t.mulai();
    t.aturUlang(10);
    int u = jalan(t, true, 1000 + 300 * 10, &TombolPintar::berulang);
    assert(u == 300); // 1 tekan lama + 299 ulangan (lewat 1010..3990)
  }
  { // ditahan sejak mulai: tanpa event sampai dilepas lalu ditekan lagi
    TombolPintar t(5);
    pinPalsu = LOW;
    t.mulai();
    assert(t.sedangDitekan());
    int e = 0;
    for (int i = 0; i < 3000; i++) { t.perbarui(); e += t.ditekan() || t.ditekanLama() || t.berulang(); waktuPalsu++; }
    assert(e == 0);
    pinPalsu = HIGH;
    for (int i = 0; i < 1000; i++) { t.perbarui(); e += t.diklik(); waktuPalsu++; }
    assert(e == 0);
    pinPalsu = LOW;
    t.perbarui();
    assert(t.ditekan());
  }
  { // millis() meluap tidak merusak apa pun
    waktuPalsu = 0xFFFFFF00u;
    TombolPintar t;
    t.mulai();
    klik(t);
    assert(jalan(t, false, 400, &TombolPintar::diklik) == 1);
  }
  printf("Semua uji lolos (sizeof = %u byte)\n", (unsigned)sizeof(TombolPintar));
  return 0;
}
