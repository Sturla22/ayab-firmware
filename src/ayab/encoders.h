/*!
 * \file encoders.h
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
 *    Original Work Copyright 2013 Christian Obersteiner, Andreas Müller
 *    Modified Work Copyright 2020 Sturla Lange
 *    http://ayab-knitting.com
 */
#pragma once

#include <Arduino.h>

// Should be calibrated to each device
// These values are for the K carriage
#if defined(KH910) || defined(AYAB_HW_TEST)
constexpr uint16_t FILTER_L_MIN = 200U; // below: L Carriage
constexpr uint16_t FILTER_L_MAX = 600U; // above: K Carriage
constexpr uint16_t FILTER_R_MIN = 200U;
constexpr uint16_t FILTER_R_MAX = 1023U;
#elif defined(KH930)
constexpr uint16_t FILTER_L_MIN = 200U; // below: L Carriage
constexpr uint16_t FILTER_L_MAX = 600U; // above: K Carriage
constexpr uint16_t FILTER_R_MIN = 0U;
constexpr uint16_t FILTER_R_MAX = 600U;
#elif defined(AYAB_TESTS)
// No error when running tests
#else
#error "KH910 or KH930 has to be defined as a preprocessor variable!"
#endif

constexpr uint8_t END_LEFT = 0U;
constexpr uint8_t END_RIGHT = 255U;
constexpr uint8_t END_OFFSET = 28;

enum class Direction : uint8_t { None, Left, Right, NUM };

enum class Carriage : uint8_t { None, K, L, G, NUM };
enum class Beltshift : uint8_t {
  None,
  Regular,
  Shifted,
  Lace_Regular,
  Lace_Shifted,
  NUM
};

/*!
 * \brief Encoder interface.
 *
 * Encoders for Beltshift, Direction, Active Hall sensor and Carriage Type.
 */
class Encoders {
public:
  Encoders();

  void encA_interrupt();
  static void callback();

  auto getPosition() const -> uint8_t;
  auto getBeltshift() -> Beltshift;
  auto getDirection() -> Direction;
  auto getHallActive() -> Direction;
  auto getCarriage() -> Carriage;

  static auto getHallValue(Direction pSensor) -> uint16_t;

private:
  Direction m_direction = Direction::None;
  Direction m_hallActive = Direction::None;
  Beltshift m_beltShift = Beltshift::None;
  Carriage m_carriage = Carriage::None;
  uint8_t m_encoderPos = 0x00;
  bool m_oldState = false;

  void encA_rising();
  void encA_falling();
};
