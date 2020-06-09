#include "SoftI2CMaster.h"
#define UNUSED(x) (void)(x)

static SoftI2CMock *gSoftI2CMock = NULL;
SoftI2CMock *softI2CMockInstance() {
  if (!gSoftI2CMock) {
    gSoftI2CMock = new SoftI2CMock();
  }
  return gSoftI2CMock;
}

SoftI2CMaster::SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin) {
  UNUSED(sdaPin);
  UNUSED(sclPin);
}

SoftI2CMaster::SoftI2CMaster(uint8_t sdaPin, uint8_t sclPin,
                             uint8_t usePullups) {
  UNUSED(sdaPin);
  UNUSED(sclPin);
  UNUSED(usePullups);
}

void releaseSoftI2CMock() {
  if (gSoftI2CMock) {
    delete gSoftI2CMock;
    gSoftI2CMock = NULL;
  }
}

uint8_t SoftI2CMaster::endTransmission() {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->endTransmission();
}

uint8_t SoftI2CMaster::beginTransmission(uint8_t address) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->beginTransmission(address);
}

uint8_t SoftI2CMaster::beginTransmission(int address) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->beginTransmission(address);
}

uint8_t SoftI2CMaster::send(uint8_t x) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->send(x);
}

void SoftI2CMaster::send(uint8_t *x, uint8_t y) {
  assert(gSoftI2CMock != NULL);
  gSoftI2CMock->send(x, y);
}

void SoftI2CMaster::send(int x) {
  assert(gSoftI2CMock != NULL);
  gSoftI2CMock->send(x);
}

void SoftI2CMaster::send(char *x) {
  assert(gSoftI2CMock != NULL);
  gSoftI2CMock->send(x);
}

uint8_t SoftI2CMaster::requestFrom(int address) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->requestFrom(address);
}

uint8_t SoftI2CMaster::requestFrom(uint8_t address) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->requestFrom(address);
}

uint8_t SoftI2CMaster::receive(uint8_t ack) {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->receive(ack);
}

uint8_t SoftI2CMaster::receive() {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->receive();
}

uint8_t SoftI2CMaster::receiveLast() {
  assert(gSoftI2CMock != NULL);
  return gSoftI2CMock->receiveLast();
}
