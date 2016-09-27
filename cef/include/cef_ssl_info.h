// Copyright (c) 2015 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the CEF translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef CEF_INCLUDE_CEF_SSL_INFO_H_
#define CEF_INCLUDE_CEF_SSL_INFO_H_
#pragma once

#include <vector>

#include "include/cef_base.h"
#include "include/cef_values.h"

///
// Class representing the issuer or subject field of an X.509 certificate.
///
/*--cef(source=library)--*/
class CefSSLCertPrincipal : public virtual CefBase {
 public:
  ///
  // Returns a name that can be used to represent the issuer.  It tries in this
  // order: CN, O and OU and returns the first non-empty one found.
  ///
  /*--cef()--*/
  virtual CefString GetDisplayName() =0;

  ///
  // Returns the common name.
  ///
  /*--cef()--*/
  virtual CefString GetCommonName() =0;

  ///
  // Returns the locality name.
  ///
  /*--cef()--*/
  virtual CefString GetLocalityName() =0;

  ///
  // Returns the state or province name.
  ///
  /*--cef()--*/
  virtual CefString GetStateOrProvinceName() =0;

  ///
  // Returns the country name.
  ///
  /*--cef()--*/
  virtual CefString GetCountryName() =0;

  ///
  // Retrieve the list of street addresses.
  ///
  /*--cef()--*/
  virtual void GetStreetAddresses(std::vector<CefString>& addresses) =0;

  ///
  // Retrieve the list of organization names.
  ///
  /*--cef()--*/
  virtual void GetOrganizationNames(std::vector<CefString>& names) =0;

  ///
  // Retrieve the list of organization unit names.
  ///
  /*--cef()--*/
  virtual void GetOrganizationUnitNames(std::vector<CefString>& names) =0;

  ///
  // Retrieve the list of domain components.
  ///
  /*--cef()--*/
  virtual void GetDomainComponents(std::vector<CefString>& components) =0;
};

///
// Class representing SSL information.
///
/*--cef(source=library)--*/
class CefSSLInfo : public virtual CefBase {
 public:
  typedef std::vector<CefRefPtr<CefBinaryValue> > IssuerChainBinaryList;

  ///
  // Returns a bitmask containing any and all problems verifying the server
  // certificate.
  ///
  /*--cef(default_retval=CERT_STATUS_NONE)--*/
  virtual cef_cert_status_t GetCertStatus() =0;

  ///
  // Returns true if the certificate status has any error, major or minor.
  ///
  /*--cef()--*/
  virtual bool IsCertStatusError() =0;

  ///
  // Returns true if the certificate status represents only minor errors
  // (e.g. failure to verify certificate revocation).
  ///
  /*--cef()--*/
  virtual bool IsCertStatusMinorError() =0;

  ///
  // Returns the subject of the X.509 certificate. For HTTPS server
  // certificates this represents the web server.  The common name of the
  // subject should match the host name of the web server.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefSSLCertPrincipal> GetSubject() =0;

  ///
  // Returns the issuer of the X.509 certificate.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefSSLCertPrincipal> GetIssuer() =0;

  ///
  // Returns the DER encoded serial number for the X.509 certificate. The value
  // possibly includes a leading 00 byte.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefBinaryValue> GetSerialNumber() =0;
  
  ///
  // Returns the date before which the X.509 certificate is invalid.
  // CefTime.GetTimeT() will return 0 if no date was specified.
  ///
  /*--cef()--*/
  virtual CefTime GetValidStart() =0;

  ///
  // Returns the date after which the X.509 certificate is invalid.
  // CefTime.GetTimeT() will return 0 if no date was specified.
  ///
  /*--cef()--*/
  virtual CefTime GetValidExpiry() =0;

  ///
  // Returns the DER encoded data for the X.509 certificate.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefBinaryValue> GetDEREncoded() =0;

  ///
  // Returns the PEM encoded data for the X.509 certificate.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefBinaryValue> GetPEMEncoded() =0;

  ///
  // Returns the number of certificates in the issuer chain.
  // If 0, the certificate is self-signed.
  ///
  /*--cef()--*/
  virtual size_t GetIssuerChainSize() =0;

  ///
  // Returns the DER encoded data for the certificate issuer chain.
  // If we failed to encode a certificate in the chain it is still
  // present in the array but is an empty string.
  ///
  /*--cef(count_func=chain:GetIssuerChainSize)--*/
  virtual void GetDEREncodedIssuerChain(IssuerChainBinaryList& chain) =0;

  ///
  // Returns the PEM encoded data for the certificate issuer chain.
  // If we failed to encode a certificate in the chain it is still
  // present in the array but is an empty string.
  ///
  /*--cef(count_func=chain:GetIssuerChainSize)--*/
  virtual void GetPEMEncodedIssuerChain(IssuerChainBinaryList& chain) =0;
};

#endif  // CEF_INCLUDE_CEF_SSL_INFO_H_
