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

/**
 * @file
 * Includes Indi_Drawer's structs.
 */

#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once
#endif

// Includes.
#include "../Indicator/Indicator.struct.h"
#include "../Serializer/SerializerNode.enum.h"

// Structs.

/* Structure for indicator parameters. */
struct IndiDrawerParams : IndicatorParams {
  unsigned int period;
  ENUM_APPLIED_PRICE applied_price;

  IndiDrawerParams(unsigned int _period = 10, ENUM_APPLIED_PRICE _ap = PRICE_CLOSE)
      : period(_period), applied_price(_ap), IndicatorParams(INDI_DRAWER) {
    // Fetching history data is not yet implemented.
    SetCustomIndicatorName("Examples\\Drawer");
  };
  IndiDrawerParams(IndiDrawerParams &_params) { THIS_REF = _params; };
  // Serializers.
  SERIALIZER_EMPTY_STUB;
  SerializerNodeType Serialize(Serializer &s);
};

/* Method to serialize IndiDrawerParams structure. */
SerializerNodeType IndiDrawerParams::Serialize(Serializer &s) {
  s.Pass(THIS_REF, "period", period);
  s.PassEnum(THIS_REF, "applied_price", applied_price);
  s.Enter(SerializerEnterObject);
  IndicatorParams::Serialize(s);
  s.Leave();
  return SerializerNodeObject;
}

// Storing calculated average gain and loss for SMMA calculations.
struct DrawerGainLossData {
  double avg_gain;
  double avg_loss;
  DrawerGainLossData() { avg_gain = avg_loss = 0.0; }
};
