#pragma once

#include <SerialCommand.h>
#include <gmock/gmock.h>

// Data structure to hold Command/Handler function key-value pairs
typedef struct {
  char command[SERIALCOMMAND_MAXCOMMANDLENGTH + 1];
  void (*function)();
} SerialCommandMockCallback;

class SerialCommandMock {
public:
  MOCK_METHOD0(readSerial, void());
  MOCK_METHOD2(addCommand, void(const char *command, void (*function)()));
  MOCK_METHOD0(next, char *());
  MOCK_METHOD1(setDefaultHandler, void(void (*function)(const char *buffer)));

  void callCommand(const char *command);
  SerialCommandMockCallback *commandList =
      nullptr; // Actual definition for command/handler array
  uint8_t commandCount = 0;
  // Pointer to the default handler function
  void (*defaultHandler)(const char *);
};

SerialCommandMock *serialCommandMockInstance();
void releaseSerialCommandMock();
