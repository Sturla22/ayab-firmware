/*!
 * \file singleton.h
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
 *    Copyright 2020 Sturla Lange
 *    http://ayab-knitting.com
 */
#pragma once

/*!
 * A generic singleton wrapper.
 *
 * Can be used to refactor out global variables and
 * place them on the stack rather than the heap.
 * Wrapping rather than implementing the object itself
 * also makes testing easier.
 *
 * Note that the only objects that can be wrapped are objects with
 * a constructor that takes no arguments.
 *
 * \param T The object type to wrap.
 *
 */
template <class T> class Singleton {
  /*!
   * Private constructor that also constructs the local instance
   * of the wrapped object.
   */
  Singleton() : local() {
  }

public:
  /*!
   * Get a reference to the local instance of the wrapped object.
   */
  static T &getInstance() {
    // LCOV_EXCL_START
    static Singleton instance; ///< The singleton's own instance.
    // LCOV_EXCL_STOP
    return instance.local;
  }

  // Delete constructors that are not supported by a singleton.
  Singleton(Singleton const &) = delete;
  void operator=(Singleton const &) = delete;

  T local; ///< The local instance of the wrapped object.
};
