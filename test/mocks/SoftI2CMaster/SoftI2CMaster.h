#pragma once

#include <Wire.h>
#include <gmock/gmock.h>

class SoftI2CMock {
public:
  MOCK_METHOD1(beginTransmission, uint8_t(uint8_t address));
  MOCK_METHOD0(endTransmission, uint8_t(void));
  MOCK_METHOD1(write, size_t(uint8_t));
  MOCK_METHOD2(write, size_t(const uint8_t *, uint8_t));
  MOCK_METHOD0(available, int());
  MOCK_METHOD0(read, int());

  virtual ~SoftI2CMock() = default;
};

SoftI2CMock *softI2CMockInstance();
void releaseSoftI2CMock();

class SoftI2CMaster : public TwoWire {
public:
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin);
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin, uint8_t usePullups);

  uint8_t beginTransmission(uint8_t address);
  uint8_t endTransmission();
  size_t write(uint8_t);
  size_t write(const uint8_t *, uint8_t);
  int available(void);
  int read(void);
};
