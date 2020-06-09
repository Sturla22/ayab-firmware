#pragma once

#include <gmock/gmock.h>

class SolenoidsMock {
public:
  MOCK_METHOD2(setSolenoid, void(uint8_t, bool));
  MOCK_METHOD1(setSolenoids, void(uint16_t state));
};

SolenoidsMock *solenoidsMockInstance();
void releaseSolenoidsMock();
