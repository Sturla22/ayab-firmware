/*!
 * \file encoders.cpp
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

#include "board.h"
#include "encoders.h"
#include "knitter.h"

Encoders::Encoders() {
#ifndef AYAB_TESTS
  // Attaching ENC_PIN_A(=2), Interrupt No. 0
  attachInterrupt(0, callback, CHANGE);
#endif
  pinMode(ENC_PIN_A, INPUT);
  pinMode(ENC_PIN_B, INPUT);
  pinMode(ENC_PIN_C, INPUT);
}

/*!
 * \brief Encoder A interrupt service routine.
 *
 * Determines edge of signal and deferres to private rising/falling
 * functions.
 */
void Encoders::encA_interrupt() {
  m_hallActive = Direction::None;

  bool currentState = static_cast<bool>(digitalRead(ENC_PIN_A));

  if (!m_oldState && currentState) {
    encA_rising();
  } else if (m_oldState && !currentState) {
    encA_falling();
  }
  m_oldState = currentState;
}

/*!
 * \brief Getter for position member.
 */
auto Encoders::getPosition() const -> uint8_t {
  return m_encoderPos;
}

/*!
 * \brief Getter for beltshift member.
 */
auto Encoders::getBeltshift() const -> Beltshift {
  return m_beltShift;
}

/*!
 * \brief Getter for direction member.
 */
auto Encoders::getDirection() const -> Direction {
  return m_direction;
}

/*!
 * \brief Getter for hallActive member.
 */
auto Encoders::getHallActive() const -> Direction {
  return m_hallActive;
}

/*!
 * \brief Getter for carriage member.
 */
auto Encoders::getCarriage() const -> Carriage {
  return m_carriage;
}

/*!
 * \brief Read hall sensor on left and right.
 */
auto Encoders::getHallValue(Direction pSensor) -> uint16_t {
  switch (pSensor) {
  case Direction::Left:
    return analogRead(EOL_PIN_L);
  case Direction::Right:
    return analogRead(EOL_PIN_R);
  default:
    return 0;
  }
}

/* Private Methods */

/*!
 *
 */
void Encoders::encA_rising() {
  // Direction only decided on rising edge of encoder A
  m_direction =
      digitalRead(ENC_PIN_B) != 0 ? Direction::Right : Direction::Left;

  // Update carriage position
  if (Direction::Right == m_direction) {
    if (m_encoderPos < END_RIGHT) {
      m_encoderPos++;
    }
  }

  // In front of Left Hall Sensor?
  uint16_t hallValue = analogRead(EOL_PIN_L);
  if (hallValue < FILTER_L_MIN || hallValue > FILTER_L_MAX) {
    m_hallActive = Direction::Left;

    // TODO(chris): Verify these decisions!
    if (hallValue < FILTER_L_MIN) {
      if (m_carriage == Carriage::K /*&& m_encoderPos == ?? */) {
        m_carriage = Carriage::G;
      } else {
        m_carriage = Carriage::L;
      }
    } else {
      m_carriage = Carriage::K;
    }

    // Belt shift signal only decided in front of hall sensor
    m_beltShift =
        digitalRead(ENC_PIN_C) != 0 ? Beltshift::Regular : Beltshift::Shifted;

    // Known position of the carriage -> overwrite position
    m_encoderPos = END_LEFT + END_OFFSET;
  }
}

/*!
 *
 */
void Encoders::encA_falling() {
  // Update carriage position
  if (Direction::Left == m_direction) {
    if (m_encoderPos > END_LEFT) {
      m_encoderPos--;
    }
  }

  // In front of Right Hall Sensor?
  uint16_t hallValue = analogRead(EOL_PIN_R);

  // Avoid 'comparison of unsigned expression < 0 is always false'
  // by being explicit about that behaviour being expected.
  bool hallValueSmall = false;
#if FILTER_R_MIN != 0
  hallValueSmall = (hallValue < FILTER_R_MIN);
#endif

  if (hallValueSmall || hallValue > FILTER_R_MAX) {
    m_hallActive = Direction::Right;

    if (hallValueSmall) {
      m_carriage = Carriage::K;
    }

    // Belt shift signal only decided in front of hall sensor
    m_beltShift =
        digitalRead(ENC_PIN_C) != 0 ? Beltshift::Shifted : Beltshift::Regular;

    // Known position of the carriage -> overwrite position
    m_encoderPos = END_RIGHT - END_OFFSET;
  }
}
