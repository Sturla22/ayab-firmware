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

  if (m_oldState != currentState) {
    update(currentState);
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

/*!
 * \brief Set the type of machine for filters.
 */
void Encoders::setMachine(Machine machine) {
  switch (machine) {
  case Machine::kh910:
    m_machineFilters = kh910Filters;
    break;
  case Machine::kh930:
    m_machineFilters = kh930Filters;
    break;
  default:
    break;
  }
}

/* Private Methods */

void Encoders::updateBeltshift() {
  bool enc_c = digitalRead(ENC_PIN_C) != LOW;
  if (m_hallActive == Direction::Left) {
    m_beltShift = (enc_c ? Beltshift::Regular : Beltshift::Shifted);
  } else if (m_hallActive == Direction::Right) {
    m_beltShift = enc_c ? Beltshift::Shifted : Beltshift::Regular;
  }
}

void Encoders::updatePosition(bool rising) {
  if (!rising && m_direction == Direction::Left && m_encoderPos > END_LEFT) {
    m_encoderPos--;
  } else if (rising && m_direction == Direction::Right &&
             m_encoderPos < END_RIGHT) {
    m_encoderPos++;
  }
}

uint16_t Encoders::updateHallActive(bool rising) {
  Direction dir;
  uint8_t pin;
  if (rising) {
    dir = Direction::Left;
    pin = EOL_PIN_L;
  } else {
    dir = Direction::Right;
    pin = EOL_PIN_R;
  }
  uint16_t hallValue = analogRead(pin);
  if (m_machineFilters.isOutside(dir, hallValue)) {
    m_hallActive = dir;
  }
  return hallValue;
}

void Encoders::updateDirection() {
  bool enc_b = digitalRead(ENC_PIN_B) != LOW;
  m_direction = ((enc_b) ? Direction::Right : Direction::Left);
}

void Encoders::update(bool rising) {
  if (rising) {
    // Direction only decided on rising edge of encoder A
    updateDirection();
  }
  updatePosition(rising);
  uint16_t hallValue = updateHallActive(rising);
  if (rising && m_hallActive == Direction::Left) {
    updateCarriage(rising, hallValue);
    // Belt shift signal only decided in front of hall sensor
    updateBeltshift();
    // Known position of the carriage -> overwrite position
    m_encoderPos = END_LEFT + END_OFFSET;
  } else if (!rising && m_hallActive == Direction::Right) {
    updateCarriage(rising, hallValue);
    // Belt shift signal only decided in front of hall sensor
    updateBeltshift();
    // Known position of the carriage -> overwrite position
    m_encoderPos = END_RIGHT - END_OFFSET;
  }
}

/*!
 *
 */
void Encoders::updateCarriage(bool rising, uint16_t hallValue) {
  bool below = m_machineFilters.isBelow(m_hallActive, hallValue);
  if (rising) {
    // TODO(chris): Verify these decisions!
    if (below) {
      if (m_carriage == Carriage::K /*&& m_encoderPos == ?? */) {
        m_carriage = Carriage::G;
      } else {
        m_carriage = Carriage::L;
      }
    } else {
      m_carriage = Carriage::K;
    }
  } else {
    if (below) {
      m_carriage = Carriage::K;
    }
  }
}
