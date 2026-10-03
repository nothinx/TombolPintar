// TombolPintar - tombol dengan debounce, klik, klik ganda, klik N kali,
// tekan lama, dan tekan lama berulang. Non-blocking, 16 byte RAM per tombol.
// Copyright (c) 2026 Amadeo Wisesa. Lisensi MIT.
//
// - Debounce aktif sejak awal dan bereaksi pada sentuhan pertama (tanpa jeda).
// - pinMode() dipanggil di mulai(), bukan di constructor.
// - Sumber apa saja: pin digital, TTP223, tombol analog, touch ESP32,
//   IO expander. Untuk selain pin digital, pakai perbarui(bool).
#pragma once
#include <Arduino.h>

class TombolPintar {
public:
  enum Mode : uint8_t {
    PULLUP_INTERNAL,   // tombol ke GND, tanpa resistor (paling umum)
    PULLDOWN_INTERNAL, // tombol ke VCC, tanpa resistor (ESP32 & STM32 saja)
    AKTIF_LOW,         // resistor pull-up eksternal, tombol ke GND
    AKTIF_HIGH         // resistor pull-down eksternal, atau modul TTP223
  };

  // Tanpa pin: sumber tombol dibaca sendiri lalu dikirim ke perbarui(bool).
  TombolPintar() {}
  TombolPintar(uint8_t pin, Mode mode = PULLUP_INTERNAL) : _pin(pin), _status(mode) {}

  // Panggil di setup(). false jika mode PULLDOWN_INTERNAL tidak didukung board.
  bool mulai();

  // Panggil di setiap loop(), sebelum memeriksa event.
  // Mengembalikan true jika ada event baru.
  bool perbarui() { return perbarui(bacaPin()); }
  // Untuk sumber selain pin digital: ditekan = true saat tombol sedang ditekan.
  // Contoh: tombol.perbarui(analogRead(A0) < 50);
  bool perbarui(bool ditekan);

  // --- Event: true hanya selama satu perbarui() ---
  bool ditekan() const { return _event & E_TEKAN; }   // langsung saat disentuh
  bool dilepas() const { return _event & E_LEPAS; }
  // Klik dianggap pasti setelah tidak ada klik lanjutan selama jeda klik
  // (default 300 ms). Butuh reaksi instan? Pakai ditekan().
  bool diklik() const { return jumlahKlik() == 1; }
  bool diklikGanda() const { return jumlahKlik() == 2; }
  uint8_t jumlahKlik() const { return (_event & E_KLIK) ? _klik : 0; } // 0 = belum ada
  bool ditekanLama() const { return _event & E_LAMA; } // sekali, saat mencapai batas
  // Seperti tombol keyboard yang ditahan: true saat mencapai batas tekan lama,
  // lalu berulang setiap interval selama tombol tetap ditekan.
  bool berulang() const { return _event & E_ULANG; }

  // --- Status ---
  bool sedangDitekan() const { return _status & S_DITEKAN; }
  uint32_t durasiTekan() const; // ms sejak ditekan, 0 jika tidak ditekan

  // --- Pengaturan (ms) ---
  void aturDebounce(uint8_t ms) { _debounce = ms; }  // default 20
  void aturJedaKlik(uint16_t ms) { _jeda = ms; }     // default 300, 0 = klik langsung tanpa klik ganda
  void aturTekanLama(uint16_t ms) { _lama = ms; }    // default 1000
  void aturUlang(uint16_t ms) { _ulang = ms; }       // default 200, 0 = tidak berulang

private:
  enum : uint8_t { S_MODE = 0x03, S_DITEKAN = 0x04, S_LAMA = 0x08, S_ABAIKAN = 0x10 };
  enum : uint8_t { E_TEKAN = 0x01, E_LEPAS = 0x02, E_KLIK = 0x04, E_LAMA = 0x08, E_ULANG = 0x10 };

  bool bacaPin() const;

  uint32_t _waktu = 0;        // millis() saat status stabil terakhir berubah
  uint16_t _jeda = 300, _lama = 1000, _ulang = 200;
  uint8_t _pin = 0xFF;        // 0xFF = tanpa pin
  uint8_t _status = PULLUP_INTERNAL;
  uint8_t _event = 0;
  uint8_t _klik = 0;
  uint8_t _hitungUlang = 0;
  uint8_t _debounce = 20;
};
