#include "solenoids_mock.h"
#include "solenoids.h"

static SolenoidsMock *gSolenoidsMock = NULL;
SolenoidsMock *solenoidsMockInstance() {
  if (!gSolenoidsMock) {
    gSolenoidsMock = new SolenoidsMock();
  }
  return gSolenoidsMock;
}

void releaseSolenoidsMock() {
  if (gSolenoidsMock) {
    delete gSolenoidsMock;
    gSolenoidsMock = NULL;
  }
}

Solenoids::Solenoids()
#if defined(HARD_I2C)
    : mcp_0(), mcp_1()
#elif defined(SOFT_I2C)
    : SoftI2C(A4, A5)
#endif
{
}

void Solenoids::setSolenoid(uint8_t solenoid, bool state) {
  assert(gSolenoidsMock != NULL);
  gSolenoidsMock->setSolenoid(solenoid, state);
}

void Solenoids::setSolenoids(uint16_t state) {
  assert(gSolenoidsMock != NULL);
  gSolenoidsMock->setSolenoids(state);
}
