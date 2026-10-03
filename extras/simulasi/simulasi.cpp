// Simulasi TombolPintar di PC: tombol fisik dengan getaran kontak (bouncing)
// dibaca lewat digitalRead() tiruan, perbarui() dipanggil tiap 0,1 ms.
// Keluaran CSV ke stdout, dibaca oleh gambar.py.
#include <stdio.h>
#include "TombolPintar.h"

uint32_t waktuPalsu = 1000;
int pinPalsu = HIGH; // pull-up internal: HIGH = dilepas, LOW = ditekan

static uint32_t acak = 12345; // LCG, seed tetap agar hasil selalu sama
static uint32_t lcg() { return acak = acak * 1103515245u + 12345u; }

// Satu tekanan tombol (ms). Kontak bergetar 3-6 ms setiap kali ditekan dan
// dilepas: pin berganti-ganti dengan selang acak 0,1-0,8 ms sebelum stabil.
struct Tekanan { uint32_t mulai, selesai; };

static void getar(int *pin, uint32_t awal, int akhir) {
  uint32_t lama = (3 + (lcg() >> 16) % 4) * 10;  // 3-6 ms
  int nilai = akhir;
  for (uint32_t t = awal; t < awal + lama; t += 1 + (lcg() >> 16) % 8, nilai = !nilai) {
    for (uint32_t i = t; i < awal + lama; i++) pin[i] = nilai;
  }
  pin[awal] = akhir; // sentuhan pertama
}

static void skenario(const char *nama, uint32_t durasi, const Tekanan *tk, int n) {
  static int pin[30000]; // per 0,1 ms
  uint32_t langkah = durasi * 10;
  for (uint32_t i = 0; i < langkah; i++) pin[i] = HIGH;
  for (int k = 0; k < n; k++) {
    for (uint32_t i = tk[k].mulai * 10; i < tk[k].selesai * 10; i++) pin[i] = LOW;
    getar(pin, tk[k].mulai * 10, LOW);
    getar(pin, tk[k].selesai * 10, HIGH);
  }

  TombolPintar tombol(2); // PULLUP_INTERNAL, pengaturan default
  pinPalsu = HIGH;
  tombol.mulai();
  printf("# %s\n", nama);
  printf("t_ms,pin,stabil,event,klik\n");
  uint32_t awal = waktuPalsu;
  int sebelum = -1;
  for (uint32_t i = 0; i < langkah; i++) {
    waktuPalsu = awal + i / 10; // millis() hanya bertambah tiap 1 ms
    pinPalsu = pin[i];
    tombol.perbarui();
    int e = tombol.ditekan() | tombol.dilepas() << 1 | tombol.diklik() << 2 |
            tombol.diklikGanda() << 3 | tombol.ditekanLama() << 4 | tombol.berulang() << 5;
    int keadaan = pinPalsu << 1 | tombol.sedangDitekan();
    if (e || keadaan != sebelum || i == langkah - 1)  // cetak hanya saat ada perubahan
      printf("%.1f,%d,%d,%d,%d\n", i / 10.0, pinPalsu, tombol.sedangDitekan(), e, tombol.jumlahKlik());
    sebelum = keadaan;
  }
  waktuPalsu += 1000;
}

int main() {
  const Tekanan klik[] = {{100, 220}};
  const Tekanan ganda[] = {{100, 210}, {340, 450}};
  const Tekanan tahan[] = {{100, 2100}};
  skenario("klik", 750, klik, 1);
  skenario("klik_ganda", 900, ganda, 2);
  skenario("tahan", 2600, tahan, 1);
  return 0;
}
