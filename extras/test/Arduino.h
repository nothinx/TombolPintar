// Arduino.h tiruan untuk menguji logika TombolPintar di PC.
#pragma once
#include <stdint.h>
#define HIGH 1
#define LOW 0
#define INPUT 0
#define INPUT_PULLUP 2
extern uint32_t waktuPalsu;
extern int pinPalsu;
inline uint32_t millis() { return waktuPalsu; }
inline void pinMode(uint8_t, uint8_t) {}
inline int digitalRead(uint8_t) { return pinPalsu; }
