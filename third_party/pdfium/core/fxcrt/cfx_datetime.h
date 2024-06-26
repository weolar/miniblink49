// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_DATETIME_H_
#define CORE_FXCRT_CFX_DATETIME_H_

#include "core/fxcrt/fx_system.h"

bool FX_IsLeapYear(int32_t iYear);
uint8_t FX_DaysInMonth(int32_t iYear, uint8_t iMonth);

class CFX_DateTime {
 public:
  static CFX_DateTime Now();

  CFX_DateTime()
      : year_(0),
        month_(0),
        day_(0),
        hour_(0),
        minute_(0),
        second_(0),
        millisecond_(0) {}
  CFX_DateTime(int32_t year,
               uint8_t month,
               uint8_t day,
               uint8_t hour,
               uint8_t minute,
               uint8_t second,
               uint16_t millisecond)
      : year_(year),
        month_(month),
        day_(day),
        hour_(hour),
        minute_(minute),
        second_(second),
        millisecond_(millisecond) {}

  void Reset() {
    year_ = 0;
    month_ = 0;
    day_ = 0;
    hour_ = 0;
    minute_ = 0;
    second_ = 0;
    millisecond_ = 0;
  }

  bool IsSet() const {
    return year_ != 0 || month_ != 0 || day_ != 0 || hour_ != 0 ||
           minute_ != 0 || second_ != 0 || millisecond_ != 0;
  }

  void SetDate(int32_t year, uint8_t month, uint8_t day) {
    year_ = year;
    month_ = month;
    day_ = day;
  }

  void SetTime(uint8_t hour,
               uint8_t minute,
               uint8_t second,
               uint16_t millisecond) {
    hour_ = hour;
    minute_ = minute;
    second_ = second;
    millisecond_ = millisecond;
  }

  int32_t GetYear() const { return year_; }
  uint8_t GetMonth() const { return month_; }
  uint8_t GetDay() const { return day_; }
  uint8_t GetHour() const { return hour_; }
  uint8_t GetMinute() const { return minute_; }
  uint8_t GetSecond() const { return second_; }
  uint16_t GetMillisecond() const { return millisecond_; }
  int32_t GetDayOfWeek() const;

  bool operator==(const CFX_DateTime& other) const;

 private:
  int32_t year_;
  uint8_t month_;
  uint8_t day_;
  uint8_t hour_;
  uint8_t minute_;
  uint8_t second_;
  uint16_t millisecond_;
};

#if _FX_OS_ != _FX_OS_ANDROID_
#pragma pack(push, 1)
#endif
struct FX_TIMEZONE {
  int8_t tzHour;
  uint8_t tzMinute;
};
#if _FX_OS_ != _FX_OS_ANDROID_
#pragma pack(pop)
#endif

#endif  // CORE_FXCRT_CFX_DATETIME_H_
