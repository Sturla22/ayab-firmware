#include <gtest/gtest.h>

#include <Arduino.h>
#include <SoftI2CMaster.h>
#include <Wire.h>
#include <beeper_mock.h>
#include <board.h>
#include <hw_test.h>
#include <serial_command_mock.h>
#include <solenoids_mock.h>

using ::testing::Return;

class HwTestTest : public ::testing::Test {
protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
    serialCommandMock = serialCommandMockInstance();
    serialMock = serialMockInstance();
    wireMock = WireMockInstance();
    softI2CMock = softI2CMockInstance();
    beeperMock = beeperMockInstance();
    solenoidsMock = solenoidsMockInstance();
    hw_test_setup();
  }

  void TearDown() override {
    releaseArduinoMock();
    releaseSerialCommandMock();
    releaseSerialMock();
    releaseWireMock();
    releaseSoftI2CMock();
    releaseBeeperMock();
    releaseSolenoidsMock();
  }

  ArduinoMock *arduinoMock;
  SerialCommandMock *serialCommandMock;
  SerialMock *serialMock;
  WireMock *wireMock;
  SoftI2CMock *softI2CMock;
  BeeperMock *beeperMock;
  SolenoidsMock *solenoidsMock;
};

TEST_F(HwTestTest, test_help) {
  serialCommandMock->callCommand("help");
}

TEST_F(HwTestTest, test_stop) {
  serialCommandMock->callCommand("stop");
}

TEST_F(HwTestTest, test_beep) {
  serialCommandMock->callCommand("beep");
}

TEST_F(HwTestTest, test_unrecognized) {
  serialCommandMock->callCommand("unrecognized");
}

TEST_F(HwTestTest, test_setSingle) {
  serialCommandMock->callCommand("setSingle");

  EXPECT_CALL(*serialCommandMock, next)
      .WillOnce(Return((char *)"1"))
      .WillOnce(Return(nullptr));
  serialCommandMock->callCommand("setSingle");

  EXPECT_CALL(*serialCommandMock, next).WillRepeatedly(Return((char *)"1"));
  serialCommandMock->callCommand("setSingle");

  EXPECT_CALL(*serialCommandMock, next).WillRepeatedly(Return((char *)"16"));
  serialCommandMock->callCommand("setSingle");

  EXPECT_CALL(*serialCommandMock, next).WillRepeatedly(Return((char *)"2"));
  serialCommandMock->callCommand("setSingle");
}

TEST_F(HwTestTest, test_isr) {
  serialCommandMock->callCommand("isr");
}

TEST_F(HwTestTest, test_setAll) {
  serialCommandMock->callCommand("setAll");

  EXPECT_CALL(*serialCommandMock, next)
      .WillOnce(Return((char *)"1"))
      .WillOnce(Return(nullptr));
  serialCommandMock->callCommand("setAll");

  EXPECT_CALL(*serialCommandMock, next).WillRepeatedly(Return((char *)"1"));
  serialCommandMock->callCommand("setAll");
}

TEST_F(HwTestTest, test_readEOLsensorsCmd) {
  serialCommandMock->callCommand("readEOLsensorsCmd");
}

TEST_F(HwTestTest, test_readEncodersCmd) {
  serialCommandMock->callCommand("readEncodersCmd");

  EXPECT_CALL(*arduinoMock, digitalRead(ENC_PIN_A)).WillOnce(Return(1));
  EXPECT_CALL(*arduinoMock, digitalRead(ENC_PIN_B)).WillOnce(Return(1));
  EXPECT_CALL(*arduinoMock, digitalRead(ENC_PIN_C)).WillOnce(Return(1));
  serialCommandMock->callCommand("readEncodersCmd");
}

TEST_F(HwTestTest, test_beepCmd) {
  serialCommandMock->callCommand("beepCmd");
}

TEST_F(HwTestTest, test_autoReadCmd) {
  serialCommandMock->callCommand("autoReadCmd");
  hw_test_loop();
  serialCommandMock->callCommand("stop");
  hw_test_loop();
}

TEST_F(HwTestTest, test_autoTestCmd) {
  // Why does this cause a segfault?
  serialCommandMock->callCommand("autoTestCmd");
  hw_test_loop();
  serialCommandMock->callCommand("stop");
  hw_test_loop();
}
