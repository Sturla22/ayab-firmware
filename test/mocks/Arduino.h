#pragma once

#include <arduino-mock/Arduino.h>
#include <arduino-mock/Serial.h>

#define digitalPinToBitMask(x) (x)
#define digitalPinToPort(x) (x)
#define portOutputRegister(x) (&x)
#define portInputRegister(x) (&x)
#define portModeRegister(x) (&x)

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue)                                         \
  ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

#define A0 0
#define A1 0
#define A2 0
#define A3 0
#define A4 0
#define A5 0
#define A6 0
#define A7 0
