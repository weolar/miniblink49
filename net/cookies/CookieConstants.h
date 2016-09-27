// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_COOKIES_COOKIE_CONSTANTS_H_
#define NET_COOKIES_COOKIE_CONSTANTS_H_

#include <string>
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

namespace net {

enum CookiePriority {
  COOKIE_PRIORITY_LOW     = 0,
  COOKIE_PRIORITY_MEDIUM  = 1,
  COOKIE_PRIORITY_HIGH    = 2,
  COOKIE_PRIORITY_DEFAULT = COOKIE_PRIORITY_MEDIUM
};

enum CookieSameSite {
  NO_RESTRICTION = 0,
  LAX_MODE = 1,
  STRICT_MODE = 2,
  DEFAULT_MODE = NO_RESTRICTION
};

const char kPriorityLow[] = "low";
const char kPriorityMedium[] = "medium";
const char kPriorityHigh[] = "high";

// Returns the Set-Cookie header priority token corresponding to |priority|.
//
// TODO(mkwst): Remove this once its callsites are refactored.
inline const std::string CookiePriorityToString(CookiePriority priority) {
	switch (priority) {
	case COOKIE_PRIORITY_HIGH:
		return kPriorityHigh;
	case COOKIE_PRIORITY_MEDIUM:
		return kPriorityMedium;
	case COOKIE_PRIORITY_LOW:
		return kPriorityLow;
	default:
		notImplemented();
	}
	return std::string();
}

// Converts the Set-Cookie header priority token |priority| to a CookiePriority.
// Defaults to COOKIE_PRIORITY_DEFAULT for empty or unrecognized strings.
inline CookiePriority StringToCookiePriority(const std::string& priority) {
	WTF::String priority_comp = (priority).c_str();
	priority_comp = priority_comp.lower();

	if (priority_comp == kPriorityHigh)
		return COOKIE_PRIORITY_HIGH;
	if (priority_comp == kPriorityMedium)
		return COOKIE_PRIORITY_MEDIUM;
	if (priority_comp == kPriorityLow)
		return COOKIE_PRIORITY_LOW;

	return COOKIE_PRIORITY_DEFAULT;
}


// Converst the Set-Cookie header SameSite token |same_site| to a
// CookieSameSite. Defaults to CookieSameSite::DEFAULT_MODE for empty or
// unrecognized strings.

// NET_EXPORT CookieSameSite StringToCookieSameSite(const std::string& same_site);




}  // namespace net

#endif  // NET_COOKIES_COOKIE_CONSTANTS_H_
