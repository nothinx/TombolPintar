# TombolPintar (English)

[Bahasa Indonesia](README.md)

An Arduino **button** library with debounce, click, double click, N clicks, long press, and auto-repeat. Non-blocking, **16 bytes of RAM per button**. The API and examples are in Indonesian. This page maps every function to English.

```cpp
#include <TombolPintar.h>

TombolPintar button(2);   // button between pin 2 and GND, no resistor needed

void setup() {
  Serial.begin(115200);
  button.mulai();         // begin(): pinMode() is called here, not in the constructor
}

void loop() {
  button.perbarui();      // update(): call every loop()

  if (button.diklik()) Serial.println("Click");
  if (button.diklikGanda()) Serial.println("Double click");
  if (button.ditekanLama()) Serial.println("Long press");
}
```

## Why

| Library | Double click & long press | RAM per button (Uno) |
|---|---|---|
| **TombolPintar** | ✅ | **16 B** |
| OneButton | ✅ | 83 B |
| Button2 | ✅ | 59 B |
| EasyButton | ✅ | 113 B |

- Debounce is on by default and reacts to the **first** contact edge, so there is no added latency.
- `ditekan()` (pressed) fires instantly, while `diklik()` (clicked) waits for the double-click window. Pick the one that fits.
- `pinMode()` runs in `mulai()`, not in the constructor, which avoids unread buttons on ESP32 / Nano ESP32 / STM32.
- Any input source works through `perbarui(bool)`: analog keypads (LCD Keypad Shield), ESP32 touch pins, IO expanders, and shift registers.
- A button held during `mulai()` is reported by `sedangDitekan()` without firing false events.
- Safe across the `millis()` overflow.

## Function reference

| Indonesian | English | Notes |
|---|---|---|
| `TombolPintar(pin, mode)` | constructor | mode: `PULLUP_INTERNAL` (default), `PULLDOWN_INTERNAL`, `AKTIF_LOW` (active low), `AKTIF_HIGH` (active high, TTP223) |
| `TombolPintar()` | constructor without pin | use with `perbarui(bool)` |
| `mulai()` | begin | `false` if the pull-down is unsupported (AVR) |
| `perbarui()` / `perbarui(bool ditekan)` | update / update(isPressed) | `true` if there is a new event |
| `ditekan()` | pressed | instant |
| `dilepas()` | released | |
| `diklik()` | clicked | single click, after the click gap |
| `diklikGanda()` | double clicked | |
| `jumlahKlik()` | click count | 1, 2, 3, …; 0 = none |
| `ditekanLama()` | long pressed | once per press |
| `berulang()` | repeat | at long press, then every interval while held |
| `sedangDitekan()` | is pressed | state |
| `durasiTekan()` | press duration | ms |
| `aturDebounce(ms)` | set debounce | default 20 |
| `aturJedaKlik(ms)` | set click gap | default 300; 0 = no double click, instant click |
| `aturTekanLama(ms)` | set long press | default 1000 |
| `aturUlang(ms)` | set repeat interval | default 200; 0 = no repeat |

Events are `true` for exactly one `perbarui()` call.

## Examples

`DasarKlik` (basics), `TekanInstan` (instant press vs click), `KlikBanyak` (N clicks), `TahanBerulang` (hold to repeat), `BanyakTombol` (button array), `SentuhTTP223` (touch module), `KeypadShieldLCD` (5 buttons on A0), `SentuhESP32` (ESP32 touch pin).

## Status

Version 1.0.0 passes automated logic tests and compiles on Uno, Mega, ESP32, ESP32-C3, ESP32-S3, STM32 Blackpill F411, and Bluepill F103. It has **not yet been tested on real hardware**.

## License

MIT © 2026 Amadeo Wisesa.
