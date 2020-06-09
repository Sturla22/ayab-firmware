#include "serial_encoding_mock.h"
#include <serial_encoding.h>

static SerialEncodingMock *gSerialEncodingMock = NULL;
SerialEncodingMock *serialEncodingMockInstance() {
  if (!gSerialEncodingMock) {
    gSerialEncodingMock = new SerialEncodingMock();
  }
  return gSerialEncodingMock;
}

void releaseSerialEncodingMock() {
  if (gSerialEncodingMock) {
    delete gSerialEncodingMock;
    gSerialEncodingMock = NULL;
  }
}

SerialEncoding::SerialEncoding() {
}

void SerialEncoding::update() {
  assert(gSerialEncodingMock != nullptr);
  gSerialEncodingMock->update();
}

void SerialEncoding::send(const uint8_t *payload, size_t length) {
  assert(gSerialEncodingMock != nullptr);
  gSerialEncodingMock->send(payload, length);
}

void SerialEncoding::onPacketReceived(Knitter *knitter, const uint8_t *buffer,
                                      size_t size) {
  assert(gSerialEncodingMock != nullptr);
  gSerialEncodingMock->onPacketReceived(knitter, buffer, size);
}

void SerialEncoding::requestLine(const uint8_t lineNumber) {
  assert(gSerialEncodingMock != nullptr);
  gSerialEncodingMock->requestLine(lineNumber);
}

void SerialEncoding::indicateState(uint8_t initState, uint16_t leftHallValue,
                                   uint16_t rightHallValue, uint8_t carriage,
                                   uint8_t position, uint8_t direction) {
  assert(gSerialEncodingMock != nullptr);
  gSerialEncodingMock->indicateState(initState, leftHallValue, rightHallValue,
                                     carriage, position, direction);
}
