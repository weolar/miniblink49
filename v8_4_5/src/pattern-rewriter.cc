// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/ast.h"
#include "src/messages.h"
#include "src/parser.h"

namespace v8 {

namespace internal {


void Parser::PatternRewriter::DeclareAndInitializeVariables(
    Block* block, const DeclarationDescriptor* declaration_descriptor,
    const DeclarationParsingResult::Declaration* declaration,
    ZoneList<const AstRawString*>* names, bool* ok) {
  PatternRewriter rewriter;

  rewriter.pattern_ = declaration->pattern;
  rewriter.initializer_position_ = declaration->initializer_position;
  rewriter.block_ = block;
  rewriter.descriptor_ = declaration_descriptor;
  rewriter.names_ = names;
  rewriter.ok_ = ok;

  rewriter.RecurseIntoSubpattern(rewriter.pattern_, declaration->initializer);
}


void Parser::PatternRewriter::VisitVariableProxy(VariableProxy* pattern) {
  Expression* value = current_value_;
  descriptor_->scope->RemoveUnresolved(pattern->AsVariableProxy());

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
  Parser* parser = descriptor_->parser;
  const AstRawString* name = pattern->raw_name();
  VariableProxy* proxy = parser->NewUnresolved(name, descriptor_->mode);
  Declaration* declaration = factory()->NewVariableDeclaration(
      proxy, descriptor_->mode, descriptor_->scope,
      descriptor_->declaration_pos);
  Variable* var = parser->Declare(declaration, descriptor_->declaration_kind,
                                  descriptor_->mode != VAR, ok_);
  if (!*ok_) return;
  DCHECK_NOT_NULL(var);
  DCHECK(!proxy->is_resolved() || proxy->var() == var);
  var->set_initializer_position(initializer_position_);

  DCHECK(initializer_position_ != RelocInfo::kNoPosition);

  if (descriptor_->declaration_scope->num_var_or_const() >
      kMaxNumFunctionLocals) {
    parser->ReportMessage(MessageTemplate::kTooManyVariables);
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
  Scope* initialization_scope = descriptor_->is_const
                                    ? descriptor_->declaration_scope
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

    if (descriptor_->is_const) {
      arguments->Add(value, zone());
      value = NULL;  // zap the value to avoid the unnecessary assignment

      // Construct the call to Runtime_InitializeConstGlobal
      // and add it to the initialization statement block.
      // Note that the function does different things depending on
      // the number of arguments (1 or 2).
      initialize = factory()->NewCallRuntime(
          ast_value_factory()->initialize_const_global_string(),
          Runtime::FunctionForId(Runtime::kInitializeConstGlobal), arguments,
          descriptor_->initialization_pos);
    } else {
      // Add language mode.
      // We may want to pass singleton to avoid Literal allocations.
      LanguageMode language_mode = initialization_scope->language_mode();
      arguments->Add(factory()->NewNumberLiteral(language_mode,
                                                 descriptor_->declaration_pos),
                     zone());

      // Be careful not to assign a value to the global variable if
      // we're in a with. The initialization value should not
      // necessarily be stored in the global object in that case,
      // which is why we need to generate a separate assignment node.
      if (value != NULL && !inside_with()) {
        arguments->Add(value, zone());
        value = NULL;  // zap the value to avoid the unnecessary assignment
        // Construct the call to Runtime_InitializeVarGlobal
        // and add it to the initialization statement block.
        initialize = factory()->NewCallRuntime(
            ast_value_factory()->initialize_var_global_string(),
            Runtime::FunctionForId(Runtime::kInitializeVarGlobal), arguments,
            descriptor_->declaration_pos);
      } else {
        initialize = NULL;
      }
    }

    if (initialize != NULL) {
      block_->AddStatement(
          factory()->NewExpressionStatement(initialize, RelocInfo::kNoPosition),
          zone());
    }
  } else if (value != nullptr && (descriptor_->needs_init ||
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
    Assignment* assignment = factory()->NewAssignment(
        descriptor_->init_op, proxy, value, descriptor_->initialization_pos);
    block_->AddStatement(
        factory()->NewExpressionStatement(assignment, RelocInfo::kNoPosition),
        zone());
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
    Assignment* assignment = factory()->NewAssignment(
        descriptor_->init_op, proxy, value, descriptor_->initialization_pos);
    block_->AddStatement(
        factory()->NewExpressionStatement(assignment, RelocInfo::kNoPosition),
        zone());
  }
}


Variable* Parser::PatternRewriter::CreateTempVar(Expression* value) {
  auto temp_scope = descriptor_->parser->scope_->DeclarationScope();
  auto temp = temp_scope->NewTemporary(ast_value_factory()->empty_string());
  if (value != nullptr) {
    auto assignment = factory()->NewAssignment(
        Token::ASSIGN, factory()->NewVariableProxy(temp), value,
        RelocInfo::kNoPosition);

    block_->AddStatement(
        factory()->NewExpressionStatement(assignment, RelocInfo::kNoPosition),
        zone());
  }
  return temp;
}


void Parser::PatternRewriter::VisitObjectLiteral(ObjectLiteral* pattern) {
  auto temp = CreateTempVar(current_value_);

  block_->AddStatement(descriptor_->parser->BuildAssertIsCoercible(temp),
                       zone());

  for (ObjectLiteralProperty* property : *pattern->properties()) {
    RecurseIntoSubpattern(
        property->value(),
        factory()->NewProperty(factory()->NewVariableProxy(temp),
                               property->key(), RelocInfo::kNoPosition));
  }
}


void Parser::PatternRewriter::VisitArrayLiteral(ArrayLiteral* node) {
  auto iterator = CreateTempVar(
      descriptor_->parser->GetIterator(current_value_, factory()));
  auto done = CreateTempVar(
      factory()->NewBooleanLiteral(false, RelocInfo::kNoPosition));
  auto result = CreateTempVar();
  auto v = CreateTempVar();

  Spread* spread = nullptr;
  for (Expression* value : *node->values()) {
    if (value->IsSpread()) {
      spread = value->AsSpread();
      break;
    }

    // if (!done) {
    //   result = IteratorNext(iterator);
    //   v = (done = result.done) ? undefined : result.value;
    // }
    auto next_block =
        factory()->NewBlock(nullptr, 2, true, RelocInfo::kNoPosition);
    next_block->AddStatement(factory()->NewExpressionStatement(
                                 descriptor_->parser->BuildIteratorNextResult(
                                     factory()->NewVariableProxy(iterator),
                                     result, RelocInfo::kNoPosition),
                                 RelocInfo::kNoPosition),
                             zone());

    auto assign_to_done = factory()->NewAssignment(
        Token::ASSIGN, factory()->NewVariableProxy(done),
        factory()->NewProperty(
            factory()->NewVariableProxy(result),
            factory()->NewStringLiteral(ast_value_factory()->done_string(),
                                        RelocInfo::kNoPosition),
            RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    auto next_value = factory()->NewConditional(
        assign_to_done, factory()->NewUndefinedLiteral(RelocInfo::kNoPosition),
        factory()->NewProperty(
            factory()->NewVariableProxy(result),
            factory()->NewStringLiteral(ast_value_factory()->value_string(),
                                        RelocInfo::kNoPosition),
            RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    next_block->AddStatement(
        factory()->NewExpressionStatement(
            factory()->NewAssignment(Token::ASSIGN,
                                     factory()->NewVariableProxy(v), next_value,
                                     RelocInfo::kNoPosition),
            RelocInfo::kNoPosition),
        zone());

    auto if_statement = factory()->NewIfStatement(
        factory()->NewUnaryOperation(Token::NOT,
                                     factory()->NewVariableProxy(done),
                                     RelocInfo::kNoPosition),
        next_block, factory()->NewEmptyStatement(RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    block_->AddStatement(if_statement, zone());

    if (!(value->IsLiteral() && value->AsLiteral()->raw_value()->IsTheHole())) {
      RecurseIntoSubpattern(value, factory()->NewVariableProxy(v));
    }
  }

  if (spread != nullptr) {
    // array = [];
    // if (!done) $concatIterableToArray(array, iterator);
    auto empty_exprs = new (zone()) ZoneList<Expression*>(0, zone());
    auto array = CreateTempVar(factory()->NewArrayLiteral(
        empty_exprs,
        // Reuse pattern's literal index - it is unused since there is no
        // actual literal allocated.
        node->literal_index(), is_strong(descriptor_->parser->language_mode()),
        RelocInfo::kNoPosition));

    auto arguments = new (zone()) ZoneList<Expression*>(2, zone());
    arguments->Add(factory()->NewVariableProxy(array), zone());
    arguments->Add(factory()->NewVariableProxy(iterator), zone());
    auto spread_into_array_call = factory()->NewCallRuntime(
        ast_value_factory()->concat_iterable_to_array_string(), nullptr,
        arguments, RelocInfo::kNoPosition);

    auto if_statement = factory()->NewIfStatement(
        factory()->NewUnaryOperation(Token::NOT,
                                     factory()->NewVariableProxy(done),
                                     RelocInfo::kNoPosition),
        factory()->NewExpressionStatement(spread_into_array_call,
                                          RelocInfo::kNoPosition),
        factory()->NewEmptyStatement(RelocInfo::kNoPosition),
        RelocInfo::kNoPosition);
    block_->AddStatement(if_statement, zone());


    RecurseIntoSubpattern(spread->expression(),
                          factory()->NewVariableProxy(array));
  }
}


void Parser::PatternRewriter::VisitAssignment(Assignment* node) {
  // let {<pattern> = <init>} = <value>
  //   becomes
  // temp = <value>;
  // <pattern> = temp === undefined ? <init> : temp;
  DCHECK(node->op() == Token::ASSIGN);
  auto temp = CreateTempVar(current_value_);
  Expression* is_undefined = factory()->NewCompareOperation(
      Token::EQ_STRICT, factory()->NewVariableProxy(temp),
      factory()->NewUndefinedLiteral(RelocInfo::kNoPosition),
      RelocInfo::kNoPosition);
  Expression* value = factory()->NewConditional(
      is_undefined, node->value(), factory()->NewVariableProxy(temp),
      RelocInfo::kNoPosition);
  RecurseIntoSubpattern(node->target(), value);
}


void Parser::PatternRewriter::VisitSpread(Spread* node) {
  // TODO(dslomov): implement.
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
NOT_A_PATTERN(DoWhileStatement)
NOT_A_PATTERN(EmptyStatement)
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
NOT_A_PATTERN(Property)
NOT_A_PATTERN(RegExpLiteral)
NOT_A_PATTERN(ReturnStatement)
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
