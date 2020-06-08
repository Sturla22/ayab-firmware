#include "gtest/gtest.h"

#include "solenoids.h"
#ifdef HARD_I2C
#include <Wire.h>
#endif

using ::testing::Return;

class SolenoidsTest : public ::testing::Test {
protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
#ifdef HARD_I2C
    wireMock = WireMockInstance();
#endif
    s = new Solenoids();
  }

  void TearDown() override {
    releaseArduinoMock();
#ifdef HARD_I2C
    releaseWireMock();
#endif
  }

  ArduinoMock *arduinoMock;
#ifdef HARD_I2C
  WireMock *wireMock;
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
