/*!
 * \file serial_encoding.cpp
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

#include <Arduino.h>

#include "knitter.h"
#include "serial_encoding.h"

/* Serial Command handling */

/*!
 * \brief Handle start request command.
 *
 * \todo sl: Assert size? Handle error?
 */
void SerialEncoding::h_reqStart(Knitter *knitter, const uint8_t *buffer,
                                size_t size) {
  if (size < 4U) {
    // Need 4 bytes from buffer below.
    return;
  }

  uint8_t startNeedle = buffer[1];
  uint8_t stopNeedle = buffer[2];
  bool continuousReportingEnabled = static_cast<bool>(buffer[3]);

  // TODO(who?): verify operation
  // memset(lineBuffer,0,sizeof(lineBuffer));
  // temporary solution:
  for (uint8_t i = 0U; i < LINEBUFFER_LEN; i++) {
    lineBuffer[i] = 0xFFU;
  }

  bool success = knitter->startOperation(
      startNeedle, stopNeedle, continuousReportingEnabled, lineBuffer);

  uint8_t payload[2];
  payload[0] = static_cast<uint8_t>(AYAB_API::cnfStart_msgid);
  payload[1] = static_cast<uint8_t>(success);
  send(payload, 2);
}

#ifdef AYAB_ENABLE_CRC
/*!
 * \brief Calculate CRC8 of a buffer
 *
 * Based on
 * https://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
 *
 * CRC-8 - based on the CRC8 formulas by Dallas/Maxim
 * code released under the therms of the GNU GPL 3.0 license
 */
static uint8_t CRC8(const uint8_t *buffer, size_t len) {
  uint8_t crc = 0x00U;

  while (len--) {
    uint8_t extract = *buffer;
    buffer++;

    for (uint8_t tempI = 8U; tempI; tempI--) {
      uint8_t sum = (crc ^ extract) & 0x01U;
      crc >>= 1U;

      if (sum) {
        crc ^= 0x8CU;
      }
      extract >>= 1U;
    }
  }
  return crc;
}
#endif

/*!
 * \brief Handle configure line command.
 *
 * \todo sl: Handle CRC-8 error?
 * \todo sl: Assert size? Handle error?
 */
void SerialEncoding::h_cnfLine(Knitter *knitter, const uint8_t *buffer,
                               size_t size) {
  if (size < 29U) {
    // Need 29 bytes from buffer below.
    return;
  }

  uint8_t lineNumber = buffer[1];

  for (uint8_t i = 0U; i < LINEBUFFER_LEN; i++) {
    // Values have to be inverted because of needle states
    lineBuffer[i] = ~buffer[i + 2];
  }
  uint8_t flags = buffer[27];

#ifdef AYAB_ENABLE_CRC
  uint8_t crc8 = buffer[28];
  // Check crc on bytes 0-28 of buffer.
  if (crc8 != CRC8(buffer, 28)) {
    return;
  }
#endif

  if (knitter->setNextLine(lineNumber)) {
    // Line was accepted
    bool flagLastLine = bitRead(flags, 0U);
    if (flagLastLine) {
      knitter->setLastLine();
    }
  }
}

void SerialEncoding::h_reqInfo() {
  uint8_t payload[4];
  payload[0] = static_cast<uint8_t>(AYAB_API::cnfInfo_msgid);
  payload[1] = API_VERSION;
  payload[2] = FW_VERSION_MAJ;
  payload[3] = FW_VERSION_MIN;
  send(payload, 4);
}

void SerialEncoding::h_reqTest(Knitter *knitter) {
  bool success = knitter->startTest();

  uint8_t payload[2];
  payload[0] = static_cast<uint8_t>(AYAB_API::cnfTest_msgid);
  payload[1] = static_cast<uint8_t>(success);
  send(payload, 2);
}

static void h_unrecognized() {
}

/*! Callback for PacketSerial
 *
 */
void SerialEncoding::onPacketReceived(Knitter *knitter, const uint8_t *buffer,
                                      size_t size) {
  switch (static_cast<AYAB_API>(buffer[0])) {
  case AYAB_API::reqStart_msgid:
    h_reqStart(knitter, buffer, size);
    break;

  case AYAB_API::cnfLine_msgid:
    h_cnfLine(knitter, buffer, size);
    break;

  case AYAB_API::reqInfo_msgid:
    h_reqInfo();
    break;

  case AYAB_API::reqTest_msgid:
    h_reqTest(knitter);
    break;

  default:
    h_unrecognized();
    break;
  }
}

SerialEncoding::SerialEncoding() {
  m_packetSerial.begin(SERIAL_BAUDRATE);
#ifndef AYAB_TESTS
  m_packetSerial.setPacketHandler(callback);
#endif
}

void SerialEncoding::update() {
  m_packetSerial.update();
}

void SerialEncoding::send(const uint8_t *payload, size_t length) {
  m_packetSerial.send(payload, length);
}

/*!
 * Send the request line state message.
 */
void SerialEncoding::requestLine(const uint8_t lineNumber) {
  constexpr uint8_t REQLINE_LEN = 2U;
  // Don't know why gcov doesn't hit these lines but they are not that
  // important. Maybe optimized out? The important line here is where we send
  // the payload. LCOV_EXCL_START
  const uint8_t payload[REQLINE_LEN] = {
      static_cast<uint8_t>(AYAB_API::reqLine_msgid),
      lineNumber,
  };
  // LCOV_EXCL_STOP
  send(static_cast<const uint8_t *>(payload), REQLINE_LEN);
}

// Don't know why gcov doesn't hit these lines but they are not that important.
// Maybe optimized out?
// The important line here is where we send the payload.
// LCOV_EXCL_START
/*!
 * Send the indicate state message.
 */
void SerialEncoding::indicateState(uint8_t initState, uint16_t leftHallValue,
                                   uint16_t rightHallValue, uint8_t carriage,
                                   uint8_t position, uint8_t direction) {
  constexpr uint8_t INDSTATE_LEN = 9U;
  const uint8_t payload[INDSTATE_LEN] = {
      static_cast<uint8_t>(AYAB_API::indState_msgid),
      initState,
      highByte(leftHallValue),
      lowByte(leftHallValue),
      highByte(rightHallValue),
      lowByte(rightHallValue),
      carriage,
      position,
      direction,
  };
  // LCOV_EXCL_STOP
  send(static_cast<const uint8_t *>(payload), INDSTATE_LEN);
}
