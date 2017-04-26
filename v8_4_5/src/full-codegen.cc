// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#include "src/ast.h"
#include "src/ast-numbering.h"
#include "src/code-factory.h"
#include "src/codegen.h"
#include "src/compiler.h"
#include "src/debug.h"
#include "src/full-codegen.h"
#include "src/liveedit.h"
#include "src/macro-assembler.h"
#include "src/prettyprinter.h"
#include "src/scopeinfo.h"
#include "src/scopes.h"
#include "src/snapshot/snapshot.h"

namespace v8 {
namespace internal {

#define __ ACCESS_MASM(masm())

bool FullCodeGenerator::MakeCode(CompilationInfo* info) {
  Isolate* isolate = info->isolate();

  TimerEventScope<TimerEventCompileFullCode> timer(info->isolate());

  // Ensure that the feedback vector is large enough.
  info->EnsureFeedbackVector();

  Handle<Script> script = info->script();
  if (!script->IsUndefined() && !script->source()->IsUndefined()) {
    int len = String::cast(script->source())->length();
    isolate->counters()->total_full_codegen_source_size()->Increment(len);
  }
  CodeGenerator::MakeCodePrologue(info, "full");
  const int kInitialBufferSize = 4 * KB;
  MacroAssembler masm(info->isolate(), NULL, kInitialBufferSize);
  if (info->will_serialize()) masm.enable_serializer();

  LOG_CODE_EVENT(isolate,
                 CodeStartLinePosInfoRecordEvent(masm.positions_recorder()));

  FullCodeGenerator cgen(&masm, info);
  cgen.Generate();
  if (cgen.HasStackOverflow()) {
    DCHECK(!isolate->has_pending_exception());
    return false;
  }
  unsigned table_offset = cgen.EmitBackEdgeTable();

  Code::Flags flags = Code::ComputeFlags(Code::FUNCTION);
  Handle<Code> code = CodeGenerator::MakeCodeEpilogue(&masm, flags, info);
  cgen.PopulateDeoptimizationData(code);
  cgen.PopulateTypeFeedbackInfo(code);
  cgen.PopulateHandlerTable(code);
  code->set_has_deoptimization_support(info->HasDeoptimizationSupport());
  code->set_has_reloc_info_for_serialization(info->will_serialize());
  code->set_compiled_optimizable(info->IsOptimizable());
  code->set_allow_osr_at_loop_nesting_level(0);
  code->set_profiler_ticks(0);
  code->set_back_edge_table_offset(table_offset);
  CodeGenerator::PrintCode(code, info);
  info->SetCode(code);
  void* line_info = masm.positions_recorder()->DetachJITHandlerData();
  LOG_CODE_EVENT(isolate, CodeEndLinePosInfoRecordEvent(*code, line_info));

#ifdef DEBUG
  // Check that no context-specific object has been embedded.
  code->VerifyEmbeddedObjects(Code::kNoContextSpecificPointers);
#endif  // DEBUG
  return true;
}


unsigned FullCodeGenerator::EmitBackEdgeTable() {
  // The back edge table consists of a length (in number of entries)
  // field, and then a sequence of entries.  Each entry is a pair of AST id
  // and code-relative pc offset.
  masm()->Align(kPointerSize);
  unsigned offset = masm()->pc_offset();
  unsigned length = back_edges_.length();
  __ dd(length);
  for (unsigned i = 0; i < length; ++i) {
    __ dd(back_edges_[i].id.ToInt());
    __ dd(back_edges_[i].pc);
    __ dd(back_edges_[i].loop_depth);
  }
  return offset;
}


void FullCodeGenerator::EnsureSlotContainsAllocationSite(
    FeedbackVectorSlot slot) {
  Handle<TypeFeedbackVector> vector = FeedbackVector();
  if (!vector->Get(slot)->IsAllocationSite()) {
    Handle<AllocationSite> allocation_site =
        isolate()->factory()->NewAllocationSite();
    vector->Set(slot, *allocation_site);
  }
}


void FullCodeGenerator::EnsureSlotContainsAllocationSite(
    FeedbackVectorICSlot slot) {
  Handle<TypeFeedbackVector> vector = FeedbackVector();
  if (!vector->Get(slot)->IsAllocationSite()) {
    Handle<AllocationSite> allocation_site =
        isolate()->factory()->NewAllocationSite();
    vector->Set(slot, *allocation_site);
  }
}


void FullCodeGenerator::PopulateDeoptimizationData(Handle<Code> code) {
  // Fill in the deoptimization information.
  DCHECK(info_->HasDeoptimizationSupport() || bailout_entries_.is_empty());
  if (!info_->HasDeoptimizationSupport()) return;
  int length = bailout_entries_.length();
  Handle<DeoptimizationOutputData> data =
      DeoptimizationOutputData::New(isolate(), length, TENURED);
  for (int i = 0; i < length; i++) {
    data->SetAstId(i, bailout_entries_[i].id);
    data->SetPcAndState(i, Smi::FromInt(bailout_entries_[i].pc_and_state));
  }
  code->set_deoptimization_data(*data);
}


void FullCodeGenerator::PopulateTypeFeedbackInfo(Handle<Code> code) {
  Handle<TypeFeedbackInfo> info = isolate()->factory()->NewTypeFeedbackInfo();
  info->set_ic_total_count(ic_total_count_);
  DCHECK(!isolate()->heap()->InNewSpace(*info));
  code->set_type_feedback_info(*info);
}


void FullCodeGenerator::PopulateHandlerTable(Handle<Code> code) {
  int handler_table_size = static_cast<int>(handler_table_.size());
  Handle<HandlerTable> table =
      Handle<HandlerTable>::cast(isolate()->factory()->NewFixedArray(
          HandlerTable::LengthForRange(handler_table_size), TENURED));
  for (int i = 0; i < handler_table_size; ++i) {
    HandlerTable::CatchPrediction prediction =
        handler_table_[i].try_catch_depth > 0 ? HandlerTable::CAUGHT
                                              : HandlerTable::UNCAUGHT;
    table->SetRangeStart(i, handler_table_[i].range_start);
    table->SetRangeEnd(i, handler_table_[i].range_end);
    table->SetRangeHandler(i, handler_table_[i].handler_offset, prediction);
    table->SetRangeDepth(i, handler_table_[i].stack_depth);
  }
  code->set_handler_table(*table);
}


int FullCodeGenerator::NewHandlerTableEntry() {
  int index = static_cast<int>(handler_table_.size());
  HandlerTableEntry entry = {0, 0, 0, 0, 0};
  handler_table_.push_back(entry);
  return index;
}


bool FullCodeGenerator::MustCreateObjectLiteralWithRuntime(
    ObjectLiteral* expr) const {
  int literal_flags = expr->ComputeFlags();
  // FastCloneShallowObjectStub doesn't copy elements, and object literals don't
  // support copy-on-write (COW) elements for now.
  // TODO(mvstanton): make object literals support COW elements.
  return masm()->serializer_enabled() ||
         literal_flags != ObjectLiteral::kShallowProperties ||
         literal_flags != ObjectLiteral::kFastElements ||
         expr->properties_count() >
             FastCloneShallowObjectStub::kMaximumClonedProperties;
}


bool FullCodeGenerator::MustCreateArrayLiteralWithRuntime(
    ArrayLiteral* expr) const {
  // TODO(rossberg): Teach strong mode to FastCloneShallowArrayStub.
  return expr->depth() > 1 || expr->is_strong() ||
         expr->values()->length() > JSObject::kInitialMaxFastElementArray;
}


void FullCodeGenerator::Initialize() {
  InitializeAstVisitor(info_->isolate(), info_->zone());
  // The generation of debug code must match between the snapshot code and the
  // code that is generated later.  This is assumed by the debugger when it is
  // calculating PC offsets after generating a debug version of code.  Therefore
  // we disable the production of debug code in the full compiler if we are
  // either generating a snapshot or we booted from a snapshot.
  generate_debug_code_ = FLAG_debug_code && !masm_->serializer_enabled() &&
                         !info_->isolate()->snapshot_available();
  masm_->set_emit_debug_code(generate_debug_code_);
  masm_->set_predictable_code_size(true);
}


void FullCodeGenerator::PrepareForBailout(Expression* node, State state) {
  PrepareForBailoutForId(node->id(), state);
}


void FullCodeGenerator::CallLoadIC(ContextualMode contextual_mode,
                                   LanguageMode language_mode,
                                   TypeFeedbackId id) {
  Handle<Code> ic =
      CodeFactory::LoadIC(isolate(), contextual_mode, language_mode).code();
  CallIC(ic, id);
}


void FullCodeGenerator::CallStoreIC(TypeFeedbackId id) {
  Handle<Code> ic = CodeFactory::StoreIC(isolate(), language_mode()).code();
  CallIC(ic, id);
}


void FullCodeGenerator::RecordJSReturnSite(Call* call) {
  // We record the offset of the function return so we can rebuild the frame
  // if the function was inlined, i.e., this is the return address in the
  // inlined function's frame.
  //
  // The state is ignored.  We defensively set it to TOS_REG, which is the
  // real state of the unoptimized code at the return site.
  PrepareForBailoutForId(call->ReturnId(), TOS_REG);
#ifdef DEBUG
  // In debug builds, mark the return so we can verify that this function
  // was called.
  DCHECK(!call->return_is_recorded_);
  call->return_is_recorded_ = true;
#endif
}


void FullCodeGenerator::PrepareForBailoutForId(BailoutId id, State state) {
  // There's no need to prepare this code for bailouts from already optimized
  // code or code that can't be optimized.
  if (!info_->HasDeoptimizationSupport()) return;
  unsigned pc_and_state =
      StateField::encode(state) | PcField::encode(masm_->pc_offset());
  DCHECK(Smi::IsValid(pc_and_state));
#ifdef DEBUG
  for (int i = 0; i < bailout_entries_.length(); ++i) {
    DCHECK(bailout_entries_[i].id != id);
  }
#endif
  BailoutEntry entry = { id, pc_and_state };
  bailout_entries_.Add(entry, zone());
}


void FullCodeGenerator::RecordBackEdge(BailoutId ast_id) {
  // The pc offset does not need to be encoded and packed together with a state.
  DCHECK(masm_->pc_offset() > 0);
  DCHECK(loop_depth() > 0);
  uint8_t depth = Min(loop_depth(), Code::kMaxLoopNestingMarker);
  BackEdgeEntry entry =
      { ast_id, static_cast<unsigned>(masm_->pc_offset()), depth };
  back_edges_.Add(entry, zone());
}


bool FullCodeGenerator::ShouldInlineSmiCase(Token::Value op) {
  // Inline smi case inside loops, but not division and modulo which
  // are too complicated and take up too much space.
  if (op == Token::DIV ||op == Token::MOD) return false;
  if (FLAG_always_inline_smi_code) return true;
  return loop_depth_ > 0;
}


void FullCodeGenerator::EffectContext::Plug(Register reg) const {
}


void FullCodeGenerator::AccumulatorValueContext::Plug(Register reg) const {
  __ Move(result_register(), reg);
}


void FullCodeGenerator::StackValueContext::Plug(Register reg) const {
  __ Push(reg);
}


void FullCodeGenerator::TestContext::Plug(Register reg) const {
  // For simplicity we always test the accumulator register.
  __ Move(result_register(), reg);
  codegen()->PrepareForBailoutBeforeSplit(condition(), false, NULL, NULL);
  codegen()->DoTest(this);
}


void FullCodeGenerator::EffectContext::PlugTOS() const {
  __ Drop(1);
}


void FullCodeGenerator::AccumulatorValueContext::PlugTOS() const {
  __ Pop(result_register());
}


void FullCodeGenerator::StackValueContext::PlugTOS() const {
}


void FullCodeGenerator::TestContext::PlugTOS() const {
  // For simplicity we always test the accumulator register.
  __ Pop(result_register());
  codegen()->PrepareForBailoutBeforeSplit(condition(), false, NULL, NULL);
  codegen()->DoTest(this);
}


void FullCodeGenerator::EffectContext::PrepareTest(
    Label* materialize_true,
    Label* materialize_false,
    Label** if_true,
    Label** if_false,
    Label** fall_through) const {
  // In an effect context, the true and the false case branch to the
  // same label.
  *if_true = *if_false = *fall_through = materialize_true;
}


void FullCodeGenerator::AccumulatorValueContext::PrepareTest(
    Label* materialize_true,
    Label* materialize_false,
    Label** if_true,
    Label** if_false,
    Label** fall_through) const {
  *if_true = *fall_through = materialize_true;
  *if_false = materialize_false;
}


void FullCodeGenerator::StackValueContext::PrepareTest(
    Label* materialize_true,
    Label* materialize_false,
    Label** if_true,
    Label** if_false,
    Label** fall_through) const {
  *if_true = *fall_through = materialize_true;
  *if_false = materialize_false;
}


void FullCodeGenerator::TestContext::PrepareTest(
    Label* materialize_true,
    Label* materialize_false,
    Label** if_true,
    Label** if_false,
    Label** fall_through) const {
  *if_true = true_label_;
  *if_false = false_label_;
  *fall_through = fall_through_;
}


void FullCodeGenerator::DoTest(const TestContext* context) {
  DoTest(context->condition(),
         context->true_label(),
         context->false_label(),
         context->fall_through());
}


void FullCodeGenerator::VisitDeclarations(
    ZoneList<Declaration*>* declarations) {
  ZoneList<Handle<Object> >* saved_globals = globals_;
  ZoneList<Handle<Object> > inner_globals(10, zone());
  globals_ = &inner_globals;

  AstVisitor::VisitDeclarations(declarations);

  if (!globals_->is_empty()) {
    // Invoke the platform-dependent code generator to do the actual
    // declaration of the global functions and variables.
    Handle<FixedArray> array =
       isolate()->factory()->NewFixedArray(globals_->length(), TENURED);
    for (int i = 0; i < globals_->length(); ++i)
      array->set(i, *globals_->at(i));
    DeclareGlobals(array);
  }

  globals_ = saved_globals;
}


int FullCodeGenerator::DeclareGlobalsFlags() {
  DCHECK(DeclareGlobalsLanguageMode::is_valid(language_mode()));
  return DeclareGlobalsEvalFlag::encode(is_eval()) |
         DeclareGlobalsNativeFlag::encode(is_native()) |
         DeclareGlobalsLanguageMode::encode(language_mode());
}


bool RecordStatementPosition(MacroAssembler* masm, int pos) {
  if (pos == RelocInfo::kNoPosition) return false;
  masm->positions_recorder()->RecordStatementPosition(pos);
  masm->positions_recorder()->RecordPosition(pos);
  return masm->positions_recorder()->WriteRecordedPositions();
}


bool RecordPosition(MacroAssembler* masm, int pos) {
  if (pos == RelocInfo::kNoPosition) return false;
  masm->positions_recorder()->RecordPosition(pos);
  return masm->positions_recorder()->WriteRecordedPositions();
}


void FullCodeGenerator::SetFunctionPosition(FunctionLiteral* fun) {
  RecordPosition(masm_, fun->start_position());
}


void FullCodeGenerator::SetReturnPosition(FunctionLiteral* fun) {
  RecordStatementPosition(masm_, fun->end_position() - 1);
}


void FullCodeGenerator::SetStatementPosition(
    Statement* stmt, FullCodeGenerator::InsertBreak insert_break) {
  if (stmt->position() == RelocInfo::kNoPosition) return;
  bool recorded = RecordStatementPosition(masm_, stmt->position());
  if (recorded && insert_break == INSERT_BREAK && info_->is_debug() &&
      !stmt->IsDebuggerStatement()) {
    DebugCodegen::GenerateSlot(masm_);
  }
}


void FullCodeGenerator::SetExpressionPosition(
    Expression* expr, FullCodeGenerator::InsertBreak insert_break) {
  if (expr->position() == RelocInfo::kNoPosition) return;
  bool recorded = RecordPosition(masm_, expr->position());
  if (recorded && insert_break == INSERT_BREAK && info_->is_debug()) {
    DebugCodegen::GenerateSlot(masm_);
  }
}


void FullCodeGenerator::SetExpressionAsStatementPosition(Expression* expr) {
  if (expr->position() == RelocInfo::kNoPosition) return;
  bool recorded = RecordStatementPosition(masm_, expr->position());
  if (recorded && info_->is_debug()) DebugCodegen::GenerateSlot(masm_);
}


void FullCodeGenerator::VisitSuperPropertyReference(
    SuperPropertyReference* super) {
  __ CallRuntime(Runtime::kThrowUnsupportedSuperError, 0);
}


void FullCodeGenerator::VisitSuperCallReference(SuperCallReference* super) {
  __ CallRuntime(Runtime::kThrowUnsupportedSuperError, 0);
}


void FullCodeGenerator::EmitGeneratorNext(CallRuntime* expr) {
  ZoneList<Expression*>* args = expr->arguments();
  DCHECK(args->length() == 2);
  EmitGeneratorResume(args->at(0), args->at(1), JSGeneratorObject::NEXT);
}


void FullCodeGenerator::EmitGeneratorThrow(CallRuntime* expr) {
  ZoneList<Expression*>* args = expr->arguments();
  DCHECK(args->length() == 2);
  EmitGeneratorResume(args->at(0), args->at(1), JSGeneratorObject::THROW);
}


void FullCodeGenerator::EmitDebugBreakInOptimizedCode(CallRuntime* expr) {
  context()->Plug(handle(Smi::FromInt(0), isolate()));
}


void FullCodeGenerator::VisitBinaryOperation(BinaryOperation* expr) {
  switch (expr->op()) {
    case Token::COMMA:
      return VisitComma(expr);
    case Token::OR:
    case Token::AND:
      return VisitLogicalExpression(expr);
    default:
      return VisitArithmeticExpression(expr);
  }
}


void FullCodeGenerator::VisitInDuplicateContext(Expression* expr) {
  if (context()->IsEffect()) {
    VisitForEffect(expr);
  } else if (context()->IsAccumulatorValue()) {
    VisitForAccumulatorValue(expr);
  } else if (context()->IsStackValue()) {
    VisitForStackValue(expr);
  } else if (context()->IsTest()) {
    const TestContext* test = TestContext::cast(context());
    VisitForControl(expr, test->true_label(), test->false_label(),
                    test->fall_through());
  }
}


void FullCodeGenerator::VisitComma(BinaryOperation* expr) {
  Comment cmnt(masm_, "[ Comma");
  VisitForEffect(expr->left());
  VisitInDuplicateContext(expr->right());
}


void FullCodeGenerator::VisitLogicalExpression(BinaryOperation* expr) {
  bool is_logical_and = expr->op() == Token::AND;
  Comment cmnt(masm_, is_logical_and ? "[ Logical AND" :  "[ Logical OR");
  Expression* left = expr->left();
  Expression* right = expr->right();
  BailoutId right_id = expr->RightId();
  Label done;

  if (context()->IsTest()) {
    Label eval_right;
    const TestContext* test = TestContext::cast(context());
    if (is_logical_and) {
      VisitForControl(left, &eval_right, test->false_label(), &eval_right);
    } else {
      VisitForControl(left, test->true_label(), &eval_right, &eval_right);
    }
    PrepareForBailoutForId(right_id, NO_REGISTERS);
    __ bind(&eval_right);

  } else if (context()->IsAccumulatorValue()) {
    VisitForAccumulatorValue(left);
    // We want the value in the accumulator for the test, and on the stack in
    // case we need it.
    __ Push(result_register());
    Label discard, restore;
    if (is_logical_and) {
      DoTest(left, &discard, &restore, &restore);
    } else {
      DoTest(left, &restore, &discard, &restore);
    }
    __ bind(&restore);
    __ Pop(result_register());
    __ jmp(&done);
    __ bind(&discard);
    __ Drop(1);
    PrepareForBailoutForId(right_id, NO_REGISTERS);

  } else if (context()->IsStackValue()) {
    VisitForAccumulatorValue(left);
    // We want the value in the accumulator for the test, and on the stack in
    // case we need it.
    __ Push(result_register());
    Label discard;
    if (is_logical_and) {
      DoTest(left, &discard, &done, &discard);
    } else {
      DoTest(left, &done, &discard, &discard);
    }
    __ bind(&discard);
    __ Drop(1);
    PrepareForBailoutForId(right_id, NO_REGISTERS);

  } else {
    DCHECK(context()->IsEffect());
    Label eval_right;
    if (is_logical_and) {
      VisitForControl(left, &eval_right, &done, &eval_right);
    } else {
      VisitForControl(left, &done, &eval_right, &eval_right);
    }
    PrepareForBailoutForId(right_id, NO_REGISTERS);
    __ bind(&eval_right);
  }

  VisitInDuplicateContext(right);
  __ bind(&done);
}


void FullCodeGenerator::VisitArithmeticExpression(BinaryOperation* expr) {
  Token::Value op = expr->op();
  Comment cmnt(masm_, "[ ArithmeticExpression");
  Expression* left = expr->left();
  Expression* right = expr->right();

  VisitForStackValue(left);
  VisitForAccumulatorValue(right);

  SetExpressionPosition(expr);
  if (ShouldInlineSmiCase(op)) {
    EmitInlineSmiBinaryOp(expr, op, left, right);
  } else {
    EmitBinaryOp(expr, op);
  }
}


void FullCodeGenerator::VisitForTypeofValue(Expression* expr) {
  VariableProxy* proxy = expr->AsVariableProxy();
  DCHECK(!context()->IsEffect());
  DCHECK(!context()->IsTest());

  if (proxy != NULL && (proxy->var()->IsUnallocatedOrGlobalSlot() ||
                        proxy->var()->IsLookupSlot())) {
    EmitVariableLoad(proxy, INSIDE_TYPEOF);
    PrepareForBailout(proxy, TOS_REG);
  } else {
    // This expression cannot throw a reference error at the top level.
    VisitInDuplicateContext(expr);
  }
}


void FullCodeGenerator::VisitBlock(Block* stmt) {
  Comment cmnt(masm_, "[ Block");
  NestedBlock nested_block(this, stmt);
  SetStatementPosition(stmt);

  {
    EnterBlockScopeIfNeeded block_scope_state(
        this, stmt->scope(), stmt->EntryId(), stmt->DeclsId(), stmt->ExitId());
    VisitStatements(stmt->statements());
    __ bind(nested_block.break_label());
  }
}


void FullCodeGenerator::VisitExpressionStatement(ExpressionStatement* stmt) {
  Comment cmnt(masm_, "[ ExpressionStatement");
  SetStatementPosition(stmt);
  VisitForEffect(stmt->expression());
}


void FullCodeGenerator::VisitEmptyStatement(EmptyStatement* stmt) {
  Comment cmnt(masm_, "[ EmptyStatement");
  SetStatementPosition(stmt);
}


void FullCodeGenerator::VisitIfStatement(IfStatement* stmt) {
  Comment cmnt(masm_, "[ IfStatement");
  SetStatementPosition(stmt);
  Label then_part, else_part, done;

  if (stmt->HasElseStatement()) {
    VisitForControl(stmt->condition(), &then_part, &else_part, &then_part);
    PrepareForBailoutForId(stmt->ThenId(), NO_REGISTERS);
    __ bind(&then_part);
    Visit(stmt->then_statement());
    __ jmp(&done);

    PrepareForBailoutForId(stmt->ElseId(), NO_REGISTERS);
    __ bind(&else_part);
    Visit(stmt->else_statement());
  } else {
    VisitForControl(stmt->condition(), &then_part, &done, &then_part);
    PrepareForBailoutForId(stmt->ThenId(), NO_REGISTERS);
    __ bind(&then_part);
    Visit(stmt->then_statement());

    PrepareForBailoutForId(stmt->ElseId(), NO_REGISTERS);
  }
  __ bind(&done);
  PrepareForBailoutForId(stmt->IfId(), NO_REGISTERS);
}


void FullCodeGenerator::VisitContinueStatement(ContinueStatement* stmt) {
  Comment cmnt(masm_,  "[ ContinueStatement");
  SetStatementPosition(stmt);
  NestedStatement* current = nesting_stack_;
  int stack_depth = 0;
  int context_length = 0;
  // When continuing, we clobber the unpredictable value in the accumulator
  // with one that's safe for GC.  If we hit an exit from the try block of
  // try...finally on our way out, we will unconditionally preserve the
  // accumulator on the stack.
  ClearAccumulator();
  while (!current->IsContinueTarget(stmt->target())) {
    current = current->Exit(&stack_depth, &context_length);
  }
  __ Drop(stack_depth);
  if (context_length > 0) {
    while (context_length > 0) {
      LoadContextField(context_register(), Context::PREVIOUS_INDEX);
      --context_length;
    }
    StoreToFrameField(StandardFrameConstants::kContextOffset,
                      context_register());
  }

  __ jmp(current->AsIteration()->continue_label());
}


void FullCodeGenerator::VisitBreakStatement(BreakStatement* stmt) {
  Comment cmnt(masm_,  "[ BreakStatement");
  SetStatementPosition(stmt);
  NestedStatement* current = nesting_stack_;
  int stack_depth = 0;
  int context_length = 0;
  // When breaking, we clobber the unpredictable value in the accumulator
  // with one that's safe for GC.  If we hit an exit from the try block of
  // try...finally on our way out, we will unconditionally preserve the
  // accumulator on the stack.
  ClearAccumulator();
  while (!current->IsBreakTarget(stmt->target())) {
    current = current->Exit(&stack_depth, &context_length);
  }
  __ Drop(stack_depth);
  if (context_length > 0) {
    while (context_length > 0) {
      LoadContextField(context_register(), Context::PREVIOUS_INDEX);
      --context_length;
    }
    StoreToFrameField(StandardFrameConstants::kContextOffset,
                      context_register());
  }

  __ jmp(current->AsBreakable()->break_label());
}


void FullCodeGenerator::EmitUnwindBeforeReturn() {
  NestedStatement* current = nesting_stack_;
  int stack_depth = 0;
  int context_length = 0;
  while (current != NULL) {
    current = current->Exit(&stack_depth, &context_length);
  }
  __ Drop(stack_depth);
}


void FullCodeGenerator::EmitPropertyKey(ObjectLiteralProperty* property,
                                        BailoutId bailout_id) {
  VisitForStackValue(property->key());
  __ InvokeBuiltin(Builtins::TO_NAME, CALL_FUNCTION);
  PrepareForBailoutForId(bailout_id, NO_REGISTERS);
  __ Push(result_register());
}


void FullCodeGenerator::EmitLoadSuperConstructor(SuperCallReference* ref) {
  VisitForStackValue(ref->this_function_var());
  __ CallRuntime(Runtime::kGetPrototype, 1);
}


void FullCodeGenerator::VisitReturnStatement(ReturnStatement* stmt) {
  Comment cmnt(masm_, "[ ReturnStatement");
  SetStatementPosition(stmt);
  Expression* expr = stmt->expression();
  VisitForAccumulatorValue(expr);
  EmitUnwindBeforeReturn();
  EmitReturnSequence();
}


void FullCodeGenerator::VisitWithStatement(WithStatement* stmt) {
  Comment cmnt(masm_, "[ WithStatement");
  SetStatementPosition(stmt);

  VisitForStackValue(stmt->expression());
  PushFunctionArgumentForContextAllocation();
  __ CallRuntime(Runtime::kPushWithContext, 2);
  StoreToFrameField(StandardFrameConstants::kContextOffset, context_register());
  PrepareForBailoutForId(stmt->EntryId(), NO_REGISTERS);

  Scope* saved_scope = scope();
  scope_ = stmt->scope();
  { WithOrCatch body(this);
    Visit(stmt->statement());
  }
  scope_ = saved_scope;

  // Pop context.
  LoadContextField(context_register(), Context::PREVIOUS_INDEX);
  // Update local stack frame context field.
  StoreToFrameField(StandardFrameConstants::kContextOffset, context_register());
}


void FullCodeGenerator::VisitDoWhileStatement(DoWhileStatement* stmt) {
  Comment cmnt(masm_, "[ DoWhileStatement");
  // Do not insert break location as we do that below.
  SetStatementPosition(stmt, SKIP_BREAK);

  Label body, book_keeping;

  Iteration loop_statement(this, stmt);
  increment_loop_depth();

  __ bind(&body);
  Visit(stmt->body());

  // Record the position of the do while condition and make sure it is
  // possible to break on the condition.
  __ bind(loop_statement.continue_label());
  PrepareForBailoutForId(stmt->ContinueId(), NO_REGISTERS);

  // Here is the actual 'while' keyword.
  SetExpressionAsStatementPosition(stmt->cond());
  VisitForControl(stmt->cond(),
                  &book_keeping,
                  loop_statement.break_label(),
                  &book_keeping);

  // Check stack before looping.
  PrepareForBailoutForId(stmt->BackEdgeId(), NO_REGISTERS);
  __ bind(&book_keeping);
  EmitBackEdgeBookkeeping(stmt, &body);
  __ jmp(&body);

  PrepareForBailoutForId(stmt->ExitId(), NO_REGISTERS);
  __ bind(loop_statement.break_label());
  decrement_loop_depth();
}


void FullCodeGenerator::VisitWhileStatement(WhileStatement* stmt) {
  Comment cmnt(masm_, "[ WhileStatement");
  Label loop, body;

  Iteration loop_statement(this, stmt);
  increment_loop_depth();

  __ bind(&loop);

  SetExpressionAsStatementPosition(stmt->cond());
  VisitForControl(stmt->cond(),
                  &body,
                  loop_statement.break_label(),
                  &body);

  PrepareForBailoutForId(stmt->BodyId(), NO_REGISTERS);
  __ bind(&body);
  Visit(stmt->body());

  __ bind(loop_statement.continue_label());

  // Check stack before looping.
  EmitBackEdgeBookkeeping(stmt, &loop);
  __ jmp(&loop);

  PrepareForBailoutForId(stmt->ExitId(), NO_REGISTERS);
  __ bind(loop_statement.break_label());
  decrement_loop_depth();
}


void FullCodeGenerator::VisitForStatement(ForStatement* stmt) {
  Comment cmnt(masm_, "[ ForStatement");
  // Do not insert break location as we do it below.
  SetStatementPosition(stmt, SKIP_BREAK);

  Label test, body;

  Iteration loop_statement(this, stmt);

  if (stmt->init() != NULL) {
    SetStatementPosition(stmt->init());
    Visit(stmt->init());
  }

  increment_loop_depth();
  // Emit the test at the bottom of the loop (even if empty).
  __ jmp(&test);

  PrepareForBailoutForId(stmt->BodyId(), NO_REGISTERS);
  __ bind(&body);
  Visit(stmt->body());

  PrepareForBailoutForId(stmt->ContinueId(), NO_REGISTERS);
  __ bind(loop_statement.continue_label());
  if (stmt->next() != NULL) {
    SetStatementPosition(stmt->next());
    Visit(stmt->next());
  }

  // Check stack before looping.
  EmitBackEdgeBookkeeping(stmt, &body);

  __ bind(&test);
  if (stmt->cond() != NULL) {
    SetExpressionAsStatementPosition(stmt->cond());
    VisitForControl(stmt->cond(),
                    &body,
                    loop_statement.break_label(),
                    loop_statement.break_label());
  } else {
    __ jmp(&body);
  }

  PrepareForBailoutForId(stmt->ExitId(), NO_REGISTERS);
  __ bind(loop_statement.break_label());
  decrement_loop_depth();
}


void FullCodeGenerator::VisitForOfStatement(ForOfStatement* stmt) {
  Comment cmnt(masm_, "[ ForOfStatement");

  Iteration loop_statement(this, stmt);
  increment_loop_depth();

  // var iterator = iterable[Symbol.iterator]();
  VisitForEffect(stmt->assign_iterator());

  // Loop entry.
  __ bind(loop_statement.continue_label());

  // result = iterator.next()
  SetExpressionAsStatementPosition(stmt->next_result());
  VisitForEffect(stmt->next_result());

  // if (result.done) break;
  Label result_not_done;
  VisitForControl(stmt->result_done(), loop_statement.break_label(),
                  &result_not_done, &result_not_done);
  __ bind(&result_not_done);

  // each = result.value
  VisitForEffect(stmt->assign_each());

  // Generate code for the body of the loop.
  Visit(stmt->body());

  // Check stack before looping.
  PrepareForBailoutForId(stmt->BackEdgeId(), NO_REGISTERS);
  EmitBackEdgeBookkeeping(stmt, loop_statement.continue_label());
  __ jmp(loop_statement.continue_label());

  // Exit and decrement the loop depth.
  PrepareForBailoutForId(stmt->ExitId(), NO_REGISTERS);
  __ bind(loop_statement.break_label());
  decrement_loop_depth();
}


void FullCodeGenerator::VisitTryCatchStatement(TryCatchStatement* stmt) {
  Comment cmnt(masm_, "[ TryCatchStatement");
  SetStatementPosition(stmt, SKIP_BREAK);

  // The try block adds a handler to the exception handler chain before
  // entering, and removes it again when exiting normally.  If an exception
  // is thrown during execution of the try block, the handler is consumed
  // and control is passed to the catch block with the exception in the
  // result register.

  Label try_entry, handler_entry, exit;
  __ jmp(&try_entry);
  __ bind(&handler_entry);
  PrepareForBailoutForId(stmt->HandlerId(), NO_REGISTERS);
  ClearPendingMessage();

  // Exception handler code, the exception is in the result register.
  // Extend the context before executing the catch block.
  { Comment cmnt(masm_, "[ Extend catch context");
    __ Push(stmt->variable()->name());
    __ Push(result_register());
    PushFunctionArgumentForContextAllocation();
    __ CallRuntime(Runtime::kPushCatchContext, 3);
    StoreToFrameField(StandardFrameConstants::kContextOffset,
                      context_register());
  }

  Scope* saved_scope = scope();
  scope_ = stmt->scope();
  DCHECK(scope_->declarations()->is_empty());
  { WithOrCatch catch_body(this);
    Visit(stmt->catch_block());
  }
  // Restore the context.
  LoadContextField(context_register(), Context::PREVIOUS_INDEX);
  StoreToFrameField(StandardFrameConstants::kContextOffset, context_register());
  scope_ = saved_scope;
  __ jmp(&exit);

  // Try block code. Sets up the exception handler chain.
  __ bind(&try_entry);

  try_catch_depth_++;
  int handler_index = NewHandlerTableEntry();
  EnterTryBlock(handler_index, &handler_entry);
  { TryCatch try_body(this);
    Visit(stmt->try_block());
  }
  ExitTryBlock(handler_index);
  try_catch_depth_--;
  __ bind(&exit);
}


void FullCodeGenerator::VisitTryFinallyStatement(TryFinallyStatement* stmt) {
  Comment cmnt(masm_, "[ TryFinallyStatement");
  SetStatementPosition(stmt, SKIP_BREAK);

  // Try finally is compiled by setting up a try-handler on the stack while
  // executing the try body, and removing it again afterwards.
  //
  // The try-finally construct can enter the finally block in three ways:
  // 1. By exiting the try-block normally. This removes the try-handler and
  //    calls the finally block code before continuing.
  // 2. By exiting the try-block with a function-local control flow transfer
  //    (break/continue/return). The site of the, e.g., break removes the
  //    try handler and calls the finally block code before continuing
  //    its outward control transfer.
  // 3. By exiting the try-block with a thrown exception.
  //    This can happen in nested function calls. It traverses the try-handler
  //    chain and consumes the try-handler entry before jumping to the
  //    handler code. The handler code then calls the finally-block before
  //    rethrowing the exception.
  //
  // The finally block must assume a return address on top of the stack
  // (or in the link register on ARM chips) and a value (return value or
  // exception) in the result register (rax/eax/r0), both of which must
  // be preserved. The return address isn't GC-safe, so it should be
  // cooked before GC.
  Label try_entry, handler_entry, finally_entry;

  // Jump to try-handler setup and try-block code.
  __ jmp(&try_entry);
  __ bind(&handler_entry);
  PrepareForBailoutForId(stmt->HandlerId(), NO_REGISTERS);

  // Exception handler code.  This code is only executed when an exception
  // is thrown.  The exception is in the result register, and must be
  // preserved by the finally block.  Call the finally block and then
  // rethrow the exception if it returns.
  __ Call(&finally_entry);
  __ Push(result_register());
  __ CallRuntime(Runtime::kReThrow, 1);

  // Finally block implementation.
  __ bind(&finally_entry);
  EnterFinallyBlock();
  { Finally finally_body(this);
    Visit(stmt->finally_block());
  }
  ExitFinallyBlock();  // Return to the calling code.

  // Set up try handler.
  __ bind(&try_entry);
  int handler_index = NewHandlerTableEntry();
  EnterTryBlock(handler_index, &handler_entry);
  { TryFinally try_body(this, &finally_entry);
    Visit(stmt->try_block());
  }
  ExitTryBlock(handler_index);
  // Execute the finally block on the way out.  Clobber the unpredictable
  // value in the result register with one that's safe for GC because the
  // finally block will unconditionally preserve the result register on the
  // stack.
  ClearAccumulator();
  __ Call(&finally_entry);
}


void FullCodeGenerator::VisitDebuggerStatement(DebuggerStatement* stmt) {
  Comment cmnt(masm_, "[ DebuggerStatement");
  SetStatementPosition(stmt);

  __ DebugBreak();
  // Ignore the return value.

  PrepareForBailoutForId(stmt->DebugBreakId(), NO_REGISTERS);
}


void FullCodeGenerator::VisitCaseClause(CaseClause* clause) {
  UNREACHABLE();
}


void FullCodeGenerator::VisitConditional(Conditional* expr) {
  Comment cmnt(masm_, "[ Conditional");
  Label true_case, false_case, done;
  VisitForControl(expr->condition(), &true_case, &false_case, &true_case);

  PrepareForBailoutForId(expr->ThenId(), NO_REGISTERS);
  __ bind(&true_case);
  SetExpressionPosition(expr->then_expression());
  if (context()->IsTest()) {
    const TestContext* for_test = TestContext::cast(context());
    VisitForControl(expr->then_expression(),
                    for_test->true_label(),
                    for_test->false_label(),
                    NULL);
  } else {
    VisitInDuplicateContext(expr->then_expression());
    __ jmp(&done);
  }

  PrepareForBailoutForId(expr->ElseId(), NO_REGISTERS);
  __ bind(&false_case);
  SetExpressionPosition(expr->else_expression());
  VisitInDuplicateContext(expr->else_expression());
  // If control flow falls through Visit, merge it with true case here.
  if (!context()->IsTest()) {
    __ bind(&done);
  }
}


void FullCodeGenerator::VisitLiteral(Literal* expr) {
  Comment cmnt(masm_, "[ Literal");
  context()->Plug(expr->value());
}


void FullCodeGenerator::VisitFunctionLiteral(FunctionLiteral* expr) {
  Comment cmnt(masm_, "[ FunctionLiteral");

  // Build the function boilerplate and instantiate it.
  Handle<SharedFunctionInfo> function_info =
      Compiler::GetSharedFunctionInfo(expr, script(), info_);
  if (function_info.is_null()) {
    SetStackOverflow();
    return;
  }
  EmitNewClosure(function_info, expr->pretenure());
}


void FullCodeGenerator::VisitClassLiteral(ClassLiteral* lit) {
  Comment cmnt(masm_, "[ ClassLiteral");

  {
    EnterBlockScopeIfNeeded block_scope_state(
        this, lit->scope(), lit->EntryId(), lit->DeclsId(), lit->ExitId());

    if (lit->raw_name() != NULL) {
      __ Push(lit->name());
    } else {
      __ Push(isolate()->factory()->undefined_value());
    }

    if (lit->extends() != NULL) {
      VisitForStackValue(lit->extends());
    } else {
      __ Push(isolate()->factory()->the_hole_value());
    }

    VisitForStackValue(lit->constructor());

    __ Push(script());
    __ Push(Smi::FromInt(lit->start_position()));
    __ Push(Smi::FromInt(lit->end_position()));

    __ CallRuntime(Runtime::kDefineClass, 6);
    PrepareForBailoutForId(lit->CreateLiteralId(), TOS_REG);

    int store_slot_index = 0;
    EmitClassDefineProperties(lit, &store_slot_index);

    if (lit->scope() != NULL) {
      DCHECK_NOT_NULL(lit->class_variable_proxy());
      FeedbackVectorICSlot slot = FLAG_vector_stores
                                      ? lit->GetNthSlot(store_slot_index++)
                                      : FeedbackVectorICSlot::Invalid();
      EmitVariableAssignment(lit->class_variable_proxy()->var(),
                             Token::INIT_CONST, slot);
    }

    // Verify that compilation exactly consumed the number of store ic slots
    // that the ClassLiteral node had to offer.
    DCHECK(!FLAG_vector_stores || store_slot_index == lit->slot_count());
  }

  context()->Plug(result_register());
}


void FullCodeGenerator::VisitNativeFunctionLiteral(
    NativeFunctionLiteral* expr) {
  Comment cmnt(masm_, "[ NativeFunctionLiteral");

  v8::Isolate* v8_isolate = reinterpret_cast<v8::Isolate*>(isolate());

  // Compute the function template for the native function.
  Handle<String> name = expr->name();
  v8::Local<v8::FunctionTemplate> fun_template =
      expr->extension()->GetNativeFunctionTemplate(v8_isolate,
                                                   v8::Utils::ToLocal(name));
  DCHECK(!fun_template.IsEmpty());

  // Instantiate the function and create a shared function info from it.
  Handle<JSFunction> fun = Utils::OpenHandle(
      *fun_template->GetFunction(v8_isolate->GetCurrentContext())
           .ToLocalChecked());
  const int literals = fun->NumberOfLiterals();
  Handle<Code> code = Handle<Code>(fun->shared()->code());
  Handle<Code> construct_stub = Handle<Code>(fun->shared()->construct_stub());
  Handle<SharedFunctionInfo> shared =
      isolate()->factory()->NewSharedFunctionInfo(
          name, literals, FunctionKind::kNormalFunction, code,
          Handle<ScopeInfo>(fun->shared()->scope_info()),
          Handle<TypeFeedbackVector>(fun->shared()->feedback_vector()));
  shared->set_construct_stub(*construct_stub);

  // Copy the function data to the shared function info.
  shared->set_function_data(fun->shared()->function_data());
  int parameters = fun->shared()->internal_formal_parameter_count();
  shared->set_internal_formal_parameter_count(parameters);

  EmitNewClosure(shared, false);
}


void FullCodeGenerator::VisitThrow(Throw* expr) {
  Comment cmnt(masm_, "[ Throw");
  VisitForStackValue(expr->exception());
  SetExpressionPosition(expr);
  __ CallRuntime(Runtime::kThrow, 1);
  // Never returns here.
}


void FullCodeGenerator::EnterTryBlock(int handler_index, Label* handler) {
  HandlerTableEntry* entry = &handler_table_[handler_index];
  entry->range_start = masm()->pc_offset();
  entry->handler_offset = handler->pos();
  entry->try_catch_depth = try_catch_depth_;

  // Determine expression stack depth of try statement.
  int stack_depth = info_->scope()->num_stack_slots();  // Include stack locals.
  for (NestedStatement* current = nesting_stack_; current != NULL; /*nop*/) {
    current = current->AccumulateDepth(&stack_depth);
  }
  entry->stack_depth = stack_depth;

  // Push context onto operand stack.
  STATIC_ASSERT(TryBlockConstant::kElementCount == 1);
  __ Push(context_register());
}


void FullCodeGenerator::ExitTryBlock(int handler_index) {
  HandlerTableEntry* entry = &handler_table_[handler_index];
  entry->range_end = masm()->pc_offset();

  // Drop context from operand stack.
  __ Drop(TryBlockConstant::kElementCount);
}


void FullCodeGenerator::VisitSpread(Spread* expr) { UNREACHABLE(); }


FullCodeGenerator::NestedStatement* FullCodeGenerator::TryFinally::Exit(
    int* stack_depth, int* context_length) {
  // The macros used here must preserve the result register.

  // Because the handler block contains the context of the finally
  // code, we can restore it directly from there for the finally code
  // rather than iteratively unwinding contexts via their previous
  // links.
  if (*context_length > 0) {
    __ Drop(*stack_depth);  // Down to the handler block.
    // Restore the context to its dedicated register and the stack.
    STATIC_ASSERT(TryFinally::kElementCount == 1);
    __ Pop(codegen_->context_register());
    codegen_->StoreToFrameField(StandardFrameConstants::kContextOffset,
                                codegen_->context_register());
  } else {
    // Down to the handler block and also drop context.
    __ Drop(*stack_depth + kElementCount);
  }
  __ Call(finally_entry_);

  *stack_depth = 0;
  *context_length = 0;
  return previous_;
}


bool FullCodeGenerator::TryLiteralCompare(CompareOperation* expr) {
  Expression* sub_expr;
  Handle<String> check;
  if (expr->IsLiteralCompareTypeof(&sub_expr, &check)) {
    EmitLiteralCompareTypeof(expr, sub_expr, check);
    return true;
  }

  if (expr->IsLiteralCompareUndefined(&sub_expr, isolate())) {
    EmitLiteralCompareNil(expr, sub_expr, kUndefinedValue);
    return true;
  }

  if (expr->IsLiteralCompareNull(&sub_expr)) {
    EmitLiteralCompareNil(expr, sub_expr, kNullValue);
    return true;
  }

  return false;
}


void BackEdgeTable::Patch(Isolate* isolate, Code* unoptimized) {
  DisallowHeapAllocation no_gc;
  Code* patch = isolate->builtins()->builtin(Builtins::kOnStackReplacement);

  // Increment loop nesting level by one and iterate over the back edge table
  // to find the matching loops to patch the interrupt
  // call to an unconditional call to the replacement code.
  int loop_nesting_level = unoptimized->allow_osr_at_loop_nesting_level() + 1;
  if (loop_nesting_level > Code::kMaxLoopNestingMarker) return;

  BackEdgeTable back_edges(unoptimized, &no_gc);
  for (uint32_t i = 0; i < back_edges.length(); i++) {
    if (static_cast<int>(back_edges.loop_depth(i)) == loop_nesting_level) {
      DCHECK_EQ(INTERRUPT, GetBackEdgeState(isolate,
                                            unoptimized,
                                            back_edges.pc(i)));
      PatchAt(unoptimized, back_edges.pc(i), ON_STACK_REPLACEMENT, patch);
    }
  }

  unoptimized->set_allow_osr_at_loop_nesting_level(loop_nesting_level);
  DCHECK(Verify(isolate, unoptimized));
}


void BackEdgeTable::Revert(Isolate* isolate, Code* unoptimized) {
  DisallowHeapAllocation no_gc;
  Code* patch = isolate->builtins()->builtin(Builtins::kInterruptCheck);

  // Iterate over the back edge table and revert the patched interrupt calls.
  int loop_nesting_level = unoptimized->allow_osr_at_loop_nesting_level();

  BackEdgeTable back_edges(unoptimized, &no_gc);
  for (uint32_t i = 0; i < back_edges.length(); i++) {
    if (static_cast<int>(back_edges.loop_depth(i)) <= loop_nesting_level) {
      DCHECK_NE(INTERRUPT, GetBackEdgeState(isolate,
                                            unoptimized,
                                            back_edges.pc(i)));
      PatchAt(unoptimized, back_edges.pc(i), INTERRUPT, patch);
    }
  }

  unoptimized->set_allow_osr_at_loop_nesting_level(0);
  // Assert that none of the back edges are patched anymore.
  DCHECK(Verify(isolate, unoptimized));
}


void BackEdgeTable::AddStackCheck(Handle<Code> code, uint32_t pc_offset) {
  DisallowHeapAllocation no_gc;
  Isolate* isolate = code->GetIsolate();
  Address pc = code->instruction_start() + pc_offset;
  Code* patch = isolate->builtins()->builtin(Builtins::kOsrAfterStackCheck);
  PatchAt(*code, pc, OSR_AFTER_STACK_CHECK, patch);
}


void BackEdgeTable::RemoveStackCheck(Handle<Code> code, uint32_t pc_offset) {
  DisallowHeapAllocation no_gc;
  Isolate* isolate = code->GetIsolate();
  Address pc = code->instruction_start() + pc_offset;

  if (OSR_AFTER_STACK_CHECK == GetBackEdgeState(isolate, *code, pc)) {
    Code* patch = isolate->builtins()->builtin(Builtins::kOnStackReplacement);
    PatchAt(*code, pc, ON_STACK_REPLACEMENT, patch);
  }
}


#ifdef DEBUG
bool BackEdgeTable::Verify(Isolate* isolate, Code* unoptimized) {
  DisallowHeapAllocation no_gc;
  int loop_nesting_level = unoptimized->allow_osr_at_loop_nesting_level();
  BackEdgeTable back_edges(unoptimized, &no_gc);
  for (uint32_t i = 0; i < back_edges.length(); i++) {
    uint32_t loop_depth = back_edges.loop_depth(i);
    CHECK_LE(static_cast<int>(loop_depth), Code::kMaxLoopNestingMarker);
    // Assert that all back edges for shallower loops (and only those)
    // have already been patched.
    CHECK_EQ((static_cast<int>(loop_depth) <= loop_nesting_level),
             GetBackEdgeState(isolate,
                              unoptimized,
                              back_edges.pc(i)) != INTERRUPT);
  }
  return true;
}
#endif  // DEBUG


FullCodeGenerator::EnterBlockScopeIfNeeded::EnterBlockScopeIfNeeded(
    FullCodeGenerator* codegen, Scope* scope, BailoutId entry_id,
    BailoutId declarations_id, BailoutId exit_id)
    : codegen_(codegen), exit_id_(exit_id) {
  saved_scope_ = codegen_->scope();

  if (scope == NULL) {
    codegen_->PrepareForBailoutForId(entry_id, NO_REGISTERS);
    needs_block_context_ = false;
  } else {
    needs_block_context_ = scope->ContextLocalCount() > 0;
    codegen_->scope_ = scope;
    {
      if (needs_block_context_) {
        Comment cmnt(masm(), "[ Extend block context");
        __ Push(scope->GetScopeInfo(codegen->isolate()));
        codegen_->PushFunctionArgumentForContextAllocation();
        __ CallRuntime(Runtime::kPushBlockContext, 2);

        // Replace the context stored in the frame.
        codegen_->StoreToFrameField(StandardFrameConstants::kContextOffset,
                                    codegen_->context_register());
      }
      CHECK_EQ(0, scope->num_stack_slots());
      codegen_->PrepareForBailoutForId(entry_id, NO_REGISTERS);
    }
    {
      Comment cmnt(masm(), "[ Declarations");
      codegen_->VisitDeclarations(scope->declarations());
      codegen_->PrepareForBailoutForId(declarations_id, NO_REGISTERS);
    }
  }
}


FullCodeGenerator::EnterBlockScopeIfNeeded::~EnterBlockScopeIfNeeded() {
  if (needs_block_context_) {
    codegen_->LoadContextField(codegen_->context_register(),
                               Context::PREVIOUS_INDEX);
    // Update local stack frame context field.
    codegen_->StoreToFrameField(StandardFrameConstants::kContextOffset,
                                codegen_->context_register());
  }
  codegen_->PrepareForBailoutForId(exit_id_, NO_REGISTERS);
  codegen_->scope_ = saved_scope_;
}


#undef __


}  // namespace internal
}  // namespace v8
