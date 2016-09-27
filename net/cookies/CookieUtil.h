// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_COOKIES_COOKIE_UTIL_H_
#define NET_COOKIES_COOKIE_UTIL_H_

#include <string>

namespace blink {
class KURL;
}

namespace net {
namespace cookie_util {

// Returns the effective TLD+1 for a given host. This only makes sense for http
// and https schemes. For other schemes, the host will be returned unchanged
// (minus any leading period).
std::string GetEffectiveDomain(const std::string& scheme,
                                          const std::string& host);

// Determine the actual cookie domain based on the domain string passed
// (if any) and the URL from which the cookie came.
// On success returns true, and sets cookie_domain to either a
//   -host cookie domain (ex: "google.com")
//   -domain cookie domain (ex: ".google.com")
bool GetCookieDomainWithString(const blink::KURL& url, const std::string& domain_string, std::string* result);

enum UnknownRegistryFilter {
	EXCLUDE_UNKNOWN_REGISTRIES = 0,
	INCLUDE_UNKNOWN_REGISTRIES
};

enum PrivateRegistryFilter {
	EXCLUDE_PRIVATE_REGISTRIES = 0,
	INCLUDE_PRIVATE_REGISTRIES
};

std::string GetDomainAndRegistry(const std::string& host, PrivateRegistryFilter filter);

// Returns true if a domain string represents a host-only cookie,
// i.e. it doesn't begin with a leading '.' character.
bool DomainIsHostOnly(const std::string& domain_string);

// Parses the string with the cookie time (very forgivingly).
double ParseCookieTime(const std::string& time_string);

// Convenience for converting a cookie origin (domain and https pair) to a URL.
blink::KURL CookieOriginToURL(const std::string& domain, bool is_https);

}  // namspace cookie_util
}  // namespace net

#endif  // NET_COOKIES_COOKIE_UTIL_H_
