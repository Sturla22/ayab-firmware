#include "serial_command_mock.h"
#include <SerialCommand.h>

static SerialCommandMock *gSerialCommandMock = NULL;
SerialCommandMock *serialCommandMockInstance() {
  if (!gSerialCommandMock) {
    gSerialCommandMock = new SerialCommandMock();
  }
  return gSerialCommandMock;
}

SerialCommand::SerialCommand() {
}

void releaseSerialCommandMock() {
  if (gSerialCommandMock) {
    delete gSerialCommandMock;
    gSerialCommandMock = NULL;
  }
}

void SerialCommand::readSerial() {
  assert(gSerialCommandMock != NULL);
  gSerialCommandMock->readSerial();
}

char *SerialCommand::next() {
  assert(gSerialCommandMock != NULL);
  return gSerialCommandMock->next();
}

void SerialCommand::addCommand(const char *command, void (*function)()) {
  gSerialCommandMock->commandList = (SerialCommandMockCallback *)realloc(
      gSerialCommandMock->commandList, (gSerialCommandMock->commandCount + 1) *
                                           sizeof(SerialCommandMockCallback));
  strncpy(
      gSerialCommandMock->commandList[gSerialCommandMock->commandCount].command,
      command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  gSerialCommandMock->commandList[gSerialCommandMock->commandCount].function =
      function;
  gSerialCommandMock->commandCount++;
}

void SerialCommand::setDefaultHandler(void (*function)(const char *)) {
  gSerialCommandMock->defaultHandler = function;
}

void SerialCommandMock::callCommand(const char *command) {
  if (command != NULL) {
    bool matched = false;
    for (int i = 0; i < commandCount; i++) {

      // Compare the found command against the list of known commands for a
      // match
      if (strncmp(command, commandList[i].command,
                  SERIALCOMMAND_MAXCOMMANDLENGTH) == 0) {

        // Execute the stored handler function for the command
        (*commandList[i].function)();
        matched = true;
        break;
      }
    }
    if (!matched && (defaultHandler != NULL)) {
      (*defaultHandler)(command);
    }
  }
}
