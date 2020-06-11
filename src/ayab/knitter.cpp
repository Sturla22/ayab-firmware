/*!
 * \file knitter.cpp
 *
 * This file is part of AYAB.
 *
 *    AYAB is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    AYAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with AYAB.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    Original Work Copyright 2013-2015 Christian Obersteiner, Andreas Müller
 *    Modified Work Copyright 2020 Sturla Lange
 *    http://ayab-knitting.com
 */

#include <Arduino.h>

#include "knitter.h"

#include "board.h"

#ifdef CLANG_TIDY
// clang-tidy doesn't find these macros for some reason,
// no problem when building or testing though.
constexpr uint8_t UINT8_MAX = 0xFFU;
constexpr uint16_t UINT16_MAX = 0xFFFFU;
#endif

constexpr uint8_t startOffsetLUT[static_cast<uint16_t>(Direction::NUM)]
                                [static_cast<uint16_t>(Carriage::NUM)] = {
                                    // NC,  K,  L,  G
                                    {0, 0, 0, 0},    // NoDirection
                                    {0, 40, 40, 8},  // Left
                                    {0, 16, 16, 32}, // Right
};

/*!
 * \brief Knitter constructor.
 *
 * Initializes the solenoids as well as pins and interrupts.
 */
Knitter::Knitter()
    : m_beeper(), m_encoders(), m_serial_encoding(), m_solenoids() {
  pinMode(LED_PIN_A, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  digitalWrite(LED_PIN_A, 1);
  digitalWrite(LED_PIN_B, 1);

#if DBG_NOMACHINE
  pinMode(DBG_BTN_PIN, INPUT);
#endif
}

auto Knitter::getState() const -> OpState {
  return m_opState;
}

void Knitter::send(const uint8_t *payload, size_t length) {
  m_serial_encoding.send(payload, length);
}

void Knitter::isr() {
  // Update machine state data
  m_encoders.encA_interrupt();
  m_position = m_encoders.getPosition();
  m_direction = m_encoders.getDirection();
  m_hallActive = m_encoders.getHallActive();
  m_beltshift = m_encoders.getBeltshift();
  m_carriage = m_encoders.getCarriage();
}

void Knitter::fsm() {
  switch (m_opState) {
  case OpState::s_init:
    if (init_guard()) {
      state_init();
    }
    break;

  case OpState::s_ready:
    state_ready();
    break;

  case OpState::s_operate:
    digitalWrite(LED_PIN_A, 1);

    if (m_firstRun) {
      m_firstRun = false;
      m_currentLineNumber = first_operate(m_currentLineNumber + 1);
    }

#if DBG_NOMACHINE
    debug_operate();
#else
    if (operate_guard()) {
      state_operate();
    }
#endif
    break;

  case OpState::s_test:
    state_test();
    break;

  default:
    break;
  }
  m_serial_encoding.update();
}

/*!
 * \brief Enter operate state.
 *
 * \todo sl: Check that functionality is correct after removing always true
 * comparison.
 */
auto Knitter::startOperation(Machine machine, uint8_t startNeedle,
                             uint8_t stopNeedle,
                             bool continuousReportingEnabled, uint8_t *line)
    -> bool {
  bool success = false;

  m_encoders.setMachine(machine);

  if (line == nullptr) {
    return success;
  }

  // TODO(sl): Check ok after removed always true comparison.
  if (stopNeedle < NUM_NEEDLES && startNeedle < stopNeedle) {
    if (OpState::s_ready == m_opState) {
      // Proceed to next state
      m_opState = OpState::s_operate;
      // Assign image width
      m_startNeedle = startNeedle;
      m_stopNeedle = stopNeedle;
      // Continuous Reporting enabled?
      m_continuousReportingEnabled = continuousReportingEnabled;
      // Set pixel data source
      m_lineBuffer = line;

      // Reset variables to start conditions
      m_currentLineNumber = UINT8_MAX; // because counter will
                                       // be increased before request
      m_lineRequested = false;
      m_lastLineFlag = false;

      Beeper::ready();

      success = true;
    }
  }
  return success;
}

auto Knitter::startTest() -> bool {
  bool success = false;
  if (OpState::s_init == m_opState || OpState::s_ready == m_opState) {
    m_opState = OpState::s_test;
    success = true;
  }
  return success;
}

auto Knitter::setNextLine(uint8_t lineNumber) -> bool {
  bool success = false;
  if (m_lineRequested) {
    // Is there even a need for a new line?
    if (lineNumber == m_currentLineNumber) {
      m_lineRequested = false;
      Beeper::finishedLine();
      success = true;
    } else {
      //  line numbers didnt match -> request again
      reqLine(m_currentLineNumber);
    }
  }
  return success;
}

void Knitter::setLastLine() {
  // lastLineFlag is evaluated in s_operate
  m_lastLineFlag = true;
}

/* Private Methods */

bool Knitter::init_guard() {
#ifdef DBG_NOMACHINE
  // TODO(Who?): Check if debounce is needed
  bool accept = (m_prevState && !digitalRead(DBG_BTN_PIN)) m_prevState = state;
  return accept;
#else
  // Machine is initialized when left hall sensor is passed in Right direction
  return (Direction::Right == m_direction && Direction::Left == m_hallActive);
#endif // DBG_NOMACHINE
}

void Knitter::state_init() {
  m_opState = OpState::s_ready;
  m_solenoids.setSolenoids(UINT16_MAX);
  indState(true);
}

void Knitter::state_ready() {
  digitalWrite(LED_PIN_A, 0);
  // This state is left when the startOperation() method
  // is called successfully
}

#ifdef DBG_NOMACHINE
void Knitter::debug_operate() {
  bool state = digitalRead(DBG_BTN_PIN);

  // TODO(Who?): Check if debounce is needed
  if (m_prevState != state) {
    if (!m_lineRequested) {
      reqLine(++m_currentLineNumber);
    }
  }
  m_prevState = state;
}
#endif

uint8_t Knitter::first_operate(uint8_t currentLineNumber) {
  delay(START_OPERATION_DELAY);
  Beeper::finishedLine();
  reqLine(currentLineNumber);
  return currentLineNumber;
}

static bool pixel_value(uint8_t *buffer, uint8_t pixelToSet) {
  // Find the right byte from the currentLine array,
  // then read the appropriate Pixel(/Bit) for the current needle to set
  uint8_t currentByte = pixelToSet / 8U;
  return bitRead(buffer[currentByte], pixelToSet - (8U * currentByte));
}

void Knitter::finishedWork() {
  Beeper::endWork();
  m_solenoids.setSolenoids(UINT16_MAX);
  Beeper::finishedLine();
}

bool Knitter::isNewPosition() {
  bool newPos = m_sOldPosition != m_position;
  m_sOldPosition = m_position;
  return newPos;
}

bool Knitter::operate_guard() {
  bool accept = false;
  // Only act if there is an actual change of position
  if (isNewPosition()) {
    if (m_continuousReportingEnabled) {
      // Send current position to GUI
      indState();
    }
    accept = calculatePixelAndSolenoid();
  }
  return accept;
}

void Knitter::state_operate() {
  // Inside of the active needles?
  bool inside = (m_pixelToSet >= m_startNeedle - END_OF_LINE_OFFSET_L) &&
                (m_pixelToSet <= m_stopNeedle + END_OF_LINE_OFFSET_R);

  if (inside) {
    if ((m_pixelToSet >= m_startNeedle) && (m_pixelToSet <= m_stopNeedle)) {
      m_workedOnLine = true;
    }
    // Write Pixel state to the appropriate needle
    bool pixelValue = pixel_value(m_lineBuffer, m_pixelToSet);
    m_solenoids.setSolenoid(m_solenoidToSet, pixelValue);
  } else {
    // Reset Solenoids when out of range
    m_solenoids.setSolenoid(m_solenoidToSet, true);

    if (!m_workedOnLine) {
      return;
    }

    // Finished the line
    m_workedOnLine = false;

    if (m_lastLineFlag) {
      m_opState = OpState::s_ready;
      finishedWork();
      // TODO(sl): Reset m_firstRun to true? Or does that belong in
      // startOperation?
    } else if (!m_lineRequested) {
      reqLine(++m_currentLineNumber);
    }
  }
}

void Knitter::state_test() {
  if (isNewPosition()) {
    calculatePixelAndSolenoid();
    indState();
  }
}

auto Knitter::calculatePixelAndSolenoid() -> bool {
  uint8_t startOffset = 0;
  bool success = true;

  switch (m_direction) {
  // Calculate the solenoid and pixel to be set
  // Implemented according to machine manual
  // Magic numbers result from machine manual
  // TODO(sl): 16 is number of solenoids? 8 is half? Replace with named
  // constant.
  case Direction::Right:
    startOffset = getStartOffset(Direction::Left);
    if (m_position >= startOffset) {
      m_pixelToSet = m_position - startOffset;

      if (Beltshift::Regular == m_beltshift) {
        m_solenoidToSet = m_position % 16;
      } else if (Beltshift::Shifted == m_beltshift) {
        m_solenoidToSet = (m_position - 8) % 16;
      }

      if (Carriage::L == m_carriage) {
        m_pixelToSet = m_pixelToSet + 8;
      }
    } else {
      success = false;
    }
    break;

  case Direction::Left:
    startOffset = getStartOffset(Direction::Right);
    if (m_position <= (END_RIGHT - startOffset)) {
      m_pixelToSet = m_position - startOffset;

      if (Beltshift::Regular == m_beltshift) {
        m_solenoidToSet = (m_position + 8) % 16;
      } else if (Beltshift::Shifted == m_beltshift) {
        m_solenoidToSet = m_position % 16;
      }

      if (Carriage::L == m_carriage) {
        m_pixelToSet = m_pixelToSet - 16;
      }
    } else {
      success = false;
    }
    break;

  default:
    success = false;
    break;
  }
  return success;
}

auto Knitter::getStartOffset(const Direction direction) -> uint8_t {
  if (direction >= Direction::NUM || m_carriage >= Carriage::NUM) {
    return 0U;
  }

  return startOffsetLUT[static_cast<uint16_t>(direction)]
                       [static_cast<uint16_t>(m_carriage)];
}

void Knitter::reqLine(const uint8_t lineNumber) {
  m_serial_encoding.requestLine(lineNumber);
  m_lineRequested = true;
}

void Knitter::indState(const bool initState) {
  m_serial_encoding.indicateState(
      static_cast<uint8_t>(initState), Encoders::getHallValue(Direction::Left),
      Encoders::getHallValue(Direction::Right),
      static_cast<uint8_t>(m_encoders.getCarriage()),
      static_cast<uint8_t>(m_encoders.getPosition()),
      static_cast<uint8_t>(m_encoders.getDirection()));
}

void Knitter::onPacketReceived(const uint8_t *buffer, size_t size) {
  m_serial_encoding.onPacketReceived(this, buffer, size);
}
