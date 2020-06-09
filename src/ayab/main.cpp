/*!
 * \file main.cpp
 * \brief Main entry point of ayab-firmware.
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

#ifdef AYAB_HW_TEST
#include "hw_test.h"
#endif

#include "knitter.h"
#include "singleton.h"

using KnitterSingleton =
    Singleton<Knitter>; ///< A singleton wrapper for knitter.

#ifndef AYAB_TESTS
/*!
 * Serial encoding callback, called when packet is received from serial.
 *
 * Callback of SerialEncoding is implemented by Knitter.
 */
void SerialEncoding::callback(const uint8_t *buffer, size_t size) {
  KnitterSingleton::getInstance().onPacketReceived(buffer, size);
}

/*!
 * Encoders callback, called when encoder A changes.
 *
 * The interrupt service routine is implemented by Knitter.
 */
void Encoders::callback() {
  KnitterSingleton::getInstance().isr();
}
#endif

/*!
 * Setup - steps to take before going to the main loop.
 */
void setup() {
#ifdef AYAB_HW_TEST
  hw_test_setup();
#endif
}

/*!
 * Main Loop - repeated until the heat death of the universe,
 * or someone cuts power to us.
 */
void loop() {
#ifdef AYAB_HW_TEST
  hw_test_loop();
#else
  KnitterSingleton::getInstance().fsm();
#endif
}
