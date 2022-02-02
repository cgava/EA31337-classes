//+------------------------------------------------------------------+
//|                                                EA31337 framework |
//|                                 Copyright 2016-2021, EA31337 Ltd |
//|                                       https://github.com/EA31337 |
//+------------------------------------------------------------------+

/*
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * Includes TaskGetter's structures.
 */

#ifndef __MQL__
// Allows the preprocessor to include a header file when it is needed.
#pragma once
#endif

// Includes.
#include "../Data.struct.h"
#include "../Std.h"
#include "../Terminal.define.h"
#include "Task.enum.h"

/* Entry for TaskGetter class. */
struct TaskGetterEntry {
 public:
  /* Enumerations */

  // Defines action entry properties.
  enum ENUM_TASK_GETTER_ENTRY_PROP {
    TASK_GETTER_ENTRY_FLAGS,
    TASK_GETTER_ENTRY_FREQUENCY,
    TASK_GETTER_ENTRY_ID,
    TASK_GETTER_ENTRY_TRIES,
    TASK_GETTER_ENTRY_TIME_LAST_GET,
  };
  // Defines action entry flags.
  enum ENUM_TASK_GETTER_ENTRY_FLAG {
    TASK_GETTER_ENTRY_FLAG_NONE = 0 << 0,
    TASK_GETTER_ENTRY_FLAG_IS_ACTIVE = 1 << 0,
    TASK_GETTER_ENTRY_FLAG_IS_DONE = 1 << 1,
    TASK_GETTER_ENTRY_FLAG_IS_FAILED = 1 << 2,
    TASK_GETTER_ENTRY_FLAG_IS_INVALID = 1 << 3,
  };

 protected:
  unsigned char flags;    /* TaskGetter flags. */
  datetime time_last_get; /* Time of the successful get. */
  int freq;               /* How often to run (0 for no limit). */
  int id;                 /* TaskGetter's enum ID. */
  short tries;            /* Number of retries left (-1 for unlimited). */
  DataParamEntry args[];  /* TaskGetter arguments. */
 protected:
  // Protected methods.
  void Init() { SetFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_INVALID), id == WRONG_VALUE); }

 public:
  // Constructors.
  TaskGetterEntry() : flags(0), freq(60), id(WRONG_VALUE), time_last_get(0), tries(-1) { Init(); }
  TaskGetterEntry(int _id)
      : flags(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_ACTIVE)),
        id(_id),
        freq(60),
        time_last_get(0),
        tries(-1) {
    Init();
  }
  TaskGetterEntry(TaskGetterEntry &_ae) { this = _ae; }
  // Flag methods.
  bool HasFlag(unsigned char _flag) const { return bool(flags & _flag); }
  void AddFlags(unsigned char _flags) { flags |= _flags; }
  void RemoveFlags(unsigned char _flags) { flags &= ~_flags; }
  void SetFlag(STRUCT_ENUM(TaskGetterEntry, ENUM_TASK_GETTER_ENTRY_FLAG) _flag, bool _value) {
    if (_value) {
      AddFlags(_flag);
    } else {
      RemoveFlags(_flag);
    }
  }
  void SetFlags(unsigned char _flags) { flags = _flags; }
  // State methods.
  bool HasTriesLeft() const { return tries > 0 || tries == -1; }
  bool IsActive() const { return HasFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_ACTIVE)); }
  bool IsDone() const { return HasFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_DONE)); }
  bool IsFailed() const { return HasFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_FAILED)); }
  bool IsInvalid() const { return HasFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_INVALID)); }
  bool IsValid() const { return !IsInvalid(); }
  // Getters.
  bool Get(STRUCT_ENUM(TaskGetterEntry, ENUM_TASK_GETTER_ENTRY_FLAG) _flag) const { return HasFlag(_flag); }
  template <typename T>
  T Get(STRUCT_ENUM(TaskGetterEntry, ENUM_TASK_GETTER_ENTRY_PROP) _prop) const {
    switch (_prop) {
      case TASK_GETTER_ENTRY_FLAGS:
        return (T)flags;
      case TASK_GETTER_ENTRY_FREQUENCY:
        return (T)freq;
      case TASK_GETTER_ENTRY_ID:
        return (T)id;
      case TASK_GETTER_ENTRY_TRIES:
        return (T)tries;
      case TASK_GETTER_ENTRY_TIME_LAST_RUN:
        return (T)time_last_run;
      default:
        break;
    }
    SetUserError(ERR_INVALID_PARAMETER);
    return WRONG_VALUE;
  }
  int GetId() const { return id; }
  // Setters.
  void TriesDec() { tries -= tries > 0 ? 1 : 0; }
  void Set(STRUCT_ENUM(TaskGetterEntry, ENUM_TASK_GETTER_ENTRY_FLAG) _flag, bool _value = true) {
    SetFlag(_flag, _value);
  }
  template <typename T>
  void Set(STRUCT_ENUM(TaskGetterEntry, ENUM_TASK_GETTER_ENTRY_PROP) _prop, T _value) {
    switch (_prop) {
      case TASK_GETTER_ENTRY_FLAGS:  // ID (magic number).
        flags = (unsigned char)_value;
        return;
      case TASK_GETTER_ENTRY_FREQUENCY:
        freq = (int)_value;
        return;
      case TASK_GETTER_ENTRY_ID:
        id = (int)_value;
        SetFlag(STRUCT_ENUM(TaskGetterEntry, TASK_GETTER_ENTRY_FLAG_IS_INVALID), id == WRONG_VALUE);
        return;
      case TASK_GETTER_ENTRY_TRIES:
        tries = (short)_value;
        return;
      case TASK_GETTER_ENTRY_TIME_LAST_GET:
        time_last_get = (datetime)_value;
        return;
      default:
        break;
    }
    SetUserError(ERR_INVALID_PARAMETER);
  }
  void AddArg(MqlParam &_arg) {
    // @todo: Add another value to args[].
  }
  void SetArgs(ARRAY_REF(MqlParam, _args)) {
    // @todo: for().
  }
  // Serializers
  SerializerNodeType Serialize(Serializer &s) {
    s.Pass(THIS_REF, "flags", flags);
    s.Pass(THIS_REF, "id", id);
    s.Pass(THIS_REF, "time_last_get", time_last_get);
    s.Pass(THIS_REF, "tries", tries);
    s.PassEnum(THIS_REF, "freq", freq);
    s.PassArray(this, "args", args);
    return SerializerNodeObject;
  }

  SERIALIZER_EMPTY_STUB;
};