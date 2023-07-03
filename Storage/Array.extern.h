//+------------------------------------------------------------------+
//|                                                EA31337 framework |
//|                                 Copyright 2016-2023, EA31337 Ltd |
//|                                        https://ea31337.github.io |
//+------------------------------------------------------------------+

/*
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Define external global functions.
#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once

#include "../Common.extern.h"
#include "../Std.h"
#include "String.extern.h"

template <typename T>
int ArraySize(const ARRAY_REF(T, _array)) {
  return _array.size();
}

template <typename T, int size>
constexpr int ArraySize(CONST_FIXED_ARRAY_REF(T, _array, size)) {
  return size;
}

template <typename T>
int ArrayResize(ARRAY_REF(T, _array), int _new_size, int _reserve_size = 0) {
  _array.resize(_new_size, _reserve_size);
  return _new_size;
}

template <typename T>
bool ArraySetAsSeries(ARRAY_REF(T, _array), bool _flag) {
  _array.setIsSeries(_flag);
  return true;
}

template <typename T>
bool ArrayGetAsSeries(CONST_ARRAY_REF(T, _array)) {
  return _array.getIsSeries();
}

template <typename T>
int ArrayMaximum(const ARRAY_REF(T, _array), int _start = 0, unsigned int _count = WHOLE_ARRAY) {
  Print("Not yet implemented: ", __FUNCTION__, " returns 0.");
  return 0;
}

template <typename T>
int ArrayMinimum(const ARRAY_REF(T, _array), int _start = 0, unsigned int _count = WHOLE_ARRAY) {
  Print("Not yet implemented: ", __FUNCTION__, " returns 0.");
  return 0;
}

template <typename T>
int ArrayFree(ARRAY_REF(T, _array)) {
  _array.resize(0, 0);
  return 0;
}

template <typename T>
bool ArrayReverse(ARRAY_REF(T, _array)) {
  _array.reverse();
  return true;
}

template <typename T>
int ArrayInitialize(ARRAY_REF(T, array), char value) {
  Print("Not yet implemented: ", __FUNCTION__, " returns 0.");
  return 0;
}

template <typename T, int size>
int ArrayInitialize(FIXED_ARRAY_REF(T, array, size), char value) {
  Print("Not yet implemented: ", __FUNCTION__, " returns 0.");
  return 0;
}

template <typename T>
int ArraySort(ARRAY_REF(T, array)) {
  Print("Not yet implemented: ", __FUNCTION__, " returns 0.");
  return 0;
}

template <typename X>
void ArrayFill(ARRAY_REF(X, array), int start, int count, X value) {
  Array::ArrayFill(array, start, count, value);
}

template <typename X, int size>
void ArrayFill(FIXED_ARRAY_REF(X, array, size), int start, int count, X value) {
  Array::ArrayFill(array, start, count, value);
}

#endif
