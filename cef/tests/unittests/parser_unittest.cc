// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/cef_parser.h"
#include "testing/gtest/include/gtest/gtest.h"

// Create the URL using the spec.
TEST(ParserTest, CreateURLSpec) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.spec).FromASCII(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2");
  EXPECT_TRUE(CefCreateURL(parts, url));
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2",
      url.ToString().c_str());
}

// Test that host is required.
TEST(ParserTest, CreateURLHostRequired) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.scheme).FromASCII("http");
  EXPECT_FALSE(CefCreateURL(parts, url));
}

// Test that scheme is required.
TEST(ParserTest, CreateURLSchemeRequired) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.host).FromASCII("www.example.com");
  EXPECT_FALSE(CefCreateURL(parts, url));
}

// Create the URL using scheme and host.
TEST(ParserTest, CreateURLSchemeHost) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.scheme).FromASCII("http");
  CefString(&parts.host).FromASCII("www.example.com");
  EXPECT_TRUE(CefCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/", url.ToString().c_str());
}

// Create the URL using scheme, host and path.
TEST(ParserTest, CreateURLSchemeHostPath) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.scheme).FromASCII("http");
  CefString(&parts.host).FromASCII("www.example.com");
  CefString(&parts.path).FromASCII("/path/to.html");
  EXPECT_TRUE(CefCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/path/to.html", url.ToString().c_str());
}

// Create the URL using scheme, host, path and query.
TEST(ParserTest, CreateURLSchemeHostPathQuery) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.scheme).FromASCII("http");
  CefString(&parts.host).FromASCII("www.example.com");
  CefString(&parts.path).FromASCII("/path/to.html");
  CefString(&parts.query).FromASCII("foo=test&bar=test2");
  EXPECT_TRUE(CefCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2",
                url.ToString().c_str());
}

// Create the URL using all the various components.
TEST(ParserTest, CreateURLAll) {
  CefURLParts parts;
  CefString url;
  CefString(&parts.scheme).FromASCII("http");
  CefString(&parts.username).FromASCII("user");
  CefString(&parts.password).FromASCII("pass");
  CefString(&parts.host).FromASCII("www.example.com");
  CefString(&parts.port).FromASCII("88");
  CefString(&parts.path).FromASCII("/path/to.html");
  CefString(&parts.query).FromASCII("foo=test&bar=test2");
  EXPECT_TRUE(CefCreateURL(parts, url));
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2",
      url.ToString().c_str());
}

// Parse the URL using scheme and host.
TEST(ParserTest, ParseURLSchemeHost) {
  CefURLParts parts;
  CefString url;
  url.FromASCII("http://www.example.com");
  EXPECT_TRUE(CefParseURL(url, parts));

  CefString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/", spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  CefString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  CefString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  CefString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  CefString path(&parts.path);
  EXPECT_STREQ("/", path.ToString().c_str());
  EXPECT_EQ(0U, parts.query.length);
}

// Parse the URL using scheme, host and path.
TEST(ParserTest, ParseURLSchemeHostPath) {
  CefURLParts parts;
  CefString url;
  url.FromASCII("http://www.example.com/path/to.html");
  EXPECT_TRUE(CefParseURL(url, parts));

  CefString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/path/to.html",
                spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  CefString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  CefString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  CefString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  CefString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  EXPECT_EQ(0U, parts.query.length);
}

// Parse the URL using scheme, host, path and query.
TEST(ParserTest, ParseURLSchemeHostPathQuery) {
  CefURLParts parts;
  CefString url;
  url.FromASCII("http://www.example.com/path/to.html?foo=test&bar=test2");
  EXPECT_TRUE(CefParseURL(url, parts));

  CefString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2",
                spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  CefString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  CefString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  CefString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  CefString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  CefString query(&parts.query);
  EXPECT_STREQ("foo=test&bar=test2", query.ToString().c_str());
}

// Parse the URL using all the various components.
TEST(ParserTest, ParseURLAll) {
  CefURLParts parts;
  CefString url;
  url.FromASCII(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2");
  EXPECT_TRUE(CefParseURL(url, parts));

  CefString spec(&parts.spec);
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2",
      spec.ToString().c_str());
  CefString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  CefString username(&parts.username);
  EXPECT_STREQ("user", username.ToString().c_str());
  CefString password(&parts.password);
  EXPECT_STREQ("pass", password.ToString().c_str());
  CefString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  CefString port(&parts.port);
  EXPECT_STREQ("88", port.ToString().c_str());
  CefString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com:88/");
  CefString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  CefString query(&parts.query);
  EXPECT_STREQ("foo=test&bar=test2", query.ToString().c_str());
}

// Parse an invalid URL.
TEST(ParserTest, ParseURLInvalid) {
  CefURLParts parts;
  CefString url;
  url.FromASCII("www.example.com");
  EXPECT_FALSE(CefParseURL(url, parts));
}

// Parse a non-standard scheme.
TEST(ParserTest, ParseURLNonStandard) {
  CefURLParts parts;
  CefString url;
  url.FromASCII("custom:something%20else?foo");
  EXPECT_TRUE(CefParseURL(url, parts));

  CefString spec(&parts.spec);
  EXPECT_STREQ("custom:something%20else?foo", spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  CefString scheme(&parts.scheme);
  EXPECT_STREQ("custom", scheme.ToString().c_str());
  EXPECT_EQ(0U, parts.host.length);
  EXPECT_EQ(0U, parts.port.length);
  EXPECT_EQ(0U, parts.origin.length);
  CefString path(&parts.path);
  EXPECT_STREQ("something%20else", path.ToString().c_str());
  CefString query(&parts.query);
  EXPECT_STREQ("foo", query.ToString().c_str());
}

TEST(ParserTest, FormatUrlForSecurityDisplay) {
  CefString result;
  const char* kLang = "en-US";

  // Omits the protocol if it's standard.
  result = CefFormatUrlForSecurityDisplay("http://tests.com/foo.html", kLang);
  EXPECT_STREQ("http://tests.com", result.ToString().c_str());

  // Omits the port if it's the expected value for the protocol.
  result = CefFormatUrlForSecurityDisplay("http://tests.com:80/foo.html",
                                          kLang);
  EXPECT_STREQ("http://tests.com", result.ToString().c_str());

  // Don't omit non-standard ports.
  result = CefFormatUrlForSecurityDisplay("http://tests.com:8088/foo.html",
                                          kLang);
  EXPECT_STREQ("http://tests.com:8088", result.ToString().c_str());

  // Don't omit the protocol for file URLs.
  result = CefFormatUrlForSecurityDisplay("file:///c/tests/foo.html", kLang);
  EXPECT_STREQ("file:///c/tests/foo.html", result.ToString().c_str());

  // Empty |languages| should be OK.
  result = CefFormatUrlForSecurityDisplay("http://tests.com/foo.html",
                                          CefString());
  EXPECT_STREQ("http://tests.com", result.ToString().c_str());
}

TEST(ParserTest, GetMimeType) {
  CefString mime_type;

  mime_type = CefGetMimeType("html");
  EXPECT_STREQ("text/html", mime_type.ToString().c_str());

  mime_type = CefGetMimeType("txt");
  EXPECT_STREQ("text/plain", mime_type.ToString().c_str());

  mime_type = CefGetMimeType("gif");
  EXPECT_STREQ("image/gif", mime_type.ToString().c_str());
}

TEST(ParserTest, Base64Encode) {
  const std::string& test_str_decoded = "A test string";
  const std::string& test_str_encoded = "QSB0ZXN0IHN0cmluZw==";
  const CefString& encoded_value =
      CefBase64Encode(test_str_decoded.data(), test_str_decoded.size());
  EXPECT_STREQ(test_str_encoded.c_str(), encoded_value.ToString().c_str());
}

TEST(ParserTest, Base64Decode) {
  const std::string& test_str_decoded = "A test string";
  const std::string& test_str_encoded = "QSB0ZXN0IHN0cmluZw==";
  CefRefPtr<CefBinaryValue> decoded_value = CefBase64Decode(test_str_encoded);
  EXPECT_TRUE(decoded_value.get());

  const size_t decoded_size = decoded_value->GetSize();
  EXPECT_EQ(test_str_decoded.size(), decoded_size);

  std::string decoded_str;
  decoded_str.resize(decoded_size + 1);  // Include space for NUL-terminator.
  const size_t get_data_result =
      decoded_value->GetData(const_cast<char*>(decoded_str.data()),
                             decoded_size, 0);
  EXPECT_EQ(decoded_size, get_data_result);
  EXPECT_STREQ(test_str_decoded.c_str(), decoded_str.c_str());
}

TEST(ParserTest, URIEncode) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A%20test%20string%3D";
  const CefString& encoded_value = CefURIEncode(test_str_decoded, false);
  EXPECT_STREQ(test_str_encoded.c_str(), encoded_value.ToString().c_str());
}

TEST(ParserTest, URIDecode) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A%20test%20string%3D";
  const CefString& decoded_value =
      CefURIDecode(test_str_encoded, false,
                   static_cast<cef_uri_unescape_rule_t>(
                      UU_SPACES | UU_URL_SPECIAL_CHARS));
  EXPECT_STREQ(test_str_decoded.c_str(), decoded_value.ToString().c_str());
}

TEST(ParserTest, ParseCSSColor) {
  std::string value;
  cef_color_t color;

  // Color by name.
  value = "red";
  color = 0;
  EXPECT_TRUE(CefParseCSSColor(value, false, color));
  EXPECT_EQ(CefColorSetARGB(255, 255, 0, 0), color);

  // Color by RGB.
  value = "rgb(1,2,3)";
  color = 0;
  EXPECT_TRUE(CefParseCSSColor(value, false, color));
  EXPECT_EQ(CefColorSetARGB(255, 1, 2, 3), color);

  // Color by RGBA.
  value = "rgba(1,2,3,0.0)";
  color = 0;
  EXPECT_TRUE(CefParseCSSColor(value, false, color));
  EXPECT_EQ(CefColorSetARGB(0, 1, 2, 3), color);

  // Color by hex code.
  value = "#FFAACC";
  color = 0;
  EXPECT_TRUE(CefParseCSSColor(value, false, color));
  EXPECT_EQ(CefColorSetARGB(255, 0xFF, 0xAA, 0xCC), color);

  // Invalid color.
  value = "not_a_color";
  color = 0;
  EXPECT_FALSE(CefParseCSSColor(value, false, color));
  EXPECT_EQ(0U, color);
}


TEST(ParserTest, ParseJSONInvalid) {
  const char data[] = "This is my test data";
  CefRefPtr<CefValue> value = CefParseJSON(data, JSON_PARSER_RFC);
  EXPECT_FALSE(value.get());
}

TEST(ParserTest, ParseJSONNull) {
  const char data[] = "{\"key1\":null}";
  CefRefPtr<CefValue> value = CefParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value->GetType() == VTYPE_DICTIONARY);
  EXPECT_FALSE(value->IsOwned());
  CefRefPtr<CefDictionaryValue> dict = value->GetDictionary();
  CefDictionaryValue::KeyList key_list;
  EXPECT_TRUE(dict->GetKeys(key_list));
  EXPECT_EQ((size_t)1, key_list.size());
  EXPECT_EQ("key1", key_list[0].ToString());
  EXPECT_EQ(VTYPE_NULL, dict->GetType("key1"));

  // generate string from parsed result
  CefString result = CefWriteJSON(value, JSON_WRITER_DEFAULT);
  CefString expected_result = data;
  EXPECT_EQ(expected_result, result);
}

TEST(ParserTest, WriteJSONBinary) {
  const char data[] = "\00\01\02";
  CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
  CefRefPtr<CefBinaryValue> binary = CefBinaryValue::Create(data, sizeof(data));
  dict->SetBinary("key1", binary);
  CefRefPtr<CefValue> node = CefValue::Create();
  node->SetDictionary(dict);
  CefString result = CefWriteJSON(node, JSON_WRITER_DEFAULT);
  CefString expect_result = "";
  // binary data will be omitted.
  EXPECT_EQ(expect_result, result);
}

TEST(ParserTest, ParseJSONDictionary) {
  const char data[] = "{\"key1\":\"value1\",\"key2\":123,\"key3\":[1,2,3]}";
  CefRefPtr<CefValue> value = CefParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_TRUE(value->GetType() == VTYPE_DICTIONARY);
  CefRefPtr<CefDictionaryValue> dict = value->GetDictionary();
  CefDictionaryValue::KeyList key_list;
  EXPECT_TRUE(dict->GetKeys(key_list));
  EXPECT_EQ((size_t)3, key_list.size());
  EXPECT_EQ("key1", key_list[0].ToString());
  EXPECT_EQ("key2", key_list[1].ToString());
  EXPECT_EQ("key3", key_list[2].ToString());
  EXPECT_EQ(VTYPE_STRING, dict->GetType("key1"));
  EXPECT_EQ(dict->GetString("key1"), "value1");
  EXPECT_EQ(VTYPE_INT, dict->GetType("key2"));
  EXPECT_EQ(123, dict->GetInt("key2"));
  EXPECT_EQ(VTYPE_LIST, dict->GetType("key3"));
  CefRefPtr<CefListValue> key3 = dict->GetList("key3");
  EXPECT_TRUE(NULL != key3);
  EXPECT_TRUE(key3->IsValid());
  EXPECT_EQ((size_t)3, key3->GetSize());
  EXPECT_EQ(1, key3->GetInt(0));
  EXPECT_EQ(2, key3->GetInt(1));
  EXPECT_EQ(3, key3->GetInt(2));

  // generate string from parsed result
  CefString result = CefWriteJSON(value, JSON_WRITER_DEFAULT);
  CefString expected_result = data;
  EXPECT_EQ(expected_result, result);
}

TEST(ParserTest, ParseJSONList) {
  const char data[] = "[\"value1\", 123, {\"key3\": [1, 2, 3]}]";
  CefRefPtr<CefValue> value = CefParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value->GetType() == VTYPE_LIST);
  EXPECT_FALSE(value->IsOwned());
  CefRefPtr<CefListValue> list = value->GetList();
  EXPECT_TRUE(NULL != list);
  EXPECT_TRUE(list->IsValid());
  EXPECT_EQ((size_t)3, list->GetSize());

  EXPECT_EQ(VTYPE_STRING, list->GetType(0));
  EXPECT_EQ(list->GetString(0), "value1");
  EXPECT_EQ(VTYPE_INT, list->GetType(1));
  EXPECT_EQ(123, list->GetInt(1));
  EXPECT_EQ(VTYPE_DICTIONARY, list->GetType(2));
  CefRefPtr<CefDictionaryValue> dict = list->GetDictionary(2);
  CefDictionaryValue::KeyList key_list2;
  EXPECT_TRUE(dict->GetKeys(key_list2));
  EXPECT_EQ((size_t)1, key_list2.size());
  CefRefPtr<CefListValue> list2 = dict->GetList("key3");
  EXPECT_EQ((size_t)3, list2->GetSize());
  EXPECT_EQ(1, list2->GetInt(0));
  EXPECT_EQ(2, list2->GetInt(1));
  EXPECT_EQ(3, list2->GetInt(2));

  // generate string from parsed result
  CefString result = CefWriteJSON(value, JSON_WRITER_DEFAULT);
  CefString expected_result = "[\"value1\",123,{\"key3\":[1,2,3]}]";
  EXPECT_EQ(expected_result.ToString(), result.ToString());
}

TEST(ParserTest, ParseJSONAndReturnErrorInvalid) {
  const char data[] = "This is my test data";
  cef_json_parser_error_t error_code;
  CefString error_msg;
  CefRefPtr<CefValue> value = CefParseJSONAndReturnError(data,
      JSON_PARSER_RFC, error_code, error_msg);
  CefString expect_error_msg = "Line: 1, column: 1, Unexpected token.";
  EXPECT_FALSE(value.get());
  EXPECT_EQ(JSON_UNEXPECTED_TOKEN, error_code);
  EXPECT_EQ(expect_error_msg, error_msg);
}

TEST(ParserTest, ParseJSONAndReturnErrorTrailingComma) {
  const char data[] = "{\"key1\":123,}";
  cef_json_parser_error_t error_code;
  CefString error_msg;
  CefRefPtr<CefValue> value = CefParseJSONAndReturnError(data,
      JSON_PARSER_RFC, error_code, error_msg);
  CefString expect_error_msg =
      "Line: 1, column: 13, Trailing comma not allowed.";
  EXPECT_FALSE(value.get());
  EXPECT_EQ(JSON_TRAILING_COMMA, error_code);
  EXPECT_EQ(expect_error_msg, error_msg);
}
