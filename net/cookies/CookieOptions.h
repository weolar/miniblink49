// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Brought to you by number 42.

#ifndef NET_COOKIES_COOKIE_OPTIONS_H_
#define NET_COOKIES_COOKIE_OPTIONS_H_

namespace net {

class CookieOptions {
 public:
  // Creates a CookieOptions object which:
  //
  // * Excludes HttpOnly cookies
  // * Excludes SameSite cookies
  // * Does not enforce prefix restrictions (e.g. "$Secure-*")
  // * Updates last-accessed time.
  //
  // These settings can be altered by calling:
  //
  // * |set_{include,exclude}_httponly()|
  // * |set_include_same_site()|
  // * |set_enforce_prefixes()|
  // * |set_do_not_update_access_time()|
  CookieOptions()
	  : exclude_httponly_(true),
	  include_same_site_(false),
	  enforce_strict_secure_(false),
	  update_access_time_(true),
	  server_time_(0) {}

  void set_exclude_httponly() { exclude_httponly_ = true; }
  void set_include_httponly() { exclude_httponly_ = false; }
  bool exclude_httponly() const { return exclude_httponly_; }

  // Default is to exclude 'same_site' cookies.
  void set_include_same_site() { include_same_site_ = true; }
  bool include_same_site() const { return include_same_site_; }

  // TODO(jww): Remove once we decide whether to ship modifying 'secure' cookies
  // only from secure schemes. https://crbug.com/546820
  void set_enforce_strict_secure() { enforce_strict_secure_ = true; }
  bool enforce_strict_secure() const { return enforce_strict_secure_; }

  // |server_time| indicates what the server sending us the Cookie thought the
  // current time was when the cookie was produced.  This is used to adjust for
  // clock skew between server and host.
  void set_server_time(const double& server_time) {
    server_time_ = server_time;
  }
  bool has_server_time() const { return server_time_ != 0; }
  double server_time() const { return server_time_; }

  void set_do_not_update_access_time() { update_access_time_ = false; }
  bool update_access_time() const { return update_access_time_; }

 private:
  bool exclude_httponly_;
  bool include_same_site_;
  bool enforce_strict_secure_;
  bool update_access_time_;
  double server_time_;
};

}  // namespace net

#endif  // NET_COOKIES_COOKIE_OPTIONS_H_
