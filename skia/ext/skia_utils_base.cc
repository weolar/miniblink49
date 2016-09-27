// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/skia_utils_base.h"

namespace skia {

bool ReadSkString(const Pickle& pickle, PickleIterator* iter, SkString* str) {
  int         reply_length;
  const char* reply_text;

  if (!pickle.ReadData(iter, &reply_text, &reply_length))
    return false;

  if (str)
    str->set(reply_text, reply_length);
  return true;
}

bool ReadSkFontIdentity(const Pickle& pickle, PickleIterator* iter,
                        SkFontConfigInterface::FontIdentity* identity) {
  uint32_t    reply_id;
  uint32_t    reply_ttcIndex;
  int         reply_length;
  const char* reply_text;

  if (!pickle.ReadUInt32(iter, &reply_id) ||
      !pickle.ReadUInt32(iter, &reply_ttcIndex) ||
      !pickle.ReadData(iter, &reply_text, &reply_length))
    return false;

  if (identity) {
    identity->fID = reply_id;
    identity->fTTCIndex = reply_ttcIndex;
    identity->fString.set(reply_text, reply_length);
  }
  return true;
}

bool WriteSkString(Pickle* pickle, const SkString& str) {
  return pickle->WriteData(str.c_str(), str.size());
}

bool WriteSkFontIdentity(Pickle* pickle,
                         const SkFontConfigInterface::FontIdentity& identity) {
  return pickle->WriteUInt32(identity.fID) &&
         pickle->WriteUInt32(identity.fTTCIndex) &&
         WriteSkString(pickle, identity.fString);
}

}  // namespace skia

