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

constexpr uint8_t END_LEFT = 0U;
constexpr uint8_t END_RIGHT = 255U;
constexpr uint8_t END_OFFSET = 28;

// TODO(sl): Add kh270 when filter values have been determined.
enum class Machine : uint8_t { None, kh910, kh930, NUM };
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

struct Filter {
  uint16_t min;
  uint16_t max;

  bool isBelow(uint16_t val) const {
    return val < min;
  }
  bool isAbove(uint16_t val) const {
    return val > max;
  }
  bool isOutside(uint16_t val) const {
    return isBelow(val) || isAbove(val);
  }
  bool isInside(uint16_t val) const {
    return !isOutside(val);
  }
};

struct MachineFilters {
  Filter right;
  Filter left;

  const Filter getDirection(Direction dir) const {
    if (dir == Direction::Right) {
      return right;
    } else if (dir == Direction::Left) { // LCOV_EXCL_LINE
      return left;
    }
    return {0, 0};
  }

  bool isBelow(Direction dir, uint16_t val) const {
    return getDirection(dir).isBelow(val);
  }

  bool isOutside(Direction dir, uint16_t val) const {
    return getDirection(dir).isOutside(val);
  }
};

// Should be calibrated to each device
// These values are for the K carriage
constexpr MachineFilters kh910Filters = {{200, 1023}, {200, 600}};

constexpr MachineFilters kh930Filters = {{0, 600}, {200, 600}};

/*!
 * \brief Encoder interface.
 *
 * Encoders for Beltshift, Direction, Active Hall sensor and Carriage Type.
 */
class Encoders {
public:
#ifdef AYAB_TESTS
  FRIEND_TEST(EncodersTest, updateBeltshiftNoDir);
#endif
  Encoders();

  void encA_interrupt();
  static void callback();

  auto getPosition() const -> uint8_t;
  auto getBeltshift() const -> Beltshift;
  auto getDirection() const -> Direction;
  auto getHallActive() const -> Direction;
  auto getCarriage() const -> Carriage;

  void setMachine(Machine machine);

  static auto getHallValue(Direction pSensor) -> uint16_t;

private:
  MachineFilters m_machineFilters = {{0, 0}, {0, 0}};

  Direction m_direction = Direction::None;
  Direction m_hallActive = Direction::None;
  Beltshift m_beltShift = Beltshift::None;
  Carriage m_carriage = Carriage::None;
  uint8_t m_encoderPos = 0x00;
  bool m_oldState = false;

  void encA_rising();
  void encA_falling();

  void updateBeltshift();
  void updatePosition(bool rising);
  uint16_t updateHallActive(bool rising);
  void updateCarriage(bool rising, uint16_t hallValue);
  void updateDirection();

  void update(bool rising);
};
