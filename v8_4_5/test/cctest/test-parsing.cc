// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/v8.h"

#include "src/ast.h"
#include "src/ast-numbering.h"
#include "src/ast-value-factory.h"
#include "src/compiler.h"
#include "src/execution.h"
#include "src/isolate.h"
#include "src/objects.h"
#include "src/parser.h"
#include "src/preparser.h"
#include "src/rewriter.h"
#include "src/scanner-character-streams.h"
#include "src/token.h"
#include "src/utils.h"

#include "test/cctest/cctest.h"

TEST(ScanKeywords) {
  struct KeywordToken {
    const char* keyword;
    i::Token::Value token;
  };

  static const KeywordToken keywords[] = {
#define KEYWORD(t, s, d) { s, i::Token::t },
      TOKEN_LIST(IGNORE_TOKEN, KEYWORD)
#undef KEYWORD
      { NULL, i::Token::IDENTIFIER }
  };

  KeywordToken key_token;
  i::UnicodeCache unicode_cache;
  i::byte buffer[32];
  for (int i = 0; (key_token = keywords[i]).keyword != NULL; i++) {
    const i::byte* keyword =
        reinterpret_cast<const i::byte*>(key_token.keyword);
    int length = i::StrLength(key_token.keyword);
    CHECK(static_cast<int>(sizeof(buffer)) >= length);
    {
      i::Utf8ToUtf16CharacterStream stream(keyword, length);
      i::Scanner scanner(&unicode_cache);
      // The scanner should parse Harmony keywords for this test.
      scanner.SetHarmonyModules(true);
      scanner.Initialize(&stream);
      CHECK_EQ(key_token.token, scanner.Next());
      CHECK_EQ(i::Token::EOS, scanner.Next());
    }
    // Removing characters will make keyword matching fail.
    {
      i::Utf8ToUtf16CharacterStream stream(keyword, length - 1);
      i::Scanner scanner(&unicode_cache);
      scanner.Initialize(&stream);
      CHECK_EQ(i::Token::IDENTIFIER, scanner.Next());
      CHECK_EQ(i::Token::EOS, scanner.Next());
    }
    // Adding characters will make keyword matching fail.
    static const char chars_to_append[] = { 'z', '0', '_' };
    for (int j = 0; j < static_cast<int>(arraysize(chars_to_append)); ++j) {
      i::MemMove(buffer, keyword, length);
      buffer[length] = chars_to_append[j];
      i::Utf8ToUtf16CharacterStream stream(buffer, length + 1);
      i::Scanner scanner(&unicode_cache);
      scanner.Initialize(&stream);
      CHECK_EQ(i::Token::IDENTIFIER, scanner.Next());
      CHECK_EQ(i::Token::EOS, scanner.Next());
    }
    // Replacing characters will make keyword matching fail.
    {
      i::MemMove(buffer, keyword, length);
      buffer[length - 1] = '_';
      i::Utf8ToUtf16CharacterStream stream(buffer, length);
      i::Scanner scanner(&unicode_cache);
      scanner.Initialize(&stream);
      CHECK_EQ(i::Token::IDENTIFIER, scanner.Next());
      CHECK_EQ(i::Token::EOS, scanner.Next());
    }
  }
}


TEST(ScanHTMLEndComments) {
  v8::V8::Initialize();
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);

  // Regression test. See:
  //    http://code.google.com/p/chromium/issues/detail?id=53548
  // Tests that --> is correctly interpreted as comment-to-end-of-line if there
  // is only whitespace before it on the line (with comments considered as
  // whitespace, even a multiline-comment containing a newline).
  // This was not the case if it occurred before the first real token
  // in the input.
  const char* tests[] = {
      // Before first real token.
      "--> is eol-comment\nvar y = 37;\n",
      "\n --> is eol-comment\nvar y = 37;\n",
      "/* precomment */ --> is eol-comment\nvar y = 37;\n",
      "\n/* precomment */ --> is eol-comment\nvar y = 37;\n",
      // After first real token.
      "var x = 42;\n--> is eol-comment\nvar y = 37;\n",
      "var x = 42;\n/* precomment */ --> is eol-comment\nvar y = 37;\n",
      NULL
  };

  const char* fail_tests[] = {
      "x --> is eol-comment\nvar y = 37;\n",
      "\"\\n\" --> is eol-comment\nvar y = 37;\n",
      "x/* precomment */ --> is eol-comment\nvar y = 37;\n",
      "x/* precomment\n */ --> is eol-comment\nvar y = 37;\n",
      "var x = 42; --> is eol-comment\nvar y = 37;\n",
      "var x = 42; /* precomment\n */ --> is eol-comment\nvar y = 37;\n",
      NULL
  };

  // Parser/Scanner needs a stack limit.
  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);
  uintptr_t stack_limit = CcTest::i_isolate()->stack_guard()->real_climit();
  for (int i = 0; tests[i]; i++) {
    const i::byte* source =
        reinterpret_cast<const i::byte*>(tests[i]);
    i::Utf8ToUtf16CharacterStream stream(source, i::StrLength(tests[i]));
    i::CompleteParserRecorder log;
    i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
    scanner.Initialize(&stream);
    i::Zone zone;
    i::AstValueFactory ast_value_factory(
        &zone, CcTest::i_isolate()->heap()->HashSeed());
    i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                           stack_limit);
    preparser.set_allow_lazy(true);
    i::PreParser::PreParseResult result = preparser.PreParseProgram();
    CHECK_EQ(i::PreParser::kPreParseSuccess, result);
    CHECK(!log.HasError());
  }

  for (int i = 0; fail_tests[i]; i++) {
    const i::byte* source =
        reinterpret_cast<const i::byte*>(fail_tests[i]);
    i::Utf8ToUtf16CharacterStream stream(source, i::StrLength(fail_tests[i]));
    i::CompleteParserRecorder log;
    i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
    scanner.Initialize(&stream);
    i::Zone zone;
    i::AstValueFactory ast_value_factory(
        &zone, CcTest::i_isolate()->heap()->HashSeed());
    i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                           stack_limit);
    preparser.set_allow_lazy(true);
    i::PreParser::PreParseResult result = preparser.PreParseProgram();
    // Even in the case of a syntax error, kPreParseSuccess is returned.
    CHECK_EQ(i::PreParser::kPreParseSuccess, result);
    CHECK(log.HasError());
  }
}


class ScriptResource : public v8::String::ExternalOneByteStringResource {
 public:
  ScriptResource(const char* data, size_t length)
      : data_(data), length_(length) { }

  const char* data() const { return data_; }
  size_t length() const { return length_; }

 private:
  const char* data_;
  size_t length_;
};


TEST(UsingCachedData) {
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);
  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  // Source containing functions that might be lazily compiled  and all types
  // of symbols (string, propertyName, regexp).
  const char* source =
      "var x = 42;"
      "function foo(a) { return function nolazy(b) { return a + b; } }"
      "function bar(a) { if (a) return function lazy(b) { return b; } }"
      "var z = {'string': 'string literal', bareword: 'propertyName', "
      "         42: 'number literal', for: 'keyword as propertyName', "
      "         f\\u006fr: 'keyword propertyname with escape'};"
      "var v = /RegExp Literal/;"
      "var w = /RegExp Literal\\u0020With Escape/gin;"
      "var y = { get getter() { return 42; }, "
      "          set setter(v) { this.value = v; }};"
      "var f = a => function (b) { return a + b; };"
      "var g = a => b => a + b;";
  int source_length = i::StrLength(source);

  // ScriptResource will be deleted when the corresponding String is GCd.
  v8::ScriptCompiler::Source script_source(v8::String::NewExternal(
      isolate, new ScriptResource(source, source_length)));
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_min_preparse_length = 0;
  v8::ScriptCompiler::Compile(isolate, &script_source,
                              v8::ScriptCompiler::kProduceParserCache);
  CHECK(script_source.GetCachedData());

  // Compile the script again, using the cached data.
  bool lazy_flag = i::FLAG_lazy;
  i::FLAG_lazy = true;
  v8::ScriptCompiler::Compile(isolate, &script_source,
                              v8::ScriptCompiler::kConsumeParserCache);
  i::FLAG_lazy = false;
  v8::ScriptCompiler::CompileUnbound(isolate, &script_source,
                                     v8::ScriptCompiler::kConsumeParserCache);
  i::FLAG_lazy = lazy_flag;
}


TEST(PreparseFunctionDataIsUsed) {
  // This tests that we actually do use the function data generated by the
  // preparser.

  // Make preparsing work for short scripts.
  i::FLAG_min_preparse_length = 0;
  i::FLAG_harmony_arrow_functions = true;

  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);
  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  const char* good_code[] = {
      "function this_is_lazy() { var a; } function foo() { return 25; } foo();",
      "var this_is_lazy = () => { var a; }; var foo = () => 25; foo();",
  };

  // Insert a syntax error inside the lazy function.
  const char* bad_code[] = {
      "function this_is_lazy() { if (   } function foo() { return 25; } foo();",
      "var this_is_lazy = () => { if (   }; var foo = () => 25; foo();",
  };

  for (unsigned i = 0; i < arraysize(good_code); i++) {
    v8::ScriptCompiler::Source good_source(v8_str(good_code[i]));
    v8::ScriptCompiler::Compile(isolate, &good_source,
                                v8::ScriptCompiler::kProduceParserCache);

    const v8::ScriptCompiler::CachedData* cached_data =
        good_source.GetCachedData();
    CHECK(cached_data->data != NULL);
    CHECK_GT(cached_data->length, 0);

    // Now compile the erroneous code with the good preparse data. If the
    // preparse data is used, the lazy function is skipped and it should
    // compile fine.
    v8::ScriptCompiler::Source bad_source(
        v8_str(bad_code[i]), new v8::ScriptCompiler::CachedData(
                                 cached_data->data, cached_data->length));
    v8::Local<v8::Value> result =
        v8::ScriptCompiler::Compile(isolate, &bad_source,
                                    v8::ScriptCompiler::kConsumeParserCache)
            ->Run();
    CHECK(result->IsInt32());
    CHECK_EQ(25, result->Int32Value());
  }
}


TEST(StandAlonePreParser) {
  v8::V8::Initialize();

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  const char* programs[] = {
      "{label: 42}",
      "var x = 42;",
      "function foo(x, y) { return x + y; }",
      "%ArgleBargle(glop);",
      "var x = new new Function('this.x = 42');",
      "var f = (x, y) => x + y;",
      NULL
  };

  uintptr_t stack_limit = CcTest::i_isolate()->stack_guard()->real_climit();
  for (int i = 0; programs[i]; i++) {
    const char* program = programs[i];
    i::Utf8ToUtf16CharacterStream stream(
        reinterpret_cast<const i::byte*>(program),
        static_cast<unsigned>(strlen(program)));
    i::CompleteParserRecorder log;
    i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
    scanner.Initialize(&stream);

    i::Zone zone;
    i::AstValueFactory ast_value_factory(
        &zone, CcTest::i_isolate()->heap()->HashSeed());
    i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                           stack_limit);
    preparser.set_allow_lazy(true);
    preparser.set_allow_natives(true);
    preparser.set_allow_harmony_arrow_functions(true);
    i::PreParser::PreParseResult result = preparser.PreParseProgram();
    CHECK_EQ(i::PreParser::kPreParseSuccess, result);
    CHECK(!log.HasError());
  }
}


TEST(StandAlonePreParserNoNatives) {
  v8::V8::Initialize();

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  const char* programs[] = {
      "%ArgleBargle(glop);",
      "var x = %_IsSmi(42);",
      NULL
  };

  uintptr_t stack_limit = CcTest::i_isolate()->stack_guard()->real_climit();
  for (int i = 0; programs[i]; i++) {
    const char* program = programs[i];
    i::Utf8ToUtf16CharacterStream stream(
        reinterpret_cast<const i::byte*>(program),
        static_cast<unsigned>(strlen(program)));
    i::CompleteParserRecorder log;
    i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
    scanner.Initialize(&stream);

    // Preparser defaults to disallowing natives syntax.
    i::Zone zone;
    i::AstValueFactory ast_value_factory(
        &zone, CcTest::i_isolate()->heap()->HashSeed());
    i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                           stack_limit);
    preparser.set_allow_lazy(true);
    i::PreParser::PreParseResult result = preparser.PreParseProgram();
    CHECK_EQ(i::PreParser::kPreParseSuccess, result);
    CHECK(log.HasError());
  }
}


TEST(PreparsingObjectLiterals) {
  // Regression test for a bug where the symbol stream produced by PreParser
  // didn't match what Parser wanted to consume.
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);
  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  {
    const char* source = "var myo = {if: \"foo\"}; myo.if;";
    v8::Local<v8::Value> result = ParserCacheCompileRun(source);
    CHECK(result->IsString());
    v8::String::Utf8Value utf8(result);
    CHECK_EQ(0, strcmp("foo", *utf8));
  }

  {
    const char* source = "var myo = {\"bar\": \"foo\"}; myo[\"bar\"];";
    v8::Local<v8::Value> result = ParserCacheCompileRun(source);
    CHECK(result->IsString());
    v8::String::Utf8Value utf8(result);
    CHECK_EQ(0, strcmp("foo", *utf8));
  }

  {
    const char* source = "var myo = {1: \"foo\"}; myo[1];";
    v8::Local<v8::Value> result = ParserCacheCompileRun(source);
    CHECK(result->IsString());
    v8::String::Utf8Value utf8(result);
    CHECK_EQ(0, strcmp("foo", *utf8));
  }
}


TEST(RegressChromium62639) {
  v8::V8::Initialize();
  i::Isolate* isolate = CcTest::i_isolate();

  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  const char* program = "var x = 'something';\n"
                        "escape: function() {}";
  // Fails parsing expecting an identifier after "function".
  // Before fix, didn't check *ok after Expect(Token::Identifier, ok),
  // and then used the invalid currently scanned literal. This always
  // failed in debug mode, and sometimes crashed in release mode.

  i::Utf8ToUtf16CharacterStream stream(
      reinterpret_cast<const i::byte*>(program),
      static_cast<unsigned>(strlen(program)));
  i::CompleteParserRecorder log;
  i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
  scanner.Initialize(&stream);
  i::Zone zone;
  i::AstValueFactory ast_value_factory(&zone,
                                       CcTest::i_isolate()->heap()->HashSeed());
  i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                         CcTest::i_isolate()->stack_guard()->real_climit());
  preparser.set_allow_lazy(true);
  i::PreParser::PreParseResult result = preparser.PreParseProgram();
  // Even in the case of a syntax error, kPreParseSuccess is returned.
  CHECK_EQ(i::PreParser::kPreParseSuccess, result);
  CHECK(log.HasError());
}


TEST(Regress928) {
  v8::V8::Initialize();
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  // Preparsing didn't consider the catch clause of a try statement
  // as with-content, which made it assume that a function inside
  // the block could be lazily compiled, and an extra, unexpected,
  // entry was added to the data.
  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  const char* program =
      "try { } catch (e) { var foo = function () { /* first */ } }"
      "var bar = function () { /* second */ }";

  v8::HandleScope handles(CcTest::isolate());
  i::Handle<i::String> source = factory->NewStringFromAsciiChecked(program);
  i::GenericStringUtf16CharacterStream stream(source, 0, source->length());
  i::CompleteParserRecorder log;
  i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
  scanner.Initialize(&stream);
  i::Zone zone;
  i::AstValueFactory ast_value_factory(&zone,
                                       CcTest::i_isolate()->heap()->HashSeed());
  i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                         CcTest::i_isolate()->stack_guard()->real_climit());
  preparser.set_allow_lazy(true);
  i::PreParser::PreParseResult result = preparser.PreParseProgram();
  CHECK_EQ(i::PreParser::kPreParseSuccess, result);
  i::ScriptData* sd = log.GetScriptData();
  i::ParseData* pd = i::ParseData::FromCachedData(sd);
  pd->Initialize();

  int first_function =
      static_cast<int>(strstr(program, "function") - program);
  int first_lbrace = first_function + i::StrLength("function () ");
  CHECK_EQ('{', program[first_lbrace]);
  i::FunctionEntry entry1 = pd->GetFunctionEntry(first_lbrace);
  CHECK(!entry1.is_valid());

  int second_function =
      static_cast<int>(strstr(program + first_lbrace, "function") - program);
  int second_lbrace =
      second_function + i::StrLength("function () ");
  CHECK_EQ('{', program[second_lbrace]);
  i::FunctionEntry entry2 = pd->GetFunctionEntry(second_lbrace);
  CHECK(entry2.is_valid());
  CHECK_EQ('}', program[entry2.end_pos() - 1]);
  delete sd;
  delete pd;
}


TEST(PreParseOverflow) {
  v8::V8::Initialize();

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  size_t kProgramSize = 1024 * 1024;
  i::SmartArrayPointer<char> program(i::NewArray<char>(kProgramSize + 1));
  memset(program.get(), '(', kProgramSize);
  program[kProgramSize] = '\0';

  uintptr_t stack_limit = CcTest::i_isolate()->stack_guard()->real_climit();

  i::Utf8ToUtf16CharacterStream stream(
      reinterpret_cast<const i::byte*>(program.get()),
      static_cast<unsigned>(kProgramSize));
  i::CompleteParserRecorder log;
  i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
  scanner.Initialize(&stream);

  i::Zone zone;
  i::AstValueFactory ast_value_factory(&zone,
                                       CcTest::i_isolate()->heap()->HashSeed());
  i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                         stack_limit);
  preparser.set_allow_lazy(true);
  preparser.set_allow_harmony_arrow_functions(true);
  i::PreParser::PreParseResult result = preparser.PreParseProgram();
  CHECK_EQ(i::PreParser::kPreParseStackOverflow, result);
}


class TestExternalResource: public v8::String::ExternalStringResource {
 public:
  explicit TestExternalResource(uint16_t* data, int length)
      : data_(data), length_(static_cast<size_t>(length)) { }

  ~TestExternalResource() { }

  const uint16_t* data() const {
    return data_;
  }

  size_t length() const {
    return length_;
  }
 private:
  uint16_t* data_;
  size_t length_;
};


#define CHECK_EQU(v1, v2) CHECK_EQ(static_cast<int>(v1), static_cast<int>(v2))

void TestCharacterStream(const char* one_byte_source, unsigned length,
                         unsigned start = 0, unsigned end = 0) {
  if (end == 0) end = length;
  unsigned sub_length = end - start;
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  i::HandleScope test_scope(isolate);
  i::SmartArrayPointer<i::uc16> uc16_buffer(new i::uc16[length]);
  for (unsigned i = 0; i < length; i++) {
    uc16_buffer[i] = static_cast<i::uc16>(one_byte_source[i]);
  }
  i::Vector<const char> one_byte_vector(one_byte_source,
                                        static_cast<int>(length));
  i::Handle<i::String> one_byte_string =
      factory->NewStringFromAscii(one_byte_vector).ToHandleChecked();
  TestExternalResource resource(uc16_buffer.get(), length);
  i::Handle<i::String> uc16_string(
      factory->NewExternalStringFromTwoByte(&resource).ToHandleChecked());

  i::ExternalTwoByteStringUtf16CharacterStream uc16_stream(
      i::Handle<i::ExternalTwoByteString>::cast(uc16_string), start, end);
  i::GenericStringUtf16CharacterStream string_stream(one_byte_string, start,
                                                     end);
  i::Utf8ToUtf16CharacterStream utf8_stream(
      reinterpret_cast<const i::byte*>(one_byte_source), end);
  utf8_stream.SeekForward(start);

  unsigned i = start;
  while (i < end) {
    // Read streams one char at a time
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
    int32_t c0 = one_byte_source[i];
    int32_t c1 = uc16_stream.Advance();
    int32_t c2 = string_stream.Advance();
    int32_t c3 = utf8_stream.Advance();
    i++;
    CHECK_EQ(c0, c1);
    CHECK_EQ(c0, c2);
    CHECK_EQ(c0, c3);
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
  }
  while (i > start + sub_length / 4) {
    // Pushback, re-read, pushback again.
    int32_t c0 = one_byte_source[i - 1];
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
    uc16_stream.PushBack(c0);
    string_stream.PushBack(c0);
    utf8_stream.PushBack(c0);
    i--;
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
    int32_t c1 = uc16_stream.Advance();
    int32_t c2 = string_stream.Advance();
    int32_t c3 = utf8_stream.Advance();
    i++;
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
    CHECK_EQ(c0, c1);
    CHECK_EQ(c0, c2);
    CHECK_EQ(c0, c3);
    uc16_stream.PushBack(c0);
    string_stream.PushBack(c0);
    utf8_stream.PushBack(c0);
    i--;
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
  }
  unsigned halfway = start + sub_length / 2;
  uc16_stream.SeekForward(halfway - i);
  string_stream.SeekForward(halfway - i);
  utf8_stream.SeekForward(halfway - i);
  i = halfway;
  CHECK_EQU(i, uc16_stream.pos());
  CHECK_EQU(i, string_stream.pos());
  CHECK_EQU(i, utf8_stream.pos());

  while (i < end) {
    // Read streams one char at a time
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
    int32_t c0 = one_byte_source[i];
    int32_t c1 = uc16_stream.Advance();
    int32_t c2 = string_stream.Advance();
    int32_t c3 = utf8_stream.Advance();
    i++;
    CHECK_EQ(c0, c1);
    CHECK_EQ(c0, c2);
    CHECK_EQ(c0, c3);
    CHECK_EQU(i, uc16_stream.pos());
    CHECK_EQU(i, string_stream.pos());
    CHECK_EQU(i, utf8_stream.pos());
  }

  int32_t c1 = uc16_stream.Advance();
  int32_t c2 = string_stream.Advance();
  int32_t c3 = utf8_stream.Advance();
  CHECK_LT(c1, 0);
  CHECK_LT(c2, 0);
  CHECK_LT(c3, 0);
}


TEST(CharacterStreams) {
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);

  TestCharacterStream("abc\0\n\r\x7f", 7);
  static const unsigned kBigStringSize = 4096;
  char buffer[kBigStringSize + 1];
  for (unsigned i = 0; i < kBigStringSize; i++) {
    buffer[i] = static_cast<char>(i & 0x7f);
  }
  TestCharacterStream(buffer, kBigStringSize);

  TestCharacterStream(buffer, kBigStringSize, 576, 3298);

  TestCharacterStream("\0", 1);
  TestCharacterStream("", 0);
}


TEST(Utf8CharacterStream) {
  static const unsigned kMaxUC16CharU = unibrow::Utf8::kMaxThreeByteChar;
  static const int kMaxUC16Char = static_cast<int>(kMaxUC16CharU);

  static const int kAllUtf8CharsSize =
      (unibrow::Utf8::kMaxOneByteChar + 1) +
      (unibrow::Utf8::kMaxTwoByteChar - unibrow::Utf8::kMaxOneByteChar) * 2 +
      (unibrow::Utf8::kMaxThreeByteChar - unibrow::Utf8::kMaxTwoByteChar) * 3;
  static const unsigned kAllUtf8CharsSizeU =
      static_cast<unsigned>(kAllUtf8CharsSize);

  char buffer[kAllUtf8CharsSizeU];
  unsigned cursor = 0;
  for (int i = 0; i <= kMaxUC16Char; i++) {
    cursor += unibrow::Utf8::Encode(buffer + cursor, i,
                                    unibrow::Utf16::kNoPreviousCharacter, true);
  }
  DCHECK(cursor == kAllUtf8CharsSizeU);

  i::Utf8ToUtf16CharacterStream stream(reinterpret_cast<const i::byte*>(buffer),
                                       kAllUtf8CharsSizeU);
  int32_t bad = unibrow::Utf8::kBadChar;
  for (int i = 0; i <= kMaxUC16Char; i++) {
    CHECK_EQU(i, stream.pos());
    int32_t c = stream.Advance();
    if (i >= 0xd800 && i <= 0xdfff) {
      CHECK_EQ(bad, c);
    } else {
      CHECK_EQ(i, c);
    }
    CHECK_EQU(i + 1, stream.pos());
  }
  for (int i = kMaxUC16Char; i >= 0; i--) {
    CHECK_EQU(i + 1, stream.pos());
    stream.PushBack(i);
    CHECK_EQU(i, stream.pos());
  }
  int i = 0;
  while (stream.pos() < kMaxUC16CharU) {
    CHECK_EQU(i, stream.pos());
    int progress = static_cast<int>(stream.SeekForward(12));
    i += progress;
    int32_t c = stream.Advance();
    if (i >= 0xd800 && i <= 0xdfff) {
      CHECK_EQ(bad, c);
    } else if (i <= kMaxUC16Char) {
      CHECK_EQ(i, c);
    } else {
      CHECK_EQ(-1, c);
    }
    i += 1;
    CHECK_EQU(i, stream.pos());
  }
}

#undef CHECK_EQU

void TestStreamScanner(i::Utf16CharacterStream* stream,
                       i::Token::Value* expected_tokens,
                       int skip_pos = 0,  // Zero means not skipping.
                       int skip_to = 0) {
  i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
  scanner.Initialize(stream);

  int i = 0;
  do {
    i::Token::Value expected = expected_tokens[i];
    i::Token::Value actual = scanner.Next();
    CHECK_EQ(i::Token::String(expected), i::Token::String(actual));
    if (scanner.location().end_pos == skip_pos) {
      scanner.SeekForward(skip_to);
    }
    i++;
  } while (expected_tokens[i] != i::Token::ILLEGAL);
}


TEST(StreamScanner) {
  v8::V8::Initialize();

  const char* str1 = "{ foo get for : */ <- \n\n /*foo*/ bib";
  i::Utf8ToUtf16CharacterStream stream1(reinterpret_cast<const i::byte*>(str1),
                                        static_cast<unsigned>(strlen(str1)));
  i::Token::Value expectations1[] = {
      i::Token::LBRACE,
      i::Token::IDENTIFIER,
      i::Token::IDENTIFIER,
      i::Token::FOR,
      i::Token::COLON,
      i::Token::MUL,
      i::Token::DIV,
      i::Token::LT,
      i::Token::SUB,
      i::Token::IDENTIFIER,
      i::Token::EOS,
      i::Token::ILLEGAL
  };
  TestStreamScanner(&stream1, expectations1, 0, 0);

  const char* str2 = "case default const {THIS\nPART\nSKIPPED} do";
  i::Utf8ToUtf16CharacterStream stream2(reinterpret_cast<const i::byte*>(str2),
                                        static_cast<unsigned>(strlen(str2)));
  i::Token::Value expectations2[] = {
      i::Token::CASE,
      i::Token::DEFAULT,
      i::Token::CONST,
      i::Token::LBRACE,
      // Skipped part here
      i::Token::RBRACE,
      i::Token::DO,
      i::Token::EOS,
      i::Token::ILLEGAL
  };
  DCHECK_EQ('{', str2[19]);
  DCHECK_EQ('}', str2[37]);
  TestStreamScanner(&stream2, expectations2, 20, 37);

  const char* str3 = "{}}}}";
  i::Token::Value expectations3[] = {
      i::Token::LBRACE,
      i::Token::RBRACE,
      i::Token::RBRACE,
      i::Token::RBRACE,
      i::Token::RBRACE,
      i::Token::EOS,
      i::Token::ILLEGAL
  };
  // Skip zero-four RBRACEs.
  for (int i = 0; i <= 4; i++) {
     expectations3[6 - i] = i::Token::ILLEGAL;
     expectations3[5 - i] = i::Token::EOS;
     i::Utf8ToUtf16CharacterStream stream3(
         reinterpret_cast<const i::byte*>(str3),
         static_cast<unsigned>(strlen(str3)));
     TestStreamScanner(&stream3, expectations3, 1, 1 + i);
  }
}


void TestScanRegExp(const char* re_source, const char* expected) {
  i::Utf8ToUtf16CharacterStream stream(
       reinterpret_cast<const i::byte*>(re_source),
       static_cast<unsigned>(strlen(re_source)));
  i::HandleScope scope(CcTest::i_isolate());
  i::Scanner scanner(CcTest::i_isolate()->unicode_cache());
  scanner.Initialize(&stream);

  i::Token::Value start = scanner.peek();
  CHECK(start == i::Token::DIV || start == i::Token::ASSIGN_DIV);
  CHECK(scanner.ScanRegExpPattern(start == i::Token::ASSIGN_DIV));
  scanner.Next();  // Current token is now the regexp literal.
  i::Zone zone;
  i::AstValueFactory ast_value_factory(&zone,
                                       CcTest::i_isolate()->heap()->HashSeed());
  ast_value_factory.Internalize(CcTest::i_isolate());
  i::Handle<i::String> val =
      scanner.CurrentSymbol(&ast_value_factory)->string();
  i::DisallowHeapAllocation no_alloc;
  i::String::FlatContent content = val->GetFlatContent();
  CHECK(content.IsOneByte());
  i::Vector<const uint8_t> actual = content.ToOneByteVector();
  for (int i = 0; i < actual.length(); i++) {
    CHECK_NE('\0', expected[i]);
    CHECK_EQ(expected[i], actual[i]);
  }
}


TEST(RegExpScanning) {
  v8::V8::Initialize();

  // RegExp token with added garbage at the end. The scanner should only
  // scan the RegExp until the terminating slash just before "flipperwald".
  TestScanRegExp("/b/flipperwald", "b");
  // Incomplete escape sequences doesn't hide the terminating slash.
  TestScanRegExp("/\\x/flipperwald", "\\x");
  TestScanRegExp("/\\u/flipperwald", "\\u");
  TestScanRegExp("/\\u1/flipperwald", "\\u1");
  TestScanRegExp("/\\u12/flipperwald", "\\u12");
  TestScanRegExp("/\\u123/flipperwald", "\\u123");
  TestScanRegExp("/\\c/flipperwald", "\\c");
  TestScanRegExp("/\\c//flipperwald", "\\c");
  // Slashes inside character classes are not terminating.
  TestScanRegExp("/[/]/flipperwald", "[/]");
  TestScanRegExp("/[\\s-/]/flipperwald", "[\\s-/]");
  // Incomplete escape sequences inside a character class doesn't hide
  // the end of the character class.
  TestScanRegExp("/[\\c/]/flipperwald", "[\\c/]");
  TestScanRegExp("/[\\c]/flipperwald", "[\\c]");
  TestScanRegExp("/[\\x]/flipperwald", "[\\x]");
  TestScanRegExp("/[\\x1]/flipperwald", "[\\x1]");
  TestScanRegExp("/[\\u]/flipperwald", "[\\u]");
  TestScanRegExp("/[\\u1]/flipperwald", "[\\u1]");
  TestScanRegExp("/[\\u12]/flipperwald", "[\\u12]");
  TestScanRegExp("/[\\u123]/flipperwald", "[\\u123]");
  // Escaped ']'s wont end the character class.
  TestScanRegExp("/[\\]/]/flipperwald", "[\\]/]");
  // Escaped slashes are not terminating.
  TestScanRegExp("/\\//flipperwald", "\\/");
  // Starting with '=' works too.
  TestScanRegExp("/=/", "=");
  TestScanRegExp("/=?/", "=?");
}


static int Utf8LengthHelper(const char* s) {
  int len = i::StrLength(s);
  int character_length = len;
  for (int i = 0; i < len; i++) {
    unsigned char c = s[i];
    int input_offset = 0;
    int output_adjust = 0;
    if (c > 0x7f) {
      if (c < 0xc0) continue;
      if (c >= 0xf0) {
        if (c >= 0xf8) {
          // 5 and 6 byte UTF-8 sequences turn into a kBadChar for each UTF-8
          // byte.
          continue;  // Handle first UTF-8 byte.
        }
        if ((c & 7) == 0 && ((s[i + 1] & 0x30) == 0)) {
          // This 4 byte sequence could have been coded as a 3 byte sequence.
          // Record a single kBadChar for the first byte and continue.
          continue;
        }
        input_offset = 3;
        // 4 bytes of UTF-8 turn into 2 UTF-16 code units.
        character_length -= 2;
      } else if (c >= 0xe0) {
        if ((c & 0xf) == 0 && ((s[i + 1] & 0x20) == 0)) {
          // This 3 byte sequence could have been coded as a 2 byte sequence.
          // Record a single kBadChar for the first byte and continue.
          continue;
        }
        if (c == 0xed) {
          unsigned char d = s[i + 1];
          if ((d < 0x80) || (d > 0x9f)) {
            // This 3 byte sequence is part of a surrogate pair which is not
            // supported by UTF-8. Record a single kBadChar for the first byte
            // and continue.
            continue;
          }
        }
        input_offset = 2;
        // 3 bytes of UTF-8 turn into 1 UTF-16 code unit.
        output_adjust = 2;
      } else {
        if ((c & 0x1e) == 0) {
          // This 2 byte sequence could have been coded as a 1 byte sequence.
          // Record a single kBadChar for the first byte and continue.
          continue;
        }
        input_offset = 1;
        // 2 bytes of UTF-8 turn into 1 UTF-16 code unit.
        output_adjust = 1;
      }
      bool bad = false;
      for (int j = 1; j <= input_offset; j++) {
        if ((s[i + j] & 0xc0) != 0x80) {
          // Bad UTF-8 sequence turns the first in the sequence into kBadChar,
          // which is a single UTF-16 code unit.
          bad = true;
          break;
        }
      }
      if (!bad) {
        i += input_offset;
        character_length -= output_adjust;
      }
    }
  }
  return character_length;
}


TEST(ScopeUsesArgumentsSuperThis) {
  static const struct {
    const char* prefix;
    const char* suffix;
  } surroundings[] = {
    { "function f() {", "}" },
    { "var f = () => {", "};" },
    { "class C { constructor() {", "} }" },
  };

  enum Expected {
    NONE = 0,
    ARGUMENTS = 1,
    SUPER_PROPERTY = 1 << 1,
    THIS = 1 << 2,
    INNER_ARGUMENTS = 1 << 3,
    EVAL = 1 << 4
  };

  // clang-format off
  static const struct {
    const char* body;
    int expected;
  } source_data[] = {
    {"", NONE},
    {"return this", THIS},
    {"return arguments", ARGUMENTS},
    {"return super.x", SUPER_PROPERTY},
    {"return arguments[0]", ARGUMENTS},
    {"return this + arguments[0]", ARGUMENTS | THIS},
    {"return this + arguments[0] + super.x",
     ARGUMENTS | SUPER_PROPERTY | THIS},
    {"return x => this + x", THIS},
    {"return x => super.f() + x", SUPER_PROPERTY},
    {"this.foo = 42;", THIS},
    {"this.foo();", THIS},
    {"if (foo()) { this.f() }", THIS},
    {"if (foo()) { super.f() }", SUPER_PROPERTY},
    {"if (arguments.length) { this.f() }", ARGUMENTS | THIS},
    {"while (true) { this.f() }", THIS},
    {"while (true) { super.f() }", SUPER_PROPERTY},
    {"if (true) { while (true) this.foo(arguments) }", ARGUMENTS | THIS},
    // Multiple nesting levels must work as well.
    {"while (true) { while (true) { while (true) return this } }", THIS},
    {"while (true) { while (true) { while (true) return super.f() } }",
     SUPER_PROPERTY},
    {"if (1) { return () => { while (true) new this() } }", THIS},
    {"return function (x) { return this + x }", NONE},
    {"return { m(x) { return super.m() + x } }", NONE},
    {"var x = function () { this.foo = 42 };", NONE},
    {"var x = { m() { super.foo = 42 } };", NONE},
    {"if (1) { return function () { while (true) new this() } }", NONE},
    {"if (1) { return { m() { while (true) super.m() } } }", NONE},
    {"return function (x) { return () => this }", NONE},
    {"return { m(x) { return () => super.m() } }", NONE},
    // Flags must be correctly set when using block scoping.
    {"\"use strict\"; while (true) { let x; this, arguments; }",
     INNER_ARGUMENTS | THIS},
    {"\"use strict\"; while (true) { let x; this, super.f(), arguments; }",
     INNER_ARGUMENTS | SUPER_PROPERTY | THIS},
    {"\"use strict\"; if (foo()) { let x; this.f() }", THIS},
    {"\"use strict\"; if (foo()) { let x; super.f() }", SUPER_PROPERTY},
    {"\"use strict\"; if (1) {"
     "  let x; return { m() { return this + super.m() + arguments } }"
     "}",
     NONE},
    {"eval(42)", EVAL},
    {"if (1) { eval(42) }", EVAL},
    {"eval('super.x')", EVAL},
    {"eval('this.x')", EVAL},
    {"eval('arguments')", EVAL},
  };
  // clang-format on

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  for (unsigned j = 0; j < arraysize(surroundings); ++j) {
    for (unsigned i = 0; i < arraysize(source_data); ++i) {
      // Super property is only allowed in constructor and method.
      if (((source_data[i].expected & SUPER_PROPERTY) ||
           (source_data[i].expected == NONE)) && j != 2) {
        continue;
      }
      int kProgramByteSize = i::StrLength(surroundings[j].prefix) +
                             i::StrLength(surroundings[j].suffix) +
                             i::StrLength(source_data[i].body);
      i::ScopedVector<char> program(kProgramByteSize + 1);
      i::SNPrintF(program, "%s%s%s", surroundings[j].prefix,
                  source_data[i].body, surroundings[j].suffix);
      i::Handle<i::String> source =
          factory->NewStringFromUtf8(i::CStrVector(program.start()))
              .ToHandleChecked();
      i::Handle<i::Script> script = factory->NewScript(source);
      i::Zone zone;
      i::ParseInfo info(&zone, script);
      i::Parser parser(&info);
      parser.set_allow_harmony_arrow_functions(true);
      parser.set_allow_harmony_sloppy(true);
      info.set_global();
      CHECK(parser.Parse(&info));
      CHECK(i::Rewriter::Rewrite(&info));
      CHECK(i::Scope::Analyze(&info));
      CHECK(info.function() != NULL);

      i::Scope* script_scope = info.function()->scope();
      CHECK(script_scope->is_script_scope());
      CHECK_EQ(1, script_scope->inner_scopes()->length());

      i::Scope* scope = script_scope->inner_scopes()->at(0);
      // Adjust for constructor scope.
      if (j == 2) {
        CHECK_EQ(1, scope->inner_scopes()->length());
        scope = scope->inner_scopes()->at(0);
      }
      CHECK_EQ((source_data[i].expected & ARGUMENTS) != 0,
               scope->uses_arguments());
      CHECK_EQ((source_data[i].expected & SUPER_PROPERTY) != 0,
               scope->uses_super_property());
      if ((source_data[i].expected & THIS) != 0) {
        // Currently the is_used() flag is conservative; all variables in a
        // script scope are marked as used.
        CHECK(scope->LookupThis()->is_used());
      }
      CHECK_EQ((source_data[i].expected & INNER_ARGUMENTS) != 0,
               scope->inner_uses_arguments());
      CHECK_EQ((source_data[i].expected & EVAL) != 0, scope->calls_eval());
    }
  }
}


static void CheckParsesToNumber(const char* source, bool with_dot) {
  v8::V8::Initialize();
  HandleAndZoneScope handles;

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  std::string full_source = "function f() { return ";
  full_source += source;
  full_source += "; }";

  i::Handle<i::String> source_code =
      factory->NewStringFromUtf8(i::CStrVector(full_source.c_str()))
          .ToHandleChecked();

  i::Handle<i::Script> script = factory->NewScript(source_code);

  i::ParseInfo info(handles.main_zone(), script);
  i::Parser parser(&info);
  parser.set_allow_harmony_arrow_functions(true);
  parser.set_allow_harmony_sloppy(true);
  info.set_global();
  info.set_lazy(false);
  info.set_allow_lazy_parsing(false);
  info.set_toplevel(true);

  i::CompilationInfo compilation_info(&info);
  CHECK(i::Compiler::ParseAndAnalyze(&info));

  CHECK(info.scope()->declarations()->length() == 1);
  i::FunctionLiteral* fun =
      info.scope()->declarations()->at(0)->AsFunctionDeclaration()->fun();
  CHECK(fun->body()->length() == 1);
  CHECK(fun->body()->at(0)->IsReturnStatement());
  i::ReturnStatement* ret = fun->body()->at(0)->AsReturnStatement();
  CHECK(ret->expression()->IsLiteral());
  i::Literal* lit = ret->expression()->AsLiteral();
  const i::AstValue* val = lit->raw_value();
  CHECK(with_dot == val->ContainsDot());
}


TEST(ParseNumbers) {
  CheckParsesToNumber("1.34", true);
  CheckParsesToNumber("134", false);
  CheckParsesToNumber("134e44", false);
  CheckParsesToNumber("134.e44", true);
  CheckParsesToNumber("134.44e44", true);
  CheckParsesToNumber(".44", true);
}


TEST(ScopePositions) {
  // Test the parser for correctly setting the start and end positions
  // of a scope. We check the scope positions of exactly one scope
  // nested in the global scope of a program. 'inner source' is the
  // source code that determines the part of the source belonging
  // to the nested scope. 'outer_prefix' and 'outer_suffix' are
  // parts of the source that belong to the global scope.
  struct SourceData {
    const char* outer_prefix;
    const char* inner_source;
    const char* outer_suffix;
    i::ScopeType scope_type;
    i::LanguageMode language_mode;
  };

  const SourceData source_data[] = {
    { "  with ({}) ", "{ block; }", " more;", i::WITH_SCOPE, i::SLOPPY },
    { "  with ({}) ", "{ block; }", "; more;", i::WITH_SCOPE, i::SLOPPY },
    { "  with ({}) ", "{\n"
      "    block;\n"
      "  }", "\n"
      "  more;", i::WITH_SCOPE, i::SLOPPY },
    { "  with ({}) ", "statement;", " more;", i::WITH_SCOPE, i::SLOPPY },
    { "  with ({}) ", "statement", "\n"
      "  more;", i::WITH_SCOPE, i::SLOPPY },
    { "  with ({})\n"
      "    ", "statement;", "\n"
      "  more;", i::WITH_SCOPE, i::SLOPPY },
    { "  try {} catch ", "(e) { block; }", " more;",
      i::CATCH_SCOPE, i::SLOPPY },
    { "  try {} catch ", "(e) { block; }", "; more;",
      i::CATCH_SCOPE, i::SLOPPY },
    { "  try {} catch ", "(e) {\n"
      "    block;\n"
      "  }", "\n"
      "  more;", i::CATCH_SCOPE, i::SLOPPY },
    { "  try {} catch ", "(e) { block; }", " finally { block; } more;",
      i::CATCH_SCOPE, i::SLOPPY },
    { "  start;\n"
      "  ", "{ let block; }", " more;", i::BLOCK_SCOPE, i::STRICT },
    { "  start;\n"
      "  ", "{ let block; }", "; more;", i::BLOCK_SCOPE, i::STRICT },
    { "  start;\n"
      "  ", "{\n"
      "    let block;\n"
      "  }", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  start;\n"
      "  function fun", "(a,b) { infunction; }", " more;",
      i::FUNCTION_SCOPE, i::SLOPPY },
    { "  start;\n"
      "  function fun", "(a,b) {\n"
      "    infunction;\n"
      "  }", "\n"
      "  more;", i::FUNCTION_SCOPE, i::SLOPPY },
    { "  start;\n", "(a,b) => a + b", "; more;",
      i::ARROW_SCOPE, i::SLOPPY },
    { "  start;\n", "(a,b) => { return a+b; }", "\nmore;",
      i::ARROW_SCOPE, i::SLOPPY },
    { "  start;\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    { "  for ", "(let x = 1 ; x < 10; ++ x) { block; }", " more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x = 1 ; x < 10; ++ x) { block; }", "; more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x = 1 ; x < 10; ++ x) {\n"
      "    block;\n"
      "  }", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x = 1 ; x < 10; ++ x) statement;", " more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x = 1 ; x < 10; ++ x) statement", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x = 1 ; x < 10; ++ x)\n"
      "    statement;", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {}) { block; }", " more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {}) { block; }", "; more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {}) {\n"
      "    block;\n"
      "  }", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {}) statement;", " more;",
      i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {}) statement", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    { "  for ", "(let x in {})\n"
      "    statement;", "\n"
      "  more;", i::BLOCK_SCOPE, i::STRICT },
    // Check that 6-byte and 4-byte encodings of UTF-8 strings do not throw
    // the preparser off in terms of byte offsets.
    // 6 byte encoding.
    { "  'foo\355\240\201\355\260\211';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // 4 byte encoding.
    { "  'foo\360\220\220\212';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // 3 byte encoding of \u0fff.
    { "  'foo\340\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 6 byte encoding with missing last byte.
    { "  'foo\355\240\201\355\211';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 3 byte encoding of \u0fff with missing last byte.
    { "  'foo\340\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 3 byte encoding of \u0fff with missing 2 last bytes.
    { "  'foo\340';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 3 byte encoding of \u00ff should be a 2 byte encoding.
    { "  'foo\340\203\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 3 byte encoding of \u007f should be a 2 byte encoding.
    { "  'foo\340\201\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Unpaired lead surrogate.
    { "  'foo\355\240\201';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Unpaired lead surrogate where following code point is a 3 byte sequence.
    { "  'foo\355\240\201\340\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Unpaired lead surrogate where following code point is a 4 byte encoding
    // of a trail surrogate.
    { "  'foo\355\240\201\360\215\260\211';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Unpaired trail surrogate.
    { "  'foo\355\260\211';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // 2 byte encoding of \u00ff.
    { "  'foo\303\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 2 byte encoding of \u00ff with missing last byte.
    { "  'foo\303';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Broken 2 byte encoding of \u007f should be a 1 byte encoding.
    { "  'foo\301\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Illegal 5 byte encoding.
    { "  'foo\370\277\277\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Illegal 6 byte encoding.
    { "  'foo\374\277\277\277\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Illegal 0xfe byte
    { "  'foo\376\277\277\277\277\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    // Illegal 0xff byte
    { "  'foo\377\277\277\277\277\277\277\277';\n"
      "  (function fun", "(a,b) { infunction; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    { "  'foo';\n"
      "  (function fun", "(a,b) { 'bar\355\240\201\355\260\213'; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    { "  'foo';\n"
      "  (function fun", "(a,b) { 'bar\360\220\220\214'; }", ")();",
      i::FUNCTION_SCOPE, i::SLOPPY },
    { NULL, NULL, NULL, i::EVAL_SCOPE, i::SLOPPY }
  };

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  for (int i = 0; source_data[i].outer_prefix; i++) {
    int kPrefixLen = Utf8LengthHelper(source_data[i].outer_prefix);
    int kInnerLen = Utf8LengthHelper(source_data[i].inner_source);
    int kSuffixLen = Utf8LengthHelper(source_data[i].outer_suffix);
    int kPrefixByteLen = i::StrLength(source_data[i].outer_prefix);
    int kInnerByteLen = i::StrLength(source_data[i].inner_source);
    int kSuffixByteLen = i::StrLength(source_data[i].outer_suffix);
    int kProgramSize = kPrefixLen + kInnerLen + kSuffixLen;
    int kProgramByteSize = kPrefixByteLen + kInnerByteLen + kSuffixByteLen;
    i::ScopedVector<char> program(kProgramByteSize + 1);
    i::SNPrintF(program, "%s%s%s",
                         source_data[i].outer_prefix,
                         source_data[i].inner_source,
                         source_data[i].outer_suffix);

    // Parse program source.
    i::Handle<i::String> source = factory->NewStringFromUtf8(
        i::CStrVector(program.start())).ToHandleChecked();
    CHECK_EQ(source->length(), kProgramSize);
    i::Handle<i::Script> script = factory->NewScript(source);
    i::Zone zone;
    i::ParseInfo info(&zone, script);
    i::Parser parser(&info);
    parser.set_allow_lazy(true);
    parser.set_allow_harmony_arrow_functions(true);
    info.set_global();
    info.set_language_mode(source_data[i].language_mode);
    parser.Parse(&info);
    CHECK(info.function() != NULL);

    // Check scope types and positions.
    i::Scope* scope = info.function()->scope();
    CHECK(scope->is_script_scope());
    CHECK_EQ(scope->start_position(), 0);
    CHECK_EQ(scope->end_position(), kProgramSize);
    CHECK_EQ(scope->inner_scopes()->length(), 1);

    i::Scope* inner_scope = scope->inner_scopes()->at(0);
    CHECK_EQ(inner_scope->scope_type(), source_data[i].scope_type);
    CHECK_EQ(inner_scope->start_position(), kPrefixLen);
    // The end position of a token is one position after the last
    // character belonging to that token.
    CHECK_EQ(inner_scope->end_position(), kPrefixLen + kInnerLen);
  }
}


const char* ReadString(unsigned* start) {
  int length = start[0];
  char* result = i::NewArray<char>(length + 1);
  for (int i = 0; i < length; i++) {
    result[i] = start[i + 1];
  }
  result[length] = '\0';
  return result;
}


i::Handle<i::String> FormatMessage(i::Vector<unsigned> data) {
  i::Isolate* isolate = CcTest::i_isolate();
  int message = data[i::PreparseDataConstants::kMessageTemplatePos];
  int arg_count = data[i::PreparseDataConstants::kMessageArgCountPos];
  i::Handle<i::Object> arg_object;
  if (arg_count == 1) {
    // Position after text found by skipping past length field and
    // length field content words.
    const char* arg =
        ReadString(&data[i::PreparseDataConstants::kMessageArgPos]);
    arg_object =
        v8::Utils::OpenHandle(*v8::String::NewFromUtf8(CcTest::isolate(), arg));
    i::DeleteArray(arg);
  } else {
    CHECK_EQ(0, arg_count);
    arg_object = isolate->factory()->undefined_value();
  }

  data.Dispose();
  return i::MessageTemplate::FormatMessage(isolate, message, arg_object);
}


enum ParserFlag {
  kAllowLazy,
  kAllowNatives,
  kAllowHarmonyModules,
  kAllowHarmonyArrowFunctions,
  kAllowHarmonyRestParameters,
  kAllowHarmonySloppy,
  kAllowHarmonyUnicode,
  kAllowHarmonyComputedPropertyNames,
  kAllowHarmonySpreadCalls,
  kAllowHarmonyDestructuring,
  kAllowHarmonySpreadArrays,
  kAllowHarmonyNewTarget,
  kAllowStrongMode,
  kNoLegacyConst
};


enum ParserSyncTestResult {
  kSuccessOrError,
  kSuccess,
  kError
};

template <typename Traits>
void SetParserFlags(i::ParserBase<Traits>* parser,
                    i::EnumSet<ParserFlag> flags) {
  parser->set_allow_lazy(flags.Contains(kAllowLazy));
  parser->set_allow_natives(flags.Contains(kAllowNatives));
  parser->set_allow_harmony_modules(flags.Contains(kAllowHarmonyModules));
  parser->set_allow_harmony_arrow_functions(
      flags.Contains(kAllowHarmonyArrowFunctions));
  parser->set_allow_harmony_rest_params(
      flags.Contains(kAllowHarmonyRestParameters));
  parser->set_allow_harmony_spreadcalls(
      flags.Contains(kAllowHarmonySpreadCalls));
  parser->set_allow_harmony_sloppy(flags.Contains(kAllowHarmonySloppy));
  parser->set_allow_harmony_unicode(flags.Contains(kAllowHarmonyUnicode));
  parser->set_allow_harmony_computed_property_names(
      flags.Contains(kAllowHarmonyComputedPropertyNames));
  parser->set_allow_harmony_destructuring(
      flags.Contains(kAllowHarmonyDestructuring));
  parser->set_allow_harmony_spread_arrays(
      flags.Contains(kAllowHarmonySpreadArrays));
  parser->set_allow_harmony_new_target(flags.Contains(kAllowHarmonyNewTarget));
  parser->set_allow_strong_mode(flags.Contains(kAllowStrongMode));
  parser->set_allow_legacy_const(!flags.Contains(kNoLegacyConst));
}


void TestParserSyncWithFlags(i::Handle<i::String> source,
                             i::EnumSet<ParserFlag> flags,
                             ParserSyncTestResult result) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  uintptr_t stack_limit = isolate->stack_guard()->real_climit();
  int preparser_materialized_literals = -1;
  int parser_materialized_literals = -2;

  // Preparse the data.
  i::CompleteParserRecorder log;
  {
    i::Scanner scanner(isolate->unicode_cache());
    i::GenericStringUtf16CharacterStream stream(source, 0, source->length());
    i::Zone zone;
    i::AstValueFactory ast_value_factory(
        &zone, CcTest::i_isolate()->heap()->HashSeed());
    i::PreParser preparser(&zone, &scanner, &ast_value_factory, &log,
                           stack_limit);
    SetParserFlags(&preparser, flags);
    scanner.Initialize(&stream);
    i::PreParser::PreParseResult result = preparser.PreParseProgram(
        &preparser_materialized_literals);
    CHECK_EQ(i::PreParser::kPreParseSuccess, result);
  }

  bool preparse_error = log.HasError();

  // Parse the data
  i::FunctionLiteral* function;
  {
    i::Handle<i::Script> script = factory->NewScript(source);
    i::Zone zone;
    i::ParseInfo info(&zone, script);
    i::Parser parser(&info);
    SetParserFlags(&parser, flags);
    info.set_global();
    parser.Parse(&info);
    function = info.function();
    if (function) {
      parser_materialized_literals = function->materialized_literal_count();
    }
  }

  // Check that preparsing fails iff parsing fails.
  if (function == NULL) {
    // Extract exception from the parser.
    CHECK(isolate->has_pending_exception());
    i::Handle<i::JSObject> exception_handle(
        i::JSObject::cast(isolate->pending_exception()));
    i::Handle<i::String> message_string =
        i::Handle<i::String>::cast(i::Object::GetProperty(
            isolate, exception_handle, "message").ToHandleChecked());

    if (result == kSuccess) {
      v8::base::OS::Print(
          "Parser failed on:\n"
          "\t%s\n"
          "with error:\n"
          "\t%s\n"
          "However, we expected no error.",
          source->ToCString().get(), message_string->ToCString().get());
      CHECK(false);
    }

    if (!preparse_error) {
      v8::base::OS::Print(
          "Parser failed on:\n"
          "\t%s\n"
          "with error:\n"
          "\t%s\n"
          "However, the preparser succeeded",
          source->ToCString().get(), message_string->ToCString().get());
      CHECK(false);
    }
    // Check that preparser and parser produce the same error.
    i::Handle<i::String> preparser_message =
        FormatMessage(log.ErrorMessageData());
    if (!i::String::Equals(message_string, preparser_message)) {
      v8::base::OS::Print(
          "Expected parser and preparser to produce the same error on:\n"
          "\t%s\n"
          "However, found the following error messages\n"
          "\tparser:    %s\n"
          "\tpreparser: %s\n",
          source->ToCString().get(),
          message_string->ToCString().get(),
          preparser_message->ToCString().get());
      CHECK(false);
    }
  } else if (preparse_error) {
    v8::base::OS::Print(
        "Preparser failed on:\n"
        "\t%s\n"
        "with error:\n"
        "\t%s\n"
        "However, the parser succeeded",
        source->ToCString().get(),
        FormatMessage(log.ErrorMessageData())->ToCString().get());
    CHECK(false);
  } else if (result == kError) {
    v8::base::OS::Print(
        "Expected error on:\n"
        "\t%s\n"
        "However, parser and preparser succeeded",
        source->ToCString().get());
    CHECK(false);
  } else if (preparser_materialized_literals != parser_materialized_literals) {
    v8::base::OS::Print(
        "Preparser materialized literals (%d) differ from Parser materialized "
        "literals (%d) on:\n"
        "\t%s\n"
        "However, parser and preparser succeeded",
        preparser_materialized_literals, parser_materialized_literals,
        source->ToCString().get());
    CHECK(false);
  }
}


void TestParserSync(const char* source,
                    const ParserFlag* varying_flags,
                    size_t varying_flags_length,
                    ParserSyncTestResult result = kSuccessOrError,
                    const ParserFlag* always_true_flags = NULL,
                    size_t always_true_flags_length = 0,
                    const ParserFlag* always_false_flags = NULL,
                    size_t always_false_flags_length = 0) {
  i::Handle<i::String> str =
      CcTest::i_isolate()->factory()->NewStringFromAsciiChecked(source);
  for (int bits = 0; bits < (1 << varying_flags_length); bits++) {
    i::EnumSet<ParserFlag> flags;
    for (size_t flag_index = 0; flag_index < varying_flags_length;
         ++flag_index) {
      if ((bits & (1 << flag_index)) != 0) flags.Add(varying_flags[flag_index]);
    }
    for (size_t flag_index = 0; flag_index < always_true_flags_length;
         ++flag_index) {
      flags.Add(always_true_flags[flag_index]);
    }
    for (size_t flag_index = 0; flag_index < always_false_flags_length;
         ++flag_index) {
      flags.Remove(always_false_flags[flag_index]);
    }
    TestParserSyncWithFlags(str, flags, result);
  }
}


TEST(ParserSync) {
  const char* context_data[][2] = {
    { "", "" },
    { "{", "}" },
    { "if (true) ", " else {}" },
    { "if (true) {} else ", "" },
    { "if (true) ", "" },
    { "do ", " while (false)" },
    { "while (false) ", "" },
    { "for (;;) ", "" },
    { "with ({})", "" },
    { "switch (12) { case 12: ", "}" },
    { "switch (12) { default: ", "}" },
    { "switch (12) { ", "case 12: }" },
    { "label2: ", "" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "{}",
    "var x",
    "var x = 1",
    "const x",
    "const x = 1",
    ";",
    "12",
    "if (false) {} else ;",
    "if (false) {} else {}",
    "if (false) {} else 12",
    "if (false) ;",
    "if (false) {}",
    "if (false) 12",
    "do {} while (false)",
    "for (;;) ;",
    "for (;;) {}",
    "for (;;) 12",
    "continue",
    "continue label",
    "continue\nlabel",
    "break",
    "break label",
    "break\nlabel",
    // TODO(marja): activate once parsing 'return' is merged into ParserBase.
    // "return",
    // "return  12",
    // "return\n12",
    "with ({}) ;",
    "with ({}) {}",
    "with ({}) 12",
    "switch ({}) { default: }",
    "label3: ",
    "throw",
    "throw  12",
    "throw\n12",
    "try {} catch(e) {}",
    "try {} finally {}",
    "try {} catch(e) {} finally {}",
    "debugger",
    NULL
  };

  const char* termination_data[] = {
    "",
    ";",
    "\n",
    ";\n",
    "\n;",
    NULL
  };

  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  for (int i = 0; context_data[i][0] != NULL; ++i) {
    for (int j = 0; statement_data[j] != NULL; ++j) {
      for (int k = 0; termination_data[k] != NULL; ++k) {
        int kPrefixLen = i::StrLength(context_data[i][0]);
        int kStatementLen = i::StrLength(statement_data[j]);
        int kTerminationLen = i::StrLength(termination_data[k]);
        int kSuffixLen = i::StrLength(context_data[i][1]);
        int kProgramSize = kPrefixLen + kStatementLen + kTerminationLen
            + kSuffixLen + i::StrLength("label: for (;;) {  }");

        // Plug the source code pieces together.
        i::ScopedVector<char> program(kProgramSize + 1);
        int length = i::SNPrintF(program,
            "label: for (;;) { %s%s%s%s }",
            context_data[i][0],
            statement_data[j],
            termination_data[k],
            context_data[i][1]);
        CHECK(length == kProgramSize);
        TestParserSync(program.start(), NULL, 0);
      }
    }
  }

  // Neither Harmony numeric literals nor our natives syntax have any
  // interaction with the flags above, so test these separately to reduce
  // the combinatorial explosion.
  TestParserSync("0o1234", NULL, 0);
  TestParserSync("0b1011", NULL, 0);

  static const ParserFlag flags3[] = { kAllowNatives };
  TestParserSync("%DebugPrint(123)", flags3, arraysize(flags3));
}


TEST(StrictOctal) {
  // Test that syntax error caused by octal literal is reported correctly as
  // such (issue 2220).
  v8::V8::Initialize();
  v8::HandleScope scope(CcTest::isolate());
  v8::Context::Scope context_scope(
      v8::Context::New(CcTest::isolate()));
  v8::TryCatch try_catch(CcTest::isolate());
  const char* script =
      "\"use strict\";       \n"
      "a = function() {      \n"
      "  b = function() {    \n"
      "    01;               \n"
      "  };                  \n"
      "};                    \n";
  v8::Script::Compile(v8::String::NewFromUtf8(CcTest::isolate(), script));
  CHECK(try_catch.HasCaught());
  v8::String::Utf8Value exception(try_catch.Exception());
  CHECK_EQ(0,
           strcmp("SyntaxError: Octal literals are not allowed in strict mode.",
                  *exception));
}


void RunParserSyncTest(const char* context_data[][2],
                       const char* statement_data[],
                       ParserSyncTestResult result,
                       const ParserFlag* flags = NULL,
                       int flags_len = 0,
                       const ParserFlag* always_true_flags = NULL,
                       int always_true_len = 0,
                       const ParserFlag* always_false_flags = NULL,
                       int always_false_len = 0) {
  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  // Experimental feature flags should not go here; pass the flags as
  // always_true_flags if the test needs them.
  static const ParserFlag default_flags[] = {
    kAllowLazy,
    kAllowNatives,
  };
  ParserFlag* generated_flags = NULL;
  if (flags == NULL) {
    flags = default_flags;
    flags_len = arraysize(default_flags);
    if (always_true_flags != NULL || always_false_flags != NULL) {
      // Remove always_true/false_flags from default_flags (if present).
      CHECK((always_true_flags != NULL) == (always_true_len > 0));
      CHECK((always_false_flags != NULL) == (always_false_len > 0));
      generated_flags = new ParserFlag[flags_len + always_true_len];
      int flag_index = 0;
      for (int i = 0; i < flags_len; ++i) {
        bool use_flag = true;
        for (int j = 0; use_flag && j < always_true_len; ++j) {
          if (flags[i] == always_true_flags[j]) use_flag = false;
        }
        for (int j = 0; use_flag && j < always_false_len; ++j) {
          if (flags[i] == always_false_flags[j]) use_flag = false;
        }
        if (use_flag) generated_flags[flag_index++] = flags[i];
      }
      flags_len = flag_index;
      flags = generated_flags;
    }
  }
  for (int i = 0; context_data[i][0] != NULL; ++i) {
    for (int j = 0; statement_data[j] != NULL; ++j) {
      int kPrefixLen = i::StrLength(context_data[i][0]);
      int kStatementLen = i::StrLength(statement_data[j]);
      int kSuffixLen = i::StrLength(context_data[i][1]);
      int kProgramSize = kPrefixLen + kStatementLen + kSuffixLen;

      // Plug the source code pieces together.
      i::ScopedVector<char> program(kProgramSize + 1);
      int length = i::SNPrintF(program,
                               "%s%s%s",
                               context_data[i][0],
                               statement_data[j],
                               context_data[i][1]);
      CHECK(length == kProgramSize);
      TestParserSync(program.start(),
                     flags,
                     flags_len,
                     result,
                     always_true_flags,
                     always_true_len,
                     always_false_flags,
                     always_false_len);
    }
  }
  delete[] generated_flags;
}


TEST(ErrorsEvalAndArguments) {
  // Tests that both preparsing and parsing produce the right kind of errors for
  // using "eval" and "arguments" as identifiers. Without the strict mode, it's
  // ok to use "eval" or "arguments" as identifiers. With the strict mode, it
  // isn't.
  const char* context_data[][2] = {
      {"\"use strict\";", ""},
      {"\"use strong\";", ""},
      {"var eval; function test_func() {\"use strict\"; ", "}"},
      {"var eval; function test_func() {\"use strong\"; ", "}"},
      {NULL, NULL}};

  const char* statement_data[] = {
    "var eval;",
    "var arguments",
    "var foo, eval;",
    "var foo, arguments;",
    "try { } catch (eval) { }",
    "try { } catch (arguments) { }",
    "function eval() { }",
    "function arguments() { }",
    "function foo(eval) { }",
    "function foo(arguments) { }",
    "function foo(bar, eval) { }",
    "function foo(bar, arguments) { }",
    "(eval) => { }",
    "(arguments) => { }",
    "(foo, eval) => { }",
    "(foo, arguments) => { }",
    "eval = 1;",
    "arguments = 1;",
    "var foo = eval = 1;",
    "var foo = arguments = 1;",
    "++eval;",
    "++arguments;",
    "eval++;",
    "arguments++;",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(NoErrorsEvalAndArgumentsSloppy) {
  // Tests that both preparsing and parsing accept "eval" and "arguments" as
  // identifiers when needed.
  const char* context_data[][2] = {
    { "", "" },
    { "function test_func() {", "}"},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var eval;",
    "var arguments",
    "var foo, eval;",
    "var foo, arguments;",
    "try { } catch (eval) { }",
    "try { } catch (arguments) { }",
    "function eval() { }",
    "function arguments() { }",
    "function foo(eval) { }",
    "function foo(arguments) { }",
    "function foo(bar, eval) { }",
    "function foo(bar, arguments) { }",
    "eval = 1;",
    "arguments = 1;",
    "var foo = eval = 1;",
    "var foo = arguments = 1;",
    "++eval;",
    "++arguments;",
    "eval++;",
    "arguments++;",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(NoErrorsEvalAndArgumentsStrict) {
  const char* context_data[][2] = {
    { "\"use strict\";", "" },
    { "function test_func() { \"use strict\";", "}" },
    { "() => { \"use strict\"; ", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "eval;",
    "arguments;",
    "var foo = eval;",
    "var foo = arguments;",
    "var foo = { eval: 1 };",
    "var foo = { arguments: 1 };",
    "var foo = { }; foo.eval = {};",
    "var foo = { }; foo.arguments = {};",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


#define FUTURE_STRICT_RESERVED_WORDS(V) \
  V(implements)                         \
  V(interface)                          \
  V(let)                                \
  V(package)                            \
  V(private)                            \
  V(protected)                          \
  V(public)                             \
  V(static)                             \
  V(yield)


#define LIMITED_FUTURE_STRICT_RESERVED_WORDS(V) \
  V(implements)                                 \
  V(let)                                        \
  V(static)                                     \
  V(yield)


#define FUTURE_STRICT_RESERVED_STATEMENTS(NAME) \
  "var " #NAME ";",                             \
  "var foo, " #NAME ";",                        \
  "try { } catch (" #NAME ") { }",              \
  "function " #NAME "() { }",                   \
  "(function " #NAME "() { })",                 \
  "function foo(" #NAME ") { }",                \
  "function foo(bar, " #NAME ") { }",           \
  #NAME " = 1;",                                \
  #NAME " += 1;",                               \
  "var foo = " #NAME " = 1;",                   \
  "++" #NAME ";",                               \
  #NAME " ++;",


TEST(ErrorsFutureStrictReservedWords) {
  // Tests that both preparsing and parsing produce the right kind of errors for
  // using future strict reserved words as identifiers. Without the strict mode,
  // it's ok to use future strict reserved words as identifiers. With the strict
  // mode, it isn't.
  const char* context_data[][2] = {
      {"function test_func() {\"use strict\"; ", "}"},
      {"() => { \"use strict\"; ", "}"},
      {"function test_func() {\"use strong\"; ", "}"},
      {"() => { \"use strong\"; ", "}"},
      {NULL, NULL}};

  const char* statement_data[] {
    LIMITED_FUTURE_STRICT_RESERVED_WORDS(FUTURE_STRICT_RESERVED_STATEMENTS)
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


#undef LIMITED_FUTURE_STRICT_RESERVED_WORDS


TEST(NoErrorsFutureStrictReservedWords) {
  const char* context_data[][2] = {
    { "", "" },
    { "function test_func() {", "}"},
    { "() => {", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    FUTURE_STRICT_RESERVED_WORDS(FUTURE_STRICT_RESERVED_STATEMENTS)
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));

  static const ParserFlag classes_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    classes_flags, arraysize(classes_flags));
}


TEST(ErrorsReservedWords) {
  // Tests that both preparsing and parsing produce the right kind of errors for
  // using future reserved words as identifiers. These tests don't depend on the
  // strict mode.
  const char* context_data[][2] = {
    { "", "" },
    { "\"use strict\";", "" },
    { "var eval; function test_func() {", "}"},
    { "var eval; function test_func() {\"use strict\"; ", "}"},
    { "var eval; () => {", "}"},
    { "var eval; () => {\"use strict\"; ", "}"},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var super;",
    "var foo, super;",
    "try { } catch (super) { }",
    "function super() { }",
    "function foo(super) { }",
    "function foo(bar, super) { }",
    "(super) => { }",
    "(bar, super) => { }",
    "super = 1;",
    "var foo = super = 1;",
    "++super;",
    "super++;",
    "function foo super",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsLetSloppyAllModes) {
  // In sloppy mode, it's okay to use "let" as identifier.
  const char* context_data[][2] = {
    { "", "" },
    { "function f() {", "}" },
    { "(function f() {", "})" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var let;",
    "var foo, let;",
    "try { } catch (let) { }",
    "function let() { }",
    "(function let() { })",
    "function foo(let) { }",
    "function foo(bar, let) { }",
    "let = 1;",
    "var foo = let = 1;",
    "let * 2;",
    "++let;",
    "let++;",
    "let: 34",
    "function let(let) { let: let(let + let(0)); }",
    "({ let: 1 })",
    "({ get let() { 1 } })",
    "let(100)",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(NoErrorsYieldSloppyAllModes) {
  // In sloppy mode, it's okay to use "yield" as identifier, *except* inside a
  // generator (see other test).
  const char* context_data[][2] = {
    { "", "" },
    { "function not_gen() {", "}" },
    { "(function not_gen() {", "})" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var yield;",
    "var foo, yield;",
    "try { } catch (yield) { }",
    "function yield() { }",
    "(function yield() { })",
    "function foo(yield) { }",
    "function foo(bar, yield) { }",
    "yield = 1;",
    "var foo = yield = 1;",
    "yield * 2;",
    "++yield;",
    "yield++;",
    "yield: 34",
    "function yield(yield) { yield: yield (yield + yield(0)); }",
    "({ yield: 1 })",
    "({ get yield() { 1 } })",
    "yield(100)",
    "yield[100]",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(NoErrorsYieldSloppyGeneratorsEnabled) {
  // In sloppy mode, it's okay to use "yield" as identifier, *except* inside a
  // generator (see next test).
  const char* context_data[][2] = {
    { "", "" },
    { "function not_gen() {", "}" },
    { "function * gen() { function not_gen() {", "} }" },
    { "(function not_gen() {", "})" },
    { "(function * gen() { (function not_gen() {", "}) })" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var yield;",
    "var foo, yield;",
    "try { } catch (yield) { }",
    "function yield() { }",
    "(function yield() { })",
    "function foo(yield) { }",
    "function foo(bar, yield) { }",
    "function * yield() { }",
    "(function * yield() { })",
    "yield = 1;",
    "var foo = yield = 1;",
    "yield * 2;",
    "++yield;",
    "yield++;",
    "yield: 34",
    "function yield(yield) { yield: yield (yield + yield(0)); }",
    "({ yield: 1 })",
    "({ get yield() { 1 } })",
    "yield(100)",
    "yield[100]",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsYieldStrict) {
  const char* context_data[][2] = {
      {"\"use strict\";", ""},
      {"\"use strict\"; function not_gen() {", "}"},
      {"function test_func() {\"use strict\"; ", "}"},
      {"\"use strict\"; function * gen() { function not_gen() {", "} }"},
      {"\"use strict\"; (function not_gen() {", "})"},
      {"\"use strict\"; (function * gen() { (function not_gen() {", "}) })"},
      {"() => {\"use strict\"; ", "}"},
      {"\"use strong\";", ""},
      {"\"use strong\"; function not_gen() {", "}"},
      {"function test_func() {\"use strong\"; ", "}"},
      {"\"use strong\"; function * gen() { function not_gen() {", "} }"},
      {"\"use strong\"; (function not_gen() {", "})"},
      {"\"use strong\"; (function * gen() { (function not_gen() {", "}) })"},
      {"() => {\"use strong\"; ", "}"},
      {NULL, NULL}};

  const char* statement_data[] = {
    "var yield;",
    "var foo, yield;",
    "try { } catch (yield) { }",
    "function yield() { }",
    "(function yield() { })",
    "function foo(yield) { }",
    "function foo(bar, yield) { }",
    "function * yield() { }",
    "(function * yield() { })",
    "yield = 1;",
    "var foo = yield = 1;",
    "++yield;",
    "yield++;",
    "yield: 34;",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(NoErrorsGenerator) {
  const char* context_data[][2] = {
    { "function * gen() {", "}" },
    { "(function * gen() {", "})" },
    { "(function * () {", "})" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    // A generator without a body is valid.
    ""
    // Valid yield expressions inside generators.
    "yield 2;",
    "yield * 2;",
    "yield * \n 2;",
    "yield yield 1;",
    "yield * yield * 1;",
    "yield 3 + (yield 4);",
    "yield * 3 + (yield * 4);",
    "(yield * 3) + (yield * 4);",
    "yield 3; yield 4;",
    "yield * 3; yield * 4;",
    "(function (yield) { })",
    "yield { yield: 12 }",
    "yield /* comment */ { yield: 12 }",
    "yield * \n { yield: 12 }",
    "yield /* comment */ * \n { yield: 12 }",
    // You can return in a generator.
    "yield 1; return",
    "yield * 1; return",
    "yield 1; return 37",
    "yield * 1; return 37",
    "yield 1; return 37; yield 'dead';",
    "yield * 1; return 37; yield * 'dead';",
    // Yield is still a valid key in object literals.
    "({ yield: 1 })",
    "({ get yield() { } })",
    // Yield without RHS.
    "yield;",
    "yield",
    "yield\n",
    "yield /* comment */"
    "yield // comment\n"
    "(yield)",
    "[yield]",
    "{yield}",
    "yield, yield",
    "yield; yield",
    "(yield) ? yield : yield",
    "(yield) \n ? yield : yield",
    // If there is a newline before the next token, we don't look for RHS.
    "yield\nfor (;;) {}",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsYieldGenerator) {
  const char* context_data[][2] = {
    { "function * gen() {", "}" },
    { "\"use strict\"; function * gen() {", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    // Invalid yield expressions inside generators.
    "var yield;",
    "var foo, yield;",
    "try { } catch (yield) { }",
    "function yield() { }",
    // The name of the NFE is let-bound in the generator, which does not permit
    // yield to be an identifier.
    "(function yield() { })",
    "(function * yield() { })",
    // Yield isn't valid as a formal parameter for generators.
    "function * foo(yield) { }",
    "(function * foo(yield) { })",
    "yield = 1;",
    "var foo = yield = 1;",
    "++yield;",
    "yield++;",
    "yield *",
    "(yield *)",
    // Yield binds very loosely, so this parses as "yield (3 + yield 4)", which
    // is invalid.
    "yield 3 + yield 4;",
    "yield: 34",
    "yield ? 1 : 2",
    // Parses as yield (/ yield): invalid.
    "yield / yield",
    "+ yield",
    "+ yield 3",
    // Invalid (no newline allowed between yield and *).
    "yield\n*3",
    // Invalid (we see a newline, so we parse {yield:42} as a statement, not an
    // object literal, and yield is not a valid label).
    "yield\n{yield: 42}",
    "yield /* comment */\n {yield: 42}",
    "yield //comment\n {yield: 42}",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(ErrorsNameOfStrictFunction) {
  // Tests that illegal tokens as names of a strict function produce the correct
  // errors.
  const char* context_data[][2] = {
    { "function ", ""},
    { "\"use strict\"; function", ""},
    { "\"use strong\"; function", ""},
    { "function * ", ""},
    { "\"use strict\"; function * ", ""},
    { "\"use strong\"; function * ", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "eval() {\"use strict\";}",
    "arguments() {\"use strict\";}",
    "interface() {\"use strict\";}",
    "yield() {\"use strict\";}",
    // Future reserved words are always illegal
    "super() { }",
    "super() {\"use strict\";}",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(NoErrorsNameOfStrictFunction) {
  const char* context_data[][2] = {
    { "function ", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "eval() { }",
    "arguments() { }",
    "interface() { }",
    "yield() { }",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(NoErrorsNameOfStrictGenerator) {
  const char* context_data[][2] = {
    { "function * ", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "eval() { }",
    "arguments() { }",
    "interface() { }",
    "yield() { }",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsIllegalWordsAsLabelsSloppy) {
  // Using future reserved words as labels is always an error.
  const char* context_data[][2] = {
    { "", ""},
    { "function test_func() {", "}" },
    { "() => {", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "super: while(true) { break super; }",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(ErrorsIllegalWordsAsLabelsStrict) {
  // Tests that illegal tokens as labels produce the correct errors.
  const char* context_data[][2] = {
      {"\"use strict\";", ""},
      {"function test_func() {\"use strict\"; ", "}"},
      {"() => {\"use strict\"; ", "}"},
      {"\"use strong\";", ""},
      {"function test_func() {\"use strong\"; ", "}"},
      {"() => {\"use strong\"; ", "}"},
      {NULL, NULL}};

#define LABELLED_WHILE(NAME) #NAME ": while (true) { break " #NAME "; }",
  const char* statement_data[] = {
    "super: while(true) { break super; }",
    FUTURE_STRICT_RESERVED_WORDS(LABELLED_WHILE)
    NULL
  };
#undef LABELLED_WHILE

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(NoErrorsIllegalWordsAsLabels) {
  // Using eval and arguments as labels is legal even in strict mode.
  const char* context_data[][2] = {
    { "", ""},
    { "function test_func() {", "}" },
    { "() => {", "}" },
    { "\"use strict\";", "" },
    { "\"use strict\"; function test_func() {", "}" },
    { "\"use strict\"; () => {", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "mylabel: while(true) { break mylabel; }",
    "eval: while(true) { break eval; }",
    "arguments: while(true) { break arguments; }",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(NoErrorsFutureStrictReservedAsLabelsSloppy) {
  const char* context_data[][2] = {
    { "", ""},
    { "function test_func() {", "}" },
    { "() => {", "}" },
    { NULL, NULL }
  };

#define LABELLED_WHILE(NAME) #NAME ": while (true) { break " #NAME "; }",
  const char* statement_data[] {
    FUTURE_STRICT_RESERVED_WORDS(LABELLED_WHILE)
    NULL
  };
#undef LABELLED_WHILE

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ErrorsParenthesizedLabels) {
  // Parenthesized identifiers shouldn't be recognized as labels.
  const char* context_data[][2] = {
    { "", ""},
    { "function test_func() {", "}" },
    { "() => {", "}" },
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "(mylabel): while(true) { break mylabel; }",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsParenthesizedDirectivePrologue) {
  // Parenthesized directive prologue shouldn't be recognized.
  const char* context_data[][2] = {
    { "", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "(\"use strict\"); var eval;",
    "(\"use strong\"); var eval;",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ErrorsNotAnIdentifierName) {
  const char* context_data[][2] = {
    { "", ""},
    { "\"use strict\";", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var foo = {}; foo.{;",
    "var foo = {}; foo.};",
    "var foo = {}; foo.=;",
    "var foo = {}; foo.888;",
    "var foo = {}; foo.-;",
    "var foo = {}; foo.--;",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsIdentifierNames) {
  // Keywords etc. are valid as property names.
  const char* context_data[][2] = {
    { "", ""},
    { "\"use strict\";", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "var foo = {}; foo.if;",
    "var foo = {}; foo.yield;",
    "var foo = {}; foo.super;",
    "var foo = {}; foo.interface;",
    "var foo = {}; foo.eval;",
    "var foo = {}; foo.arguments;",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(DontRegressPreParserDataSizes) {
  // These tests make sure that Parser doesn't start producing less "preparse
  // data" (data which the embedder can cache).
  v8::V8::Initialize();
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope handles(isolate);

  CcTest::i_isolate()->stack_guard()->SetStackLimit(
      i::GetCurrentStackPosition() - 128 * 1024);

  struct TestCase {
    const char* program;
    int functions;
  } test_cases[] = {
    // No functions.
    {"var x = 42;", 0},
    // Functions.
    {"function foo() {}", 1},
    {"function foo() {} function bar() {}", 2},
    // Getter / setter functions are recorded as functions if they're on the top
    // level.
    {"var x = {get foo(){} };", 1},
    // Functions insize lazy functions are not recorded.
    {"function lazy() { function a() {} function b() {} function c() {} }", 1},
    {"function lazy() { var x = {get foo(){} } }", 1},
    {NULL, 0}
  };

  for (int i = 0; test_cases[i].program; i++) {
    const char* program = test_cases[i].program;
    i::Factory* factory = CcTest::i_isolate()->factory();
    i::Handle<i::String> source =
        factory->NewStringFromUtf8(i::CStrVector(program)).ToHandleChecked();
    i::Handle<i::Script> script = factory->NewScript(source);
    i::Zone zone;
    i::ParseInfo info(&zone, script);
    i::ScriptData* sd = NULL;
    info.set_cached_data(&sd);
    info.set_compile_options(v8::ScriptCompiler::kProduceParserCache);
    info.set_allow_lazy_parsing();
    i::Parser::ParseStatic(&info);
    i::ParseData* pd = i::ParseData::FromCachedData(sd);

    if (pd->FunctionCount() != test_cases[i].functions) {
      v8::base::OS::Print(
          "Expected preparse data for program:\n"
          "\t%s\n"
          "to contain %d functions, however, received %d functions.\n",
          program, test_cases[i].functions, pd->FunctionCount());
      CHECK(false);
    }
    delete sd;
    delete pd;
  }
}


TEST(FunctionDeclaresItselfStrict) {
  // Tests that we produce the right kinds of errors when a function declares
  // itself strict (we cannot produce there errors as soon as we see the
  // offending identifiers, because we don't know at that point whether the
  // function is strict or not).
  const char* context_data[][2] = {
    {"function eval() {", "}"},
    {"function arguments() {", "}"},
    {"function yield() {", "}"},
    {"function interface() {", "}"},
    {"function foo(eval) {", "}"},
    {"function foo(arguments) {", "}"},
    {"function foo(yield) {", "}"},
    {"function foo(interface) {", "}"},
    {"function foo(bar, eval) {", "}"},
    {"function foo(bar, arguments) {", "}"},
    {"function foo(bar, yield) {", "}"},
    {"function foo(bar, interface) {", "}"},
    {"function foo(bar, bar) {", "}"},
    { NULL, NULL }
  };

  const char* strict_statement_data[] = {
    "\"use strict\";",
    "\"use strong\";",
    NULL
  };

  const char* non_strict_statement_data[] = {
    ";",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, strict_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(context_data, non_strict_statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ErrorsTryWithoutCatchOrFinally) {
  const char* context_data[][2] = {
    {"", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "try { }",
    "try { } foo();",
    "try { } catch (e) foo();",
    "try { } catch { }",
    "try { } finally foo();",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsTryCatchFinally) {
  const char* context_data[][2] = {
    {"", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "try { } catch (e) { }",
    "try { } catch (e) { } finally { }",
    "try { } finally { }",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsRegexpLiteral) {
  const char* context_data[][2] = {
    {"var r = ", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "/unterminated",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsRegexpLiteral) {
  const char* context_data[][2] = {
    {"var r = ", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "/foo/",
    "/foo/g",
    "/foo/whatever",  // This is an error but not detected by the parser.
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(Intrinsics) {
  const char* context_data[][2] = {
    {"", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "%someintrinsic(arg)",
    NULL
  };

  // This test requires kAllowNatives to succeed.
  static const ParserFlag always_true_flags[] = {
    kAllowNatives
  };

  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_true_flags, 1);
}


TEST(NoErrorsNewExpression) {
  const char* context_data[][2] = {
    {"", ""},
    {"var f =", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "new foo",
    "new foo();",
    "new foo(1);",
    "new foo(1, 2);",
    // The first () will be processed as a part of the NewExpression and the
    // second () will be processed as part of LeftHandSideExpression.
    "new foo()();",
    // The first () will be processed as a part of the inner NewExpression and
    // the second () will be processed as a part of the outer NewExpression.
    "new new foo()();",
    "new foo.bar;",
    "new foo.bar();",
    "new foo.bar.baz;",
    "new foo.bar().baz;",
    "new foo[bar];",
    "new foo[bar]();",
    "new foo[bar][baz];",
    "new foo[bar]()[baz];",
    "new foo[bar].baz(baz)()[bar].baz;",
    "new \"foo\"",  // Runtime error
    "new 1",  // Runtime error
    // This even runs:
    "(new new Function(\"this.x = 1\")).x;",
    "new new Test_Two(String, 2).v(0123).length;",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsNewExpression) {
  const char* context_data[][2] = {
    {"", ""},
    {"var f =", ""},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "new foo bar",
    "new ) foo",
    "new ++foo",
    "new foo ++",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(StrictObjectLiteralChecking) {
  const char* context_data[][2] = {
    {"\"use strict\"; var myobject = {", "};"},
    {"\"use strict\"; var myobject = {", ",};"},
    {"var myobject = {", "};"},
    {"var myobject = {", ",};"},
    { NULL, NULL }
  };

  // These are only errors in strict mode.
  const char* statement_data[] = {
    "foo: 1, foo: 2",
    "\"foo\": 1, \"foo\": 2",
    "foo: 1, \"foo\": 2",
    "1: 1, 1: 2",
    "1: 1, \"1\": 2",
    "get: 1, get: 2",  // Not a getter for real, just a property called get.
    "set: 1, set: 2",  // Not a setter for real, just a property called set.
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ErrorsObjectLiteralChecking) {
  const char* context_data[][2] = {
    {"\"use strict\"; var myobject = {", "};"},
    {"var myobject = {", "};"},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    ",",
    // Wrong number of parameters
    "get bar(x) {}",
    "get bar(x, y) {}",
    "set bar() {}",
    "set bar(x, y) {}",
    // Parsing FunctionLiteral for getter or setter fails
    "get foo( +",
    "get foo() \"error\"",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kError);
}


TEST(NoErrorsObjectLiteralChecking) {
  const char* context_data[][2] = {
    {"var myobject = {", "};"},
    {"var myobject = {", ",};"},
    {"\"use strict\"; var myobject = {", "};"},
    {"\"use strict\"; var myobject = {", ",};"},
    { NULL, NULL }
  };

  const char* statement_data[] = {
    "foo: 1, get foo() {}",
    "foo: 1, set foo(v) {}",
    "\"foo\": 1, get \"foo\"() {}",
    "\"foo\": 1, set \"foo\"(v) {}",
    "1: 1, get 1() {}",
    "1: 1, set 1(v) {}",
    "get foo() {}, get foo() {}",
    "set foo(_) {}, set foo(v) {}",
    "foo: 1, get \"foo\"() {}",
    "foo: 1, set \"foo\"(v) {}",
    "\"foo\": 1, get foo() {}",
    "\"foo\": 1, set foo(v) {}",
    "1: 1, get \"1\"() {}",
    "1: 1, set \"1\"(v) {}",
    "\"1\": 1, get 1() {}",
    "\"1\": 1, set 1(v) {}",
    "foo: 1, bar: 2",
    "\"foo\": 1, \"bar\": 2",
    "1: 1, 2: 2",
    // Syntax: IdentifierName ':' AssignmentExpression
    "foo: bar = 5 + baz",
    // Syntax: 'get' PropertyName '(' ')' '{' FunctionBody '}'
    "get foo() {}",
    "get \"foo\"() {}",
    "get 1() {}",
    // Syntax: 'set' PropertyName '(' PropertySetParameterList ')'
    //     '{' FunctionBody '}'
    "set foo(v) {}",
    "set \"foo\"(v) {}",
    "set 1(v) {}",
    // Non-colliding getters and setters -> no errors
    "foo: 1, get bar() {}",
    "foo: 1, set bar(v) {}",
    "\"foo\": 1, get \"bar\"() {}",
    "\"foo\": 1, set \"bar\"(v) {}",
    "1: 1, get 2() {}",
    "1: 1, set 2(v) {}",
    "get: 1, get foo() {}",
    "set: 1, set foo(_) {}",
    // Keywords, future reserved and strict future reserved are also allowed as
    // property names.
    "if: 4",
    "interface: 5",
    "super: 6",
    "eval: 7",
    "arguments: 8",
    NULL
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(TooManyArguments) {
  const char* context_data[][2] = {
    {"foo(", "0)"},
    { NULL, NULL }
  };

  using v8::internal::Code;
  char statement[Code::kMaxArguments * 2 + 1];
  for (int i = 0; i < Code::kMaxArguments; ++i) {
    statement[2 * i] = '0';
    statement[2 * i + 1] = ',';
  }
  statement[Code::kMaxArguments * 2] = 0;

  const char* statement_data[] = {
    statement,
    NULL
  };

  // The test is quite slow, so run it with a reduced set of flags.
  static const ParserFlag empty_flags[] = {kAllowLazy};
  RunParserSyncTest(context_data, statement_data, kError, empty_flags, 1);
}


TEST(StrictDelete) {
  // "delete <Identifier>" is not allowed in strict mode.
  const char* strong_context_data[][2] = {
    {"\"use strong\"; ", ""},
    { NULL, NULL }
  };

  const char* strict_context_data[][2] = {
    {"\"use strict\"; ", ""},
    { NULL, NULL }
  };

  const char* sloppy_context_data[][2] = {
    {"", ""},
    { NULL, NULL }
  };

  // These are errors in the strict mode.
  const char* sloppy_statement_data[] = {
    "delete foo;",
    "delete foo + 1;",
    "delete (foo);",
    "delete eval;",
    "delete interface;",
    NULL
  };

  // These are always OK
  const char* good_statement_data[] = {
    "delete this;",
    "delete 1;",
    "delete 1 + 2;",
    "delete foo();",
    "delete foo.bar;",
    "delete foo[bar];",
    "delete foo--;",
    "delete --foo;",
    "delete new foo();",
    "delete new foo(bar);",
    NULL
  };

  // These are always errors
  const char* bad_statement_data[] = {
    "delete if;",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(strong_context_data, sloppy_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, sloppy_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, sloppy_statement_data, kSuccess, NULL,
                    0, always_flags, arraysize(always_flags));

  RunParserSyncTest(strong_context_data, good_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, good_statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, good_statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(strong_context_data, bad_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, bad_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, bad_statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(NoErrorsDeclsInCase) {
  const char* context_data[][2] = {
    {"'use strict'; switch(x) { case 1:", "}"},
    {"function foo() {'use strict'; switch(x) { case 1:", "}}"},
    {"'use strict'; switch(x) { case 1: case 2:", "}"},
    {"function foo() {'use strict'; switch(x) { case 1: case 2:", "}}"},
    {"'use strict'; switch(x) { default:", "}"},
    {"function foo() {'use strict'; switch(x) { default:", "}}"},
    {"'use strict'; switch(x) { case 1: default:", "}"},
    {"function foo() {'use strict'; switch(x) { case 1: default:", "}}"},
    { nullptr, nullptr }
  };

  const char* statement_data[] = {
    "function f() { }",
    "class C { }",
    "class C extends Q {}",
    "function f() { } class C {}",
    "function f() { }; class C {}",
    "class C {}; function f() {}",
    nullptr
  };

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(InvalidLeftHandSide) {
  const char* assignment_context_data[][2] = {
    {"", " = 1;"},
    {"\"use strict\"; ", " = 1;"},
    { NULL, NULL }
  };

  const char* prefix_context_data[][2] = {
    {"++", ";"},
    {"\"use strict\"; ++", ";"},
    {NULL, NULL},
  };

  const char* postfix_context_data[][2] = {
    {"", "++;"},
    {"\"use strict\"; ", "++;"},
    { NULL, NULL }
  };

  // Good left hand sides for assigment or prefix / postfix operations.
  const char* good_statement_data[] = {
    "foo",
    "foo.bar",
    "foo[bar]",
    "foo()[bar]",
    "foo().bar",
    "this.foo",
    "this[foo]",
    "new foo()[bar]",
    "new foo().bar",
    "foo()",
    "foo(bar)",
    "foo[bar]()",
    "foo.bar()",
    "this()",
    "this.foo()",
    "this[foo].bar()",
    "this.foo[foo].bar(this)(bar)[foo]()",
    NULL
  };

  // Bad left hand sides for assigment or prefix / postfix operations.
  const char* bad_statement_data_common[] = {
    "2",
    "new foo",
    "new foo()",
    "null",
    "if",  // Unexpected token
    "{x: 1}",  // Unexpected token
    "this",
    "\"bar\"",
    "(foo + bar)",
    "new new foo()[bar]",  // means: new (new foo()[bar])
    "new new foo().bar",  // means: new (new foo()[bar])
    NULL
  };

  // These are not okay for assignment, but okay for prefix / postix.
  const char* bad_statement_data_for_assignment[] = {
    "++foo",
    "foo++",
    "foo + bar",
    NULL
  };

  RunParserSyncTest(assignment_context_data, good_statement_data, kSuccess);
  RunParserSyncTest(assignment_context_data, bad_statement_data_common, kError);
  RunParserSyncTest(assignment_context_data, bad_statement_data_for_assignment,
                    kError);

  RunParserSyncTest(prefix_context_data, good_statement_data, kSuccess);
  RunParserSyncTest(prefix_context_data, bad_statement_data_common, kError);

  RunParserSyncTest(postfix_context_data, good_statement_data, kSuccess);
  RunParserSyncTest(postfix_context_data, bad_statement_data_common, kError);
}


TEST(FuncNameInferrerBasic) {
  // Tests that function names are inferred properly.
  i::FLAG_allow_natives_syntax = true;
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope scope(isolate);
  LocalContext env;
  CompileRun("var foo1 = function() {}; "
             "var foo2 = function foo3() {}; "
             "function not_ctor() { "
             "  var foo4 = function() {}; "
             "  return %FunctionGetInferredName(foo4); "
             "} "
             "function Ctor() { "
             "  var foo5 = function() {}; "
             "  return %FunctionGetInferredName(foo5); "
             "} "
             "var obj1 = { foo6: function() {} }; "
             "var obj2 = { 'foo7': function() {} }; "
             "var obj3 = {}; "
             "obj3[1] = function() {}; "
             "var obj4 = {}; "
             "obj4[1] = function foo8() {}; "
             "var obj5 = {}; "
             "obj5['foo9'] = function() {}; "
             "var obj6 = { obj7 : { foo10: function() {} } };");
  ExpectString("%FunctionGetInferredName(foo1)", "foo1");
  // foo2 is not unnamed -> its name is not inferred.
  ExpectString("%FunctionGetInferredName(foo2)", "");
  ExpectString("not_ctor()", "foo4");
  ExpectString("Ctor()", "Ctor.foo5");
  ExpectString("%FunctionGetInferredName(obj1.foo6)", "obj1.foo6");
  ExpectString("%FunctionGetInferredName(obj2.foo7)", "obj2.foo7");
  ExpectString("%FunctionGetInferredName(obj3[1])",
               "obj3.(anonymous function)");
  ExpectString("%FunctionGetInferredName(obj4[1])", "");
  ExpectString("%FunctionGetInferredName(obj5['foo9'])", "obj5.foo9");
  ExpectString("%FunctionGetInferredName(obj6.obj7.foo10)", "obj6.obj7.foo10");
}


TEST(FuncNameInferrerTwoByte) {
  // Tests function name inferring in cases where some parts of the inferred
  // function name are two-byte strings.
  i::FLAG_allow_natives_syntax = true;
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope scope(isolate);
  LocalContext env;
  uint16_t* two_byte_source = AsciiToTwoByteString(
      "var obj1 = { oXj2 : { foo1: function() {} } }; "
      "%FunctionGetInferredName(obj1.oXj2.foo1)");
  uint16_t* two_byte_name = AsciiToTwoByteString("obj1.oXj2.foo1");
  // Make it really non-Latin1 (replace the Xs with a non-Latin1 character).
  two_byte_source[14] = two_byte_source[78] = two_byte_name[6] = 0x010d;
  v8::Local<v8::String> source =
      v8::String::NewFromTwoByte(isolate, two_byte_source);
  v8::Local<v8::Value> result = CompileRun(source);
  CHECK(result->IsString());
  v8::Local<v8::String> expected_name =
      v8::String::NewFromTwoByte(isolate, two_byte_name);
  CHECK(result->Equals(expected_name));
  i::DeleteArray(two_byte_source);
  i::DeleteArray(two_byte_name);
}


TEST(FuncNameInferrerEscaped) {
  // The same as FuncNameInferrerTwoByte, except that we express the two-byte
  // character as a unicode escape.
  i::FLAG_allow_natives_syntax = true;
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope scope(isolate);
  LocalContext env;
  uint16_t* two_byte_source = AsciiToTwoByteString(
      "var obj1 = { o\\u010dj2 : { foo1: function() {} } }; "
      "%FunctionGetInferredName(obj1.o\\u010dj2.foo1)");
  uint16_t* two_byte_name = AsciiToTwoByteString("obj1.oXj2.foo1");
  // Fix to correspond to the non-ASCII name in two_byte_source.
  two_byte_name[6] = 0x010d;
  v8::Local<v8::String> source =
      v8::String::NewFromTwoByte(isolate, two_byte_source);
  v8::Local<v8::Value> result = CompileRun(source);
  CHECK(result->IsString());
  v8::Local<v8::String> expected_name =
      v8::String::NewFromTwoByte(isolate, two_byte_name);
  CHECK(result->Equals(expected_name));
  i::DeleteArray(two_byte_source);
  i::DeleteArray(two_byte_name);
}


TEST(RegressionLazyFunctionWithErrorWithArg) {
  // The bug occurred when a lazy function had an error which requires a
  // parameter (such as "unknown label" here). The error message was processed
  // before the AstValueFactory containing the error message string was
  // internalized.
  v8::Isolate* isolate = CcTest::isolate();
  v8::HandleScope scope(isolate);
  LocalContext env;
  i::FLAG_lazy = true;
  i::FLAG_min_preparse_length = 0;
  CompileRun("function this_is_lazy() {\n"
             "  break p;\n"
             "}\n"
             "this_is_lazy();\n");
}


TEST(SerializationOfMaybeAssignmentFlag) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  i::HandleScope scope(isolate);
  LocalContext env;

  const char* src =
      "function h() {"
      "  var result = [];"
      "  function f() {"
      "    result.push(2);"
      "  }"
      "  function assertResult(r) {"
      "    f();"
      "    result = [];"
      "  }"
      "  assertResult([2]);"
      "  assertResult([2]);"
      "  return f;"
      "};"
      "h();";

  i::ScopedVector<char> program(Utf8LengthHelper(src) + 1);
  i::SNPrintF(program, "%s", src);
  i::Handle<i::String> source = factory->InternalizeUtf8String(program.start());
  source->PrintOn(stdout);
  printf("\n");
  i::Zone zone;
  v8::Local<v8::Value> v = CompileRun(src);
  i::Handle<i::Object> o = v8::Utils::OpenHandle(*v);
  i::Handle<i::JSFunction> f = i::Handle<i::JSFunction>::cast(o);
  i::Context* context = f->context();
  i::AstValueFactory avf(&zone, isolate->heap()->HashSeed());
  avf.Internalize(isolate);
  const i::AstRawString* name = avf.GetOneByteString("result");
  i::Handle<i::String> str = name->string();
  CHECK(str->IsInternalizedString());
  i::Scope* script_scope =
      new (&zone) i::Scope(&zone, NULL, i::SCRIPT_SCOPE, &avf);
  script_scope->Initialize();
  i::Scope* s =
      i::Scope::DeserializeScopeChain(isolate, &zone, context, script_scope);
  DCHECK(s != script_scope);
  DCHECK(name != NULL);

  // Get result from h's function context (that is f's context)
  i::Variable* var = s->Lookup(name);

  CHECK(var != NULL);
  // Maybe assigned should survive deserialization
  CHECK(var->maybe_assigned() == i::kMaybeAssigned);
  // TODO(sigurds) Figure out if is_used should survive context serialization.
}


TEST(IfArgumentsArrayAccessedThenParametersMaybeAssigned) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  i::HandleScope scope(isolate);
  LocalContext env;


  const char* src =
      "function f(x) {"
      "    var a = arguments;"
      "    function g(i) {"
      "      ++a[0];"
      "    };"
      "    return g;"
      "  }"
      "f(0);";

  i::ScopedVector<char> program(Utf8LengthHelper(src) + 1);
  i::SNPrintF(program, "%s", src);
  i::Handle<i::String> source = factory->InternalizeUtf8String(program.start());
  source->PrintOn(stdout);
  printf("\n");
  i::Zone zone;
  v8::Local<v8::Value> v = CompileRun(src);
  i::Handle<i::Object> o = v8::Utils::OpenHandle(*v);
  i::Handle<i::JSFunction> f = i::Handle<i::JSFunction>::cast(o);
  i::Context* context = f->context();
  i::AstValueFactory avf(&zone, isolate->heap()->HashSeed());
  avf.Internalize(isolate);

  i::Scope* script_scope =
      new (&zone) i::Scope(&zone, NULL, i::SCRIPT_SCOPE, &avf);
  script_scope->Initialize();
  i::Scope* s =
      i::Scope::DeserializeScopeChain(isolate, &zone, context, script_scope);
  DCHECK(s != script_scope);
  const i::AstRawString* name_x = avf.GetOneByteString("x");

  // Get result from f's function context (that is g's outer context)
  i::Variable* var_x = s->Lookup(name_x);
  CHECK(var_x != NULL);
  CHECK(var_x->maybe_assigned() == i::kMaybeAssigned);
}


TEST(InnerAssignment) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  i::HandleScope scope(isolate);
  LocalContext env;

  const char* prefix = "function f() {";
  const char* midfix = " function g() {";
  const char* suffix = "}}";
  struct { const char* source; bool assigned; bool strict; } outers[] = {
    // Actual assignments.
    { "var x; var x = 5;", true, false },
    { "var x; { var x = 5; }", true, false },
    { "'use strict'; let x; x = 6;", true, true },
    { "var x = 5; function x() {}", true, false },
    // Actual non-assignments.
    { "var x;", false, false },
    { "var x = 5;", false, false },
    { "'use strict'; let x;", false, true },
    { "'use strict'; let x = 6;", false, true },
    { "'use strict'; var x = 0; { let x = 6; }", false, true },
    { "'use strict'; var x = 0; { let x; x = 6; }", false, true },
    { "'use strict'; let x = 0; { let x = 6; }", false, true },
    { "'use strict'; let x = 0; { let x; x = 6; }", false, true },
    { "var x; try {} catch (x) { x = 5; }", false, false },
    { "function x() {}", false, false },
    // Eval approximation.
    { "var x; eval('');", true, false },
    { "eval(''); var x;", true, false },
    { "'use strict'; let x; eval('');", true, true },
    { "'use strict'; eval(''); let x;", true, true },
    // Non-assignments not recognized, because the analysis is approximative.
    { "var x; var x;", true, false },
    { "var x = 5; var x;", true, false },
    { "var x; { var x; }", true, false },
    { "var x; function x() {}", true, false },
    { "function x() {}; var x;", true, false },
    { "var x; try {} catch (x) { var x = 5; }", true, false },
  };
  struct { const char* source; bool assigned; bool with; } inners[] = {
    // Actual assignments.
    { "x = 1;", true, false },
    { "x++;", true, false },
    { "++x;", true, false },
    { "x--;", true, false },
    { "--x;", true, false },
    { "{ x = 1; }", true, false },
    { "'use strict'; { let x; }; x = 0;", true, false },
    { "'use strict'; { const x = 1; }; x = 0;", true, false },
    { "'use strict'; { function x() {} }; x = 0;", true, false },
    { "with ({}) { x = 1; }", true, true },
    { "eval('');", true, false },
    { "'use strict'; { let y; eval('') }", true, false },
    { "function h() { x = 0; }", true, false },
    { "(function() { x = 0; })", true, false },
    { "(function() { x = 0; })", true, false },
    { "with ({}) (function() { x = 0; })", true, true },
    // Actual non-assignments.
    { "", false, false },
    { "x;", false, false },
    { "var x;", false, false },
    { "var x = 8;", false, false },
    { "var x; x = 8;", false, false },
    { "'use strict'; let x;", false, false },
    { "'use strict'; let x = 8;", false, false },
    { "'use strict'; let x; x = 8;", false, false },
    { "'use strict'; const x = 8;", false, false },
    { "function x() {}", false, false },
    { "function x() { x = 0; }", false, false },
    { "function h(x) { x = 0; }", false, false },
    { "'use strict'; { let x; x = 0; }", false, false },
    { "{ var x; }; x = 0;", false, false },
    { "with ({}) {}", false, true },
    { "var x; { with ({}) { x = 1; } }", false, true },
    { "try {} catch(x) { x = 0; }", false, false },
    { "try {} catch(x) { with ({}) { x = 1; } }", false, true },
    // Eval approximation.
    { "eval('');", true, false },
    { "function h() { eval(''); }", true, false },
    { "(function() { eval(''); })", true, false },
    // Shadowing not recognized because of eval approximation.
    { "var x; eval('');", true, false },
    { "'use strict'; let x; eval('');", true, false },
    { "try {} catch(x) { eval(''); }", true, false },
    { "function x() { eval(''); }", true, false },
    { "(function(x) { eval(''); })", true, false },
  };

  // Used to trigger lazy compilation of function
  int comment_len = 2048;
  i::ScopedVector<char> comment(comment_len + 1);
  i::SNPrintF(comment, "/*%0*d*/", comment_len - 4, 0);
  int prefix_len = Utf8LengthHelper(prefix);
  int midfix_len = Utf8LengthHelper(midfix);
  int suffix_len = Utf8LengthHelper(suffix);
  for (unsigned i = 0; i < arraysize(outers); ++i) {
    const char* outer = outers[i].source;
    int outer_len = Utf8LengthHelper(outer);
    for (unsigned j = 0; j < arraysize(inners); ++j) {
      for (unsigned outer_lazy = 0; outer_lazy < 2; ++outer_lazy) {
        for (unsigned inner_lazy = 0; inner_lazy < 2; ++inner_lazy) {
          if (outers[i].strict && inners[j].with) continue;
          const char* inner = inners[j].source;
          int inner_len = Utf8LengthHelper(inner);

          int outer_comment_len = outer_lazy ? comment_len : 0;
          int inner_comment_len = inner_lazy ? comment_len : 0;
          const char* outer_comment = outer_lazy ? comment.start() : "";
          const char* inner_comment = inner_lazy ? comment.start() : "";
          int len = prefix_len + outer_comment_len + outer_len + midfix_len +
                    inner_comment_len + inner_len + suffix_len;
          i::ScopedVector<char> program(len + 1);

          i::SNPrintF(program, "%s%s%s%s%s%s%s", prefix, outer_comment, outer,
                      midfix, inner_comment, inner, suffix);
          i::Handle<i::String> source =
              factory->InternalizeUtf8String(program.start());
          source->PrintOn(stdout);
          printf("\n");

          i::Handle<i::Script> script = factory->NewScript(source);
          i::Zone zone;
          i::ParseInfo info(&zone, script);
          i::Parser parser(&info);
          CHECK(parser.Parse(&info));
          CHECK(i::Compiler::Analyze(&info));
          CHECK(info.function() != NULL);

          i::Scope* scope = info.function()->scope();
          CHECK_EQ(scope->inner_scopes()->length(), 1);
          i::Scope* inner_scope = scope->inner_scopes()->at(0);
          const i::AstRawString* var_name =
              info.ast_value_factory()->GetOneByteString("x");
          i::Variable* var = inner_scope->Lookup(var_name);
          bool expected = outers[i].assigned || inners[j].assigned;
          CHECK(var != NULL);
          CHECK(var->is_used() || !expected);
          CHECK((var->maybe_assigned() == i::kMaybeAssigned) == expected);
        }
      }
    }
  }
}

namespace {

int* global_use_counts = NULL;

void MockUseCounterCallback(v8::Isolate* isolate,
                            v8::Isolate::UseCounterFeature feature) {
  ++global_use_counts[feature];
}

}


TEST(UseAsmUseCount) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::HandleScope scope(isolate);
  LocalContext env;
  int use_counts[v8::Isolate::kUseCounterFeatureCount] = {};
  global_use_counts = use_counts;
  CcTest::isolate()->SetUseCounterCallback(MockUseCounterCallback);
  CompileRun("\"use asm\";\n"
             "var foo = 1;\n"
             "\"use asm\";\n"  // Only the first one counts.
             "function bar() { \"use asm\"; var baz = 1; }");
  // Optimizing will double-count because the source is parsed twice.
  CHECK_EQ(i::FLAG_always_opt ? 4 : 2, use_counts[v8::Isolate::kUseAsm]);
}


TEST(UseConstLegacyCount) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::HandleScope scope(isolate);
  LocalContext env;
  int use_counts[v8::Isolate::kUseCounterFeatureCount] = {};
  global_use_counts = use_counts;
  CcTest::isolate()->SetUseCounterCallback(MockUseCounterCallback);
  CompileRun(
      "const x = 1;\n"
      "var foo = 1;\n"
      "const y = 1;\n"
      "function bar() {\n"
      "    const z = 1; var baz = 1;\n"
      "    function q() { const k = 42; }\n"
      "}");
  // Optimizing will double-count because the source is parsed twice.
  CHECK_EQ(i::FLAG_always_opt ? 8 : 4, use_counts[v8::Isolate::kLegacyConst]);
}


TEST(ErrorsArrowFormalParameters) {
  const char* context_data[][2] = {
    { "()", "=>{}" },
    { "()", "=>{};" },
    { "var x = ()", "=>{}" },
    { "var x = ()", "=>{};" },

    { "a", "=>{}" },
    { "a", "=>{};" },
    { "var x = a", "=>{}" },
    { "var x = a", "=>{};" },

    { "(a)", "=>{}" },
    { "(a)", "=>{};" },
    { "var x = (a)", "=>{}" },
    { "var x = (a)", "=>{};" },

    { "(...a)", "=>{}" },
    { "(...a)", "=>{};" },
    { "var x = (...a)", "=>{}" },
    { "var x = (...a)", "=>{};" },

    { "(a,b)", "=>{}" },
    { "(a,b)", "=>{};" },
    { "var x = (a,b)", "=>{}" },
    { "var x = (a,b)", "=>{};" },

    { "(a,...b)", "=>{}" },
    { "(a,...b)", "=>{};" },
    { "var x = (a,...b)", "=>{}" },
    { "var x = (a,...b)", "=>{};" },

    { nullptr, nullptr }
  };
  const char* assignment_expression_suffix_data[] = {
    "?c:d=>{}",
    "=c=>{}",
    "()",
    "(c)",
    "[1]",
    "[c]",
    ".c",
    "-c",
    "+c",
    "c++",
    "`c`",
    "`${c}`",
    "`template-head${c}`",
    "`${c}template-tail`",
    "`template-head${c}template-tail`",
    "`${c}template-tail`",
    nullptr
  };

  static const ParserFlag always_flags[] = { kAllowHarmonyArrowFunctions,
                                             kAllowHarmonyRestParameters };
  RunParserSyncTest(context_data, assignment_expression_suffix_data, kError,
                    NULL, 0, always_flags, arraysize(always_flags));
}


TEST(ErrorsArrowFunctions) {
  // Tests that parser and preparser generate the same kind of errors
  // on invalid arrow function syntax.
  const char* context_data[][2] = {
    {"", ";"},
    {"v = ", ";"},
    {"bar ? (", ") : baz;"},
    {"bar ? baz : (", ");"},
    {"bar[", "];"},
    {"bar, ", ";"},
    {"", ", bar;"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "=> 0",
    "=>",
    "() =>",
    "=> {}",
    ") => {}",
    ", => {}",
    "(,) => {}",
    "return => {}",
    "() => {'value': 42}",

    // Check that the early return introduced in ParsePrimaryExpression
    // does not accept stray closing parentheses.
    ")",
    ") => 0",
    "foo[()]",
    "()",

    // Parameter lists with extra parens should be recognized as errors.
    "(()) => 0",
    "((x)) => 0",
    "((x, y)) => 0",
    "(x, (y)) => 0",
    "((x, y, z)) => 0",
    "(x, (y, z)) => 0",
    "((x, y), z) => 0",

    // Arrow function formal parameters are parsed as StrictFormalParameters,
    // which confusingly only implies that there are no duplicates.  Words
    // reserved in strict mode, and eval or arguments, are indeed valid in
    // sloppy mode.
    "eval => { 'use strict'; 0 }",
    "arguments => { 'use strict'; 0 }",
    "yield => { 'use strict'; 0 }",
    "interface => { 'use strict'; 0 }",
    "(eval) => { 'use strict'; 0 }",
    "(arguments) => { 'use strict'; 0 }",
    "(yield) => { 'use strict'; 0 }",
    "(interface) => { 'use strict'; 0 }",
    "(eval, bar) => { 'use strict'; 0 }",
    "(bar, eval) => { 'use strict'; 0 }",
    "(bar, arguments) => { 'use strict'; 0 }",
    "(bar, yield) => { 'use strict'; 0 }",
    "(bar, interface) => { 'use strict'; 0 }",
    // TODO(aperez): Detecting duplicates does not work in PreParser.
    // "(bar, bar) => {}",

    // The parameter list is parsed as an expression, but only
    // a comma-separated list of identifier is valid.
    "32 => {}",
    "(32) => {}",
    "(a, 32) => {}",
    "if => {}",
    "(if) => {}",
    "(a, if) => {}",
    "a + b => {}",
    "(a + b) => {}",
    "(a + b, c) => {}",
    "(a, b - c) => {}",
    "\"a\" => {}",
    "(\"a\") => {}",
    "(\"a\", b) => {}",
    "(a, \"b\") => {}",
    "-a => {}",
    "(-a) => {}",
    "(-a, b) => {}",
    "(a, -b) => {}",
    "{} => {}",
    "({}) => {}",
    "(a, {}) => {}",
    "({}, a) => {}",
    "a++ => {}",
    "(a++) => {}",
    "(a++, b) => {}",
    "(a, b++) => {}",
    "[] => {}",
    "([]) => {}",
    "(a, []) => {}",
    "([], a) => {}",
    "(a = b) => {}",
    "(a = b, c) => {}",
    "(a, b = c) => {}",
    "(foo ? bar : baz) => {}",
    "(a, foo ? bar : baz) => {}",
    "(foo ? bar : baz, a) => {}",
    "(a.b, c) => {}",
    "(c, a.b) => {}",
    "(a['b'], c) => {}",
    "(c, a['b']) => {}",
    NULL
  };

  // The test is quite slow, so run it with a reduced set of flags.
  static const ParserFlag flags[] = {kAllowLazy};
  static const ParserFlag always_flags[] = { kAllowHarmonyArrowFunctions };
  RunParserSyncTest(context_data, statement_data, kError, flags,
                    arraysize(flags), always_flags, arraysize(always_flags));
}


TEST(NoErrorsArrowFunctions) {
  // Tests that parser and preparser accept valid arrow functions syntax.
  const char* context_data[][2] = {
    {"", ";"},
    {"bar ? (", ") : baz;"},
    {"bar ? baz : (", ");"},
    {"bar, ", ";"},
    {"", ", bar;"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "() => {}",
    "() => { return 42 }",
    "x => { return x; }",
    "(x) => { return x; }",
    "(x, y) => { return x + y; }",
    "(x, y, z) => { return x + y + z; }",
    "(x, y) => { x.a = y; }",
    "() => 42",
    "x => x",
    "x => x * x",
    "(x) => x",
    "(x) => x * x",
    "(x, y) => x + y",
    "(x, y, z) => x, y, z",
    "(x, y) => x.a = y",
    "() => ({'value': 42})",
    "x => y => x + y",
    "(x, y) => (u, v) => x*u + y*v",
    "(x, y) => z => z * (x + y)",
    "x => (y, z) => z * (x + y)",

    // Those are comma-separated expressions, with arrow functions as items.
    // They stress the code for validating arrow function parameter lists.
    "a, b => 0",
    "a, b, (c, d) => 0",
    "(a, b, (c, d) => 0)",
    "(a, b) => 0, (c, d) => 1",
    "(a, b => {}, a => a + 1)",
    "((a, b) => {}, (a => a + 1))",
    "(a, (a, (b, c) => 0))",

    // Arrow has more precedence, this is the same as: foo ? bar : (baz = {})
    "foo ? bar : baz => {}",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ArrowFunctionsSloppyParameterNames) {
  const char* strong_context_data[][2] = {
    {"'use strong'; ", ";"},
    {"'use strong'; bar ? (", ") : baz;"},
    {"'use strong'; bar ? baz : (", ");"},
    {"'use strong'; bar, ", ";"},
    {"'use strong'; ", ", bar;"},
    {NULL, NULL}
  };

  const char* strict_context_data[][2] = {
    {"'use strict'; ", ";"},
    {"'use strict'; bar ? (", ") : baz;"},
    {"'use strict'; bar ? baz : (", ");"},
    {"'use strict'; bar, ", ";"},
    {"'use strict'; ", ", bar;"},
    {NULL, NULL}
  };

  const char* sloppy_context_data[][2] = {
    {"", ";"},
    {"bar ? (", ") : baz;"},
    {"bar ? baz : (", ");"},
    {"bar, ", ";"},
    {"", ", bar;"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "eval => {}",
    "arguments => {}",
    "yield => {}",
    "interface => {}",
    "(eval) => {}",
    "(arguments) => {}",
    "(yield) => {}",
    "(interface) => {}",
    "(eval, bar) => {}",
    "(bar, eval) => {}",
    "(bar, arguments) => {}",
    "(bar, yield) => {}",
    "(bar, interface) => {}",
    "(interface, eval) => {}",
    "(interface, arguments) => {}",
    "(eval, interface) => {}",
    "(arguments, interface) => {}",
    NULL
  };

  static const ParserFlag always_flags[] = { kAllowHarmonyArrowFunctions,
                                             kAllowStrongMode};
  RunParserSyncTest(strong_context_data, statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperNoErrors) {
  // Tests that parser and preparser accept 'super' keyword in right places.
  const char* context_data[][2] = {
    {"class C { m() { ", "; } }"},
    {"class C { m() { k = ", "; } }"},
    {"class C { m() { foo(", "); } }"},
    {"class C { m() { () => ", "; } }"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "super.x",
    "super[27]",
    "new super.x",
    "new super.x()",
    "new super[27]",
    "new super[27]()",
    "z.super",  // Ok, property lookup.
    NULL
  };

  static const ParserFlag always_flags[] = {
    kAllowHarmonyArrowFunctions,
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperErrors) {
  const char* context_data[][2] = {
    {"class C { m() { ", "; } }"},
    {"class C { m() { k = ", "; } }"},
    {"class C { m() { foo(", "); } }"},
    {"class C { m() { () => ", "; } }"},
    {NULL, NULL}
  };

  const char* expression_data[] = {
    "super",
    "super = x",
    "y = super",
    "f(super)",
    "new super",
    "new super()",
    "new super(12, 45)",
    "new new super",
    "new new super()",
    "new new super()()",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, expression_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperCall) {
  const char* context_data[][2] = {{"", ""},
                                   {NULL, NULL}};

  const char* success_data[] = {
    "class C extends B { constructor() { super(); } }",
    "class C extends B { constructor() { () => super(); } }",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions,
                                            kAllowHarmonySloppy};
  RunParserSyncTest(context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));

  const char* error_data[] = {
    "class C { constructor() { super(); } }",
    "class C { method() { super(); } }",
    "class C { method() { () => super(); } }",
    "class C { *method() { super(); } }",
    "class C { get x() { super(); } }",
    "class C { set x(_) { super(); } }",
    "({ method() { super(); } })",
    "({ *method() { super(); } })",
    "({ get x() { super(); } })",
    "({ set x(_) { super(); } })",
    "({ f: function() { super(); } })",
    "(function() { super(); })",
    "var f = function() { super(); }",
    "({ f: function*() { super(); } })",
    "(function*() { super(); })",
    "var f = function*() { super(); }",
    NULL
  };

  RunParserSyncTest(context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperNewNoErrors) {
  const char* context_data[][2] = {
    {"class C { constructor() { ", " } }"},
    {"class C { *method() { ", " } }"},
    {"class C { get x() { ", " } }"},
    {"class C { set x(_) { ", " } }"},
    {"({ method() { ", " } })"},
    {"({ *method() { ", " } })"},
    {"({ get x() { ", " } })"},
    {"({ set x(_) { ", " } })"},
    {NULL, NULL}
  };

  const char* expression_data[] = {
    "new super.x;",
    "new super.x();",
    "() => new super.x;",
    "() => new super.x();",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions,
                                            kAllowHarmonySloppy};
  RunParserSyncTest(context_data, expression_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperNewErrors) {
  const char* context_data[][2] = {
    {"class C { method() { ", " } }"},
    {"class C { *method() { ", " } }"},
    {"class C { get x() { ", " } }"},
    {"class C { set x(_) { ", " } }"},
    {"({ method() { ", " } })"},
    {"({ *method() { ", " } })"},
    {"({ get x() { ", " } })"},
    {"({ set x(_) { ", " } })"},
    {"({ f: function() { ", " } })"},
    {"(function() { ", " })"},
    {"var f = function() { ", " }"},
    {"({ f: function*() { ", " } })"},
    {"(function*() { ", " })"},
    {"var f = function*() { ", " }"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "new super;",
    "new super();",
    "() => new super;",
    "() => new super();",
    NULL
  };

  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions,
                                            kAllowHarmonySloppy};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(SuperErrorsNonMethods) {
  // super is only allowed in methods, accessors and constructors.
  const char* context_data[][2] = {
    {"", ";"},
    {"k = ", ";"},
    {"foo(", ");"},
    {"if (", ") {}"},
    {"if (true) {", "}"},
    {"if (false) {} else {", "}"},
    {"while (true) {", "}"},
    {"function f() {", "}"},
    {"class C extends (", ") {}"},
    {"class C { m() { function f() {", "} } }"},
    {"({ m() { function f() {", "} } })"},
    {NULL, NULL}
  };

  const char* statement_data[] = {
    "super",
    "super = x",
    "y = super",
    "f(super)",
    "super.x",
    "super[27]",
    "super.x()",
    "super[27]()",
    "super()",
    "new super.x",
    "new super.x()",
    "new super[27]",
    "new super[27]()",
    NULL
  };

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(NoErrorsMethodDefinition) {
  const char* context_data[][2] = {{"({", "});"},
                                   {"'use strict'; ({", "});"},
                                   {"({*", "});"},
                                   {"'use strict'; ({*", "});"},
                                   {NULL, NULL}};

  const char* object_literal_body_data[] = {
    "m() {}",
    "m(x) { return x; }",
    "m(x, y) {}, n() {}",
    "set(x, y) {}",
    "get(x, y) {}",
    NULL
  };

  RunParserSyncTest(context_data, object_literal_body_data, kSuccess);
}


TEST(MethodDefinitionNames) {
  const char* context_data[][2] = {{"({", "(x, y) {}});"},
                                   {"'use strict'; ({", "(x, y) {}});"},
                                   {"({*", "(x, y) {}});"},
                                   {"'use strict'; ({*", "(x, y) {}});"},
                                   {NULL, NULL}};

  const char* name_data[] = {
    "m",
    "'m'",
    "\"m\"",
    "\"m n\"",
    "true",
    "false",
    "null",
    "0",
    "1.2",
    "1e1",
    "1E1",
    "1e+1",
    "1e-1",

    // Keywords
    "async",
    "await",
    "break",
    "case",
    "catch",
    "class",
    "const",
    "continue",
    "debugger",
    "default",
    "delete",
    "do",
    "else",
    "enum",
    "export",
    "extends",
    "finally",
    "for",
    "function",
    "if",
    "implements",
    "import",
    "in",
    "instanceof",
    "interface",
    "let",
    "new",
    "package",
    "private",
    "protected",
    "public",
    "return",
    "static",
    "super",
    "switch",
    "this",
    "throw",
    "try",
    "typeof",
    "var",
    "void",
    "while",
    "with",
    "yield",
    NULL
  };

  RunParserSyncTest(context_data, name_data, kSuccess);
}


TEST(MethodDefinitionStrictFormalParamereters) {
  const char* context_data[][2] = {{"({method(", "){}});"},
                                   {"'use strict'; ({method(", "){}});"},
                                   {"({*method(", "){}});"},
                                   {"'use strict'; ({*method(", "){}});"},
                                   {NULL, NULL}};

  const char* params_data[] = {
    "x, x",
    "x, y, x",
    "var",
    "const",
    NULL
  };

  RunParserSyncTest(context_data, params_data, kError);
}


TEST(MethodDefinitionEvalArguments) {
  const char* strict_context_data[][2] =
      {{"'use strict'; ({method(", "){}});"},
       {"'use strict'; ({*method(", "){}});"},
       {NULL, NULL}};
  const char* sloppy_context_data[][2] =
      {{"({method(", "){}});"},
       {"({*method(", "){}});"},
       {NULL, NULL}};

  const char* data[] = {
      "eval",
      "arguments",
      NULL};

  // Fail in strict mode
  RunParserSyncTest(strict_context_data, data, kError);

  // OK in sloppy mode
  RunParserSyncTest(sloppy_context_data, data, kSuccess);
}


TEST(MethodDefinitionDuplicateEvalArguments) {
  const char* context_data[][2] =
      {{"'use strict'; ({method(", "){}});"},
       {"'use strict'; ({*method(", "){}});"},
       {"({method(", "){}});"},
       {"({*method(", "){}});"},
       {NULL, NULL}};

  const char* data[] = {
      "eval, eval",
      "eval, a, eval",
      "arguments, arguments",
      "arguments, a, arguments",
      NULL};

  // In strict mode, the error is using "eval" or "arguments" as parameter names
  // In sloppy mode, the error is that eval / arguments are duplicated
  RunParserSyncTest(context_data, data, kError);
}


TEST(MethodDefinitionDuplicateProperty) {
  const char* context_data[][2] = {{"'use strict'; ({", "});"},
                                   {NULL, NULL}};

  const char* params_data[] = {
    "x: 1, x() {}",
    "x() {}, x: 1",
    "x() {}, get x() {}",
    "x() {}, set x(_) {}",
    "x() {}, x() {}",
    "x() {}, y() {}, x() {}",
    "x() {}, \"x\"() {}",
    "x() {}, 'x'() {}",
    "0() {}, '0'() {}",
    "1.0() {}, 1: 1",

    "x: 1, *x() {}",
    "*x() {}, x: 1",
    "*x() {}, get x() {}",
    "*x() {}, set x(_) {}",
    "*x() {}, *x() {}",
    "*x() {}, y() {}, *x() {}",
    "*x() {}, *\"x\"() {}",
    "*x() {}, *'x'() {}",
    "*0() {}, *'0'() {}",
    "*1.0() {}, 1: 1",

    NULL
  };

  RunParserSyncTest(context_data, params_data, kSuccess);
}


TEST(ClassExpressionNoErrors) {
  const char* context_data[][2] = {{"(", ");"},
                                   {"var C = ", ";"},
                                   {"bar, ", ";"},
                                   {NULL, NULL}};
  const char* class_data[] = {
    "class {}",
    "class name {}",
    "class extends F {}",
    "class name extends F {}",
    "class extends (F, G) {}",
    "class name extends (F, G) {}",
    "class extends class {} {}",
    "class name extends class {} {}",
    "class extends class base {} {}",
    "class name extends class base {} {}",
    NULL};

  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, class_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassDeclarationNoErrors) {
  const char* context_data[][2] = {{"'use strict'; ", ""},
                                   {"'use strict'; {", "}"},
                                   {"'use strict'; if (true) {", "}"},
                                   {NULL, NULL}};
  const char* statement_data[] = {
    "class name {}",
    "class name extends F {}",
    "class name extends (F, G) {}",
    "class name extends class {} {}",
    "class name extends class base {} {}",
    NULL};

  RunParserSyncTest(context_data, statement_data, kSuccess);
}


TEST(ClassBodyNoErrors) {
  // Tests that parser and preparser accept valid class syntax.
  const char* context_data[][2] = {{"(class {", "});"},
                                   {"(class extends Base {", "});"},
                                   {"class C {", "}"},
                                   {"class C extends Base {", "}"},
                                   {NULL, NULL}};
  const char* class_body_data[] = {
    ";",
    ";;",
    "m() {}",
    "m() {};",
    "; m() {}",
    "m() {}; n(x) {}",
    "get x() {}",
    "set x(v) {}",
    "get() {}",
    "set() {}",
    "*g() {}",
    "*g() {};",
    "; *g() {}",
    "*g() {}; *h(x) {}",
    "static() {}",
    "static m() {}",
    "static get x() {}",
    "static set x(v) {}",
    "static get() {}",
    "static set() {}",
    "static static() {}",
    "static get static() {}",
    "static set static(v) {}",
    "*static() {}",
    "*get() {}",
    "*set() {}",
    "static *g() {}",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassPropertyNameNoErrors) {
  const char* context_data[][2] = {{"(class {", "() {}});"},
                                   {"(class { get ", "() {}});"},
                                   {"(class { set ", "(v) {}});"},
                                   {"(class { static ", "() {}});"},
                                   {"(class { static get ", "() {}});"},
                                   {"(class { static set ", "(v) {}});"},
                                   {"(class { *", "() {}});"},
                                   {"(class { static *", "() {}});"},
                                   {"class C {", "() {}}"},
                                   {"class C { get ", "() {}}"},
                                   {"class C { set ", "(v) {}}"},
                                   {"class C { static ", "() {}}"},
                                   {"class C { static get ", "() {}}"},
                                   {"class C { static set ", "(v) {}}"},
                                   {"class C { *", "() {}}"},
                                   {"class C { static *", "() {}}"},
                                   {NULL, NULL}};
  const char* name_data[] = {
    "42",
    "42.5",
    "42e2",
    "42e+2",
    "42e-2",
    "null",
    "false",
    "true",
    "'str'",
    "\"str\"",
    "static",
    "get",
    "set",
    "var",
    "const",
    "let",
    "this",
    "class",
    "function",
    "yield",
    "if",
    "else",
    "for",
    "while",
    "do",
    "try",
    "catch",
    "finally",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, name_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassExpressionErrors) {
  const char* context_data[][2] = {{"(", ");"},
                                   {"var C = ", ";"},
                                   {"bar, ", ";"},
                                   {NULL, NULL}};
  const char* class_data[] = {
    "class",
    "class name",
    "class name extends",
    "class extends",
    "class {",
    "class { m }",
    "class { m; n }",
    "class { m: 1 }",
    "class { m(); n() }",
    "class { get m }",
    "class { get m() }",
    "class { get m() { }",
    "class { set m() {} }",  // Missing required parameter.
    "class { m() {}, n() {} }",  // No commas allowed.
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassDeclarationErrors) {
  const char* context_data[][2] = {{"", ""},
                                   {"{", "}"},
                                   {"if (true) {", "}"},
                                   {NULL, NULL}};
  const char* class_data[] = {
    "class",
    "class name",
    "class name extends",
    "class extends",
    "class name {",
    "class name { m }",
    "class name { m; n }",
    "class name { m: 1 }",
    "class name { m(); n() }",
    "class name { get x }",
    "class name { get x() }",
    "class name { set x() {) }",  // missing required param
    "class {}",  // Name is required for declaration
    "class extends base {}",
    "class name { *",
    "class name { * }",
    "class name { *; }",
    "class name { *get x() {} }",
    "class name { *set x(_) {} }",
    "class name { *static m() {} }",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassNameErrors) {
  const char* context_data[][2] = {{"class ", "{}"},
                                   {"(class ", "{});"},
                                   {"'use strict'; class ", "{}"},
                                   {"'use strict'; (class ", "{});"},
                                   {NULL, NULL}};
  const char* class_name[] = {
    "arguments",
    "eval",
    "implements",
    "interface",
    "let",
    "package",
    "private",
    "protected",
    "public",
    "static",
    "var",
    "yield",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_name, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassGetterParamNameErrors) {
  const char* context_data[][2] = {
    {"class C { get name(", ") {} }"},
    {"(class { get name(", ") {} });"},
    {"'use strict'; class C { get name(", ") {} }"},
    {"'use strict'; (class { get name(", ") {} })"},
    {NULL, NULL}
  };

  const char* class_name[] = {
    "arguments",
    "eval",
    "implements",
    "interface",
    "let",
    "package",
    "private",
    "protected",
    "public",
    "static",
    "var",
    "yield",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_name, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassStaticPrototypeErrors) {
  const char* context_data[][2] = {{"class C {", "}"},
                                   {"(class {", "});"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "static prototype() {}",
    "static get prototype() {}",
    "static set prototype(_) {}",
    "static *prototype() {}",
    "static 'prototype'() {}",
    "static *'prototype'() {}",
    "static prot\\u006ftype() {}",
    "static 'prot\\u006ftype'() {}",
    "static get 'prot\\u006ftype'() {}",
    "static set 'prot\\u006ftype'(_) {}",
    "static *'prot\\u006ftype'() {}",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassSpecialConstructorErrors) {
  const char* context_data[][2] = {{"class C {", "}"},
                                   {"(class {", "});"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "get constructor() {}",
    "get constructor(_) {}",
    "*constructor() {}",
    "get 'constructor'() {}",
    "*'constructor'() {}",
    "get c\\u006fnstructor() {}",
    "*c\\u006fnstructor() {}",
    "get 'c\\u006fnstructor'() {}",
    "get 'c\\u006fnstructor'(_) {}",
    "*'c\\u006fnstructor'() {}",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassConstructorNoErrors) {
  const char* context_data[][2] = {{"class C {", "}"},
                                   {"(class {", "});"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "constructor() {}",
    "static constructor() {}",
    "static get constructor() {}",
    "static set constructor(_) {}",
    "static *constructor() {}",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassMultipleConstructorErrors) {
  const char* context_data[][2] = {{"class C {", "}"},
                                   {"(class {", "});"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "constructor() {}; constructor() {}",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassMultiplePropertyNamesNoErrors) {
  const char* context_data[][2] = {{"class C {", "}"},
                                   {"(class {", "});"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "constructor() {}; static constructor() {}",
    "m() {}; static m() {}",
    "m() {}; m() {}",
    "static m() {}; static m() {}",
    "get m() {}; set m(_) {}; get m() {}; set m(_) {};",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ClassesAreStrictErrors) {
  const char* context_data[][2] = {{"", ""},
                                   {"(", ");"},
                                   {NULL, NULL}};

  const char* class_body_data[] = {
    "class C { method() { with ({}) {} } }",
    "class C extends function() { with ({}) {} } {}",
    "class C { *method() { with ({}) {} } }",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonySloppy
  };
  RunParserSyncTest(context_data, class_body_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ObjectLiteralPropertyShorthandKeywordsError) {
  const char* context_data[][2] = {{"({", "});"},
                                   {"'use strict'; ({", "});"},
                                   {NULL, NULL}};

  const char* name_data[] = {
    "break",
    "case",
    "catch",
    "class",
    "const",
    "continue",
    "debugger",
    "default",
    "delete",
    "do",
    "else",
    "enum",
    "export",
    "extends",
    "false",
    "finally",
    "for",
    "function",
    "if",
    "import",
    "in",
    "instanceof",
    "new",
    "null",
    "return",
    "super",
    "switch",
    "this",
    "throw",
    "true",
    "try",
    "typeof",
    "var",
    "void",
    "while",
    "with",
    NULL
  };

  RunParserSyncTest(context_data, name_data, kError);
}


TEST(ObjectLiteralPropertyShorthandStrictKeywords) {
  const char* context_data[][2] = {{"({", "});"},
                                   {NULL, NULL}};

  const char* name_data[] = {
    "implements",
    "interface",
    "let",
    "package",
    "private",
    "protected",
    "public",
    "static",
    "yield",
    NULL
  };

  RunParserSyncTest(context_data, name_data, kSuccess);

  const char* context_strict_data[][2] = {{"'use strict'; ({", "});"},
                                          {NULL, NULL}};
  RunParserSyncTest(context_strict_data, name_data, kError);
}


TEST(ObjectLiteralPropertyShorthandError) {
  const char* context_data[][2] = {{"({", "});"},
                                   {"'use strict'; ({", "});"},
                                   {NULL, NULL}};

  const char* name_data[] = {
    "1",
    "1.2",
    "0",
    "0.1",
    "1.0",
    "1e1",
    "0x1",
    "\"s\"",
    "'s'",
    NULL
  };

  RunParserSyncTest(context_data, name_data, kError);
}


TEST(ObjectLiteralPropertyShorthandYieldInGeneratorError) {
  const char* context_data[][2] = {{"", ""},
                                   {NULL, NULL}};

  const char* name_data[] = {
    "function* g() { ({yield}); }",
    NULL
  };

  RunParserSyncTest(context_data, name_data, kError);
}


TEST(ConstParsingInForIn) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for(const x = 1; ; ) {}",
      "for(const x = 1, y = 2;;){}",
      "for(const x in [1,2,3]) {}",
      "for(const x of [1,2,3]) {}",
      NULL};
  RunParserSyncTest(context_data, data, kSuccess, nullptr, 0, nullptr, 0);
}


TEST(ConstParsingInForInError) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for(const x,y = 1; ; ) {}",
      "for(const x = 4 in [1,2,3]) {}",
      "for(const x = 4, y in [1,2,3]) {}",
      "for(const x = 4 of [1,2,3]) {}",
      "for(const x = 4, y of [1,2,3]) {}",
      "for(const x = 1, y = 2 in []) {}",
      "for(const x,y in []) {}",
      "for(const x = 1, y = 2 of []) {}",
      "for(const x,y of []) {}",
      NULL};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, nullptr, 0);
}


TEST(InitializedDeclarationsInStrictForInError) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i = 1 in {}) {}",
      "for (var i = void 0 in [1, 2, 3]) {}",
      "for (let i = 1 in {}) {}",
      "for (let i = void 0 in [1, 2, 3]) {}",
      "for (const i = 1 in {}) {}",
      "for (const i = void 0 in [1, 2, 3]) {}",
      NULL};
  RunParserSyncTest(context_data, data, kError);
}


TEST(InitializedDeclarationsInStrictForOfError) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i = 1 of {}) {}",
      "for (var i = void 0 of [1, 2, 3]) {}",
      "for (let i = 1 of {}) {}",
      "for (let i = void 0 of [1, 2, 3]) {}",
      "for (const i = 1 of {}) {}",
      "for (const i = void 0 of [1, 2, 3]) {}",
      NULL};
  RunParserSyncTest(context_data, data, kError);
}


TEST(InitializedDeclarationsInSloppyForInError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i = 1 in {}) {}",
      "for (var i = void 0 in [1, 2, 3]) {}",
      NULL};
  // TODO(caitp): This should be an error in sloppy mode.
  RunParserSyncTest(context_data, data, kSuccess);
}


TEST(InitializedDeclarationsInSloppyForOfError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i = 1 of {}) {}",
      "for (var i = void 0 of [1, 2, 3]) {}",
      NULL};
  RunParserSyncTest(context_data, data, kError);
}


TEST(ForInMultipleDeclarationsError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i, j in {}) {}",
      "for (var i, j in [1, 2, 3]) {}",
      "for (var i, j = 1 in {}) {}",
      "for (var i, j = void 0 in [1, 2, 3]) {}",

      "for (let i, j in {}) {}",
      "for (let i, j in [1, 2, 3]) {}",
      "for (let i, j = 1 in {}) {}",
      "for (let i, j = void 0 in [1, 2, 3]) {}",

      "for (const i, j in {}) {}",
      "for (const i, j in [1, 2, 3]) {}",
      "for (const i, j = 1 in {}) {}",
      "for (const i, j = void 0 in [1, 2, 3]) {}",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, always_flags,
                    arraysize(always_flags));
}


TEST(ForOfMultipleDeclarationsError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var i, j of {}) {}",
      "for (var i, j of [1, 2, 3]) {}",
      "for (var i, j = 1 of {}) {}",
      "for (var i, j = void 0 of [1, 2, 3]) {}",

      "for (let i, j of {}) {}",
      "for (let i, j of [1, 2, 3]) {}",
      "for (let i, j = 1 of {}) {}",
      "for (let i, j = void 0 of [1, 2, 3]) {}",

      "for (const i, j of {}) {}",
      "for (const i, j of [1, 2, 3]) {}",
      "for (const i, j = 1 of {}) {}",
      "for (const i, j = void 0 of [1, 2, 3]) {}",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, always_flags,
                    arraysize(always_flags));
}


TEST(ForInNoDeclarationsError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var in {}) {}",
      "for (const in {}) {}",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, always_flags,
                    arraysize(always_flags));
}


TEST(ForOfNoDeclarationsError) {
  const char* context_data[][2] = {{"", ""},
                                   {"function foo(){", "}"},
                                   {"'use strict';", ""},
                                   {"function foo(){ 'use strict';", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "for (var of [1, 2, 3]) {}",
      "for (const of [1, 2, 3]) {}",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, always_flags,
                    arraysize(always_flags));
}


TEST(InvalidUnicodeEscapes) {
  const char* context_data[][2] = {{"", ""},
                                   {"'use strict';", ""},
                                   {NULL, NULL}};
  const char* data[] = {
    "var foob\\u123r = 0;",
    "var \\u123roo = 0;",
    "\"foob\\u123rr\"",
    // No escapes allowed in regexp flags
    "/regex/\\u0069g",
    "/regex/\\u006g",
    // Braces gone wrong
    "var foob\\u{c481r = 0;",
    "var foob\\uc481}r = 0;",
    "var \\u{0052oo = 0;",
    "var \\u0052}oo = 0;",
    "\"foob\\u{c481r\"",
    "var foob\\u{}ar = 0;",
    // Too high value for the unicode escape
    "\"\\u{110000}\"",
    // Not an unicode escape
    "var foob\\v1234r = 0;",
    "var foob\\U1234r = 0;",
    "var foob\\v{1234}r = 0;",
    "var foob\\U{1234}r = 0;",
    NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonyUnicode};
  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(UnicodeEscapes) {
  const char* context_data[][2] = {{"", ""},
                                   {"'use strict';", ""},
                                   {NULL, NULL}};
  const char* data[] = {
    // Identifier starting with escape
    "var \\u0052oo = 0;",
    "var \\u{0052}oo = 0;",
    "var \\u{52}oo = 0;",
    "var \\u{00000000052}oo = 0;",
    // Identifier with an escape but not starting with an escape
    "var foob\\uc481r = 0;",
    "var foob\\u{c481}r = 0;",
    // String with an escape
    "\"foob\\uc481r\"",
    "\"foob\\{uc481}r\"",
    // This character is a valid unicode character, representable as a surrogate
    // pair, not representable as 4 hex digits.
    "\"foo\\u{10e6d}\"",
    // Max value for the unicode escape
    "\"\\u{10ffff}\"",
    NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonyUnicode};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(ScanTemplateLiterals) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';"
                                    "  var a, b, c; return ", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "``",
      "`no-subst-template`",
      "`template-head${a}`",
      "`${a}`",
      "`${a}template-tail`",
      "`template-head${a}template-tail`",
      "`${a}${b}${c}`",
      "`a${a}b${b}c${c}`",
      "`${a}a${b}b${c}c`",
      "`foo\n\nbar\r\nbaz`",
      "`foo\n\n${  bar  }\r\nbaz`",
      "`foo${a /* comment */}`",
      "`foo${a // comment\n}`",
      "`foo${a \n}`",
      "`foo${a \r\n}`",
      "`foo${a \r}`",
      "`foo${/* comment */ a}`",
      "`foo${// comment\na}`",
      "`foo${\n a}`",
      "`foo${\r\n a}`",
      "`foo${\r a}`",
      "`foo${'a' in a}`",
      NULL};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, NULL, 0);
}


TEST(ScanTaggedTemplateLiterals) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';"
                                    "  function tag() {}"
                                    "  var a, b, c; return ", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "tag ``",
      "tag `no-subst-template`",
      "tag`template-head${a}`",
      "tag `${a}`",
      "tag `${a}template-tail`",
      "tag   `template-head${a}template-tail`",
      "tag\n`${a}${b}${c}`",
      "tag\r\n`a${a}b${b}c${c}`",
      "tag    `${a}a${b}b${c}c`",
      "tag\t`foo\n\nbar\r\nbaz`",
      "tag\r`foo\n\n${  bar  }\r\nbaz`",
      "tag`foo${a /* comment */}`",
      "tag`foo${a // comment\n}`",
      "tag`foo${a \n}`",
      "tag`foo${a \r\n}`",
      "tag`foo${a \r}`",
      "tag`foo${/* comment */ a}`",
      "tag`foo${// comment\na}`",
      "tag`foo${\n a}`",
      "tag`foo${\r\n a}`",
      "tag`foo${\r a}`",
      "tag`foo${'a' in a}`",
      NULL};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, NULL, 0);
}


TEST(TemplateMaterializedLiterals) {
  const char* context_data[][2] = {
    {
      "'use strict';\n"
      "function tag() {}\n"
      "var a, b, c;\n"
      "(", ")"
    },
    {NULL, NULL}
  };

  const char* data[] = {
    "tag``",
    "tag`a`",
    "tag`a${1}b`",
    "tag`a${1}b${2}c`",
    "``",
    "`a`",
    "`a${1}b`",
    "`a${1}b${2}c`",
    NULL
  };

  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, NULL, 0);
}


TEST(ScanUnterminatedTemplateLiterals) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';"
                                    "  var a, b, c; return ", "}"},
                                   {NULL, NULL}};

  const char* data[] = {
      "`no-subst-template",
      "`template-head${a}",
      "`${a}template-tail",
      "`template-head${a}template-tail",
      "`${a}${b}${c}",
      "`a${a}b${b}c${c}",
      "`${a}a${b}b${c}c",
      "`foo\n\nbar\r\nbaz",
      "`foo\n\n${  bar  }\r\nbaz",
      "`foo${a /* comment } */`",
      "`foo${a /* comment } `*/",
      "`foo${a // comment}`",
      "`foo${a \n`",
      "`foo${a \r\n`",
      "`foo${a \r`",
      "`foo${/* comment */ a`",
      "`foo${// commenta}`",
      "`foo${\n a`",
      "`foo${\r\n a`",
      "`foo${\r a`",
      "`foo${fn(}`",
      "`foo${1 if}`",
      NULL};
  RunParserSyncTest(context_data, data, kError, NULL, 0, NULL, 0);
}


TEST(TemplateLiteralsIllegalTokens) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function foo(){ 'use strict';"
                                    "  var a, b, c; return ", "}"},
                                   {NULL, NULL}};
  const char* data[] = {
      "`hello\\x`",
      "`hello\\x${1}`",
      "`hello${1}\\x`",
      "`hello${1}\\x${2}`",
      "`hello\\x\n`",
      "`hello\\x\n${1}`",
      "`hello${1}\\x\n`",
      "`hello${1}\\x\n${2}`",
      NULL};

  RunParserSyncTest(context_data, data, kError, NULL, 0, NULL, 0);
}


TEST(ParseRestParameters) {
  const char* context_data[][2] = {{"'use strict';(function(",
                                    "){ return args;})(1, [], /regexp/, 'str',"
                                    "function(){});"},
                                   {"(function(", "){ return args;})(1, [],"
                                    "/regexp/, 'str', function(){});"},
                                  {NULL, NULL}};

  const char* data[] = {
    "...args",
    "a, ...args",
    "...   args",
    "a, ...   args",
    "...\targs",
    "a, ...\targs",
    "...\r\nargs",
    "a, ...\r\nargs",
    "...\rargs",
    "a, ...\rargs",
    "...\t\n\t\t\n  args",
    "a, ...  \n  \n  args",
    NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonyRestParameters};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(ParseRestParametersErrors) {
  const char* context_data[][2] = {{"'use strict';(function(",
                                    "){ return args;}(1, [], /regexp/, 'str',"
                                    "function(){});"},
                                   {"(function(", "){ return args;}(1, [],"
                                    "/regexp/, 'str', function(){});"},
                                   {NULL, NULL}};

  const char* data[] = {
      "...args, b",
      "a, ...args, b",
      "...args,   b",
      "a, ...args,   b",
      "...args,\tb",
      "a,...args\t,b",
      "...args\r\n, b",
      "a, ... args,\r\nb",
      "...args\r,b",
      "a, ... args,\rb",
      "...args\t\n\t\t\n,  b",
      "a, ... args,  \n  \n  b",
      "a, a, ...args",
      "a,\ta, ...args",
      "a,\ra, ...args",
      "a,\na, ...args",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonyRestParameters};
  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(RestParameterInSetterMethodError) {
  const char* context_data[][2] = {
      {"'use strict';({ set prop(", ") {} }).prop = 1;"},
      {"'use strict';(class { static set prop(", ") {} }).prop = 1;"},
      {"'use strict';(new (class { set prop(", ") {} })).prop = 1;"},
      {"({ set prop(", ") {} }).prop = 1;"},
      {"(class { static set prop(", ") {} }).prop = 1;"},
      {"(new (class { set prop(", ") {} })).prop = 1;"},
      {nullptr, nullptr}};
  const char* data[] = {"...a", "...arguments", "...eval", nullptr};

  static const ParserFlag always_flags[] = {kAllowHarmonyRestParameters,
                                            kAllowHarmonySloppy};
  RunParserSyncTest(context_data, data, kError, nullptr, 0, always_flags,
                    arraysize(always_flags));
}


TEST(RestParametersEvalArguments) {
  const char* strict_context_data[][2] =
      {{"'use strict';(function(",
        "){ return;})(1, [], /regexp/, 'str',function(){});"},
       {NULL, NULL}};
  const char* sloppy_context_data[][2] =
      {{"(function(",
        "){ return;})(1, [],/regexp/, 'str', function(){});"},
       {NULL, NULL}};

  const char* data[] = {
      "...eval",
      "eval, ...args",
      "...arguments",
      "arguments, ...args",
      NULL};

  static const ParserFlag always_flags[] = {kAllowHarmonyRestParameters};

  // Fail in strict mode
  RunParserSyncTest(strict_context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));

  // OK in sloppy mode
  RunParserSyncTest(sloppy_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(RestParametersDuplicateEvalArguments) {
  const char* context_data[][2] =
      {{"'use strict';(function(",
        "){ return;})(1, [], /regexp/, 'str',function(){});"},
       {"(function(",
        "){ return;})(1, [],/regexp/, 'str', function(){});"},
       {NULL, NULL}};

  const char* data[] = {
      "eval, ...eval",
      "eval, eval, ...args",
      "arguments, ...arguments",
      "arguments, arguments, ...args",
      NULL};

  static const ParserFlag always_flags[] = {kAllowHarmonyRestParameters};

  // In strict mode, the error is using "eval" or "arguments" as parameter names
  // In sloppy mode, the error is that eval / arguments are duplicated
  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(SpreadCall) {
  const char* context_data[][2] = {{"function fn() { 'use strict';} fn(", ");"},
                                   {"function fn() {} fn(", ");"},
                                   {NULL, NULL}};

  const char* data[] = {
      "...([1, 2, 3])", "...'123', ...'456'", "...new Set([1, 2, 3]), 4",
      "1, ...[2, 3], 4", "...Array(...[1,2,3,4])", "...NaN",
      "0, 1, ...[2, 3, 4], 5, 6, 7, ...'89'",
      "0, 1, ...[2, 3, 4], 5, 6, 7, ...'89', 10",
      "...[0, 1, 2], 3, 4, 5, 6, ...'7', 8, 9",
      "...[0, 1, 2], 3, 4, 5, 6, ...'7', 8, 9, ...[10]", NULL};

  static const ParserFlag always_flags[] = {kAllowHarmonySpreadCalls};

  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(SpreadCallErrors) {
  const char* context_data[][2] = {{"function fn() { 'use strict';} fn(", ");"},
                                   {"function fn() {} fn(", ");"},
                                   {NULL, NULL}};

  const char* data[] = {"(...[1, 2, 3])", "......[1,2,3]", NULL};

  static const ParserFlag always_flags[] = {kAllowHarmonySpreadCalls};

  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(BadRestSpread) {
  const char* context_data[][2] = {{"function fn() { 'use strict';", "} fn();"},
                                   {"function fn() { ", "} fn();"},
                                   {NULL, NULL}};
  const char* data[] = {"return ...[1,2,3];",     "var ...x = [1,2,3];",
                        "var [...x,] = [1,2,3];", "var [...x, y] = [1,2,3];",
                        "var {...x} = [1,2,3];",  "var { x } = {x: ...[1,2,3]}",
                        NULL};
  RunParserSyncTest(context_data, data, kError, NULL, 0, NULL, 0);
}


TEST(LexicalScopingSloppyMode) {
  const char* context_data[][2] = {
      {"", ""},
      {"function f() {", "}"},
      {"{", "}"},
      {NULL, NULL}};
  const char* bad_data[] = {
    "let x = 1;",
    "for(let x = 1;;){}",
    "for(let x of []){}",
    "for(let x in []){}",
    "class C {}",
    "class C extends D {}",
    "(class {})",
    "(class extends D {})",
    "(class C {})",
    "(class C extends D {})",
    NULL};
  static const ParserFlag always_false_flags[] = {kAllowHarmonySloppy};
  RunParserSyncTest(context_data, bad_data, kError, NULL, 0, NULL, 0,
                    always_false_flags, arraysize(always_false_flags));

  const char* good_data[] = {
    "let = 1;",
    "for(let = 1;;){}",
    NULL};
  RunParserSyncTest(context_data, good_data, kSuccess, NULL, 0, NULL, 0,
                    always_false_flags, arraysize(always_false_flags));
}


TEST(ComputedPropertyName) {
  const char* context_data[][2] = {{"({[", "]: 1});"},
                                   {"({get [", "]() {}});"},
                                   {"({set [", "](_) {}});"},
                                   {"({[", "]() {}});"},
                                   {"({*[", "]() {}});"},
                                   {"(class {get [", "]() {}});"},
                                   {"(class {set [", "](_) {}});"},
                                   {"(class {[", "]() {}});"},
                                   {"(class {*[", "]() {}});"},
                                   {NULL, NULL}};
  const char* error_data[] = {
    "1, 2",
    "var name",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonyComputedPropertyNames,
    kAllowHarmonySloppy,
  };
  RunParserSyncTest(context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));

  const char* name_data[] = {
    "1",
    "1 + 2",
    "'name'",
    "\"name\"",
    "[]",
    "{}",
    NULL};

  RunParserSyncTest(context_data, name_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ComputedPropertyNameShorthandError) {
  const char* context_data[][2] = {{"({", "});"},
                                   {NULL, NULL}};
  const char* error_data[] = {
    "a: 1, [2]",
    "[1], a: 1",
    NULL};

  static const ParserFlag always_flags[] = {
    kAllowHarmonyComputedPropertyNames,
    kAllowHarmonySloppy,
  };
  RunParserSyncTest(context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(BasicImportExportParsing) {
  const char* kSources[] = {
      "export let x = 0;",
      "export var y = 0;",
      "export const z = 0;",
      "export function func() { };",
      "export class C { };",
      "export { };",
      "function f() {}; f(); export { f };",
      "var a, b, c; export { a, b as baz, c };",
      "var d, e; export { d as dreary, e, };",
      "export default function f() {}",
      "export default class C {}",
      "export default 42",
      "var x; export default x = 7",
      "export { Q } from 'somemodule.js';",
      "export * from 'somemodule.js';",
      "var foo; export { foo as for };",
      "export { arguments } from 'm.js';",
      "export { for } from 'm.js';",
      "export { yield } from 'm.js'",
      "export { static } from 'm.js'",
      "export { let } from 'm.js'",
      "var a; export { a as b, a as c };",

      "import 'somemodule.js';",
      "import { } from 'm.js';",
      "import { a } from 'm.js';",
      "import { a, b as d, c, } from 'm.js';",
      "import * as thing from 'm.js';",
      "import thing from 'm.js';",
      "import thing, * as rest from 'm.js';",
      "import thing, { a, b, c } from 'm.js';",
      "import { arguments as a } from 'm.js';",
      "import { for as f } from 'm.js';",
      "import { yield as y } from 'm.js';",
      "import { static as s } from 'm.js';",
      "import { let as l } from 'm.js';",
  };

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  for (unsigned i = 0; i < arraysize(kSources); ++i) {
    i::Handle<i::String> source =
        factory->NewStringFromAsciiChecked(kSources[i]);

    // Show that parsing as a module works
    {
      i::Handle<i::Script> script = factory->NewScript(source);
      i::Zone zone;
      i::ParseInfo info(&zone, script);
      i::Parser parser(&info);
      parser.set_allow_harmony_modules(true);
      info.set_module();
      if (!parser.Parse(&info)) {
        i::Handle<i::JSObject> exception_handle(
            i::JSObject::cast(isolate->pending_exception()));
        i::Handle<i::String> message_string =
            i::Handle<i::String>::cast(i::Object::GetProperty(
                  isolate, exception_handle, "message").ToHandleChecked());

        v8::base::OS::Print(
            "Parser failed on:\n"
            "\t%s\n"
            "with error:\n"
            "\t%s\n"
            "However, we expected no error.",
            source->ToCString().get(), message_string->ToCString().get());
        CHECK(false);
      }
    }

    // And that parsing a script does not.
    {
      i::Handle<i::Script> script = factory->NewScript(source);
      i::Zone zone;
      i::ParseInfo info(&zone, script);
      i::Parser parser(&info);
      parser.set_allow_harmony_modules(true);
      info.set_global();
      CHECK(!parser.Parse(&info));
    }
  }
}


TEST(ImportExportParsingErrors) {
  const char* kErrorSources[] = {
      "export {",
      "var a; export { a",
      "var a; export { a,",
      "var a; export { a, ;",
      "var a; export { a as };",
      "var a, b; export { a as , b};",
      "export }",
      "var foo, bar; export { foo bar };",
      "export { foo };",
      "export { , };",
      "export default;",
      "export default var x = 7;",
      "export default let x = 7;",
      "export default const x = 7;",
      "export *;",
      "export * from;",
      "export { Q } from;",
      "export default from 'module.js';",
      "export { for }",
      "export { for as foo }",
      "export { arguments }",
      "export { arguments as foo }",
      "var a; export { a, a };",
      "var a, b; export { a as b, b };",
      "var a, b; export { a as c, b as c };",
      "export default function f(){}; export default class C {};",
      "export default function f(){}; var a; export { a as default };",

      "import from;",
      "import from 'm.js';",
      "import { };",
      "import {;",
      "import };",
      "import { , };",
      "import { , } from 'm.js';",
      "import { a } from;",
      "import { a } 'm.js';",
      "import , from 'm.js';",
      "import a , from 'm.js';",
      "import a { b, c } from 'm.js';",
      "import arguments from 'm.js';",
      "import eval from 'm.js';",
      "import { arguments } from 'm.js';",
      "import { eval } from 'm.js';",
      "import { a as arguments } from 'm.js';",
      "import { for } from 'm.js';",
      "import { y as yield } from 'm.js'",
      "import { s as static } from 'm.js'",
      "import { l as let } from 'm.js'",
      "import { x }, def from 'm.js';",
      "import def, def2 from 'm.js';",
      "import * as x, def from 'm.js';",
      "import * as x, * as y from 'm.js';",
      "import {x}, {y} from 'm.js';",
      "import * as x, {y} from 'm.js';",

      // TODO(ES6): These two forms should be supported
      "export default function() {};",
      "export default class {};"
  };

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();

  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);

  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  for (unsigned i = 0; i < arraysize(kErrorSources); ++i) {
    i::Handle<i::String> source =
        factory->NewStringFromAsciiChecked(kErrorSources[i]);

    i::Handle<i::Script> script = factory->NewScript(source);
    i::Zone zone;
    i::ParseInfo info(&zone, script);
    i::Parser parser(&info);
    parser.set_allow_harmony_modules(true);
    info.set_module();
    CHECK(!parser.Parse(&info));
  }
}


TEST(ModuleParsingInternals) {
  i::FLAG_harmony_modules = true;

  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);
  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  static const char kSource[] =
      "let x = 5;"
      "export { x as y };"
      "import { q as z } from 'm.js';"
      "import n from 'n.js';"
      "export { a as b } from 'm.js';"
      "export * from 'p.js';"
      "import 'q.js'";
  i::Handle<i::String> source = factory->NewStringFromAsciiChecked(kSource);
  i::Handle<i::Script> script = factory->NewScript(source);
  i::Zone zone;
  i::ParseInfo info(&zone, script);
  i::Parser parser(&info);
  parser.set_allow_harmony_modules(true);
  info.set_module();
  CHECK(parser.Parse(&info));
  CHECK(i::Compiler::Analyze(&info));
  i::FunctionLiteral* func = info.function();
  i::Scope* module_scope = func->scope();
  i::Scope* outer_scope = module_scope->outer_scope();
  CHECK(outer_scope->is_script_scope());
  CHECK_NULL(outer_scope->outer_scope());
  CHECK_EQ(1, outer_scope->num_modules());
  CHECK(module_scope->is_module_scope());
  CHECK_NOT_NULL(module_scope->module_var());
  CHECK_EQ(i::INTERNAL, module_scope->module_var()->mode());
  i::ModuleDescriptor* descriptor = module_scope->module();
  CHECK_NOT_NULL(descriptor);
  CHECK_EQ(1, descriptor->Length());
  const i::AstRawString* export_name =
      info.ast_value_factory()->GetOneByteString("y");
  const i::AstRawString* local_name =
      descriptor->LookupLocalExport(export_name, &zone);
  CHECK_NOT_NULL(local_name);
  CHECK(local_name->IsOneByteEqualTo("x"));
  i::ZoneList<i::Declaration*>* declarations = module_scope->declarations();
  CHECK_EQ(3, declarations->length());
  CHECK(declarations->at(0)->proxy()->raw_name()->IsOneByteEqualTo("x"));
  i::ImportDeclaration* import_decl =
      declarations->at(1)->AsImportDeclaration();
  CHECK(import_decl->import_name()->IsOneByteEqualTo("q"));
  CHECK(import_decl->proxy()->raw_name()->IsOneByteEqualTo("z"));
  CHECK(import_decl->module_specifier()->IsOneByteEqualTo("m.js"));
  import_decl = declarations->at(2)->AsImportDeclaration();
  CHECK(import_decl->import_name()->IsOneByteEqualTo("default"));
  CHECK(import_decl->proxy()->raw_name()->IsOneByteEqualTo("n"));
  CHECK(import_decl->module_specifier()->IsOneByteEqualTo("n.js"));
  // TODO(adamk): Add test for indirect exports once they're fully implemented.
  // TODO(adamk): Add test for star exports once they're fully implemented.
  const i::ZoneList<const i::AstRawString*>& requested_modules =
      descriptor->requested_modules();
  CHECK_EQ(4, requested_modules.length());
  CHECK(requested_modules[0]->IsOneByteEqualTo("m.js"));
  CHECK(requested_modules[1]->IsOneByteEqualTo("n.js"));
  CHECK(requested_modules[2]->IsOneByteEqualTo("p.js"));
  CHECK(requested_modules[3]->IsOneByteEqualTo("q.js"));
}


TEST(DuplicateProtoError) {
  const char* context_data[][2] = {
    {"({", "});"},
    {"'use strict'; ({", "});"},
    {NULL, NULL}
  };
  const char* error_data[] = {
    "__proto__: {}, __proto__: {}",
    "__proto__: {}, \"__proto__\": {}",
    "__proto__: {}, \"__\x70roto__\": {}",
    "__proto__: {}, a: 1, __proto__: {}",
    NULL
  };

  RunParserSyncTest(context_data, error_data, kError);
}


TEST(DuplicateProtoNoError) {
  const char* context_data[][2] = {
    {"({", "});"},
    {"'use strict'; ({", "});"},
    {NULL, NULL}
  };
  const char* error_data[] = {
    "__proto__: {}, ['__proto__']: {}",
    "__proto__: {}, __proto__() {}",
    "__proto__: {}, get __proto__() {}",
    "__proto__: {}, set __proto__(v) {}",
    "__proto__: {}, __proto__",
    NULL
  };

  static const ParserFlag always_flags[] = {
    kAllowHarmonyComputedPropertyNames,
  };
  RunParserSyncTest(context_data, error_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(DeclarationsError) {
  const char* context_data[][2] = {{"'use strict'; if (true)", ""},
                                   {"'use strict'; if (false) {} else", ""},
                                   {"'use strict'; while (false)", ""},
                                   {"'use strict'; for (;;)", ""},
                                   {"'use strict'; for (x in y)", ""},
                                   {"'use strict'; do ", " while (false)"},
                                   {"'use strong'; if (true)", ""},
                                   {"'use strong'; if (false) {} else", ""},
                                   {"'use strong'; while (false)", ""},
                                   {"'use strong'; for (;;)", ""},
                                   {"'use strong'; for (x in y)", ""},
                                   {"'use strong'; do ", " while (false)"},
                                   {NULL, NULL}};

  const char* statement_data[] = {
    "let x = 1;",
    "const x = 1;",
    "class C {}",
    NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


void TestLanguageMode(const char* source,
                      i::LanguageMode expected_language_mode) {
  i::Isolate* isolate = CcTest::i_isolate();
  i::Factory* factory = isolate->factory();
  v8::HandleScope handles(CcTest::isolate());
  v8::Handle<v8::Context> context = v8::Context::New(CcTest::isolate());
  v8::Context::Scope context_scope(context);
  isolate->stack_guard()->SetStackLimit(i::GetCurrentStackPosition() -
                                        128 * 1024);

  i::Handle<i::Script> script =
      factory->NewScript(factory->NewStringFromAsciiChecked(source));
  i::Zone zone;
  i::ParseInfo info(&zone, script);
  i::Parser parser(&info);
  parser.set_allow_strong_mode(true);
  info.set_global();
  parser.Parse(&info);
  CHECK(info.function() != NULL);
  CHECK_EQ(expected_language_mode, info.function()->language_mode());
}


TEST(LanguageModeDirectives) {
  TestLanguageMode("\"use nothing\"", i::SLOPPY);
  TestLanguageMode("\"use strict\"", i::STRICT);
  TestLanguageMode("\"use strong\"", i::STRONG);

  TestLanguageMode("var x = 1; \"use strict\"", i::SLOPPY);
  TestLanguageMode("var x = 1; \"use strong\"", i::SLOPPY);

  // Test that multiple directives ("use strict" / "use strong") put the parser
  // into the correct mode.
  TestLanguageMode("\"use strict\"; \"use strong\";", i::STRONG);
  TestLanguageMode("\"use strong\"; \"use strict\";", i::STRONG);

  TestLanguageMode("\"use some future directive\"; \"use strict\";", i::STRICT);
  TestLanguageMode("\"use some future directive\"; \"use strong\";", i::STRONG);
}


TEST(PropertyNameEvalArguments) {
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"'use strong';", ""},
                                   {NULL, NULL}};

  const char* statement_data[] = {
      "({eval: 1})",
      "({arguments: 1})",
      "({eval() {}})",
      "({arguments() {}})",
      "({*eval() {}})",
      "({*arguments() {}})",
      "({get eval() {}})",
      "({get arguments() {}})",
      "({set eval(_) {}})",
      "({set arguments(_) {}})",

      "class C {eval() {}}",
      "class C {arguments() {}}",
      "class C {*eval() {}}",
      "class C {*arguments() {}}",
      "class C {get eval() {}}",
      "class C {get arguments() {}}",
      "class C {set eval(_) {}}",
      "class C {set arguments(_) {}}",

      "class C {static eval() {}}",
      "class C {static arguments() {}}",
      "class C {static *eval() {}}",
      "class C {static *arguments() {}}",
      "class C {static get eval() {}}",
      "class C {static get arguments() {}}",
      "class C {static set eval(_) {}}",
      "class C {static set arguments(_) {}}",

      NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(context_data, statement_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(FunctionLiteralDuplicateParameters) {
  const char* strict_context_data[][2] =
      {{"'use strict';(function(", "){})();"},
       {"(function(", ") { 'use strict'; })();"},
       {"'use strict'; function fn(", ") {}; fn();"},
       {"function fn(", ") { 'use strict'; }; fn();"},
       {"'use strong';(function(", "){})();"},
       {"(function(", ") { 'use strong'; })();"},
       {"'use strong'; function fn(", ") {}; fn();"},
       {"function fn(", ") { 'use strong'; }; fn();"},
       {NULL, NULL}};

  const char* sloppy_context_data[][2] =
      {{"(function(", "){})();"},
       {"(function(", ") {})();"},
       {"function fn(", ") {}; fn();"},
       {"function fn(", ") {}; fn();"},
       {NULL, NULL}};

  const char* data[] = {
      "a, a",
      "a, a, a",
      "b, a, a",
      "a, b, c, c",
      "a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, w",
      NULL};

  static const ParserFlag always_flags[] = { kAllowStrongMode };
  RunParserSyncTest(strict_context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, data, kSuccess, NULL, 0, NULL, 0);
}


TEST(VarForbiddenInStrongMode) {
  const char* strong_context_data[][2] =
      {{"'use strong'; ", ""},
       {"function f() {'use strong'; ", "}"},
       {"function f() {'use strong';  while (true) { ", "} }"},
       {NULL, NULL}};

  const char* strict_context_data[][2] =
      {{"'use strict'; ", ""},
       {"function f() {'use strict'; ", "}"},
       {"function f() {'use strict'; while (true) { ", "} }"},
       {NULL, NULL}};

  const char* sloppy_context_data[][2] =
      {{"", ""},
       {"function f() { ", "}"},
       {NULL, NULL}};

  const char* var_declarations[] = {
    "var x = 0;",
    "for (var i = 0; i < 10; i++) { }",
    NULL};

  const char* let_declarations[] = {
    "let x = 0;",
    "for (let i = 0; i < 10; i++) { }",
    NULL};

  const char* const_declarations[] = {
    "const x = 0;",
    NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(strong_context_data, var_declarations, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, let_declarations, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, const_declarations, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(strict_context_data, var_declarations, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, let_declarations, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(sloppy_context_data, var_declarations, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  // At the moment, let declarations are only available in strict mode.
  RunParserSyncTest(sloppy_context_data, let_declarations, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongEmptySubStatements) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* data_error[] = {
      "if (1);",
      "if (1) {} else;",
      "while (1);",
      "do; while (1);",
      "for (;;);",
      "for (x in []);",
      "for (x of []);",
      "for (const x = 0;;);",
      "for (const x in []);",
      "for (const x of []);",
      NULL};

  const char* data_success[] =  {
      "if (1) {} else {}",
      "switch(1) {}",
      "1+1;;",
      "1+1; ;",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode,
  };
  RunParserSyncTest(sloppy_context_data, data_error, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, data_error, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, data_error, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, data_success, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongForIn) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* data[] = {
      "for (x in []) {}",
      "for (const x in []) {}",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode,
  };
  RunParserSyncTest(sloppy_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(strict_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(strong_context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(StrongConstructorThis) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* error_data[] = {
      "class C { constructor() { this; } }",
      "class C { constructor() { this.a; } }",
      "class C { constructor() { this['a']; } }",
      "class C { constructor() { (this); } }",
      "class C { constructor() { this(); } }",
      // TODO(rossberg): arrow functions not handled yet.
      // "class C { constructor() { () => this; } }",
      "class C { constructor() { this.a = 0, 0; } }",
      "class C { constructor() { (this.a = 0); } }",
      // "class C { constructor() { (() => this.a = 0)(); } }",
      "class C { constructor() { { this.a = 0; } } }",
      "class C { constructor() { if (1) this.a = 0; } }",
      "class C { constructor() { label: this.a = 0; } }",
      "class C { constructor() { this.a = this.b; } }",
      "class C { constructor() { this.a = {b: 1}; this.a.b } }",
      "class C { constructor() { this.a = {b: 1}; this.a.b = 0 } }",
      "class C { constructor() { this.a = function(){}; this.a() } }",
      NULL};

  const char* success_data[] = {
      "class C { constructor() { this.a = 0; } }",
      "class C { constructor() { label: 0; this.a = 0; this.b = 6; } }",
      NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode,
                                            kAllowHarmonyArrowFunctions};
  RunParserSyncTest(sloppy_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, error_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(sloppy_context_data, success_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongConstructorSuper) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* error_data[] = {
      "class C extends Object { constructor() {} }",
      "class C extends Object { constructor() { super.a; } }",
      "class C extends Object { constructor() { super['a']; } }",
      "class C extends Object { constructor() { super.a = 0; } }",
      "class C extends Object { constructor() { (super.a); } }",
      // TODO(rossberg): arrow functions do not handle super yet.
      // "class C extends Object { constructor() { () => super.a; } }",
      "class C extends Object { constructor() { super(), 0; } }",
      "class C extends Object { constructor() { (super()); } }",
      // "class C extends Object { constructor() { (() => super())(); } }",
      "class C extends Object { constructor() { { super(); } } }",
      "class C extends Object { constructor() { if (1) super(); } }",
      "class C extends Object { constructor() { label: super(); } }",
      "class C extends Object { constructor() { super(), super(); } }",
      "class C extends Object { constructor() { super(); super(); } }",
      "class C extends Object { constructor() { super(); (super()); } }",
      "class C extends Object { constructor() { super(); { super() } } }",
      "class C extends Object { constructor() { this.a = 0, super(); } }",
      "class C extends Object { constructor() { this.a = 0; super(); } }",
      "class C extends Object { constructor() { super(this.a = 0); } }",
      "class C extends Object { constructor() { super().a; } }",
      NULL};

  const char* success_data[] = {
      "class C extends Object { constructor() { super(); } }",
      "class C extends Object { constructor() { label: 66; super(); } }",
      "class C extends Object { constructor() { super(3); this.x = 0; } }",
      "class C extends Object { constructor() { 3; super(3); this.x = 0; } }",
      NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode,
                                            kAllowHarmonyArrowFunctions};
  RunParserSyncTest(sloppy_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, error_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(sloppy_context_data, success_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongConstructorReturns) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* error_data[] = {
      "class C extends Object { constructor() { super(); return {}; } }",
      "class C extends Object { constructor() { super(); { return {}; } } }",
      "class C extends Object { constructor() { super(); if (1) return {}; } }",
      "class C extends Object { constructor() { return; super(); } }",
      "class C extends Object { constructor() { { return; } super(); } }",
      "class C extends Object { constructor() { if (0) return; super(); } }",
      "class C { constructor() { return; this.a = 0; } }",
      "class C { constructor() { { return; } this.a = 0; } }",
      "class C { constructor() { if (0) return; this.a = 0; } }",
      "class C { constructor() { this.a = 0; if (0) return; this.b = 0; } }",
      NULL};

  const char* success_data[] = {
      "class C extends Object { constructor() { super(); return; } }",
      "class C extends Object { constructor() { super(); { return } } }",
      "class C extends Object { constructor() { super(); if (1) return; } }",
      "class C { constructor() { this.a = 0; return; } }",
      "class C { constructor() { this.a = 0; { return; }  } }",
      "class C { constructor() { this.a = 0; if (0) return; 65; } }",
      "class C extends Array { constructor() { super(); this.a = 9; return } }",
      NULL};

  static const ParserFlag always_flags[] = {kAllowStrongMode};
  RunParserSyncTest(sloppy_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, error_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));

  RunParserSyncTest(sloppy_context_data, success_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strict_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongUndefinedLocal) {
  const char* context_data[][2] = {{"", ""}, {NULL}};

  const char* data[] = {
      "function undefined() {'use strong';}",
      "function* undefined() {'use strong';}",
      "(function undefined() {'use strong';});",
      "{foo: (function undefined(){'use strong';})};",
      "(function* undefined() {'use strong';})",
      "{foo: (function* undefined(){'use strong';})};",
      "function foo(a, b, undefined, c, d) {'use strong';}",
      "function* foo(a, b, undefined, c, d) {'use strong';}",
      "(function foo(a, b, undefined, c, d) {'use strong';})",
      "{foo: (function foo(a, b, undefined, c, d) {'use strong';})};",
      "(function* foo(a, b, undefined, c, d) {'use strong';})",
      "{foo: (function* foo(a, b, undefined, c, d) {'use strong';})};",
      "class C { foo(a, b, undefined, c, d) {'use strong';} }",
      "class C { *foo(a, b, undefined, c, d) {'use strong';} }",
      "({ foo(a, b, undefined, c, d) {'use strong';} });",
      "{ *foo(a, b, undefined, c, d) {'use strong';} });",
      "class undefined {'use strong'}",
      "(class undefined {'use strong'});",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode, kAllowHarmonySloppy
  };

  RunParserSyncTest(context_data, data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongUndefinedArrow) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strict_context_data[][2] = {{"'use strict';", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* data[] = {
      "(undefined => {return});",
      "((undefined, b, c) => {return});",
      "((a, undefined, c) => {return});",
      "((a, b, undefined) => {return});",
      NULL};

  const char* local_strong[] = {
      "(undefined => {'use strong';});",
      "((undefined, b, c) => {'use strong';});",
      "((a, undefined, c) => {'use strong';});",
      "((a, b, undefined) => {'use strong';});",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode, kAllowHarmonyArrowFunctions
  };
  RunParserSyncTest(sloppy_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(strict_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(strong_context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, local_strong, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongDirectEval) {
  const char* sloppy_context_data[][2] = {{"", ""}, {NULL}};
  const char* strong_context_data[][2] = {{"'use strong';", ""}, {NULL}};

  const char* error_data[] = {
      "eval();",
      "eval([]);",
      "(eval)();",
      "(((eval)))();",
      "eval('function f() {}');",
      "function f() {eval()}",
      NULL};

  const char* success_data[] = {
      "eval;",
      "eval`foo`;",
      "let foo = eval; foo();",
      "(1, eval)();",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode
  };

  RunParserSyncTest(sloppy_context_data, error_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, error_data, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, success_data, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(StrongSwitchFallthrough) {
  const char* sloppy_context_data[][2] = {
      {"function f() { foo:for(;;) { switch(1) {", "};}}"},
      {NULL, NULL}
  };
  const char* strong_context_data[][2] = {
      {"function f() { 'use strong'; foo:for(;;) { switch(1) {", "};}}"},
      {NULL, NULL}
  };

  const char* data_success[] = {
      "",
      "case 1:",
      "case 1: case 2:",
      "case 1: break;",
      "default: throw new TypeError();",
      "case 1: case 2: null",
      "case 1: case 2: default: 1+1",
      "case 1: break; case 2: return; default:",
      "case 1: break foo; case 2: return; default:",
      "case 1: case 2: break; case 3: continue; case 4: default:",
      "case 1: case 2: break; case 3: continue foo; case 4: default:",
      "case 1: case 2: {{return;}} case 3: default:",
      "case 1: case 2: case 3: default: {1+1;{continue;}}",
      "case 1: case 2: {1+1;{1+1;{continue;}}} case 3: default:",
      "case 1: if (1) break; else continue; case 2: case 3: default:",
      "case 1: case 2: if (1) {{break;}} else break; case 3: default:",
      "case 1: if (1) break; else {if (1) break; else break;} case 2: default:",
      "case 1: if (1) {if (1) break; else break;} else break; case 2: default:",
      NULL};

  const char* data_error[] = {
      "case 1: case 2: (function(){return}); default:",
      "case 1: 1+1; case 2:",
      "case 1: bar: break bar; case 2: break;",
      "case 1: bar:return; case 2:",
      "case 1: bar:{ continue;} case 2:",
      "case 1: break; case 2: bar:{ throw new TypeError() } default:",
      "case 1: case 2: { bar:{ { break;} } } default: break;",
      "case 1: if (1) break; else {}; case 2: default:",
      "case 1: case 2: if (1) break; default:",
      "case 1: case 2: if (1) break; else 0; default:",
      "case 1: case 2: if (1) 0; else break; default:",
      "case 1: case 2: case 3: if (1) {} default:",
      "case 1: bar:if (1) break; else continue; case 2: case 3: default:",
      NULL};

  static const ParserFlag always_flags[] = {
      kAllowStrongMode
  };
  RunParserSyncTest(strong_context_data, data_success, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(sloppy_context_data, data_error, kSuccess, NULL, 0,
                    always_flags, arraysize(always_flags));
  RunParserSyncTest(strong_context_data, data_error, kError, NULL, 0,
                    always_flags, arraysize(always_flags));
}


TEST(ArrowFunctionASIErrors) {
  const char* context_data[][2] = {{"'use strict';", ""}, {"", ""},
                                   {NULL, NULL}};

  const char* data[] = {
      "(a\n=> a)(1)",
      "(a/*\n*/=> a)(1)",
      "((a)\n=> a)(1)",
      "((a)/*\n*/=> a)(1)",
      "((a, b)\n=> a + b)(1, 2)",
      "((a, b)/*\n*/=> a + b)(1, 2)",
      NULL};
  static const ParserFlag always_flags[] = {kAllowHarmonyArrowFunctions};
  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(StrongModeFreeVariablesDeclaredByPreviousScript) {
  i::FLAG_strong_mode = true;
  v8::V8::Initialize();
  v8::HandleScope scope(CcTest::isolate());
  v8::Context::Scope context_scope(v8::Context::New(CcTest::isolate()));
  v8::TryCatch try_catch(CcTest::isolate());

  // Introduce a bunch of variables, in all language modes.
  const char* script1 =
      "var my_var1 = 0;        \n"
      "function my_func1() { } \n"
      "const my_const1 = 0;    \n";
  CompileRun(v8_str(script1));
  CHECK(!try_catch.HasCaught());

  const char* script2 =
      "\"use strict\";         \n"
      "let my_var2 = 0;        \n"
      "function my_func2() { } \n"
      "const my_const2 = 0     \n";
  CompileRun(v8_str(script2));
  CHECK(!try_catch.HasCaught());

  const char* script3 =
      "\"use strong\";         \n"
      "let my_var3 = 0;        \n"
      "function my_func3() { } \n"
      "const my_const3 = 0;    \n";
  CompileRun(v8_str(script3));
  CHECK(!try_catch.HasCaught());

  // Sloppy eval introduces variables in the surrounding scope.
  const char* script4 =
      "eval('var my_var4 = 0;')        \n"
      "eval('function my_func4() { }') \n"
      "eval('const my_const4 = 0;')    \n";
  CompileRun(v8_str(script4));
  CHECK(!try_catch.HasCaught());

  // Test that referencing these variables work.
  const char* script5 =
      "\"use strong\";         \n"
      "my_var1;                \n"
      "my_func1;               \n"
      "my_const1;              \n"
      "my_var2;                \n"
      "my_func2;               \n"
      "my_const2;              \n"
      "my_var3;                \n"
      "my_func3;               \n"
      "my_const3;              \n"
      "my_var4;                \n"
      "my_func4;               \n"
      "my_const4;              \n";
  CompileRun(v8_str(script5));
  CHECK(!try_catch.HasCaught());
}


TEST(StrongModeFreeVariablesDeclaredByLanguage) {
  i::FLAG_strong_mode = true;
  v8::V8::Initialize();
  v8::HandleScope scope(CcTest::isolate());
  v8::Context::Scope context_scope(v8::Context::New(CcTest::isolate()));
  v8::TryCatch try_catch(CcTest::isolate());

  const char* script1 =
      "\"use strong\";         \n"
      "Math;                   \n"
      "RegExp;                 \n";
  CompileRun(v8_str(script1));
  CHECK(!try_catch.HasCaught());
}


TEST(StrongModeFreeVariablesDeclaredInGlobalPrototype) {
  i::FLAG_strong_mode = true;
  v8::V8::Initialize();
  v8::HandleScope scope(CcTest::isolate());
  v8::Context::Scope context_scope(v8::Context::New(CcTest::isolate()));
  v8::TryCatch try_catch(CcTest::isolate());

  const char* script1 = "this.__proto__.my_var = 0;\n";
  CompileRun(v8_str(script1));
  CHECK(!try_catch.HasCaught());

  const char* script2 =
      "\"use strong\";         \n"
      "my_var;                 \n";
  CompileRun(v8_str(script2));
  CHECK(!try_catch.HasCaught());
}


TEST(StrongModeFreeVariablesNotDeclared) {
  i::FLAG_strong_mode = true;
  v8::V8::Initialize();
  v8::HandleScope scope(CcTest::isolate());
  v8::Context::Scope context_scope(v8::Context::New(CcTest::isolate()));
  v8::TryCatch try_catch(CcTest::isolate());

  // Test that referencing unintroduced variables in sloppy mode is ok.
  const char* script1 =
      "if (false) {            \n"
      "  not_there1;           \n"
      "}                       \n";
  CompileRun(v8_str(script1));
  CHECK(!try_catch.HasCaught());

  // But not in strong mode.
  {
    const char* script2 =
        "\"use strong\";         \n"
        "if (false) {            \n"
        "  not_there2;           \n"
        "}                       \n";
    v8::TryCatch try_catch2(CcTest::isolate());
    v8::Script::Compile(v8_str(script2));
    CHECK(try_catch2.HasCaught());
    v8::String::Utf8Value exception(try_catch2.Exception());
    CHECK_EQ(0,
             strcmp(
                 "ReferenceError: In strong mode, using an undeclared global "
                 "variable 'not_there2' is not allowed",
                 *exception));
  }

  // Check that the variable reference is detected inside a strong function too,
  // even if the script scope is not strong.
  {
    const char* script3 =
        "(function not_lazy() {  \n"
        "  \"use strong\";       \n"
        "  if (false) {          \n"
        "    not_there3;         \n"
        "  }                     \n"
        "})();                   \n";
    v8::TryCatch try_catch2(CcTest::isolate());
    v8::Script::Compile(v8_str(script3));
    CHECK(try_catch2.HasCaught());
    v8::String::Utf8Value exception(try_catch2.Exception());
    CHECK_EQ(0,
             strcmp(
                 "ReferenceError: In strong mode, using an undeclared global "
                 "variable 'not_there3' is not allowed",
                 *exception));
  }
}


TEST(DestructuringPositiveTests) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_computed_property_names = true;

  const char* context_data[][2] = {{"'use strict'; let ", " = {};"},
                                   {"var ", " = {};"},
                                   {"'use strict'; const ", " = {};"},
                                   {"function f(", ") {}"},
                                   {"function f(argument1, ", ") {}"},
                                   {"var f = (", ") => {};"},
                                   {"var f = (argument1,", ") => {};"},
                                   {NULL, NULL}};

  // clang-format off
  const char* data[] = {
    "a",
    "{ x : y }",
    "{ x : y = 1 }",
    "[a]",
    "[a = 1]",
    "[a,b,c]",
    "[a, b = 42, c]",
    "{ x : x, y : y }",
    "{ x : x = 1, y : y }",
    "{ x : x, y : y = 42 }",
    "[]",
    "{}",
    "[{x:x, y:y}, [a,b,c]]",
    "[{x:x = 1, y:y = 2}, [a = 3, b = 4, c = 5]]",
    "{x}",
    "{x, y}",
    "{x = 42, y = 15}",
    "[a,,b]",
    "{42 : x}",
    "{42 : x = 42}",
    "{42e-2 : x}",
    "{42e-2 : x = 42}",
    "{x : y, x : z}",
    "{'hi' : x}",
    "{'hi' : x = 42}",
    "{var: x}",
    "{var: x = 42}",
    "{[x] : z}",
    "{[1+1] : z}",
    "{[foo()] : z}",
    "{}",
    "[...rest]",
    "[a,b,...rest]",
    "[a,,...rest]",
    NULL};
  // clang-format on
  static const ParserFlag always_flags[] = {kAllowHarmonyComputedPropertyNames,
                                            kAllowHarmonyArrowFunctions,
                                            kAllowHarmonyDestructuring};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(DestructuringNegativeTests) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_computed_property_names = true;
  static const ParserFlag always_flags[] = {kAllowHarmonyComputedPropertyNames,
                                            kAllowHarmonyArrowFunctions,
                                            kAllowHarmonyDestructuring};

  {  // All modes.
    const char* context_data[][2] = {{"'use strict'; let ", " = {};"},
                                     {"var ", " = {};"},
                                     {"'use strict'; const ", " = {};"},
                                     {"function f(", ") {}"},
                                     {"function f(argument1, ", ") {}"},
                                     {"var f = (", ") => {};"},
                                     {"var f = ", " => {};"},
                                     {"var f = (argument1,", ") => {};"},
                                     {NULL, NULL}};

    // clang-format off
    const char* data[] = {
        "a++",
        "++a",
        "delete a",
        "void a",
        "typeof a",
        "--a",
        "+a",
        "-a",
        "~a",
        "!a",
        "{ x : y++ }",
        "[a++]",
        "(x => y)",
        "a[i]", "a()",
        "a.b",
        "new a",
        "a + a",
        "a - a",
        "a * a",
        "a / a",
        "a == a",
        "a != a",
        "a > a",
        "a < a",
        "a <<< a",
        "a >>> a",
        "function a() {}",
        "a`bcd`",
        "this",
        "null",
        "true",
        "false",
        "1",
        "'abc'",
        "class {}",
        "{+2 : x}",
        "{-2 : x}",
        "var",
        "[var]",
        "{x : {y : var}}",
        "{x : x = a+}",
        "{x : x = (a+)}",
        "{x : x += a}",
        "{m() {} = 0}",
        "{[1+1]}",
        "[...rest, x]",
        "[a,b,...rest, x]",
        "[a,,...rest, x]",
        "[...rest,]",
        "[a,b,...rest,]",
        "[a,,...rest,]",
        "[...rest,...rest1]",
        "[a,b,...rest,...rest1]",
        "[a,,..rest,...rest1]",
        NULL};
    // clang-format on
    RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                      arraysize(always_flags));
  }

  {  // All modes.
    const char* context_data[][2] = {{"'use strict'; let ", " = {};"},
                                     {"var ", " = {};"},
                                     {"'use strict'; const ", " = {};"},
                                     {"function f(", ") {}"},
                                     {"function f(argument1, ", ") {}"},
                                     {"var f = (", ") => {};"},
                                     {"var f = (argument1,", ") => {};"},
                                     {NULL, NULL}};

    // clang-format off
    const char* data[] = {
        "x => x",
        "() => x",
        NULL};
    // clang-format on
    RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                      arraysize(always_flags));
  }

  {  // Strict mode.
    const char* context_data[][2] = {
        {"'use strict'; let ", " = {};"},
        {"'use strict'; const ", " = {};"},
        {"'use strict'; function f(", ") {}"},
        {"'use strict'; function f(argument1, ", ") {}"},
        {NULL, NULL}};

    // clang-format off
    const char* data[] = {
      "[eval]",
      "{ a : arguments }",
      "[public]",
      "{ x : private }",
      NULL};
    // clang-format on
    RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                      arraysize(always_flags));
  }

  {  // 'yield' in generators.
    const char* context_data[][2] = {
        {"function*() { var ", " = {};"},
        {"function*() { 'use strict'; let ", " = {};"},
        {"function*() { 'use strict'; const ", " = {};"},
        {NULL, NULL}};

    // clang-format off
    const char* data[] = {
      "yield",
      "[yield]",
      "{ x : yield }",
      NULL};
    // clang-format on
    RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                      arraysize(always_flags));
  }
}


TEST(DestructuringDisallowPatternsInForVarIn) {
  i::FLAG_harmony_destructuring = true;
  static const ParserFlag always_flags[] = {kAllowHarmonyDestructuring};
  const char* context_data[][2] = {
      {"", ""}, {"function f() {", "}"}, {NULL, NULL}};
  // clang-format off
  const char* error_data[] = {
    "for (let x = {} in null);",
    "for (let x = {} of null);",
    NULL};
  // clang-format on
  RunParserSyncTest(context_data, error_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));

  // clang-format off
  const char* success_data[] = {
    "for (var x = {} in null);",
    NULL};
  // clang-format on
  RunParserSyncTest(context_data, success_data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(DestructuringDuplicateParams) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_computed_property_names = true;
  static const ParserFlag always_flags[] = {kAllowHarmonyComputedPropertyNames,
                                            kAllowHarmonyArrowFunctions,
                                            kAllowHarmonyDestructuring};
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function outer() { 'use strict';", "}"},
                                   {nullptr, nullptr}};


  // clang-format off
  const char* error_data[] = {
    "function f(x,x){}",
    "function f(x, {x : x}){}",
    "function f(x, {x}){}",
    "function f({x,x}) {}",
    "function f([x,x]) {}",
    "function f(x, [y,{z:x}]) {}",
    "function f([x,{y:x}]) {}",
    // non-simple parameter list causes duplicates to be errors in sloppy mode.
    "function f(x, x, {a}) {}",
    nullptr};
  // clang-format on
  RunParserSyncTest(context_data, error_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(DestructuringDuplicateParamsSloppy) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_computed_property_names = true;
  static const ParserFlag always_flags[] = {kAllowHarmonyComputedPropertyNames,
                                            kAllowHarmonyArrowFunctions,
                                            kAllowHarmonyDestructuring};
  const char* context_data[][2] = {
      {"", ""}, {"function outer() {", "}"}, {nullptr, nullptr}};


  // clang-format off
  const char* error_data[] = {
    // non-simple parameter list causes duplicates to be errors in sloppy mode.
    "function f(x, {x : x}){}",
    "function f(x, {x}){}",
    "function f({x,x}) {}",
    "function f(x, x, {a}) {}",
    nullptr};
  // clang-format on
  RunParserSyncTest(context_data, error_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(DestructuringDisallowPatternsInSingleParamArrows) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_computed_property_names = true;
  static const ParserFlag always_flags[] = {kAllowHarmonyComputedPropertyNames,
                                            kAllowHarmonyArrowFunctions,
                                            kAllowHarmonyDestructuring};
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function outer() { 'use strict';", "}"},
                                   {"", ""},
                                   {"function outer() { ", "}"},
                                   {nullptr, nullptr}};

  // clang-format off
  const char* error_data[] = {
    "var f = {x} => {};",
    "var f = {x,y} => {};",
    nullptr};
  // clang-format on
  RunParserSyncTest(context_data, error_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(DestructuringDisallowPatternsInRestParams) {
  i::FLAG_harmony_destructuring = true;
  i::FLAG_harmony_arrow_functions = true;
  i::FLAG_harmony_rest_parameters = true;
  i::FLAG_harmony_computed_property_names = true;
  static const ParserFlag always_flags[] = {
      kAllowHarmonyComputedPropertyNames, kAllowHarmonyArrowFunctions,
      kAllowHarmonyRestParameters, kAllowHarmonyDestructuring};
  const char* context_data[][2] = {{"'use strict';", ""},
                                   {"function outer() { 'use strict';", "}"},
                                   {"", ""},
                                   {"function outer() { ", "}"},
                                   {nullptr, nullptr}};

  // clang-format off
  const char* error_data[] = {
    "function(...{}) {}",
    "function(...{x}) {}",
    "function(...[x]) {}",
    "(...{}) => {}",
    "(...{x}) => {}",
    "(...[x]) => {}",
    nullptr};
  // clang-format on
  RunParserSyncTest(context_data, error_data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(SpreadArray) {
  i::FLAG_harmony_spread_arrays = true;

  const char* context_data[][2] = {
      {"'use strict';", ""}, {"", ""}, {NULL, NULL}};

  // clang-format off
  const char* data[] = {
    "[...a]",
    "[a, ...b]",
    "[...a,]",
    "[...a, ,]",
    "[, ...a]",
    "[...a, ...b]",
    "[...a, , ...b]",
    "[...[...a]]",
    "[, ...a]",
    "[, , ...a]",
    NULL};
  // clang-format on
  static const ParserFlag always_flags[] = {kAllowHarmonySpreadArrays};
  RunParserSyncTest(context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(SpreadArrayError) {
  i::FLAG_harmony_spread_arrays = true;

  const char* context_data[][2] = {
      {"'use strict';", ""}, {"", ""}, {NULL, NULL}};

  // clang-format off
  const char* data[] = {
    "[...]",
    "[a, ...]",
    "[..., ]",
    "[..., ...]",
    "[ (...a)]",
    NULL};
  // clang-format on
  static const ParserFlag always_flags[] = {kAllowHarmonySpreadArrays};
  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(NewTarget) {
  // clang-format off
  const char* good_context_data[][2] = {
    {"function f() {", "}"},
    {"'use strict'; function f() {", "}"},
    {"var f = function() {", "}"},
    {"'use strict'; var f = function() {", "}"},
    {"({m: function() {", "}})"},
    {"'use strict'; ({m: function() {", "}})"},
    {"({m() {", "}})"},
    {"'use strict'; ({m() {", "}})"},
    {"({get x() {", "}})"},
    {"'use strict'; ({get x() {", "}})"},
    {"({set x(_) {", "}})"},
    {"'use strict'; ({set x(_) {", "}})"},
    {"class C {m() {", "}}"},
    {"class C {get x() {", "}}"},
    {"class C {set x(_) {", "}}"},
    {NULL}
  };

  const char* bad_context_data[][2] = {
    {"", ""},
    {"'use strict';", ""},
    {NULL}
  };

  const char* data[] = {
    "new.target",
    "{ new.target }",
    "() => { new.target }",
    "() => new.target",
    "if (1) { new.target }",
    "if (1) {} else { new.target }",
    "while (0) { new.target }",
    "do { new.target } while (0)",
    NULL
  };

  static const ParserFlag always_flags[] = {
    kAllowHarmonyArrowFunctions,
    kAllowHarmonyNewTarget,
    kAllowHarmonySloppy,
  };
  // clang-format on

  RunParserSyncTest(good_context_data, data, kSuccess, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(bad_context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
}


TEST(LegacyConst) {
  // clang-format off
  const char* context_data[][2] = {
    {"", ""},
    {"{", "}"},
    {NULL, NULL}
  };

  const char* data[] = {
    "const x",
    "const x = 1",
    "for (const x = 1; x < 1; x++) {}",
    "for (const x in {}) {}",
    "for (const x of []) {}",
    NULL
  };
  // clang-format on

  static const ParserFlag always_flags[] = {kNoLegacyConst};

  RunParserSyncTest(context_data, data, kError, NULL, 0, always_flags,
                    arraysize(always_flags));
  RunParserSyncTest(context_data, data, kSuccess);
}
