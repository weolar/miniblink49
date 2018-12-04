// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Portions of this code based on Mozilla:
//   (netwerk/cookie/src/nsCookieService.cpp)
/* ***** BEGIN LICENSE BLOCK *****
* Version: MPL 1.1/GPL 2.0/LGPL 2.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is mozilla.org code.
*
* The Initial Developer of the Original Code is
* Netscape Communications Corporation.
* Portions created by the Initial Developer are Copyright (C) 2003
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*   Daniel Witte (dwitte@stanford.edu)
*   Michiel van Leeuwen (mvl@exedo.nl)
*
* Alternatively, the contents of this file may be used under the terms of
* either the GNU General Public License Version 2 or later (the "GPL"), or
* the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
* in which case the provisions of the GPL or the LGPL are applicable instead
* of those above. If you wish to allow use of your version of this file only
* under the terms of either the GPL or the LGPL, and not to allow others to
* use your version of this file under the terms of the MPL, indicate your
* decision by deleting the provisions above and replace them with the notice
* and other provisions required by the GPL or the LGPL. If you do not delete
* the provisions above, a recipient may use your version of this file under
* the terms of any one of the MPL, the GPL or the LGPL.
*
* ***** END LICENSE BLOCK ***** */

#include "net/cookies/CanonicalCookie.h"
#include "net/cookies/ParsedCookie.h"
#include "net/cookies/CookieUtil.h"

#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "base/logging.h"

namespace net {

namespace {

const int kVlogSetCookies = 7;

// Determine the cookie domain to use for setting the specified cookie.
bool GetCookieDomain(const blink::KURL& url, const ParsedCookie& pc, std::string* result) {
// 	std::string domain_string;
// 	if (pc.HasDomain())
// 		domain_string = pc.Domain();
// 	return cookie_util::GetCookieDomainWithString(url, domain_string, result);
	DebugBreak();
	return false;
}

std::string CanonPathWithString(const blink::KURL& url,
	const std::string& path_string) {
	// The RFC says the path should be a prefix of the current URL path.
	// However, Mozilla allows you to set any path for compatibility with
	// broken websites.  We unfortunately will mimic this behavior.  We try
	// to be generous and accept cookies with an invalid path attribute, and
	// default the path to something reasonable.

	// The path was supplied in the cookie, we'll take it.
	if (!path_string.empty() && path_string[0] == '/')
		return path_string;

	// The path was not supplied in the cookie or invalid, we will default
	// to the current URL path.
	// """Defaults to the path of the request URL that generated the
	//    Set-Cookie response, up to, but not including, the
	//    right-most /."""
	// How would this work for a cookie on /?  We will include it then.
	const std::string& url_path = WTF::WTFStringToStdString(url.path());

	size_t idx = url_path.find_last_of('/');

	// The cookie path was invalid or a single '/'.
	if (idx == 0 || idx == std::string::npos)
		return std::string("/");

	// Return up to the rightmost '/'.
	return url_path.substr(0, idx);
}

// Compares cookies using name, domain and path, so that "equivalent" cookies
// (per RFC 2965) are equal to each other.
int PartialCookieOrdering(const CanonicalCookie& a, const CanonicalCookie& b) {
	int diff = a.Name().compare(b.Name());
	if (diff != 0)
		return diff;

	diff = a.Domain().compare(b.Domain());
	if (diff != 0)
		return diff;

	return a.Path().compare(b.Path());
}

}  // namespace

CanonicalCookie::CanonicalCookie()
	: secure_(false),
	httponly_(false),
	creation_date_(0),
	expiry_date_(0),
	last_access_date_(0) {
}

CanonicalCookie::CanonicalCookie(
	const blink::KURL& url, const std::string& name, const std::string& value,
	const std::string& domain, const std::string& path,
	const double& creation, const double& expiration,
	const double& last_access, bool secure, bool httponly,
	CookiePriority priority)
	: source_(GetCookieSourceFromURL(url)),
	name_(name),
	value_(value),
	domain_(domain),
	path_(path),
	creation_date_(creation),
	expiry_date_(expiration),
	last_access_date_(last_access),
	secure_(secure),
	httponly_(httponly),
	priority_(priority) {
}

CanonicalCookie::CanonicalCookie(const blink::KURL& url, const ParsedCookie& pc)
	//: source_(url.isLocalFile() ? url : url.origin()),
	: source_(GetCookieSourceFromURL(url)),
	name_(pc.Name()),
	value_(pc.Value()),
	path_(CanonPath(url, pc)),
	creation_date_(WTF::currentTime()),
	expiry_date_(0),
	last_access_date_(0),
	secure_(pc.IsSecure()),
	httponly_(pc.IsHttpOnly()),
	same_site_(NO_RESTRICTION /*pc.SameSite()*/),
	priority_(pc.Priority()) {
	if (pc.HasExpires())
		expiry_date_ = CanonExpiration(pc, creation_date_, creation_date_);

	// Do the best we can with the domain.
	std::string cookie_domain;
	std::string domain_string;
	if (pc.HasDomain()) {
		domain_string = pc.Domain();
	}

	bool result = cookie_util::GetCookieDomainWithString(url, domain_string, &cookie_domain);
	// Caller is responsible for passing in good arguments.
	DCHECK(result);

	domain_ = cookie_domain;
}

CanonicalCookie::CanonicalCookie(const CanonicalCookie& other) = default;

CanonicalCookie::~CanonicalCookie() {
}

blink::KURL CanonicalCookie::GetCookieSourceFromURL(const blink::KURL& url) {
	if (url.isLocalFile())
		return url;

	blink::KURL url_copy = url;
	url_copy.removePort();

	String replacements = WTF::ensureStringToUTF8String(url_copy.string());
	if (replacements.startsWith("https"))
		replacements.replace("https", "http");

	return blink::KURL(blink::ParsedURLString, replacements);
}

bool CanonicalCookie::IsOnPath(const std::string& url_path) const {

	// A zero length would be unsafe for our trailing '/' checks, and
	// would also make no sense for our prefix match.  The code that
	// creates a CanonicalCookie should make sure the path is never zero length,
	// but we double check anyway.
	if (path_.empty())
		return false;

	// The Mozilla code broke this into three cases, based on if the cookie path
	// was longer, the same length, or shorter than the length of the url path.
	// I think the approach below is simpler.

	// Make sure the cookie path is a prefix of the url path.  If the url path is
	// shorter than the cookie path, then the cookie path can't be a prefix.
	WTF::String urlPath = url_path.c_str();
	if (!urlPath.startsWith(WTF::String(path_.c_str()), TextCaseSensitive))
		return false;

	// |url_path| is >= |path_|, and |path_| is a prefix of |url_path|.  If they
	// are the are the same length then they are identical, otherwise need an
	// additional check:

	// In order to avoid in correctly matching a cookie path of /blah
	// with a request path of '/blahblah/', we need to make sure that either
	// the cookie path ends in a trailing '/', or that we prefix up to a '/'
	// in the url path.  Since we know that the url path length is greater
	// than the cookie path length, it's safe to index one byte past.
	if (path_.length() != url_path.length() && path_.at(path_.size() - 1) != '/' && url_path[path_.length()] != '/') {
		return false;
	}

	return true;
}

bool CanonicalCookie::IsDomainMatch(const std::string& host) const {
	// Can domain match in two ways; as a domain cookie (where the cookie
	// domain begins with ".") or as a host cookie (where it doesn't).

	// Some consumers of the CookieMonster expect to set cookies on
	// URLs like http://.strange.url.  To retrieve cookies in this instance,
	// we allow matching as a host cookie even when the domain_ starts with
	// a period.
	if (host == domain_)
		return true;

	// Domain cookie must have an initial ".".  To match, it must be
	// equal to url's host with initial period removed, or a suffix of
	// it.

	// Arguably this should only apply to "http" or "https" cookies, but
	// extension cookie tests currently use the funtionality, and if we
	// ever decide to implement that it should be done by preventing
	// such cookies from being set.
	if (domain_.empty() || domain_[0] != '.')
		return false;

	// The host with a "." prefixed.
	if (domain_.compare(1, std::string::npos, host) == 0)
		return true;

	// A pure suffix of the host (ok since we know the domain already
	// starts with a ".")
	return (host.length() > domain_.length() && host.compare(host.length() - domain_.length(), domain_.length(), domain_) == 0);
}

bool CanonicalCookie::IncludeForRequestURL(const blink::KURL& url, const CookieOptions& options) const {
	// Filter out HttpOnly cookies, per options.
	if (options.exclude_httponly() && IsHttpOnly())
		return false;
	// Secure cookies should not be included in requests for URLs with an
	// insecure scheme.
	if (IsSecure() && !(url.protocolIs("https") || url.protocolIs("wss")))
		return false;
	// Don't include cookies for requests that don't apply to the cookie domain.
	if (!IsDomainMatch(WTF::WTFStringToStdString(url.host())))
		return false;
	// Don't include cookies for requests with a url path that does not path
	// match the cookie-path.
	if (!IsOnPath(WTF::WTFStringToStdString(url.path())))
		return false;

	return true;
}

bool CanonicalCookie::PartialCompare(const CanonicalCookie& other) const {
	return PartialCookieOrdering(*this, other) < 0;
}

bool CanonicalCookie::FullCompare(const CanonicalCookie& other) const {
	// Do the partial comparison first.
	int diff = PartialCookieOrdering(*this, other);
	if (diff != 0)
		return diff < 0;

	DCHECK(IsEquivalent(other));

	// Compare other fields.
	diff = Value().compare(other.Value());
	if (diff != 0)
		return diff < 0;

	if (CreationDate() != other.CreationDate())
		return CreationDate() < other.CreationDate();

	if (ExpiryDate() != other.ExpiryDate())
		return ExpiryDate() < other.ExpiryDate();

	if (LastAccessDate() != other.LastAccessDate())
		return LastAccessDate() < other.LastAccessDate();

	if (IsSecure() != other.IsSecure())
		return IsSecure();

	if (IsHttpOnly() != other.IsHttpOnly())
		return IsHttpOnly();

	return Priority() < other.Priority();
}

// static
std::string CanonicalCookie::CanonPath(const blink::KURL& url, const ParsedCookie& pc) {
	std::string path_string;
	if (pc.HasPath())
		path_string = pc.Path();
	return CanonPathWithString(url, path_string);
}


// static
double CanonicalCookie::CanonExpiration(const ParsedCookie& pc, const double& current, const double& server_time) {
	// First, try the Max-Age attribute.
	uint64_t max_age = 0;
	if (pc.HasMaxAge() &&
#if COMPILER_MSVC && USING_VC6RT != 1
		sscanf_s(
#else
		sscanf(
#endif
			pc.MaxAge().c_str(), " %llu", &max_age) == 1) {
		return current + (max_age)/1000.0;
	}

	// Try the Expires attribute.
	if (pc.HasExpires() && !pc.Expires().empty()) {
		// Adjust for clock skew between server and host.
// 		base::Time parsed_expiry = cookie_util::ParseCookieTime(pc.Expires());
// 		if (!parsed_expiry.is_null())
// 			return parsed_expiry + (current - server_time);
		return 0;
	}

	// Invalid or no expiration, persistent cookie.
	return 0;
}

// static
CanonicalCookie::CookiePrefix CanonicalCookie::GetCookiePrefix(
	const std::string& name) {
	const char kSecurePrefix[] = "__Secure-";
	const char kHostPrefix[] = "__Host-";
	if (name.find(kSecurePrefix) == 0)
		return CanonicalCookie::COOKIE_PREFIX_SECURE;
	if (name.find(kHostPrefix) == 0)
		return CanonicalCookie::COOKIE_PREFIX_HOST;
	return CanonicalCookie::COOKIE_PREFIX_NONE;
}

// Returns true if the cookie does not violate any constraints imposed
// by the cookie name's prefix, as described in
// https://tools.ietf.org/html/draft-west-cookie-prefixes
//
// static
bool CanonicalCookie::IsCookiePrefixValid(CanonicalCookie::CookiePrefix prefix,
	const blink::KURL& url,
	const ParsedCookie& parsed_cookie) {
	if (prefix == CanonicalCookie::COOKIE_PREFIX_SECURE)
		return parsed_cookie.IsSecure() && (url.protocolIs("https") || url.protocolIs("wss"));
	if (prefix == CanonicalCookie::COOKIE_PREFIX_HOST) {
		return parsed_cookie.IsSecure() && (url.protocolIs("https") || url.protocolIs("wss")) && !parsed_cookie.HasDomain() && parsed_cookie.Path() == "/";
	}
	return true;
}

}  // namespace net
