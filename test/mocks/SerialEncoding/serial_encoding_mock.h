#pragma once

#include <gmock/gmock.h>

class Knitter;

class SerialEncodingMock {
public:
  MOCK_METHOD0(update, void());
  MOCK_METHOD2(send, void(const uint8_t *payload, size_t length));
  MOCK_METHOD3(onPacketReceived,
               void(Knitter *knitter, const uint8_t *buffer, size_t size));
  MOCK_METHOD1(requestLine, void(const uint8_t lineNumber));
  MOCK_METHOD6(indicateState, void(uint8_t initState, uint16_t leftHallValue,
                                   uint16_t rightHallValue, uint8_t carriage,
                                   uint8_t position, uint8_t direction));
};

SerialEncodingMock *serialEncodingMockInstance();
void releaseSerialEncodingMock();
