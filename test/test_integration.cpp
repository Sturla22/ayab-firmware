#include <gtest/gtest.h>

#include <board.h>
#ifdef HARD_I2C
#include <Wire.h>
#endif

void setup();
void loop();

using ::testing::Return;

class IntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
    serialMock = serialMockInstance();
    ON_CALL(*serialMock, BoolOp).WillByDefault(Return(true));
#ifdef HARD_I2C
    wireMock = WireMockInstance();
#endif
  }

  void TearDown() override {
    releaseArduinoMock();
    releaseSerialMock();
    releaseWireMock();
  }

  ArduinoMock *arduinoMock;
  SerialMock *serialMock;
#ifdef HARD_I2C
  WireMock *wireMock;
#endif
};

TEST_F(IntegrationTest, test_setup) {
  setup();
}

TEST_F(IntegrationTest, test_loop) {
  loop();
}
