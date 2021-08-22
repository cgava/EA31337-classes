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
 * Includes Strategy's structs.
 */

#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once
#endif

// Includes.
#include "Serializer.mqh"
#include "Strategy.enum.h"
#include "Strategy.struct.pricestop.h"
#include "Task.struct.h"

// Forward class declaration.
class Indicator;
class Strategy;
class Trade;

/* Structure for strategy parameters. */
struct StgParams {
  // Strategy config parameters.
  bool is_enabled;                                     // State of the strategy (whether enabled or not).
  bool is_suspended;                                   // State of the strategy (whether suspended or not)
  bool is_boosted;                                     // State of the boost feature (to increase lot size).
  long id;                                             // Identification number of the strategy.
  float weight;                                        // Weight of the strategy.
  long order_close_time;                               // Order close time in mins (>0) or bars (<0).
  float order_close_loss;                              // Order close loss (in pips).
  float order_close_profit;                            // Order close profit (in pips).
  int signal_open_method;                              // Signal open method.
  float signal_open_level;                             // Signal open level.
  int signal_open_filter_method;                       // Signal open filter method.
  int signal_open_filter_time;                         // Signal open filter time.
  int signal_open_boost;                               // Signal open boost method (for lot size increase).
  int signal_close_method;                             // Signal close method.
  float signal_close_level;                            // Signal close level.
  int signal_close_filter;                             // Signal close filter method.
  int price_profit_method;                             // Price profit method.
  float price_profit_level;                            // Price profit level.
  int price_stop_method;                               // Price stop method.
  float price_stop_level;                              // Price stop level.
  int tick_filter_method;                              // Tick filter.
  float trend_threshold;                               // Trend strength threshold.
  float lot_size;                                      // Lot size to trade.
  float lot_size_factor;                               // Lot size multiplier factor.
  float max_risk;                                      // Maximum risk to take (1.0 = normal, 2.0 = 2x).
  float max_spread;                                    // Maximum spread to trade (in pips).
  int tp_max;                                          // Hard limit on maximum take profit (in pips).
  int sl_max;                                          // Hard limit on maximum stop loss (in pips).
  datetime refresh_time;                               // Order refresh frequency (in sec).
  short shift;                                         // Shift (relative to the current bar, 0 - default)
  DictStruct<int, Ref<Indicator>> indicators_managed;  // Indicators list keyed by id.
  Dict<int, Indicator *> indicators_unmanaged;         // Indicators list keyed by id.
  // Constructor.
  StgParams()
      : is_enabled(true),
        is_suspended(false),
        is_boosted(true),
        order_close_time(0),
        order_close_loss(0.0f),
        order_close_profit(0.0f),
        weight(0),
        signal_open_method(0),
        signal_open_level(0),
        signal_open_filter_method(0),
        signal_open_filter_time(0),
        signal_open_boost(0),
        signal_close_method(0),
        signal_close_level(0),
        signal_close_filter(0),
        price_profit_method(0),
        price_profit_level(0),
        price_stop_method(0),
        price_stop_level(0),
        tick_filter_method(0),
        trend_threshold(0.4f),
        lot_size(0),
        lot_size_factor(1.0),
        max_risk(1.0),
        max_spread(0.0),
        tp_max(0),
        sl_max(0),
        refresh_time(0) {}
  StgParams(int _som, int _sofm, float _sol, int _sob, int _scm, int _scf, float _scl, int _psm, float _psl, int _tfm,
            float _ms, short _s = 0)
      : order_close_loss(0.0f),
        order_close_profit(0.0f),
        order_close_time(0),
        signal_open_method(_som),
        signal_open_filter_method(_sofm),
        signal_open_level(_sol),
        signal_open_boost(_sob),
        signal_close_method(_scm),
        signal_close_filter(_scf),
        signal_close_level(_scl),
        price_profit_method(_psm),
        price_profit_level(_psl),
        price_stop_method(_psm),
        price_stop_level(_psl),
        tick_filter_method(_tfm),
        shift(_s),
        is_enabled(true),
        is_suspended(false),
        is_boosted(true),
        weight(0),
        lot_size(0),
        lot_size_factor(1.0),
        max_risk(1.0),
        max_spread(0.0),
        tp_max(0),
        sl_max(0),
        refresh_time(0) {}
  StgParams(StgParams &_stg_params) {
    DeleteObjects();
    this = _stg_params;
  }
  // Deconstructor.
  ~StgParams() {}

  // Getters.
  template <typename T>
  T Get(ENUM_STRATEGY_PARAM _param) {
    switch (_param) {
      case STRAT_PARAM_LS:
        return (T)lot_size;
      case STRAT_PARAM_LSF:
        return (T)lot_size_factor;
      case STRAT_PARAM_MAX_RISK:
        return (T)max_risk;
      case STRAT_PARAM_MAX_SPREAD:
        return (T)max_spread;
      case STRAT_PARAM_SOL:
        return (T)signal_open_level;
      case STRAT_PARAM_SCL:
        return (T)signal_close_level;
      case STRAT_PARAM_PPL:
        return (T)price_profit_level;
      case STRAT_PARAM_PSL:
        return (T)price_stop_level;
      case STRAT_PARAM_OCL:
        return (T)order_close_loss;
      case STRAT_PARAM_OCP:
        return (T)order_close_profit;
      case STRAT_PARAM_OCT:
        return (T)order_close_time;
      case STRAT_PARAM_SOM:
        return (T)signal_open_method;
      case STRAT_PARAM_SOFM:
        return (T)signal_open_filter_method;
      case STRAT_PARAM_SOFT:
        return (T)signal_open_filter_time;
      case STRAT_PARAM_SOB:
        return (T)signal_open_boost;
      case STRAT_PARAM_SCF:
        return (T)signal_close_filter;
      case STRAT_PARAM_SCM:
        return (T)signal_close_method;
      case STRAT_PARAM_SHIFT:
        return (T)shift;
      case STRAT_PARAM_PPM:
        return (T)price_profit_method;
      case STRAT_PARAM_PSM:
        return (T)price_stop_method;
      case STRAT_PARAM_TFM:
        return (T)tick_filter_method;
      case STRAT_PARAM_WEIGHT:
        return (T)weight;
    }
    SetUserError(ERR_INVALID_PARAMETER);
    return WRONG_VALUE;
  }
  bool HasIndicator(int _id = 0) { return GetIndicator(_id) != NULL; }
  bool IsBoosted() { return is_boosted; }
  bool IsEnabled() { return is_enabled; }
  bool IsSuspended() { return is_suspended; }
  Indicator *GetIndicator(int _id = 0) {
    if (indicators_managed.KeyExists(_id)) {
      return indicators_managed[_id].Ptr();
    } else if (indicators_unmanaged.KeyExists(_id)) {
      return indicators_unmanaged[_id];
    }

    Alert("Missing indicator id ", _id);
    return NULL;
  }
  // Setters.
  template <typename T>
  void Set(ENUM_STRATEGY_PARAM _param, T _value) {
    switch (_param) {
      case STRAT_PARAM_LS:  // Lot size
        lot_size = (float)_value;
        return;
      case STRAT_PARAM_LSF:  // Lot size factor
        lot_size_factor = (float)_value;
        return;
      case STRAT_PARAM_MAX_RISK:
        max_risk = (float)_value;
        return;
      case STRAT_PARAM_MAX_SPREAD:
        max_spread = (float)_value;
        return;
      case STRAT_PARAM_SHIFT:  // Shift
        shift = (short)_value;
        return;
      case STRAT_PARAM_SOL:  // Signal open level
        signal_open_level = (float)_value;
        return;
      case STRAT_PARAM_SCL:  // Signal close level
        signal_close_level = (float)_value;
        return;
      case STRAT_PARAM_PPL:  // Signal profit level
        price_profit_level = (float)_value;
        return;
      case STRAT_PARAM_PSL:  // Price stop level
        price_stop_level = (float)_value;
        return;
      case STRAT_PARAM_OCL:  // Order close loss
        order_close_loss = (float)_value;
        return;
      case STRAT_PARAM_OCP:  // Order close profit
        order_close_profit = (float)_value;
        return;
      case STRAT_PARAM_OCT:  // Order close time
        order_close_time = (long)_value;
        return;
      case STRAT_PARAM_SOM:  // Signal open method
        signal_open_method = (int)_value;
        return;
      case STRAT_PARAM_SOFM:  // Signal open filter method
        signal_open_filter_method = (int)_value;
        return;
      case STRAT_PARAM_SOFT:  // Signal open filter time
        signal_open_filter_time = (int)_value;
        return;
      case STRAT_PARAM_SOB:  // Signal open boost method
        signal_open_boost = (int)_value;
        return;
      case STRAT_PARAM_SCF:  // Signal close filter
        signal_close_filter = (int)_value;
        return;
      case STRAT_PARAM_SCM:  // Signal close method
        signal_close_method = (int)_value;
        return;
      case STRAT_PARAM_PPM:  // Signal profit method
        price_profit_method = (int)_value;
        return;
      case STRAT_PARAM_PSM:  // Price stop method
        price_stop_method = (int)_value;
        return;
      case STRAT_PARAM_TFM:  // Tick filter method
        tick_filter_method = (int)_value;
        return;
      case STRAT_PARAM_WEIGHT:  // Weight
        weight = (float)_value;
        return;
    }
    SetUserError(ERR_INVALID_PARAMETER);
  }
  void Set(ENUM_STRATEGY_PARAM _enum_param, MqlParam &_mql_param) {
    if (_mql_param.type == TYPE_DOUBLE || _mql_param.type == TYPE_FLOAT) {
      Set(_enum_param, _mql_param.double_value);
    } else {
      Set(_enum_param, _mql_param.integer_value);
    }
  }
  void SetId(long _id) { id = _id; }
  void SetIndicator(Indicator *_indi, int _id = 0, bool _managed = true) {
    if (_managed) {
      Ref<Indicator> _ref = _indi;
      indicators_managed.Set(_id, _ref);
    } else {
      indicators_unmanaged.Set(_id, _indi);
    }
  }
  void SetStops(Strategy *_sl = NULL, Strategy *_tp = NULL) {
    // @todo: To remove.
  }
  void SetSignals(int _som, float _sol, int _sofm, int _sob, int _csm, float _cl) {
    signal_open_method = _som;
    signal_open_level = _sol;
    signal_open_filter_method = _sofm;
    signal_open_boost = _sob;
    signal_close_method = _csm;
    signal_close_level = _cl;
  }
  void Enabled(bool _is_enabled) { is_enabled = _is_enabled; };
  void Suspended(bool _is_suspended) { is_suspended = _is_suspended; };
  void Boost(bool _is_boosted) { is_boosted = _is_boosted; };
  void DeleteObjects() {
    for (DictIterator<int, Indicator *> iter = indicators_unmanaged.Begin(); iter.IsValid(); ++iter) {
      delete iter.Value();
    }
  }
  // Printers.
  string ToString() {
    return StringFormat("Enabled:%s;Suspended:%s;Boosted:%s;Id:%d,Weight:%.2f;" + "SOM:%d,SOL:%.2f;" +
                            "SCM:%d,SCL:%.2f;" + "PSM:%d,PSL:%.2f;" + "LS:%.2f(Factor:%.2f);MS:%.2f;",
                        // @todo: "Data:%s;SL/TP-Strategy:%s/%s",
                        is_enabled ? "Yes" : "No", is_suspended ? "Yes" : "No", is_boosted ? "Yes" : "No", id, weight,
                        signal_open_method, signal_open_level, signal_close_method, signal_close_level,
                        price_stop_method, price_stop_level, lot_size, lot_size_factor, max_spread
                        // @todo: data, sl, tp
    );
  }

  // Serializers.
  SERIALIZER_EMPTY_STUB;
  SerializerNodeType Serialize(Serializer &s) {
    s.Pass(THIS_REF, "is_enabled", is_enabled);
    s.Pass(THIS_REF, "is_suspended", is_suspended);
    s.Pass(THIS_REF, "is_boosted", is_boosted);
    s.Pass(THIS_REF, "id", id);
    s.Pass(THIS_REF, "weight", weight);
    s.Pass(THIS_REF, "ocl", order_close_loss);
    s.Pass(THIS_REF, "ocp", order_close_profit);
    s.Pass(THIS_REF, "oct", order_close_time);
    s.Pass(THIS_REF, "shift", shift);
    s.Pass(THIS_REF, "som", signal_open_method);
    s.Pass(THIS_REF, "sol", signal_open_level);
    s.Pass(THIS_REF, "sofm", signal_open_filter_method);
    s.Pass(THIS_REF, "soft", signal_open_filter_time);
    s.Pass(THIS_REF, "sob", signal_open_boost);
    s.Pass(THIS_REF, "scm", signal_close_method);
    s.Pass(THIS_REF, "scl", signal_close_level);
    s.Pass(THIS_REF, "ppm", price_profit_method);
    s.Pass(THIS_REF, "ppl", price_profit_level);
    s.Pass(THIS_REF, "psm", price_stop_method);
    s.Pass(THIS_REF, "psl", price_stop_level);
    s.Pass(THIS_REF, "tfm", tick_filter_method);
    s.Pass(THIS_REF, "tt", trend_threshold);
    s.Pass(THIS_REF, "ls", lot_size);
    s.Pass(THIS_REF, "lsf", lot_size_factor);
    s.Pass(THIS_REF, "max_risk", max_risk);
    s.Pass(THIS_REF, "max_spread", max_spread);
    s.Pass(THIS_REF, "tp_max", tp_max);
    s.Pass(THIS_REF, "sl_max", sl_max);
    s.Pass(THIS_REF, "refresh_time", refresh_time);
    // @todo
    // Ref<Log> logger;           // Reference to Log object.
    // Trade *trade;              // Pointer to Trade class.
    // Indicator *data;           // Pointer to Indicator class.
    // Strategy *sl, *tp;         // References to Strategy class (stop-loss and profit-take).
    return SerializerNodeObject;
  }
} stg_params_defaults;

/* Structure for strategy's param values. */
struct Stg_Params {
  string symbol;
  ENUM_TIMEFRAMES tf;
  Stg_Params() : symbol(_Symbol), tf((ENUM_TIMEFRAMES)_Period) {}
};

/* Structure for strategy's process results. */
struct StgProcessResult {
  float boost_factor;                  // Boost factor used.
  float lot_size;                      // Lot size used.
  unsigned int last_error;             // Last error code.
  unsigned short pos_updated;          // Number of positions updated.
  unsigned short stops_invalid_sl;     // Number of invalid stop-loss values.
  unsigned short stops_invalid_tp;     // Number of invalid take-profit values.
  unsigned short tasks_processed;      // Task processed.
  unsigned short tasks_processed_not;  // Task not processed.
  // Struct constructor.
  StgProcessResult() { Reset(); }
  // Getters.
  float GetBoostFactor() { return boost_factor; }
  float GetLotSize() { return lot_size; }
  string ToString() { return StringFormat("%d,%d,%d,%d", pos_updated, stops_invalid_sl, stops_invalid_tp, last_error); }
  // Setters.
  void ProcessLastError() { last_error = fmax(last_error, Terminal::GetLastError()); }
  void Reset() {
    pos_updated = stops_invalid_sl = stops_invalid_tp = 0;
    last_error = ERR_NO_ERROR;
  }
  void SetBoostFactor(float _value) { boost_factor = _value; }
  void SetLotSize(float _value) { lot_size = _value; }
  // Serializers.
  SERIALIZER_EMPTY_STUB;
  SerializerNodeType Serialize(Serializer &_s) {
    _s.Pass(THIS_REF, "boost_factor", boost_factor, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "lot_size", lot_size, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "last_error", last_error, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "pos_updated", pos_updated, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "stops_invalid_sl", stops_invalid_sl, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "stops_invalid_tp", stops_invalid_tp, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "tasks_processed", tasks_processed, SERIALIZER_FIELD_FLAG_DYNAMIC);
    _s.Pass(THIS_REF, "tasks_processed_not", tasks_processed_not, SERIALIZER_FIELD_FLAG_DYNAMIC);
    return SerializerNodeObject;
  }
};

/* Structure for strategy's signals. */
struct StrategySignal {
 protected:
  ENUM_TIMEFRAMES tf;    // Timeframe.
  float strength;        // Signal strength.
  float weight;          // Signal weight.
  unsigned int signals;  // Store signals (@see: ENUM_STRATEGY_SIGNAL_FLAG).
  Strategy *strat;

 public:
  enum ENUM_STRATEGY_SIGNAL_PROP {
    STRATEGY_SIGNAL_PROP_SIGNALS,
    STRATEGY_SIGNAL_PROP_STRENGTH,
    STRATEGY_SIGNAL_PROP_TF,
    STRATEGY_SIGNAL_PROP_WEIGHT,
  };
  /* Constructor */
  StrategySignal(Strategy *_strat = NULL, ENUM_TIMEFRAMES _tf = NULL, float _weight = 0.0f)
      : strat(_strat), tf(_tf), weight(_weight) {}
  /* Getters */
  template <typename T>
  T Get(unsigned int _param) {
    switch (_param) {
      case STRATEGY_SIGNAL_PROP_SIGNALS:
        return (T)tf;
      case STRATEGY_SIGNAL_PROP_STRENGTH:
        return (T)strength;
      case STRATEGY_SIGNAL_PROP_TF:
        return (T)signals;
      case STRATEGY_SIGNAL_PROP_WEIGHT:
        return (T)weight;
    }
    SetUserError(ERR_INVALID_PARAMETER);
    return (T)WRONG_VALUE;
  }
  Strategy *GetStrategy() { return strat; }
  /* Setters */
  template <typename T>
  void Set(unsigned int _param, T _value) {
    switch (_param) {
      case STRATEGY_SIGNAL_PROP_SIGNALS:
        signals = (unsigned int)_value;
        return;
      case STRATEGY_SIGNAL_PROP_STRENGTH:
        strength = (float)_value;
        return;
      case STRATEGY_SIGNAL_PROP_TF:
        tf = (ENUM_TIMEFRAMES)_value;
        return;
      case STRATEGY_SIGNAL_PROP_WEIGHT:
        weight = (float)_value;
        return;
    }
    SetUserError(ERR_INVALID_PARAMETER);
  }
  void SetStrategy(Strategy *_strat) { strat = _strat; }
  /* Signal open and close methods */
  bool ShouldClose(ENUM_ORDER_TYPE _cmd) {
    switch (_cmd) {
      case ORDER_TYPE_BUY:
        return CheckSignalsAll(STRAT_SIGNAL_BUY_CLOSE | STRAT_SIGNAL_BUY_CLOSE_PASS);
      case ORDER_TYPE_SELL:
        return CheckSignalsAll(STRAT_SIGNAL_SELL_CLOSE | STRAT_SIGNAL_SELL_CLOSE_PASS);
    }
    return false;
  }
  bool ShouldOpen(ENUM_ORDER_TYPE _cmd) {
    switch (_cmd) {
      case ORDER_TYPE_BUY:
        return CheckSignalsAll(STRAT_SIGNAL_BUY_OPEN | STRAT_SIGNAL_BUY_OPEN_PASS | STRAT_SIGNAL_TIME_PASS);
      case ORDER_TYPE_SELL:
        return CheckSignalsAll(STRAT_SIGNAL_SELL_OPEN | STRAT_SIGNAL_SELL_OPEN_PASS | STRAT_SIGNAL_TIME_PASS);
    }
    return false;
  }
  /* Signal methods for bitwise operations */
  bool CheckSignals(unsigned int _flags) { return (signals & _flags) != 0; }
  bool CheckSignalsAll(unsigned int _flags) { return (signals & _flags) == _flags; }
  char GetCloseDirection() {
    if (CheckSignals(STRAT_SIGNAL_BUY_CLOSE & ~STRAT_SIGNAL_SELL_CLOSE)) {
      return 1;
    } else if (CheckSignals(STRAT_SIGNAL_SELL_CLOSE & ~STRAT_SIGNAL_BUY_CLOSE)) {
      return -1;
    }
    return 0;
  }
  char GetOpenDirection() {
    if (CheckSignals(STRAT_SIGNAL_BUY_OPEN & ~STRAT_SIGNAL_SELL_OPEN)) {
      return 1;
    } else if (CheckSignals(STRAT_SIGNAL_SELL_OPEN & ~STRAT_SIGNAL_BUY_OPEN)) {
      return -1;
    }
    return 0;
  }
  unsigned int GetSignals() { return signals; }
  /* Setters */
  void AddSignals(unsigned int _flags) { signals |= _flags; }
  void RemoveSignals(unsigned int _flags) { signals &= ~_flags; }
  void SetSignal(ENUM_STRATEGY_SIGNAL_FLAG _flag, bool _value = true) {
    if (_value) {
      AddSignals(_flag);
    } else {
      RemoveSignals(_flag);
    }
  }
  void SetSignals(unsigned int _flags) { signals = _flags; }
  // Serializers.
  SERIALIZER_EMPTY_STUB;
  SerializerNodeType Serialize(Serializer &_s) {
    // _s.Pass(THIS_REF, "signals", signals, SERIALIZER_FIELD_FLAG_DYNAMIC | SERIALIZER_FIELD_FLAG_FEATURE);
    int _size = sizeof(int) * 8;
    for (int i = 0; i < _size; i++) {
      int _value = CheckSignals(1 << i) ? 1 : 0;
      _s.Pass(THIS_REF, (string)(i + 1), _value, SERIALIZER_FIELD_FLAG_DYNAMIC | SERIALIZER_FIELD_FLAG_FEATURE);
    }
    return SerializerNodeObject;
  }
};

/* Struture for strategy statistics */
struct StgStats {
  uint orders_open;  // Number of current opened orders.
  uint errors;       // Count reported errors.
};

/* Structure for strategy's statistical periods. */
struct StgStatsPeriod {
  // Statistics variables.
  uint orders_total;     // Number of total opened orders.
  uint orders_won;       // Number of total won orders.
  uint orders_lost;      // Number of total lost orders.
  double avg_spread;     // Average spread.
  double net_profit;     // Total net profit.
  double gross_profit;   // Total gross profit.
  double gross_loss;     // Total gross loss.
  double profit_factor;  // Profit factor.
  // Getters.
  string ToCSV() {
    return StringFormat("%d,%d,%d,%g,%g,%g,%g,%g", orders_total, orders_won, orders_lost, avg_spread, net_profit,
                        gross_profit, gross_loss, profit_factor);
  }
};

/* Structure for strategy's entry values. */
struct StgEntry {
  unsigned short signals;
  StgStatsPeriod stats_period[FINAL_ENUM_STRATEGY_STATS_PERIOD];
  string ToCSV() {
    return StringFormat("%s,%s,%s,%s", stats_period[EA_STATS_DAILY].ToCSV(), stats_period[EA_STATS_WEEKLY].ToCSV(),
                        stats_period[EA_STATS_MONTHLY].ToCSV(), stats_period[EA_STATS_TOTAL].ToCSV());
  }
  // Struct setters.
  void SetStats(StgStatsPeriod &_stats, ENUM_STRATEGY_STATS_PERIOD _period) { stats_period[_period] = _stats; }
  // Serializers.
  SERIALIZER_EMPTY_STUB
  SerializerNodeType Serialize(Serializer &_s) {
    // _s.Pass(THIS_REF, "signals", (int) signals);
    return SerializerNodeObject;
  }
};
