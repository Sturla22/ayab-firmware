#include "gtest/gtest.h"

#include "solenoids.h"
#ifdef HARD_I2C
#include <Wire.h>
#endif
#include <SoftI2CMaster.h>

using ::testing::Return;

class SolenoidsTest : public ::testing::Test {
protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
#ifdef HARD_I2C
    wireMock = WireMockInstance();
#endif
    softI2CMock = softI2CMockInstance();
    s = new Solenoids();
  }

  void TearDown() override {
    releaseArduinoMock();
#ifdef HARD_I2C
    releaseWireMock();
#endif
    releaseSoftI2CMock();
  }

  ArduinoMock *arduinoMock;
#ifdef HARD_I2C
  WireMock *wireMock;
#endif
  SoftI2CMock *softI2CMock;
  Solenoids *s;
};

TEST_F(SolenoidsTest, test_construct) {
}

TEST_F(SolenoidsTest, test_setSolenoid) {
  s->setSolenoid(1, true);
  s->setSolenoid(1, false);
  s->setSolenoid(16, false);
}

TEST_F(SolenoidsTest, test_setSolenoids) {
  s->setSolenoids(0xFFFF);
}
