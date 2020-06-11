/*!
 * \file serial_encoding.h
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

#include <PacketSerial.h>

class Knitter;

constexpr uint8_t FW_VERSION_MAJ = 0U;
constexpr uint8_t FW_VERSION_MIN = 96U;
constexpr uint8_t FW_VERSION_PATCH = 0U;

constexpr uint8_t API_VERSION = 6U;

constexpr uint32_t SERIAL_BAUDRATE = 115200U;

constexpr uint8_t LINEBUFFER_LEN = 25U;

enum class AYAB_API : uint8_t {
  reqStart_msgid = 0x01,
  cnfStart_msgid = 0xC1,
  reqLine_msgid = 0x82,
  cnfLine_msgid = 0x42,
  reqInfo_msgid = 0x03,
  cnfInfo_msgid = 0xC3,
  reqTest_msgid = 0x04,
  cnfTest_msgid = 0xC4,
  indState_msgid = 0x84,
  debug_msgid = 0xFF
};

class SerialEncoding {
public:
  SerialEncoding();
  void update();
  void send(const uint8_t *payload, size_t length);
  void onPacketReceived(Knitter *knitter, const uint8_t *buffer, size_t size);
  void requestLine(const uint8_t lineNumber);
  void indicateState(uint8_t initState, uint16_t leftHallValue,
                     uint16_t rightHallValue, uint8_t carriage,
                     uint8_t position, uint8_t direction);
#ifndef AYAB_TESTS
  static void callback(const uint8_t *buffer, size_t size);
#endif

private:
  SLIPPacketSerial m_packetSerial;
  uint8_t lineBuffer[LINEBUFFER_LEN] = {0};

  void h_reqStart(Knitter *knitter, const uint8_t *buffer, size_t size);
  void h_cnfLine(Knitter *knitter, const uint8_t *buffer, size_t size);
  void h_reqInfo();
  void h_reqTest(Knitter *knitter);
};
