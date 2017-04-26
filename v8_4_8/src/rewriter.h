// Copyright 2011 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_REWRITER_H_
#define V8_REWRITER_H_

namespace v8 {
namespace internal {

class AstValueFactory;
class DoExpression;
class ParseInfo;
class Parser;

class Rewriter {
 public:
  // Rewrite top-level code (ECMA 262 "programs") so as to conservatively
  // include an assignment of the value of the last statement in the code to
  // a compiler-generated temporary variable wherever needed.
  //
  // Assumes code has been parsed and scopes have been analyzed.  Mutates the
  // AST, so the AST should not continue to be used in the case of failure.
  static bool Rewrite(ParseInfo* info);

  // Rewrite a list of statements, using the same rules as a top-level program,
  // to  ensure identical behaviour of completion result.
  static bool Rewrite(Parser* parser, DoExpression* expr,
                      AstValueFactory* factory);
};


}  // namespace internal
}  // namespace v8

#endif  // V8_REWRITER_H_
