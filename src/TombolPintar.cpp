#include "TombolPintar.h"

bool TombolPintar::mulai() {
  uint8_t mode = _status & S_MODE;
  if (_pin != 0xFF) {
    if (mode == PULLUP_INTERNAL) {
      pinMode(_pin, INPUT_PULLUP);
    } else if (mode == PULLDOWN_INTERNAL) {
#ifdef INPUT_PULLDOWN
      pinMode(_pin, INPUT_PULLDOWN);
#else
      return false; // AVR tidak punya pull-down internal
#endif
    } else {
      pinMode(_pin, INPUT);
    }
  }
  // Tombol yang sudah ditahan saat mulai tidak memicu event apa pun sampai
  // dilepas, tapi sedangDitekan() langsung true ("tahan saat menyalakan").
  _status = mode;
  if (bacaPin()) _status |= S_DITEKAN | S_ABAIKAN;
  _event = 0;
  _klik = 0;
  _waktu = millis() - _debounce; // sentuhan pertama setelah mulai() langsung diterima
  return true;
}

bool TombolPintar::bacaPin() const {
  if (_pin == 0xFF) return false;
  uint8_t mode = _status & S_MODE;
  bool aktifHigh = mode == PULLDOWN_INTERNAL || mode == AKTIF_HIGH;
  return digitalRead(_pin) == (aktifHigh ? HIGH : LOW);
}

bool TombolPintar::perbarui(bool ditekan) {
  if (_event & E_KLIK) _klik = 0; // rangkaian klik sebelumnya sudah dilaporkan
  _event = 0;

  uint32_t sekarang = millis();
  uint32_t lewat = sekarang - _waktu;

  // Debounce di sisi depan: perubahan pertama langsung diterima, lalu getaran
  // kontak selama _debounce ms diabaikan.
  if (ditekan != sedangDitekan() && lewat >= _debounce) {
    _waktu = sekarang;
    lewat = 0;
    if (ditekan) {
      _status = (_status | S_DITEKAN) & ~(S_LAMA | S_ABAIKAN);
      _event |= E_TEKAN;
    } else {
      _status &= ~S_DITEKAN;
      _event |= E_LEPAS;
      // Lepas setelah tekan lama (atau setelah ditahan sejak mulai) bukan klik.
      if (!(_status & (S_LAMA | S_ABAIKAN)) && _klik < 255) _klik++;
    }
  }

  if (sedangDitekan()) {
    if (lewat >= _lama && !(_status & S_ABAIKAN)) {
      // Hitungan 8 bit boleh meluap; yang dibandingkan hanya perubahannya.
      uint8_t n = _ulang ? (lewat - _lama) / _ulang : 0;
      if (!(_status & S_LAMA)) {
        _status |= S_LAMA;
        _klik = 0; // klik sebelum tekan lama dibatalkan
        _hitungUlang = n;
        _event |= E_LAMA | E_ULANG;
      } else if (n != _hitungUlang) {
        _hitungUlang = n;
        _event |= E_ULANG;
      }
    }
  } else if (_klik && lewat >= _jeda) {
    _event |= E_KLIK;
  }
  return _event;
}

uint32_t TombolPintar::durasiTekan() const {
  return sedangDitekan() ? millis() - _waktu : 0;
}
