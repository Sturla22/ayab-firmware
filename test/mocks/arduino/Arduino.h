#pragma once

#include <arduino-mock/Arduino.h>
#include <arduino-mock/Serial.h>

#define digitalPinToBitMask(x) (x)
#define digitalPinToPort(x) (x)
#define portOutputRegister(x) (&x)
#define portInputRegister(x) (&x)
#define portModeRegister(x) (&x)

#define lowByte(w) ((uint8_t)((w)&0xffU))
#define highByte(w) ((uint8_t)((w) >> 8U))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01U)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue)                                         \
  ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

#define A0 0U
#define A1 1U
#define A2 2U
#define A3 3U
#define A4 4U
#define A5 5U
#define A6 6U
#define A7 7U
