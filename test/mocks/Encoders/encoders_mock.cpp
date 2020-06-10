#include "encoders_mock.h"
#include "encoders.h"

static EncodersMock *gEncodersMock = NULL;
EncodersMock *encodersMockInstance() {
  if (!gEncodersMock) {
    gEncodersMock = new EncodersMock();
  }
  return gEncodersMock;
}

void releaseEncodersMock() {
  if (gEncodersMock) {
    delete gEncodersMock;
    gEncodersMock = NULL;
  }
}

Encoders::Encoders() {
}

void Encoders::encA_interrupt() {
  assert(gEncodersMock != NULL);
  gEncodersMock->encA_interrupt();
}
uint8_t Encoders::getPosition() const {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getPosition();
}
Beltshift Encoders::getBeltshift() const {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getBeltshift();
}
Direction Encoders::getDirection() const {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getDirection();
}
Direction Encoders::getHallActive() const {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getHallActive();
}
Carriage Encoders::getCarriage() const {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getCarriage();
}
uint16_t Encoders::getHallValue(Direction dir) {
  assert(gEncodersMock != NULL);
  return gEncodersMock->getHallValue(dir);
}
