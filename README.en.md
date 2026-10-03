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

## Simulation results

![Timing diagram of a single click: the raw pin bounces on press, ditekan() fires on the first edge, diklik() 300 ms after release](extras/gambar/tombol_klik.svg)

`ditekan()` (pressed) fires on the very first contact edge and the bounce is ignored; `diklik()` (clicked) fires 300 ms after release, once no second click arrives.

![Timing diagram of a double click: one diklikGanda() 300 ms after the second release, no diklik()](extras/gambar/tombol_klik_ganda.svg)

Two quick clicks are reported once as `diklikGanda()` (double click), with no extra `diklik()`.

![Timing diagram of a 2-second hold: ditekanLama() after 1000 ms, berulang() every 200 ms, release is not a click](extras/gambar/tombol_tahan.svg)

Holding fires `ditekanLama()` (long press) once at 1 s and `berulang()` (repeat) every 200 ms; releasing afterwards is not a click.

These are **simulations** with a simulated bouncing button and default settings, not hardware measurements. To regenerate:
```sh
cd extras/simulasi
python gambar.py   # needs g++ and matplotlib
```

## Speed & memory

Measured with simavr (cycle-accurate ATmega328P simulator), Arduino Uno 16 MHz, same sketch for every library (click, double click, long press enabled where supported). Cycles per update call, including `digitalRead()` (~73 cycles).

| Library | RAM per object | Idle | Held | Click gap | Long-press repeat | Sketch flash |
|---|---|---|---|---|---|---|
| **TombolPintar 1.0.0** | **16 B** | 197 (12 µs) | 203 | 208 | 800 (50 µs) | 5,292 B |
| OneButton 2.6.2 | 83 B | 266 (17 µs) | 269 | 281 | 319 | 6,046 B |
| Button2 2.7.0 | 59 B | 175 (11 µs) | 232 | 170 | 167 | 5,606 B |
| EasyButton 2.0.3 | 113 B | 328 (21 µs) | 393 | 328 | 426 | 5,896 B |
| AceButton 1.10.1 | 17 B + shared config | 233 (15 µs) | 271 | 252 | 255 | 5,712 B |
| JC_Button 2.1.6 | 24 B | 175 (11 µs) | 201 | 175 | 201 | 5,174 B |
| Bounce2 2.71 | 19 B | 174 (11 µs) | 174 | 174 | 174 | 5,004 B |
| ezButton 1.0.6 | 26 B | 184 (12 µs) | 173 | 184 | 173 | 5,000 B |

`perbarui()` is O(1). JC_Button, Bounce2 and ezButton are ~20 cycles faster because they have no double click or repeat. While a button is held past the long-press time, one 32-bit division (~600 cycles) computes `berulang()`; avoiding it would cost RAM per button, so it is left as is. Benchmark sketch: `extras/benchmark/TombolPintarBenchmark`.

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
