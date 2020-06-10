#pragma once

#include <gmock/gmock.h>

class SoftI2CMock {
public:
  MOCK_METHOD1(beginTransmission, uint8_t(uint8_t address));
  MOCK_METHOD0(endTransmission, uint8_t(void));
  MOCK_METHOD1(write, uint8_t(uint8_t));
};

SoftI2CMock *softI2CMockInstance();
void releaseSoftI2CMock();

class SoftI2CMaster {
public:
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin);
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin, uint8_t usePullups);

  uint8_t beginTransmission(uint8_t address);
  uint8_t endTransmission();
  uint8_t write(uint8_t);
};
