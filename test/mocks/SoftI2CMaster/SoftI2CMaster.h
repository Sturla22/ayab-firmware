#pragma once

#include <gmock/gmock.h>

class SoftI2CMock {
public:
  MOCK_METHOD1(beginTransmission, uint8_t(uint8_t address));
  MOCK_METHOD1(beginTransmission, uint8_t(int address));
  MOCK_METHOD0(endTransmission, uint8_t(void));
  MOCK_METHOD1(send, uint8_t(uint8_t));
  MOCK_METHOD2(send, void(uint8_t *, uint8_t));
  MOCK_METHOD1(send, void(int));
  MOCK_METHOD1(send, void(char *));
  MOCK_METHOD1(requestFrom, uint8_t(int address));
  MOCK_METHOD1(requestFrom, uint8_t(uint8_t address));
  MOCK_METHOD1(receive, uint8_t(uint8_t ack));
  MOCK_METHOD0(receive, uint8_t());
  MOCK_METHOD0(receiveLast, uint8_t());
};

SoftI2CMock *softI2CMockInstance();
void releaseSoftI2CMock();

class SoftI2CMaster {
public:
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin);
  SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin, uint8_t usePullups);

  uint8_t beginTransmission(uint8_t address);
  uint8_t beginTransmission(int address);
  uint8_t endTransmission();
  uint8_t send(uint8_t);
  void send(uint8_t *, uint8_t);
  void send(int);
  void send(char *);

  uint8_t requestFrom(int address);
  uint8_t requestFrom(uint8_t address);
  uint8_t receive(uint8_t ack);
  uint8_t receive();
  uint8_t receiveLast();
};
