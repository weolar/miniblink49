// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_H_
#define V8_COMPILER_H_

#include "src/allocation.h"
#include "src/ast/ast.h"
#include "src/bailout-reason.h"
#include "src/compilation-dependencies.h"
#include "src/source-position.h"
#include "src/zone.h"

namespace v8 {
namespace internal {

// Forward declarations.
class CompilationInfo;
class JavaScriptFrame;
class OptimizedCompileJob;
class ParseInfo;
class ScriptData;

// The V8 compiler API.
//
// This is the central hub for dispatching to the various compilers within V8.
// Logic for which compiler to choose and how to wire compilation results into
// the object heap should be kept inside this class.
//
// General strategy: Scripts are translated into anonymous functions w/o
// parameters which then can be executed. If the source code contains other
// functions, they might be compiled and allocated as part of the compilation
// of the source code or deferred for lazy compilation at a later point.
class Compiler : public AllStatic {
 public:
  enum ClearExceptionFlag { KEEP_EXCEPTION, CLEAR_EXCEPTION };
  enum ConcurrencyMode { NOT_CONCURRENT, CONCURRENT };

  // ===========================================================================
  // The following family of methods ensures a given function is compiled. The
  // general contract is that failures will be reported by returning {false},
  // whereas successful compilation ensures the {is_compiled} predicate on the
  // given function holds (except for live-edit, which compiles the world).

  static bool Compile(Handle<JSFunction> function, ClearExceptionFlag flag);
  static bool CompileOptimized(Handle<JSFunction> function, ConcurrencyMode);
  static bool CompileDebugCode(Handle<JSFunction> function);
  static bool CompileDebugCode(Handle<SharedFunctionInfo> shared);
  static void CompileForLiveEdit(Handle<Script> script);

  // Generate and install code from previously queued optimization job.
  static void FinalizeOptimizedCompileJob(OptimizedCompileJob* job);

  // Give the compiler a chance to perform low-latency initialization tasks of
  // the given {function} on its instantiation. Note that only the runtime will
  // offer this chance, optimized closure instantiation will not call this.
  static void PostInstantiation(Handle<JSFunction> function, PretenureFlag);

  // Parser::Parse, then Compiler::Analyze.
  static bool ParseAndAnalyze(ParseInfo* info);
  // Rewrite, analyze scopes, and renumber.
  static bool Analyze(ParseInfo* info);
  // Adds deoptimization support, requires ParseAndAnalyze.
  static bool EnsureDeoptimizationSupport(CompilationInfo* info);

  // ===========================================================================
  // The following family of methods instantiates new functions for scripts or
  // function literals. The decision whether those functions will be compiled,
  // is left to the discretion of the compiler.
  //
  // Please note this interface returns shared function infos.  This means you
  // need to call Factory::NewFunctionFromSharedFunctionInfo before you have a
  // real function with a context.

  // Create a (bound) function for a String source within a context for eval.
  MUST_USE_RESULT static MaybeHandle<JSFunction> GetFunctionFromEval(
      Handle<String> source, Handle<SharedFunctionInfo> outer_info,
      Handle<Context> context, LanguageMode language_mode,
      ParseRestriction restriction, int line_offset, int column_offset = 0,
      Handle<Object> script_name = Handle<Object>(),
      ScriptOriginOptions options = ScriptOriginOptions());

  // Create a shared function info object for a String source within a context.
  static Handle<SharedFunctionInfo> GetSharedFunctionInfoForScript(
      Handle<String> source, Handle<Object> script_name, int line_offset,
      int column_offset, ScriptOriginOptions resource_options,
      Handle<Object> source_map_url, Handle<Context> context,
      v8::Extension* extension, ScriptData** cached_data,
      ScriptCompiler::CompileOptions compile_options,
      NativesFlag is_natives_code, bool is_module);

  // Create a shared function info object for a Script that has already been
  // parsed while the script was being loaded from a streamed source.
  static Handle<SharedFunctionInfo> GetSharedFunctionInfoForStreamedScript(
      Handle<Script> script, ParseInfo* info, int source_length);

  // Create a shared function info object (the code may be lazily compiled).
  static Handle<SharedFunctionInfo> GetSharedFunctionInfo(
      FunctionLiteral* node, Handle<Script> script, CompilationInfo* outer);

  // Create a shared function info object for a native function literal.
  static Handle<SharedFunctionInfo> GetSharedFunctionInfoForNative(
      v8::Extension* extension, Handle<String> name);

  // ===========================================================================
  // The following family of methods provides support for OSR. Code generated
  // for entry via OSR might not be suitable for normal entry, hence will be
  // returned directly to the caller.
  //
  // Please note this interface is the only part dealing with {Code} objects
  // directly. Other methods are agnostic to {Code} and can use an interpreter
  // instead of generating JIT code for a function at all.

  // Generate and return optimized code for OSR, or empty handle on failure.
  MUST_USE_RESULT static MaybeHandle<Code> GetOptimizedCodeForOSR(
      Handle<JSFunction> function, BailoutId osr_ast_id,
      JavaScriptFrame* osr_frame);
};

struct InlinedFunctionInfo {
  InlinedFunctionInfo(int parent_id, SourcePosition inline_position,
                      int script_id, int start_position)
      : parent_id(parent_id),
        inline_position(inline_position),
        script_id(script_id),
        start_position(start_position) {}
  int parent_id;
  SourcePosition inline_position;
  int script_id;
  int start_position;
  std::vector<size_t> deopt_pc_offsets;

  static const int kNoParentId = -1;
};


// CompilationInfo encapsulates some information known at compile time.  It
// is constructed based on the resources available at compile-time.
class CompilationInfo {
 public:
  // Various configuration flags for a compilation, as well as some properties
  // of the compiled code produced by a compilation.
  enum Flag {
    kDeferredCalling = 1 << 0,
    kNonDeferredCalling = 1 << 1,
    kSavesCallerDoubles = 1 << 2,
    kRequiresFrame = 1 << 3,
    kMustNotHaveEagerFrame = 1 << 4,
    kDeoptimizationSupport = 1 << 5,
    kDebug = 1 << 6,
    kSerializing = 1 << 7,
    kFunctionContextSpecializing = 1 << 8,
    kFrameSpecializing = 1 << 9,
    kNativeContextSpecializing = 1 << 10,
    kInliningEnabled = 1 << 11,
    kTypingEnabled = 1 << 12,
    kDisableFutureOptimization = 1 << 13,
    kSplittingEnabled = 1 << 14,
    kDeoptimizationEnabled = 1 << 16,
    kSourcePositionsEnabled = 1 << 17,
    kFirstCompile = 1 << 18,
    kBailoutOnUninitialized = 1 << 19,
  };

  explicit CompilationInfo(ParseInfo* parse_info);
  CompilationInfo(const char* debug_name, Isolate* isolate, Zone* zone,
                  Code::Flags code_flags = Code::ComputeFlags(Code::STUB));
  virtual ~CompilationInfo();

  ParseInfo* parse_info() const { return parse_info_; }

  // -----------------------------------------------------------
  // TODO(titzer): inline and delete accessors of ParseInfo
  // -----------------------------------------------------------
  Handle<Script> script() const;
  bool is_eval() const;
  bool is_native() const;
  bool is_module() const;
  LanguageMode language_mode() const;
  Handle<JSFunction> closure() const;
  FunctionLiteral* literal() const;
  Scope* scope() const;
  Handle<Context> context() const;
  Handle<SharedFunctionInfo> shared_info() const;
  bool has_shared_info() const;
  bool has_context() const;
  bool has_literal() const;
  bool has_scope() const;
  // -----------------------------------------------------------

  Isolate* isolate() const {
    return isolate_;
  }
  Zone* zone() { return zone_; }
  bool is_osr() const { return !osr_ast_id_.IsNone(); }
  Handle<Code> code() const { return code_; }
  Code::Flags code_flags() const { return code_flags_; }
  BailoutId osr_ast_id() const { return osr_ast_id_; }
  int opt_count() const { return opt_count_; }
  int num_parameters() const;
  int num_parameters_including_this() const;
  bool is_this_defined() const;
  int num_heap_slots() const;

  void set_parameter_count(int parameter_count) {
    DCHECK(IsStub());
    parameter_count_ = parameter_count;
  }

  bool has_bytecode_array() const { return !bytecode_array_.is_null(); }
  Handle<BytecodeArray> bytecode_array() const { return bytecode_array_; }

  Handle<AbstractCode> abstract_code() const {
    return has_bytecode_array() ? Handle<AbstractCode>::cast(bytecode_array())
                                : Handle<AbstractCode>::cast(code());
  }

  bool is_tracking_positions() const { return track_positions_; }

  bool is_calling() const {
    return GetFlag(kDeferredCalling) || GetFlag(kNonDeferredCalling);
  }

  void MarkAsDeferredCalling() { SetFlag(kDeferredCalling); }

  bool is_deferred_calling() const { return GetFlag(kDeferredCalling); }

  void MarkAsNonDeferredCalling() { SetFlag(kNonDeferredCalling); }

  bool is_non_deferred_calling() const { return GetFlag(kNonDeferredCalling); }

  void MarkAsSavesCallerDoubles() { SetFlag(kSavesCallerDoubles); }

  bool saves_caller_doubles() const { return GetFlag(kSavesCallerDoubles); }

  void MarkAsRequiresFrame() { SetFlag(kRequiresFrame); }

  bool requires_frame() const { return GetFlag(kRequiresFrame); }

  void MarkMustNotHaveEagerFrame() { SetFlag(kMustNotHaveEagerFrame); }

  bool GetMustNotHaveEagerFrame() const {
    return GetFlag(kMustNotHaveEagerFrame);
  }

  // Compiles marked as debug produce unoptimized code with debug break slots.
  // Inner functions that cannot be compiled w/o context are compiled eagerly.
  // Always include deoptimization support to avoid having to recompile again.
  void MarkAsDebug() {
    SetFlag(kDebug);
    SetFlag(kDeoptimizationSupport);
  }

  bool is_debug() const { return GetFlag(kDebug); }

  void PrepareForSerializing() { SetFlag(kSerializing); }

  bool will_serialize() const { return GetFlag(kSerializing); }

  void MarkAsFunctionContextSpecializing() {
    SetFlag(kFunctionContextSpecializing);
  }

  bool is_function_context_specializing() const {
    return GetFlag(kFunctionContextSpecializing);
  }

  void MarkAsFrameSpecializing() { SetFlag(kFrameSpecializing); }

  bool is_frame_specializing() const { return GetFlag(kFrameSpecializing); }

  void MarkAsNativeContextSpecializing() {
    SetFlag(kNativeContextSpecializing);
  }

  bool is_native_context_specializing() const {
    return GetFlag(kNativeContextSpecializing);
  }

  void MarkAsDeoptimizationEnabled() { SetFlag(kDeoptimizationEnabled); }

  bool is_deoptimization_enabled() const {
    return GetFlag(kDeoptimizationEnabled);
  }

  void MarkAsSourcePositionsEnabled() { SetFlag(kSourcePositionsEnabled); }

  bool is_source_positions_enabled() const {
    return GetFlag(kSourcePositionsEnabled);
  }

  void MarkAsInliningEnabled() { SetFlag(kInliningEnabled); }

  bool is_inlining_enabled() const { return GetFlag(kInliningEnabled); }

  void MarkAsTypingEnabled() { SetFlag(kTypingEnabled); }

  bool is_typing_enabled() const { return GetFlag(kTypingEnabled); }

  void MarkAsSplittingEnabled() { SetFlag(kSplittingEnabled); }

  bool is_splitting_enabled() const { return GetFlag(kSplittingEnabled); }

  void MarkAsFirstCompile() { SetFlag(kFirstCompile); }

  void MarkAsCompiled() { SetFlag(kFirstCompile, false); }

  bool is_first_compile() const { return GetFlag(kFirstCompile); }

  void MarkAsBailoutOnUninitialized() { SetFlag(kBailoutOnUninitialized); }

  bool is_bailout_on_uninitialized() const {
    return GetFlag(kBailoutOnUninitialized);
  }

  bool GeneratePreagedPrologue() const {
    // Generate a pre-aged prologue if we are optimizing for size, which
    // will make code flushing more aggressive. Only apply to Code::FUNCTION,
    // since StaticMarkingVisitor::IsFlushable only flushes proper functions.
    return FLAG_optimize_for_size && FLAG_age_code && !is_debug() &&
           output_code_kind() == Code::FUNCTION;
  }

  void SetCode(Handle<Code> code) { code_ = code; }

  void SetBytecodeArray(Handle<BytecodeArray> bytecode_array) {
    bytecode_array_ = bytecode_array;
  }

  bool ShouldTrapOnDeopt() const {
    return (FLAG_trap_on_deopt && IsOptimizing()) ||
        (FLAG_trap_on_stub_deopt && IsStub());
  }

  bool has_native_context() const {
    return !closure().is_null() && (closure()->native_context() != nullptr);
  }

  Context* native_context() const {
    return has_native_context() ? closure()->native_context() : nullptr;
  }

  bool has_global_object() const { return has_native_context(); }

  JSGlobalObject* global_object() const {
    return has_global_object() ? native_context()->global_object() : nullptr;
  }

  // Accessors for the different compilation modes.
  bool IsOptimizing() const { return mode_ == OPTIMIZE; }
  bool IsStub() const { return mode_ == STUB; }
  void SetOptimizing() {
    DCHECK(has_shared_info());
    SetMode(OPTIMIZE);
    optimization_id_ = isolate()->NextOptimizationId();
    code_flags_ =
        Code::KindField::update(code_flags_, Code::OPTIMIZED_FUNCTION);
  }
  void SetOptimizingForOsr(BailoutId osr_ast_id) {
    SetOptimizing();
    osr_ast_id_ = osr_ast_id;
  }

  // Deoptimization support.
  bool HasDeoptimizationSupport() const {
    return GetFlag(kDeoptimizationSupport);
  }
  void EnableDeoptimizationSupport() {
    DCHECK_EQ(BASE, mode_);
    SetFlag(kDeoptimizationSupport);
  }
  bool ShouldEnsureSpaceForLazyDeopt() { return !IsStub(); }

  bool ExpectsJSReceiverAsReceiver();

  // Determines whether or not to insert a self-optimization header.
  bool ShouldSelfOptimize();

  void set_deferred_handles(DeferredHandles* deferred_handles) {
    DCHECK(deferred_handles_ == NULL);
    deferred_handles_ = deferred_handles;
  }

  void ReopenHandlesInNewHandleScope() {
    // Empty for now but will be needed once fields move from ParseInfo.
  }

  void AbortOptimization(BailoutReason reason) {
    DCHECK(reason != kNoReason);
    if (bailout_reason_ == kNoReason) bailout_reason_ = reason;
    SetFlag(kDisableFutureOptimization);
  }

  void RetryOptimization(BailoutReason reason) {
    DCHECK(reason != kNoReason);
    if (GetFlag(kDisableFutureOptimization)) return;
    bailout_reason_ = reason;
  }

  BailoutReason bailout_reason() const { return bailout_reason_; }

  int prologue_offset() const {
    DCHECK_NE(Code::kPrologueOffsetNotSet, prologue_offset_);
    return prologue_offset_;
  }

  void set_prologue_offset(int prologue_offset) {
    DCHECK_EQ(Code::kPrologueOffsetNotSet, prologue_offset_);
    prologue_offset_ = prologue_offset;
  }

  int start_position_for(uint32_t inlining_id) {
    return inlined_function_infos_.at(inlining_id).start_position;
  }
  const std::vector<InlinedFunctionInfo>& inlined_function_infos() {
    return inlined_function_infos_;
  }

  void LogDeoptCallPosition(int pc_offset, int inlining_id);
  int TraceInlinedFunction(Handle<SharedFunctionInfo> shared,
                           SourcePosition position, int pareint_id);

  CompilationDependencies* dependencies() { return &dependencies_; }

  bool HasSameOsrEntry(Handle<JSFunction> function, BailoutId osr_ast_id) {
    return osr_ast_id_ == osr_ast_id && function.is_identical_to(closure());
  }

  int optimization_id() const { return optimization_id_; }

  int osr_expr_stack_height() { return osr_expr_stack_height_; }
  void set_osr_expr_stack_height(int height) {
    DCHECK(height >= 0);
    osr_expr_stack_height_ = height;
  }
  JavaScriptFrame* osr_frame() const { return osr_frame_; }
  void set_osr_frame(JavaScriptFrame* osr_frame) { osr_frame_ = osr_frame; }

#if DEBUG
  void PrintAstForTesting();
#endif

  bool has_simple_parameters();

  struct InlinedFunctionHolder {
    Handle<SharedFunctionInfo> shared_info;

    // Root that holds the unoptimized code of the inlined function alive
    // (and out of reach of code flushing) until we finish compilation.
    // Do not remove.
    Handle<Code> inlined_code_object_root;

    explicit InlinedFunctionHolder(
        Handle<SharedFunctionInfo> inlined_shared_info)
        : shared_info(inlined_shared_info),
          inlined_code_object_root(inlined_shared_info->code()) {}
  };

  typedef std::vector<InlinedFunctionHolder> InlinedFunctionList;
  InlinedFunctionList const& inlined_functions() const {
    return inlined_functions_;
  }

  void AddInlinedFunction(Handle<SharedFunctionInfo> inlined_function) {
    inlined_functions_.push_back(InlinedFunctionHolder(inlined_function));
  }

  base::SmartArrayPointer<char> GetDebugName() const;

  Code::Kind output_code_kind() const {
    return Code::ExtractKindFromFlags(code_flags_);
  }

  StackFrame::Type GetOutputStackFrameType() const;

 protected:
  ParseInfo* parse_info_;

  void DisableFutureOptimization() {
    if (GetFlag(kDisableFutureOptimization) && has_shared_info()) {
      // If Crankshaft tried to optimize this function, bailed out, and
      // doesn't want to try again, then use TurboFan next time.
      if (!shared_info()->dont_crankshaft() &&
          bailout_reason() != kOptimizedTooManyTimes) {
        shared_info()->set_dont_crankshaft(true);
        if (FLAG_trace_opt) {
          PrintF("[disabled Crankshaft for ");
          shared_info()->ShortPrint();
          PrintF(", reason: %s]\n", GetBailoutReason(bailout_reason()));
        }
      } else {
        shared_info()->DisableOptimization(bailout_reason());
      }
    }
  }

 private:
  // Compilation mode.
  // BASE is generated by the full codegen, optionally prepared for bailouts.
  // OPTIMIZE is optimized code generated by the Hydrogen-based backend.
  enum Mode {
    BASE,
    OPTIMIZE,
    STUB
  };

  CompilationInfo(ParseInfo* parse_info, const char* debug_name,
                  Code::Flags code_flags, Mode mode, Isolate* isolate,
                  Zone* zone);

  Isolate* isolate_;

  void SetMode(Mode mode) {
    mode_ = mode;
  }

  void SetFlag(Flag flag) { flags_ |= flag; }

  void SetFlag(Flag flag, bool value) {
    flags_ = value ? flags_ | flag : flags_ & ~flag;
  }

  bool GetFlag(Flag flag) const { return (flags_ & flag) != 0; }

  unsigned flags_;

  Code::Flags code_flags_;

  // The compiled code.
  Handle<Code> code_;

  // Compilation mode flag and whether deoptimization is allowed.
  Mode mode_;
  BailoutId osr_ast_id_;

  // Holds the bytecode array generated by the interpreter.
  // TODO(rmcilroy/mstarzinger): Temporary work-around until compiler.cc is
  // refactored to avoid us needing to carry the BytcodeArray around.
  Handle<BytecodeArray> bytecode_array_;

  // The zone from which the compilation pipeline working on this
  // CompilationInfo allocates.
  Zone* zone_;

  DeferredHandles* deferred_handles_;

  // Dependencies for this compilation, e.g. stable maps.
  CompilationDependencies dependencies_;

  BailoutReason bailout_reason_;

  int prologue_offset_;

  std::vector<InlinedFunctionInfo> inlined_function_infos_;
  bool track_positions_;

  InlinedFunctionList inlined_functions_;

  // A copy of shared_info()->opt_count() to avoid handle deref
  // during graph optimization.
  int opt_count_;

  // Number of parameters used for compilation of stubs that require arguments.
  int parameter_count_;

  int optimization_id_;

  int osr_expr_stack_height_;

  // The current OSR frame for specialization or {nullptr}.
  JavaScriptFrame* osr_frame_ = nullptr;

  const char* debug_name_;

  DISALLOW_COPY_AND_ASSIGN(CompilationInfo);
};


class HGraph;
class LChunk;

// A helper class that calls the three compilation phases in
// Crankshaft and keeps track of its state.  The three phases
// CreateGraph, OptimizeGraph and GenerateAndInstallCode can either
// fail, bail-out to the full code generator or succeed.  Apart from
// their return value, the status of the phase last run can be checked
// using last_status().
class OptimizedCompileJob: public ZoneObject {
 public:
  explicit OptimizedCompileJob(CompilationInfo* info)
      : info_(info), graph_(NULL), chunk_(NULL), last_status_(FAILED) {}

  enum Status {
    FAILED, BAILED_OUT, SUCCEEDED
  };

  MUST_USE_RESULT Status CreateGraph();
  MUST_USE_RESULT Status OptimizeGraph();
  MUST_USE_RESULT Status GenerateCode();

  Status last_status() const { return last_status_; }
  CompilationInfo* info() const { return info_; }
  Isolate* isolate() const { return info()->isolate(); }

  Status RetryOptimization(BailoutReason reason) {
    info_->RetryOptimization(reason);
    return SetLastStatus(BAILED_OUT);
  }

  Status AbortOptimization(BailoutReason reason) {
    info_->AbortOptimization(reason);
    return SetLastStatus(BAILED_OUT);
  }

 private:
  CompilationInfo* info_;
  HGraph* graph_;
  LChunk* chunk_;
  base::TimeDelta time_taken_to_create_graph_;
  base::TimeDelta time_taken_to_optimize_;
  base::TimeDelta time_taken_to_codegen_;
  Status last_status_;

  MUST_USE_RESULT Status SetLastStatus(Status status) {
    last_status_ = status;
    return last_status_;
  }
  void RecordOptimizationStats();

  struct Timer {
    Timer(OptimizedCompileJob* job, base::TimeDelta* location)
        : job_(job), location_(location) {
      DCHECK(location_ != NULL);
      timer_.Start();
    }

    ~Timer() {
      *location_ += timer_.Elapsed();
    }

    OptimizedCompileJob* job_;
    base::ElapsedTimer timer_;
    base::TimeDelta* location_;
  };
};

}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_H_
