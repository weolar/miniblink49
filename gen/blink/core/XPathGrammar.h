// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XPathGrammarH
#define XPathGrammarH

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MULOP = 258,
     RELOP = 259,
     EQOP = 260,
     MINUS = 261,
     PLUS = 262,
     AND = 263,
     OR = 264,
     AXISNAME = 265,
     NODETYPE = 266,
     PI = 267,
     FUNCTIONNAME = 268,
     LITERAL = 269,
     VARIABLEREFERENCE = 270,
     NUMBER = 271,
     DOTDOT = 272,
     SLASHSLASH = 273,
     NAMETEST = 274,
     XPATH_ERROR = 275
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 58 "xml\\XPathGrammar.y"

    blink::XPath::Step::Axis axis;
    blink::XPath::Step::NodeTest* nodeTest;
    blink::XPath::NumericOp::Opcode numop;
    blink::XPath::EqTestOp::Opcode eqop;
    String* str;
    blink::XPath::Expression* expr;
    blink::HeapVector<blink::Member<blink::XPath::Predicate>>* predList;
    blink::HeapVector<blink::Member<blink::XPath::Expression>>* argList;
    blink::XPath::Step* step;
    blink::XPath::LocationPath* locationPath;



/* Line 1676 of yacc.c  */
#line 87 "..\\..\\..\\..\\..\\..\\out\\Debug/gen/blink/core\\XPathGrammar.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif





#endif
