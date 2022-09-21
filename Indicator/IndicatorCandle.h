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

// Ignore processing of this file if already included.
#ifndef INDICATOR_CANDLE_H
#define INDICATOR_CANDLE_H

#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once
#endif

// Includes.
#include "../Candle.struct.h"
#include "../Storage/ItemsHistory.h"
#include "../Storage/ValueStorage.price_median.h"
#include "../Storage/ValueStorage.price_typical.h"
#include "../Storage/ValueStorage.price_weighted.h"
#include "../Storage/ValueStorage.spread.h"
#include "../Storage/ValueStorage.tick_volume.h"
#include "../Storage/ValueStorage.time.h"
#include "../Storage/ValueStorage.volume.h"
#include "Indicator.h"
#include "IndicatorCandle.provider.h"
#include "IndicatorData.h"
#include "TickBarCounter.h"

// Indicator modes.
enum ENUM_INDI_CANDLE_MODE {
  INDI_CANDLE_MODE_PRICE_OPEN,
  INDI_CANDLE_MODE_PRICE_HIGH,
  INDI_CANDLE_MODE_PRICE_LOW,
  INDI_CANDLE_MODE_PRICE_CLOSE,
  INDI_CANDLE_MODE_SPREAD,
  INDI_CANDLE_MODE_TICK_VOLUME,
  INDI_CANDLE_MODE_TIME,
  INDI_CANDLE_MODE_VOLUME,
  FINAL_INDI_CANDLE_MODE_ENTRY,
  // Following modes are dynamically calculated.
  INDI_CANDLE_MODE_PRICE_MEDIAN,
  INDI_CANDLE_MODE_PRICE_TYPICAL,
  INDI_CANDLE_MODE_PRICE_WEIGHTED,
};

/**
 * Class to deal with candle indicators.
 */
template <typename TS, typename TV, typename TCP>
class IndicatorCandle : public Indicator<TS> {
 protected:
  TickBarCounter counter;
  ItemsHistory<CandleOCTOHLC<TV>, TCP> history;

 protected:
  /* Protected methods */

  /**
   * Initialize class.
   *
   * Called on constructor.
   */
  void Init() {
    // Along with indexing by shift, we can also index via timestamp!
    flags |= INDI_FLAG_INDEXABLE_BY_TIMESTAMP;
    Set<int>(STRUCT_ENUM(IndicatorDataParams, IDATA_PARAM_MAX_MODES), FINAL_INDI_CANDLE_MODE_ENTRY);
  }

 public:
  /* Special methods */

  /**
   * Class constructor.
   */
  IndicatorCandle(const TS& _icparams, const IndicatorDataParams& _idparams, IndicatorBase* _indi_src = NULL,
                  int _indi_mode = 0)
      : Indicator(_icparams, _idparams, _indi_src, _indi_mode) {
    Init();
  }
  IndicatorCandle(ENUM_INDICATOR_TYPE _itype = INDI_CANDLE, int _shift = 0, string _name = "")
      : Indicator(_itype, _shift, _name) {
    Init();
  }

  /**
   * Returns possible data source types. It is a bit mask of ENUM_INDI_SUITABLE_DS_TYPE.
   */
  unsigned int GetSuitableDataSourceTypes() override {
    return INDI_SUITABLE_DS_TYPE_TICK | INDI_SUITABLE_DS_TYPE_BASE_ONLY;
  }

  /* Getters */

  /**
   * Returns buffer where candles are temporarily stored.
   */
  ItemsHistory<CandleOCTOHLC<TV>, TCP>* GetHistory() { return &history; }

  /**
   * Gets open price for a given, optional shift.
   */
  double GetOpen(int _shift = 0) override { return GetOHLC(_shift).open; }

  /**
   * Gets high price for a given, optional shift.
   */
  double GetHigh(int _shift = 0) override { return GetOHLC(_shift).high; }

  /**
   * Gets low price for a given, optional shift.
   */
  double GetLow(int _shift = 0) override { return GetOHLC(_shift).low; }

  /**
   * Gets close price for a given, optional shift.
   */
  double GetClose(int _shift = 0) override { return GetOHLC(_shift).close; }

  /**
   * Returns the current price value given applied price type, symbol and timeframe.
   */
  double GetPrice(ENUM_APPLIED_PRICE _ap, int _shift = 0) override { return GetOHLC(_shift).GetAppliedPrice(_ap); }

  /**
   * Returns current bar index (incremented every OnTick() if IsNewBar() is true).
   */
  int GetBarIndex() override { return counter.GetBarIndex(); }

  /**
   * Returns the number of bars on the chart.
   */
  int GetBars() override { return (int)history.PeakSize(); }

  /**
   * Returns current tick index (incremented every OnTick()).
   */
  int GetTickIndex() override { return counter.GetTickIndex(); }

  /**
   * Check if there is a new bar to parse.
   */
  bool IsNewBar() override { return counter.is_new_bar; }

  /* Virtual method implementations */

  /**
   * Returns time of the bar for a given shift.
   */
  virtual datetime GetBarTime(int _shift = 0) { return history.GetItemTimeByShift(_shift); }

  /**
   * Traverses source indicators' hierarchy and tries to find OHLC-featured
   * indicator. IndicatorCandle satisfies such requirements.
   */
  IndicatorData* GetCandle(bool _warn_if_not_found = true, IndicatorData* _originator = nullptr) override {
    // We are the candle indicator!
    return THIS_PTR;
  }

  /**
   * Gets OHLC price values.
   */
  BarOHLC GetOHLC(int _shift = 0) override {
    BarOHLC _bar;

    if (!history.EnsureShiftExists(_shift)) {
      // There's no candle fort that shift.
      return _bar;
    }

    CandleOCTOHLC<double> _candle = history.GetItemByShift(_shift);
    _bar = BarOHLC(_candle.open, _candle.high, _candle.low, _candle.close, _candle.open_timestamp_ms);
    return _bar;
  }

  /**
   * Returns volume value for the bar.
   *
   * If local history is empty (not loaded), function returns 0.
   */
  long GetVolume(int _shift = 0) override {
    CandleOCTOHLC<TV> candle = history.GetItemByShift(_shift);
    return candle.volume;
  }

  /**
   * Returns spread for the bar.
   *
   * If local history is empty (not loaded), function returns 0.
   */
  long GetSpread(int _shift = 0) override { return 0; }

  /**
   * Returns tick volume value for the bar.
   *
   * If local history is empty (not loaded), function returns 0.
   */
  long GetTickVolume(int _shift = 0) override { return GetVolume(); }

  /**
   * Returns the indicator's data entry.
   *
   * @see: IndicatorDataEntry.
   *
   * @return
   *   Returns IndicatorDataEntry struct filled with indicator values.
   */
  IndicatorDataEntry GetEntry(long _index = -1) override {
    ResetLastError();
    int _ishift = _index >= 0 ? (int)_index : iparams.GetShift();
    CandleOCTOHLC<TV> _candle = history.GetItemByShift(_ishift);
    return CandleToEntry(_candle.GetTime(), _candle);
  }

  /**
   * Returns value storage for a given mode.
   */
  IValueStorage* GetValueStorage(int _mode = 0) override {
    if (_mode >= ArraySize(value_storages)) {
      ArrayResize(value_storages, _mode + 1);
    }

    if (!value_storages[_mode].IsSet()) {
      // Buffer not yet created.
      switch (_mode) {
        case INDI_CANDLE_MODE_PRICE_OPEN:
        case INDI_CANDLE_MODE_PRICE_HIGH:
        case INDI_CANDLE_MODE_PRICE_LOW:
        case INDI_CANDLE_MODE_PRICE_CLOSE:
          value_storages[_mode] = new IndicatorBufferValueStorage<double>(THIS_PTR, _mode);
          break;
        case INDI_CANDLE_MODE_SPREAD:
        case INDI_CANDLE_MODE_TICK_VOLUME:
        case INDI_CANDLE_MODE_VOLUME:
          value_storages[_mode] = new IndicatorBufferValueStorage<long>(THIS_PTR, _mode);
          break;
        case INDI_CANDLE_MODE_TIME:
          value_storages[_mode] = new IndicatorBufferValueStorage<datetime>(THIS_PTR, _mode);
          break;
        case INDI_CANDLE_MODE_PRICE_MEDIAN:
          value_storages[_mode] = new PriceMedianValueStorage(THIS_PTR);
          break;
        case INDI_CANDLE_MODE_PRICE_TYPICAL:
          value_storages[_mode] = new PriceTypicalValueStorage(THIS_PTR);
          break;
        case INDI_CANDLE_MODE_PRICE_WEIGHTED:
          value_storages[_mode] = new PriceWeightedValueStorage(THIS_PTR);
          break;
        default:
          Print("ERROR: Unsupported value storage mode ", _mode);
          DebugBreak();
      }
    }

    return value_storages[_mode].Ptr();
  }

  /**
   * Converts candle into indicator's data entry.
   */
  IndicatorDataEntry CandleToEntry(long _timestamp, CandleOCTOHLC<TV>& _candle) {
    IndicatorDataEntry _entry(FINAL_INDI_CANDLE_MODE_ENTRY);
    _entry.timestamp = _timestamp;
    _entry.values[INDI_CANDLE_MODE_PRICE_OPEN] = _candle.open;
    _entry.values[INDI_CANDLE_MODE_PRICE_HIGH] = _candle.high;
    _entry.values[INDI_CANDLE_MODE_PRICE_LOW] = _candle.low;
    _entry.values[INDI_CANDLE_MODE_PRICE_CLOSE] = _candle.close;
    _entry.values[INDI_CANDLE_MODE_SPREAD] = 0.1;  // @todo
    _entry.values[INDI_CANDLE_MODE_TICK_VOLUME] = _candle.volume;
    _entry.values[INDI_CANDLE_MODE_TIME] = _timestamp;
    _entry.values[INDI_CANDLE_MODE_VOLUME] = _candle.volume;

    // @todo We may consider adding these three buffers directly.
    // BarOHLC _ohlc(_candle.open, _candle.high, _candle.low, _candle.close);
    // _entry.values[INDI_CANDLE_MODE_PRICE_MEDIAN] = _ohlc.GetMedian();
    // _entry.values[INDI_CANDLE_MODE_PRICE_TYPICAL] = _ohlc.GetTypical();
    // _entry.values[INDI_CANDLE_MODE_PRICE_WEIGHTED] = _ohlc.GetWeighted();

    _entry.SetFlag(INDI_ENTRY_FLAG_IS_VALID, _candle.IsValid());
    return _entry;
  }

  /**
   * Called when data source emits new entry (new one in ascending order).
   */
  void OnDataSourceEntry(IndicatorDataEntry& entry) override {
    // Parent indicator (e.g., Indi_TickMt) emitted an entry containing tick's
    // ask and bid price. As an abstract class, we really don't know how to
    // update/create candles so we just pass the entry into history's
    // ItemsHistoryCandleProvider and it will do all the job.
    history.GetItemProvider() PTR_DEREF OnTick(&history, entry.timestamp * 1000, (float)entry[0], (float)entry[1]);

    // @fixit Maybe we should generate some tick/bar change?
    // counter.OnTick(CalcCandleTimestamp(entry.timestamp));
  };

  /**
   * Returns value storage of given kind.
   */
  IValueStorage* GetSpecificValueStorage(ENUM_INDI_VS_TYPE _type) override {
    switch (_type) {
      case INDI_VS_TYPE_PRICE_OPEN:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_OPEN);
      case INDI_VS_TYPE_PRICE_HIGH:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_HIGH);
      case INDI_VS_TYPE_PRICE_LOW:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_LOW);
      case INDI_VS_TYPE_PRICE_CLOSE:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_CLOSE);
      case INDI_VS_TYPE_PRICE_MEDIAN:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_MEDIAN);
      case INDI_VS_TYPE_PRICE_TYPICAL:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_TYPICAL);
      case INDI_VS_TYPE_PRICE_WEIGHTED:
        return GetValueStorage(INDI_CANDLE_MODE_PRICE_WEIGHTED);
      case INDI_VS_TYPE_SPREAD:
        return GetValueStorage(INDI_CANDLE_MODE_SPREAD);
      case INDI_VS_TYPE_TICK_VOLUME:
        return GetValueStorage(INDI_CANDLE_MODE_TICK_VOLUME);
      case INDI_VS_TYPE_TIME:
        return GetValueStorage(INDI_CANDLE_MODE_TIME);
      case INDI_VS_TYPE_VOLUME:
        return GetValueStorage(INDI_CANDLE_MODE_VOLUME);
      default:
        // Trying in parent class.
        return Indicator<TS>::GetSpecificValueStorage(_type);
    }
  }

  /**
   * Checks whether indicator support given value storage type.
   */
  bool HasSpecificValueStorage(ENUM_INDI_VS_TYPE _type) override {
    switch (_type) {
      case INDI_VS_TYPE_PRICE_OPEN:
      case INDI_VS_TYPE_PRICE_HIGH:
      case INDI_VS_TYPE_PRICE_LOW:
      case INDI_VS_TYPE_PRICE_CLOSE:
      case INDI_VS_TYPE_PRICE_MEDIAN:
      case INDI_VS_TYPE_PRICE_TYPICAL:
      case INDI_VS_TYPE_PRICE_WEIGHTED:
      case INDI_VS_TYPE_SPREAD:
      case INDI_VS_TYPE_TICK_VOLUME:
      case INDI_VS_TYPE_TIME:
      case INDI_VS_TYPE_VOLUME:
        return true;
      default:
        // Trying in parent class.
        return Indicator<TS>::HasSpecificValueStorage(_type);
    }
  }

  string CandlesToString() {
    string _result = "CandlesToString() not yet implemented!";
    return _result;
  }

  /* Virtual methods */
};

#endif
