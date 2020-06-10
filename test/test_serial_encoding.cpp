#include <gtest/gtest.h>

#include <encoders.h>
#include <knitter_mock.h>
#include <serial_encoding.h>

using ::testing::_;
using ::testing::Return;

class SerialEncodingTest : public ::testing::Test {
protected:
  void SetUp() override {
    knitterMock = knitterMockInstance();
    serialMock = serialMockInstance();
    EXPECT_CALL(*serialMock, begin);
    ON_CALL(*serialMock, BoolOp).WillByDefault(Return(true));
    s = new SerialEncoding();
  }

  void TearDown() override {
    releaseKnitterMock();
    releaseSerialMock();
  }

  KnitterMock *knitterMock;
  Knitter *knitter = nullptr;
  SerialMock *serialMock;
  SerialEncoding *s;
};

TEST_F(SerialEncodingTest, test_testmsg) {
  uint8_t buffer[] = {static_cast<uint8_t>(AYAB_API::reqTest_msgid)};
  s->onPacketReceived(knitter, buffer, sizeof(buffer));
}

TEST_F(SerialEncodingTest, test_startmsg) {
  uint8_t buffer[] = {static_cast<uint8_t>(AYAB_API::reqStart_msgid), 0, 0, 0};
  s->onPacketReceived(knitter, buffer, sizeof(buffer));

  // Not enough bytes
  s->onPacketReceived(knitter, buffer, sizeof(buffer) - 1);
}

TEST_F(SerialEncodingTest, test_infomsg) {
  uint8_t buffer[] = {static_cast<uint8_t>(AYAB_API::reqInfo_msgid)};
  s->onPacketReceived(knitter, buffer, sizeof(buffer));
}

TEST_F(SerialEncodingTest, test_cnfmsg) {
  // CRC calculated with
  // http://tomeko.net/online_tools/crc8.php?lang=en
  constexpr uint8_t crc = 0xE9;
  constexpr uint8_t msg = static_cast<uint8_t>(AYAB_API::cnfLine_msgid); // 0x42

  uint8_t buffer[29] = {msg,  0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x01, crc};

  // Line not accepted
  EXPECT_CALL(*knitterMock, setNextLine);
  s->onPacketReceived(knitter, buffer, sizeof(buffer));

  // Line accepted, last line
  EXPECT_CALL(*knitterMock, setLastLine);
  EXPECT_CALL(*knitterMock, setNextLine).WillOnce(Return(true));
  s->onPacketReceived(knitter, buffer, sizeof(buffer));

  // Not last line
  buffer[27] = 0x00;
  buffer[28] = 0xB7;
  EXPECT_CALL(*knitterMock, setNextLine).WillOnce(Return(true));
  s->onPacketReceived(knitter, buffer, sizeof(buffer));

  // crc wrong
  EXPECT_CALL(*knitterMock, setNextLine).Times(0);
  buffer[28]--;
  s->onPacketReceived(knitter, buffer, sizeof(buffer));

  // Not enough bytes in buffer
  EXPECT_CALL(*knitterMock, setNextLine).Times(0);
  s->onPacketReceived(knitter, buffer, sizeof(buffer) - 1);
}

TEST_F(SerialEncodingTest, test_debug) {
  uint8_t buffer[] = {static_cast<uint8_t>(AYAB_API::debug_msgid)};
  s->onPacketReceived(knitter, buffer, sizeof(buffer));
}

TEST_F(SerialEncodingTest, test_constructor) {
}

TEST_F(SerialEncodingTest, test_update) {
  EXPECT_CALL(*serialMock, available);
  s->update();
}

TEST_F(SerialEncodingTest, test_send) {
  EXPECT_CALL(*serialMock, write(_, _));
  EXPECT_CALL(*serialMock, write(SLIP::END));

  uint8_t p[] = {1, 2, 3};
  s->send(p, 3);
}

TEST_F(SerialEncodingTest, test_requestline) {
  EXPECT_CALL(*serialMock, write(_, _));
  EXPECT_CALL(*serialMock, write(SLIP::END));
  s->requestLine(1);
}

TEST_F(SerialEncodingTest, test_indicateState) {
  EXPECT_CALL(*serialMock, write(_, _));
  EXPECT_CALL(*serialMock, write(SLIP::END));
  s->indicateState(0, 100, 400, static_cast<uint8_t>(Carriage::K), 1,
                   static_cast<uint8_t>(Direction::Right));
}
