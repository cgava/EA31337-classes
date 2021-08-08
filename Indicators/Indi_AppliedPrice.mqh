//+------------------------------------------------------------------+
//|                                                EA31337 framework |
//|                                 Copyright 2016-2021, EA31337 Ltd |
//|                                       https://github.com/EA31337 |
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

// Includes.
#include "../BufferStruct.mqh"
#include "../Indicator.mqh"

// Structs.
struct AppliedPriceParams : IndicatorParams {
  ENUM_APPLIED_PRICE applied_price;
  // Struct constructor.
  AppliedPriceParams(ENUM_APPLIED_PRICE _applied_price = PRICE_OPEN) {
    itype = INDI_APPLIED_PRICE;
    max_modes = 1;
    applied_price = _applied_price;
    SetDataValueType(TYPE_DOUBLE);
    SetDataValueRange(IDATA_RANGE_PRICE);
    SetDataSourceType(IDATA_INDICATOR);
  };
  AppliedPriceParams(AppliedPriceParams &_params) { this = _params; };
};

/**
 * Implements the "Applied Price over OHCL Indicator" indicator, e.g. over Indi_Price.
 */
class Indi_AppliedPrice : public Indicator {
 protected:
  AppliedPriceParams params;

 public:
  /**
   * Class constructor.
   */
  Indi_AppliedPrice(AppliedPriceParams &_params) : params(_params), Indicator((IndicatorParams)_params){};
  Indi_AppliedPrice() : Indicator(INDI_APPLIED_PRICE){};

  static double iAppliedPriceOnIndicator(Indicator *_indi, ENUM_APPLIED_PRICE _applied_price, int _shift = 0) {
    double _ohlc[4];
    _indi[_shift].GetArray(_ohlc, 4);
    return BarOHLC::GetAppliedPrice(_applied_price, _ohlc[0], _ohlc[1], _ohlc[2], _ohlc[3]);
  }

  /**
   * Returns the indicator's value.
   */
  double GetValue(int _shift = 0) {
    ResetLastError();
    double _value = EMPTY_VALUE;
    switch (params.idstype) {
      case IDATA_INDICATOR:
        if (HasDataSource()) {
          // Future validation of GetDataSource() will check if we set mode for source indicator (e.g. for applied price
          // of Indi_Price).
          iparams.SetDataSourceMode(GetAppliedPrice());
        }
        if (GetDataSource().GetParams().GetMaxModes() != 4) {
          Print(
              "Indi_AppliedPrice indicator may be used only with indicator that has at least 4 modes/buffers (O, H, L, "
              "C)!");
          DebugBreak();
        }
        _value = Indi_AppliedPrice::iAppliedPriceOnIndicator(GetDataSource(), GetAppliedPrice(), _shift);
        break;
      default:
        SetUserError(ERR_INVALID_PARAMETER);
    }
    istate.is_ready = _LastError == ERR_NO_ERROR;
    istate.is_changed = false;
    return _value;
  }

  /**
   * Returns the indicator's struct value.
   */
  IndicatorDataEntry GetEntry(int _shift = 0) {
    long _bar_time = GetBarTime(_shift);
    unsigned int _position;
    IndicatorDataEntry _entry(params.max_modes);
    if (idata.KeyExists(_bar_time, _position)) {
      _entry = idata.GetByPos(_position);
    } else {
      _entry.timestamp = GetBarTime(_shift);
      _entry.values[0] = GetValue(_shift);
      _entry.SetFlag(INDI_ENTRY_FLAG_IS_VALID, !_entry.HasValue<double>(NULL) && !_entry.HasValue<double>(EMPTY_VALUE));
      if (_entry.IsValid()) {
        _entry.AddFlags(_entry.GetDataTypeFlag(params.GetDataValueType()));
        idata.Add(_entry, _bar_time);
      }
    }
    return _entry;
  }

  /**
   * Returns the indicator's entry value.
   */
  MqlParam GetEntryValue(int _shift = 0, int _mode = 0) {
    MqlParam _param = {TYPE_DOUBLE};
    _param.double_value = GetEntry(_shift)[_mode];
    return _param;
  }

  /* Getters */

  /**
   * Get applied price.
   */
  ENUM_APPLIED_PRICE GetAppliedPrice() { return params.applied_price; }

  /* Setters */

  /**
   * Get applied price.
   */
  void SetAppliedPrice(ENUM_APPLIED_PRICE _applied_price) {
    istate.is_changed = true;
    params.applied_price = _applied_price;
  }
};