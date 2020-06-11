#pragma once

#include <gmock/gmock.h>
enum class Machine : uint8_t;

class KnitterMock {
public:
  MOCK_METHOD5(startOperation,
               bool(Machine machine, uint8_t startNeedle, uint8_t stopNeedle,
                    bool continuousReportingEnabled, uint8_t *line));
  MOCK_METHOD0(startTest, bool(void));
  MOCK_METHOD1(setNextLine, bool(uint8_t lineNumber));
  MOCK_METHOD0(setLastLine, void());
  MOCK_METHOD2(send, void(const uint8_t *payload, size_t length));
  MOCK_METHOD2(onPacketReceived, void(const uint8_t *buffer, size_t size));
};

KnitterMock *knitterMockInstance();
void releaseKnitterMock();
