#include "gtest/gtest.h"

#include "solenoids.h"
#ifdef HARD_I2C
#include <Wire.h>
#else
#include <SoftI2CMaster.h>
#endif

using ::testing::Return;

class SolenoidsTest : public ::testing::Test {
protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
#ifdef HARD_I2C
    wireMock = WireMockInstance();
#else
    softI2CMock = softI2CMockInstance();
#endif
    s = new Solenoids();
  }

  void TearDown() override {
    releaseArduinoMock();
#ifdef HARD_I2C
    releaseWireMock();
#else
    releaseSoftI2CMock();
#endif
  }

  ArduinoMock *arduinoMock;
#ifdef HARD_I2C
  WireMock *wireMock;
#else
  SoftI2CMock *softI2CMock;
#endif
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
