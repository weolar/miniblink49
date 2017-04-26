// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/ast/ast.h"
#include "src/messages.h"
#include "src/parsing/parameter-initializer-rewriter.h"
#include "src/parsing/parser.h"

namespace v8 {

namespace internal {

void Parser::PatternRewriter::DeclareAndInitializeVariables(
    Block* block, const DeclarationDescriptor* declaration_descriptor,
    const DeclarationParsingResult::Declaration* declaration,
    ZoneList<const AstRawString*>* names, bool* ok) {
  PatternRewriter rewriter;

  DCHECK(block->ignore_completion_value());

  rewriter.scope_ = declaration_descriptor->scope;
  rewriter.parser_ = declaration_descriptor->parser;
  rewriter.context_ = BINDING;
  rewriter.pattern_ = declaration->pattern;
  rewriter.initializer_position_ = declaration->initializer_position;
  rewriter.block_ = block;
  rewriter.descriptor_ = declaration_descriptor;
  rewriter.names_ = names;
  rewriter.ok_ = ok;
  rewriter.recursion_level_ = 0;

  rewriter.RecurseIntoSubpattern(rewriter.pattern_, declaration->initializer);
}


void Parser::PatternRewriter::RewriteDestructuringAssignment(
    Parser* parser, RewritableExpression* to_rewrite, Scope* scope) {
  PatternRewriter rewriter;

  DCHECK(!to_rewrite->is_rewritten());

  bool ok = true;
  rewriter.scope_ = scope;
  rewriter.parser_ = parser;
  rewriter.context_ = ASSIGNMENT;
  rewriter.pattern_ = to_rewrite;
  rewriter.block_ = nullptr;
  rewriter.descriptor_ = nullptr;
  rewriter.names_ = nullptr;
  rewriter.ok_ = &ok;
  rewriter.recursion_level_ = 0;

  rewriter.RecurseIntoSubpattern(rewriter.pattern_, nullptr);
  DCHECK(ok);
}


Expression* Parser::PatternRewriter::RewriteDestructuringAssignment(
    Parser* parser, Assignment* assignment, Scope* scope) {
  DCHECK_NOT_NULL(assignment);
  DCHECK_EQ(Token::ASSIGN, assignment->op());
  auto to_rewrite = parser->factory()->NewRewritableExpression(assignment);
  RewriteDestructuringAssignment(parser, to_rewrite, scope);
  return to_rewrite->expression();
}


bool Parser::PatternRewriter::IsAssignmentContext(PatternContext c) const {
  return c == ASSIGNMENT || c == ASSIGNMENT_INITIALIZER;
}


bool Parser::PatternRewriter::IsBindingContext(PatternContext c) const {
  return c == BINDING || c == INITIALIZER;
}


Parser::PatternRewriter::PatternContext
Parser::PatternRewriter::SetAssignmentContextIfNeeded(Expression* node) {
  PatternContext old_context = context();
  // AssignmentExpressions may occur in the Initializer position of a
  // SingleNameBinding. Such expressions should not prompt a change in the
  // pattern's context.
  if (node->IsAssignment() && node->AsAssignment()->op() == Token::ASSIGN &&
      !IsInitializerContext()) {
    set_context(ASSIGNMENT);
  }
  return old_context;
}


Parser::PatternRewriter::PatternContext
Parser::PatternRewriter::SetInitializerContextIfNeeded(Expression* node) {
  // Set appropriate initializer context for BindingElement and
  // AssignmentElement nodes
  PatternContext old_context = context();
  bool is_destructuring_assignment =
      node->IsRewritableExpression() &&
      !node->AsRewritableExpression()->is_rewritten();
  bool is_assignment =
      node->IsAssignment() && node->AsAssignment()->op() == Token::ASSIGN;
  if (is_destructuring_assignment || is_assignment) {
    switch (old_context) {
      case BINDING:
        set_context(INITIALIZER);
        break;
      case ASSIGNMENT:
        set_context(ASSIGNMENT_INITIALIZER);
        break;
      default:
        break;
    }
  }
  return old_context;
}


void Parser::PatternRewriter::VisitVariableProxy(VariableProxy* pattern) {
  Expression* value = current_value_;

  if (IsAssignmentContext()) {
    // In an assignment context, simply perform the assignment
    Assignment* assignment = factory()->NewAssignment(
        Token::ASSIGN, pattern, value, pattern->position());
    block_->statements()->Add(
        factory()->NewExpressionStatement(assignment, pattern->position()),
        zone());
    return;
  }

  descriptor_->scope->RemoveUnresolved(pattern);

  // Declare variable.
  // Note that we *always* must treat the initial value via a separate init
  // assignment for variables and constants because the value must be assigned
  // when the variable is encountered in the source. But the variable/constant
  // is declared (and set to 'undefined') upon entering the function within
  // which the variable or constant is declared. Only function variables have
  // an initial value in the declaration (because they are initialized upon
  // entering the function).
  //
  // If we have a legacy const declaration, in an inner scope, the proxy
  // is always bound to the declared variable (independent of possibly
  // surrounding 'with' statements).
  // For let/const declarations in harmony mode, we can also immediately
  // pre-resolve the proxy because it resides in the same scope as the
  // declaration.
  const AstRawString* name = pattern->raw_name();
  VariableProxy* proxy = parser_->NewUnresolved(name, descriptor_->mode);
  Declaration* declaration = factory()->NewVariableDeclaration(
      proxy, descriptor_->mode, descriptor_->scope,
      descriptor_->declaration_pos);
  Variable* var =
      parser_->Declare(declaration, descriptor_->declaration_kind,
                       descriptor_->mode != VAR, ok_, descriptor_->hoist_scope);
  if (!*ok_) return;
  DCHECK_NOT_NULL(var);
  DCHECK(!proxy->is_resolved() || proxy->var() == var);
  var->set_initializer_position(initializer_position_);

  DCHECK(initializer_position_ != RelocInfo::kNoPosition);

  Scope* declaration_scope = IsLexicalVariableMode(descriptor_->mode)
                                 ? descriptor_->scope
                                 : descriptor_->scope->DeclarationScope();
  if (declaration_scope->num_var_or_const() > kMaxNumFunctionLocals) {
    parser_->ReportMessage(MessageTemplate::kTooManyVariables);
    *ok_ = false;
    return;
  }
  if (names_) {
    names_->Add(name, zone());
  }

  // Initialize variables if needed. A
  // declaration of the form:
  //
  //    var v = x;
  //
  // is syntactic sugar for:
  //
  //    var v; v = x;
  //
  // In particular, we need to re-lookup 'v' (in scope_, not
  // declaration_scope) as it may be a different 'v' than the 'v' in the
  // declaration (e.g., if we are inside a 'with' statement or 'catch'
  // block).
  //
  // However, note that const declarations are different! A const
  // declaration of the form:
  //
  //   const c = x;
  //
  // is *not* syntactic sugar for:
  //
  //   const c; c = x;
  //
  // The "variable" c initialized to x is the same as the declared
  // one - there is no re-lookup (see the last parameter of the
  // Declare() call above).
  Scope* initialization_scope = IsImmutableVariableMode(descriptor_->mode)
                                    ? declaration_scope
                                    : descriptor_->scope;


  // Global variable declarations must be compiled in a specific
  // way. When the script containing the global variable declaration
  // is entered, the global variable must be declared, so that if it
  // doesn't exist (on the global object itself, see ES5 errata) it
  // gets created with an initial undefined value. This is handled
  // by the declarations part of the function representing the
  // top-level global code; see Runtime::DeclareGlobalVariable. If
  // it already exists (in the object or in a prototype), it is
  // *not* touched until the variable declaration statement is
  // executed.
  //
  // Executing the variable declaration statement will always
  // guarantee to give the global object an own property.
  // This way, global variable declarations can shadow
  // properties in the prototype chain, but only after the variable
  // declaration statement has been executed. This is important in
  // browsers where the global object (window) has lots of
  // properties defined in prototype objects.
  if (initialization_scope->is_script_scope() &&
      !IsLexicalVariableMode(descriptor_->mode)) {
    // Compute the arguments for the runtime
    // call.test-parsing/InitializedDeclarationsInStrictForOfError
    ZoneList<Expression*>* arguments =
        new (zone()) ZoneList<Expression*>(3, zone());
    // We have at least 1 parameter.
    arguments->Add(
        factory()->NewStringLiteral(name, descriptor_->declaration_pos),
        zone());
    CallRuntime* initialize;

    if (IsImmutableVariableMode(descriptor_->mode)) {
      arguments->Add(value, zone());
      // Construct the call to Runtime_InitializeConstGlobal
      // and add it to the initialization statement block.
      // Note that the function does different things depending on
      // the number of arguments (1 or 2).
      initialize = factory()->NewCallRuntime(Runtime::kInitializeConstGlobal,
                                             arguments, value->position());
      value = NULL;  // zap the value to avoid the unnecessary assignment
    } else {
      // Add language mode.
      // We may want to pass singleton to avoid Literal allocations.
      LanguageMode language_mode = initialization_scope->language_mode();
      arguments->Add(
          factory()->NewNumberLiteral(language_mode, RelocInfo::kNoPosition),
          zone());

      // Be careful not to assign a value to the global variable if
      // we're in a with. The initialization value should not
      // necessarily be stored in the global object in that case,
      // which is why we need to generate a separate assignment node.
      if (value != NULL && !descriptor_->scope->inside_with()) {
        arguments->Add(value, zone());
        // Construct the call to Runtime_InitializeVarGlobal
        // and add it to the initialization statement block.
        initialize = factory()->NewCallRuntime(Runtime::kInitializeVarGlobal,
                                               arguments, value->position());
        value = NULL;  // zap the value to avoid the unnecessary assignment
      } else {
        initialize = NULL;
      }
    }

    if (initialize != NULL) {
      block_->statements()->Add(
          factory()->NewExpressionStatement(initialize, initialize->position()),
          zone());
    }
  } else if (value != nullptr && (descriptor_->mode == CONST_LEGACY ||
                                  IsLexicalVariableMode(descriptor_->mode))) {
    // Constant initializations always assign to the declared constant which
    // is always at the function scope level. This is only relevant for
    // dynamically looked-up variables and constants (the
    // start context for constant lookups is always the function context,
    // while it is the top context for var declared variables). Sigh...
    // For 'let' and 'const' declared variables in harmony mode the
    // initialization also always assigns to the declared variable.
    DCHECK_NOT_NULL(proxy);
    DCHECK_NOT_NULL(proxy->var());
    DCHECK_NOT_NULL(value);
    // Add break location for destructured sub-pattern.
    int pos = IsSubPattern() ? pattern->position() : value->position();
    Assignment* assignment =
        factory()->NewAssignment(Token::INIT, proxy, value, pos);
    block_->statements()->Add(
        factory()->NewExpressionStatement(assignment, pos), zone());
    value = NULL;
  }

  // Add an assignment node to the initialization statement block if we still
  // have a pending initialization value.
  if (value != NULL) {
    DCHECK(descriptor_->mode == VAR);
    // 'var' initializations are simply assignments (with all the consequences
    // if they are inside a 'with' statement - they may change a 'with' object
    // property).
    VariableProxy* proxy = initialization_scope->NewUnresolved(factory(), name);
    // Add break location for destructured sub-pattern.
    int pos = IsSubPattern() ? pattern->position() : value->position();
    Assignment* assignment =
        factory()->NewAssignment(Token::INIT, proxy, value, pos);
    block_->statements()->Add(
        factory()->NewExpressionStatement(assignment, pos), zone());
  }
}


Variable* Parser::PatternRewriter::CreateTempVar(Expression* value) {
  auto temp = scope()->NewTemporary(ast_value_factory()->empty_string());
  if (value != nullptr) {
    auto assignment = factory()->NewAssignment(
        Token::ASSIGN, factory()->NewVariableProxy(temp), value,
        RelocInfo::kNoPosition);

    block_->statements()->Add(
        factory()->NewExpressionStatement(assignment, RelocInfo::kNoPosition),
        zone());
  }
  return temp;
}


void Parser::PatternRewriter::VisitRewritableExpression(
    RewritableExpression* node) {
  // If this is not a destructuring assignment...
  if (!IsAssignmentContext() || !node->expression()->IsAssignment()) {
    // Mark the node as rewritten to prevent redundant rewriting, and
    // perform BindingPattern rewriting
    DCHECK(!node->is_rewritten());
    node->Rewrite(node->expression());
    return node->expression()->Accept(this);
  }

  if (node->is_rewritten()) return;
  DCHECK(IsAssignmentContext());
  Assignment* assign = node->expression()->AsAssignment();
  DCHECK_NOT_NULL(assign);
  DCHECK_EQ(Token::ASSIGN, assign->op());

  auto initializer = assign->value();
  auto value = initializer;

  if (IsInitializerContext()) {
    // let {<pattern> = <init>} = <value>
    //   becomes
    // temp = <value>;
    // <pattern> = temp === undefined ? <init> : temp;
    auto temp_var = CreateTempVar(current_value_);
    Expression* is_undefined = factory()->NewCompareOperation(
        Token::EQ_STRICT, factory()->NewVariableProxy(temp_var),
        factory()->NewUndefinedLiteral(RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    value = factory()->NewConditional(is_undefined, initializer,
                                      factory()->NewVariableProxy(temp_var),
                                      RelocInfo::kNoPosition);
  }

  PatternContext old_context = SetAssignmentContextIfNeeded(initializer);
  int pos = assign->position();
  Block* old_block = block_;
  block_ = factory()->NewBlock(nullptr, 8, true, pos);
  Variable* temp = nullptr;
  Expression* pattern = assign->target();
  Expression* old_value = current_value_;
  current_value_ = value;
  if (pattern->IsObjectLiteral()) {
    VisitObjectLiteral(pattern->AsObjectLiteral(), &temp);
  } else {
    DCHECK(pattern->IsArrayLiteral());
    VisitArrayLiteral(pattern->AsArrayLiteral(), &temp);
  }
  DCHECK_NOT_NULL(temp);
  current_value_ = old_value;
  Expression* expr = factory()->NewDoExpression(block_, temp, pos);
  node->Rewrite(expr);
  block_ = old_block;
  if (block_) {
    block_->statements()->Add(factory()->NewExpressionStatement(expr, pos),
                              zone());
  }
  return set_context(old_context);
}


void Parser::PatternRewriter::VisitObjectLiteral(ObjectLiteral* pattern,
                                                 Variable** temp_var) {
  auto temp = *temp_var = CreateTempVar(current_value_);

  block_->statements()->Add(parser_->BuildAssertIsCoercible(temp), zone());

  for (ObjectLiteralProperty* property : *pattern->properties()) {
    PatternContext context = SetInitializerContextIfNeeded(property->value());
    RecurseIntoSubpattern(
        property->value(),
        factory()->NewProperty(factory()->NewVariableProxy(temp),
                               property->key(), RelocInfo::kNoPosition));
    set_context(context);
  }
}


void Parser::PatternRewriter::VisitObjectLiteral(ObjectLiteral* node) {
  Variable* temp_var = nullptr;
  VisitObjectLiteral(node, &temp_var);
}


void Parser::PatternRewriter::VisitArrayLiteral(ArrayLiteral* node,
                                                Variable** temp_var) {
  DCHECK(block_->ignore_completion_value());

  auto temp = *temp_var = CreateTempVar(current_value_);
  auto iterator = CreateTempVar(parser_->GetIterator(
      factory()->NewVariableProxy(temp), factory(), RelocInfo::kNoPosition));
  auto done = CreateTempVar(
      factory()->NewBooleanLiteral(false, RelocInfo::kNoPosition));
  auto result = CreateTempVar();
  auto v = CreateTempVar();
  auto completion = CreateTempVar();
  auto nopos = RelocInfo::kNoPosition;

  // For the purpose of iterator finalization, we temporarily set block_ to a
  // new block.  In the main body of this function, we write to block_ (both
  // explicitly and implicitly via recursion).  At the end of the function, we
  // wrap this new block in a try-finally statement, restore block_ to its
  // original value, and add the try-finally statement to block_.
  auto target = block_;
  if (FLAG_harmony_iterator_close) {
    block_ = factory()->NewBlock(nullptr, 8, true, nopos);
  }

  Spread* spread = nullptr;
  for (Expression* value : *node->values()) {
    if (value->IsSpread()) {
      spread = value->AsSpread();
      break;
    }

    PatternContext context = SetInitializerContextIfNeeded(value);

    // if (!done) {
    //   done = true;  // If .next, .done or .value throws, don't close.
    //   result = IteratorNext(iterator);
    //   if (result.done) {
    //     v = undefined;
    //   } else {
    //     v = result.value;
    //     done = false;
    //   }
    // }
    Statement* if_not_done;
    {
      auto result_done = factory()->NewProperty(
          factory()->NewVariableProxy(result),
          factory()->NewStringLiteral(ast_value_factory()->done_string(),
                                      RelocInfo::kNoPosition),
          RelocInfo::kNoPosition);

      auto assign_undefined = factory()->NewAssignment(
          Token::ASSIGN, factory()->NewVariableProxy(v),
          factory()->NewUndefinedLiteral(RelocInfo::kNoPosition),
          RelocInfo::kNoPosition);

      auto assign_value = factory()->NewAssignment(
          Token::ASSIGN, factory()->NewVariableProxy(v),
          factory()->NewProperty(
              factory()->NewVariableProxy(result),
              factory()->NewStringLiteral(ast_value_factory()->value_string(),
                                          RelocInfo::kNoPosition),
              RelocInfo::kNoPosition),
          RelocInfo::kNoPosition);

      auto unset_done = factory()->NewAssignment(
          Token::ASSIGN, factory()->NewVariableProxy(done),
          factory()->NewBooleanLiteral(false, RelocInfo::kNoPosition),
          RelocInfo::kNoPosition);

      auto inner_else =
          factory()->NewBlock(nullptr, 2, true, RelocInfo::kNoPosition);
      inner_else->statements()->Add(
          factory()->NewExpressionStatement(assign_value, nopos), zone());
      inner_else->statements()->Add(
          factory()->NewExpressionStatement(unset_done, nopos), zone());

      auto inner_if = factory()->NewIfStatement(
          result_done,
          factory()->NewExpressionStatement(assign_undefined, nopos),
          inner_else, nopos);

      auto next_block =
          factory()->NewBlock(nullptr, 3, true, RelocInfo::kNoPosition);
      next_block->statements()->Add(
          factory()->NewExpressionStatement(
              factory()->NewAssignment(
                  Token::ASSIGN, factory()->NewVariableProxy(done),
                  factory()->NewBooleanLiteral(true, nopos), nopos),
              nopos),
          zone());
      next_block->statements()->Add(
          factory()->NewExpressionStatement(
              parser_->BuildIteratorNextResult(
                  factory()->NewVariableProxy(iterator), result,
                  RelocInfo::kNoPosition),
              RelocInfo::kNoPosition),
          zone());
      next_block->statements()->Add(inner_if, zone());

      if_not_done = factory()->NewIfStatement(
          factory()->NewUnaryOperation(Token::NOT,
                                       factory()->NewVariableProxy(done),
                                       RelocInfo::kNoPosition),
          next_block, factory()->NewEmptyStatement(RelocInfo::kNoPosition),
          RelocInfo::kNoPosition);
    }
    block_->statements()->Add(if_not_done, zone());

    if (!(value->IsLiteral() && value->AsLiteral()->raw_value()->IsTheHole())) {
      if (FLAG_harmony_iterator_close) {
        // completion = kAbruptCompletion;
        Expression* proxy = factory()->NewVariableProxy(completion);
        Expression* assignment = factory()->NewAssignment(
            Token::ASSIGN, proxy,
            factory()->NewSmiLiteral(kAbruptCompletion, nopos), nopos);
        block_->statements()->Add(
            factory()->NewExpressionStatement(assignment, nopos), zone());
      }

      RecurseIntoSubpattern(value, factory()->NewVariableProxy(v));

      if (FLAG_harmony_iterator_close) {
        // completion = kNormalCompletion;
        Expression* proxy = factory()->NewVariableProxy(completion);
        Expression* assignment = factory()->NewAssignment(
            Token::ASSIGN, proxy,
            factory()->NewSmiLiteral(kNormalCompletion, nopos), nopos);
        block_->statements()->Add(
            factory()->NewExpressionStatement(assignment, nopos), zone());
      }
    }
    set_context(context);
  }

  if (spread != nullptr) {
    // A spread can only occur as the last component.  It is not handled by
    // RecurseIntoSubpattern above.

    // let array = [];
    // while (!done) {
    //   result = IteratorNext(iterator);
    //   if (result.done) {
    //     done = true;
    //   } else {
    //     %AppendElement(array, result.value);
    //   }
    // }

    // let array = [];
    Variable* array;
    {
      auto empty_exprs = new (zone()) ZoneList<Expression*>(0, zone());
      array = CreateTempVar(factory()->NewArrayLiteral(
          empty_exprs,
          // Reuse pattern's literal index - it is unused since there is no
          // actual literal allocated.
          node->literal_index(), RelocInfo::kNoPosition));
    }

    // result = IteratorNext(iterator);
    Statement* get_next = factory()->NewExpressionStatement(
        parser_->BuildIteratorNextResult(factory()->NewVariableProxy(iterator),
                                         result, nopos),
        nopos);

    // done = true;
    Statement* set_done = factory()->NewExpressionStatement(
        factory()->NewAssignment(
            Token::ASSIGN, factory()->NewVariableProxy(done),
            factory()->NewBooleanLiteral(true, nopos), nopos),
        nopos);

    // %AppendElement(array, result.value);
    Statement* append_element;
    {
      auto args = new (zone()) ZoneList<Expression*>(2, zone());
      args->Add(factory()->NewVariableProxy(array), zone());
      args->Add(factory()->NewProperty(
                    factory()->NewVariableProxy(result),
                    factory()->NewStringLiteral(
                        ast_value_factory()->value_string(), nopos),
                    nopos),
                zone());
      append_element = factory()->NewExpressionStatement(
          factory()->NewCallRuntime(Runtime::kAppendElement, args, nopos),
          nopos);
    }

    // if (result.done) { #set_done } else { #append_element }
    Statement* set_done_or_append;
    {
      Expression* result_done =
          factory()->NewProperty(factory()->NewVariableProxy(result),
                                 factory()->NewStringLiteral(
                                     ast_value_factory()->done_string(), nopos),
                                 nopos);
      set_done_or_append = factory()->NewIfStatement(result_done, set_done,
                                                     append_element, nopos);
    }

    // while (!done) {
    //   #get_next;
    //   #set_done_or_append;
    // }
    WhileStatement* loop = factory()->NewWhileStatement(nullptr, nopos);
    {
      Expression* condition = factory()->NewUnaryOperation(
          Token::NOT, factory()->NewVariableProxy(done), nopos);
      Block* body = factory()->NewBlock(nullptr, 2, true, nopos);
      body->statements()->Add(get_next, zone());
      body->statements()->Add(set_done_or_append, zone());
      loop->Initialize(condition, body);
    }

    block_->statements()->Add(loop, zone());
    RecurseIntoSubpattern(spread->expression(),
                          factory()->NewVariableProxy(array));
  }

  if (FLAG_harmony_iterator_close) {
    Expression* closing_condition = factory()->NewUnaryOperation(
        Token::NOT, factory()->NewVariableProxy(done), nopos);
    parser_->FinalizeIteratorUse(completion, closing_condition, iterator,
                                 block_, target);
    block_ = target;
  }
}


void Parser::PatternRewriter::VisitArrayLiteral(ArrayLiteral* node) {
  Variable* temp_var = nullptr;
  VisitArrayLiteral(node, &temp_var);
}


void Parser::PatternRewriter::VisitAssignment(Assignment* node) {
  // let {<pattern> = <init>} = <value>
  //   becomes
  // temp = <value>;
  // <pattern> = temp === undefined ? <init> : temp;
  DCHECK_EQ(Token::ASSIGN, node->op());

  auto initializer = node->value();
  auto value = initializer;
  auto temp = CreateTempVar(current_value_);

  if (IsInitializerContext()) {
    Expression* is_undefined = factory()->NewCompareOperation(
        Token::EQ_STRICT, factory()->NewVariableProxy(temp),
        factory()->NewUndefinedLiteral(RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    value = factory()->NewConditional(is_undefined, initializer,
                                      factory()->NewVariableProxy(temp),
                                      RelocInfo::kNoPosition);
  }

  if (IsBindingContext() &&
      descriptor_->declaration_kind == DeclarationDescriptor::PARAMETER &&
      scope()->is_arrow_scope()) {
    RewriteParameterInitializerScope(parser_->stack_limit(), initializer,
                                     scope()->outer_scope(), scope());
  }

  PatternContext old_context = SetAssignmentContextIfNeeded(initializer);
  RecurseIntoSubpattern(node->target(), value);
  set_context(old_context);
}


// =============== AssignmentPattern only ==================

void Parser::PatternRewriter::VisitProperty(v8::internal::Property* node) {
  DCHECK(IsAssignmentContext());
  auto value = current_value_;

  Assignment* assignment =
      factory()->NewAssignment(Token::ASSIGN, node, value, node->position());

  block_->statements()->Add(
      factory()->NewExpressionStatement(assignment, RelocInfo::kNoPosition),
      zone());
}


// =============== UNREACHABLE =============================

void Parser::PatternRewriter::Visit(AstNode* node) { UNREACHABLE(); }

#define NOT_A_PATTERN(Node)                                        \
  void Parser::PatternRewriter::Visit##Node(v8::internal::Node*) { \
    UNREACHABLE();                                                 \
  }

NOT_A_PATTERN(BinaryOperation)
NOT_A_PATTERN(Block)
NOT_A_PATTERN(BreakStatement)
NOT_A_PATTERN(Call)
NOT_A_PATTERN(CallNew)
NOT_A_PATTERN(CallRuntime)
NOT_A_PATTERN(CaseClause)
NOT_A_PATTERN(ClassLiteral)
NOT_A_PATTERN(CompareOperation)
NOT_A_PATTERN(Conditional)
NOT_A_PATTERN(ContinueStatement)
NOT_A_PATTERN(CountOperation)
NOT_A_PATTERN(DebuggerStatement)
NOT_A_PATTERN(DoExpression)
NOT_A_PATTERN(DoWhileStatement)
NOT_A_PATTERN(EmptyStatement)
NOT_A_PATTERN(EmptyParentheses)
NOT_A_PATTERN(ExportDeclaration)
NOT_A_PATTERN(ExpressionStatement)
NOT_A_PATTERN(ForInStatement)
NOT_A_PATTERN(ForOfStatement)
NOT_A_PATTERN(ForStatement)
NOT_A_PATTERN(FunctionDeclaration)
NOT_A_PATTERN(FunctionLiteral)
NOT_A_PATTERN(IfStatement)
NOT_A_PATTERN(ImportDeclaration)
NOT_A_PATTERN(Literal)
NOT_A_PATTERN(NativeFunctionLiteral)
NOT_A_PATTERN(RegExpLiteral)
NOT_A_PATTERN(ReturnStatement)
NOT_A_PATTERN(SloppyBlockFunctionStatement)
NOT_A_PATTERN(Spread)
NOT_A_PATTERN(SuperPropertyReference)
NOT_A_PATTERN(SuperCallReference)
NOT_A_PATTERN(SwitchStatement)
NOT_A_PATTERN(ThisFunction)
NOT_A_PATTERN(Throw)
NOT_A_PATTERN(TryCatchStatement)
NOT_A_PATTERN(TryFinallyStatement)
NOT_A_PATTERN(UnaryOperation)
NOT_A_PATTERN(VariableDeclaration)
NOT_A_PATTERN(WhileStatement)
NOT_A_PATTERN(WithStatement)
NOT_A_PATTERN(Yield)

#undef NOT_A_PATTERN
}  // namespace internal
}  // namespace v8
