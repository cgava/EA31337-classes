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

/**
 * @file
 * Real tick-based indicator.
 */

#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once
#endif

// Includes.
#include "../../Chart.struct.static.h"
#include "../../Indicator/IndicatorTick.h"

#define INDICATOR_TICK_REAL_FETCH_HISTORY 1000

// Structs.
// Params for MT patform's tick-based indicator.
struct Indi_TickMtParams : IndicatorParams {
  Indi_TickMtParams() : IndicatorParams(INDI_TICK) {}
};

// MT platform's tick-based indicator.
class Indi_TickMt : public IndicatorTick<Indi_TickMtParams, double> {
 protected:
  bool _fetch_history_on_first_tick;

 public:
  Indi_TickMt(Indi_TickMtParams &_p, ENUM_IDATA_SOURCE_TYPE _idstype = IDATA_BUILTIN, IndicatorData *_indi_src = NULL,
              int _indi_src_mode = 0)
      : IndicatorTick(_p.symbol, _p,
                      IndicatorDataParams::GetInstance(2, TYPE_DOUBLE, _idstype, IDATA_RANGE_PRICE, _indi_src_mode),
                      _indi_src) {
    _fetch_history_on_first_tick = false;
  }
  Indi_TickMt(string _symbol, ENUM_IDATA_SOURCE_TYPE _idstype = IDATA_BUILTIN, IndicatorData *_indi_src = NULL,
              int _indi_src_mode = 0, string _name = "")
      : IndicatorTick(_symbol, Indi_TickMtParams(),
                      IndicatorDataParams(2, TYPE_DOUBLE, _idstype, IDATA_RANGE_PRICE, _indi_src_mode), _indi_src) {
    _fetch_history_on_first_tick = false;
  }

  string GetName() override { return "Indi_TickMt"; }

  /**
   * Returns possible data source types. It is a bit mask of ENUM_INDI_SUITABLE_DS_TYPE.
   */
  unsigned int GetSuitableDataSourceTypes() override { return INDI_SUITABLE_DS_TYPE_EXPECT_NONE; }

  /**
   * Returns possible data source modes. It is a bit mask of ENUM_IDATA_SOURCE_TYPE.
   */
  unsigned int GetPossibleDataModes() override { return IDATA_BUILTIN; }

  /**
   * Returns time of the bar for a given shift.
   */
  virtual datetime GetBarTime(int _shift = 0) {
    Print(
        "Error: Indi_TickMt's GetBarTime() requires TF to be passed. Please use GetBarTime(ENUM_TIMEFRAMES _tf, int "
        "_shift = 0) variant.");
    DebugBreak();
    return 0;
  }

  /**
   * Returns time of the bar for a given timeframe and shift.
   */
  virtual datetime GetBarTime(ENUM_TIMEFRAMES _tf, int _shift = 0) { return ::GetBarTime(_tf, _shift); }

  /**
   * Returns the indicator's struct entry for the given shift.
   *
   * @see: IndicatorDataEntry.
   *
   * @return
   *   Returns IndicatorDataEntry struct filled with indicator values.
   */
  IndicatorDataEntry GetEntry(long _index = -1) override {
    int _ishift = _index >= 0 ? (int)_index : iparams.GetShift();
    long _bar_time;
    _bar_time = GetBarTime(_ishift);

    TickAB<double> _tick = itdata.GetByKey(_bar_time);
    IndicatorDataEntry _entry = TickToEntry(_bar_time, _tick);

    if (_entry.IsValid()) {
      istate.is_changed = false;
      istate.is_ready = true;
    }

    return _entry;
  }

  void OnBecomeDataSourceFor(IndicatorData *_base_indi) override {
    // Feeding base indicator with historic entries of this indicator.
#ifdef __debug__
    Print(GetFullName(), " became a data source for ", _base_indi.GetFullName());
#endif

    _fetch_history_on_first_tick = true;
  }

  /**
   * Fetches historic ticks for a given range and emits these ticks. Used to regenerate candles.
   */
  void FetchHistory(long _range_from, long _range_to) override {
    // Number of retries for CopyTicksRange().
    int _tries = 10;

    static MqlTick _tmp_ticks[];
    ArrayResize(_tmp_ticks, 0);

    while (_tries > 0) {
      int _num_copied = (GetSymbol(), _tmp_ticks, COPY_TICKS_INFO, _range_from, _range_to);

      if (_num_copied == -1) {
        ResetLastError();
        Sleep(1000);
        --_tries;
      } else {
        for (int i = 0; i < _num_copied; ++i) {
          TickAB<double> _tick(_tmp_ticks[i].ask, _tmp_ticks[i].bid);
#ifdef __debug_verbose__
          Print("Emitting historic tick at ", TimeToString(_tmp_ticks[i].time, TIME_DATE | TIME_MINUTES | TIME_SECONDS),
                ": ", _tmp_ticks[i].ask, ", ", _tmp_ticks[i].bid);
#endif
          EmitEntry(TickToEntry(_tmp_ticks[i].time, _tick));
        }
        break;
      }
    }
  }

  /**
   * Fetches historic ticks for last two weeks and emits those ticks.
   */
  void FetchHistory() {
    if (INDICATOR_TICK_REAL_FETCH_HISTORY == 0) {
      // No history requested.
      return;
    }

#ifndef __MQL4__
    int _ticks_to_emit = 1000;

#ifdef __debug_verbose__
    Print("Listening indicators will be now filled with ", _ticks_to_emit,
          " historical entries generated by " + GetFullName());
#endif

    static MqlTick _tmp_ticks[];
    ArrayResize(_tmp_ticks, 0);

    // Number of retries for CopyTicksRange().
    int _tries = 10;

    // Number of ticks copied by CopyTicksRange().
    int _num_copied = -1;

    // Number of ticks remaining to copy in order to fulfill number of minimum required ticks (_ticks_to_emit).
    int _num_yet_to_copy = _ticks_to_emit;

    // In ms, the period we will be retrieving ticks for.
    int _period_msc = 1000 * 60 * 60;    // 1 hour distance.
    int _max_periods_to_check = 24 * 7;  // Two weeks should be enough.
    int _periods_checked = 0;

    unsigned long _range_from = TimeCurrent() * 1000 - _period_msc;
    unsigned long _range_to = TimeCurrent() * 1000 - 1;

    while (_tries > 0) {
      _num_copied = CopyTicksRange(GetSymbol(), _tmp_ticks, COPY_TICKS_INFO, _range_from, _range_to);

      if (_num_copied == -1) {
        ResetLastError();
        Sleep(1000);
        --_tries;
      } else {
        _num_yet_to_copy -= _num_copied;

        for (int i = 0; i < _num_copied; ++i) {
          TickAB<double> _tick(_tmp_ticks[i].ask, _tmp_ticks[i].bid);
          // We can't call EmitEntry() here, as tick would go to multiple sources at the same time!
#ifdef __debug_verbose__
          Print("Tick at ", TimeToString(_tmp_ticks[i].time, TIME_DATE | TIME_MINUTES | TIME_SECONDS), ": ",
                _tmp_ticks[i].ask, ", ", _tmp_ticks[i].bid);
#endif

          EmitEntry(TickToEntry(_tmp_ticks[i].time, _tick));

          if (_num_yet_to_copy <= 0) {
            break;
          }
        }

        _range_from -= _period_msc;
        _range_to -= _period_msc;
        if (++_periods_checked > _max_periods_to_check) {
          break;
        }
      }
    }

#ifdef __debug_verbose__
    Print("Listening indicators were filled with ", (_ticks_to_emit - _num_yet_to_copy), " out of ", _ticks_to_emit,
          " historical entries requested");
#endif

#endif
  }

  void OnTick(int _global_tick_index) override {
    if (_fetch_history_on_first_tick) {
      // We wait for fetching the history for the first tick, as it won't work in OnInit().
      _fetch_history_on_first_tick = false;
      FetchHistory();
    }

#ifdef __MQL4__
    // Refreshes Ask/Bid constants.
    RefreshRates();
    double _ask = Ask;
    double _bid = Bid;
    long _time = TimeCurrent();
#else
    static MqlTick _tmp_ticks[];
    // Copying only the last tick.
    int _num_copied = CopyTicks(GetSymbol(), _tmp_ticks, COPY_TICKS_INFO, 0, 1);

    if (_num_copied < 1 || _LastError != 0) {
      Print("Error. Cannot copy MT ticks via CopyTicks(). Error " + IntegerToString(_LastError));
      // DebugBreak();
      // Just emitting zeroes in case of error.
      TickAB<double> _tick(0, 0);
      EmitEntry(TickToEntry(TimeCurrent(), _tick));
      return;
    }

#ifdef __debug_verbose__
    Print("CpyT: ", TimeToString(_tmp_ticks[0].time, TIME_DATE | TIME_MINUTES | TIME_SECONDS), " = ", _tmp_ticks[0].bid,
          " (", _tmp_ticks[0].time, ")");
    Print("RlCl: ", TimeToString(::iTime(GetSymbol(), PERIOD_CURRENT, 0), TIME_DATE | TIME_MINUTES | TIME_SECONDS),
          " = ", ::iClose(GetSymbol(), PERIOD_CURRENT, 0));
#endif

    double _ask = _tmp_ticks[0].ask;
    double _bid = _tmp_ticks[0].bid;
    // long _time = _tmp_ticks[0].time;
    long _time = TimeCurrent();
#endif
    TickAB<double> _tick(_ask, _bid);
    IndicatorDataEntry _entry(TickToEntry(_time, _tick));
    StoreEntry(_entry);
    EmitEntry(_entry);
  }
};
