// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/pipeline.h"

#include <fstream>  // NOLINT(readability/streams)
#include <sstream>

#include "src/base/adapters.h"
#include "src/base/platform/elapsed-timer.h"
#include "src/compiler/ast-graph-builder.h"
#include "src/compiler/ast-loop-assignment-analyzer.h"
#include "src/compiler/basic-block-instrumentor.h"
#include "src/compiler/change-lowering.h"
#include "src/compiler/code-generator.h"
#include "src/compiler/common-operator-reducer.h"
#include "src/compiler/control-flow-optimizer.h"
#include "src/compiler/dead-code-elimination.h"
#include "src/compiler/frame-elider.h"
#include "src/compiler/graph-replay.h"
#include "src/compiler/graph-trimmer.h"
#include "src/compiler/graph-visualizer.h"
#include "src/compiler/greedy-allocator.h"
#include "src/compiler/instruction.h"
#include "src/compiler/instruction-selector.h"
#include "src/compiler/js-builtin-reducer.h"
#include "src/compiler/js-context-specialization.h"
#include "src/compiler/js-frame-specialization.h"
#include "src/compiler/js-generic-lowering.h"
#include "src/compiler/js-inlining.h"
#include "src/compiler/js-intrinsic-lowering.h"
#include "src/compiler/js-type-feedback.h"
#include "src/compiler/js-typed-lowering.h"
#include "src/compiler/jump-threading.h"
#include "src/compiler/load-elimination.h"
#include "src/compiler/loop-analysis.h"
#include "src/compiler/loop-peeling.h"
#include "src/compiler/machine-operator-reducer.h"
#include "src/compiler/move-optimizer.h"
#include "src/compiler/osr.h"
#include "src/compiler/pipeline-statistics.h"
#include "src/compiler/register-allocator.h"
#include "src/compiler/register-allocator-verifier.h"
#include "src/compiler/schedule.h"
#include "src/compiler/scheduler.h"
#include "src/compiler/select-lowering.h"
#include "src/compiler/simplified-lowering.h"
#include "src/compiler/simplified-operator-reducer.h"
#include "src/compiler/tail-call-optimization.h"
#include "src/compiler/typer.h"
#include "src/compiler/value-numbering-reducer.h"
#include "src/compiler/verifier.h"
#include "src/compiler/zone-pool.h"
#include "src/ostreams.h"
#include "src/type-info.h"
#include "src/utils.h"

namespace v8 {
namespace internal {
namespace compiler {

class PipelineData {
 public:
  // For main entry point.
  PipelineData(ZonePool* zone_pool, CompilationInfo* info,
               PipelineStatistics* pipeline_statistics)
      : isolate_(info->isolate()),
        info_(info),
        outer_zone_(info_->zone()),
        zone_pool_(zone_pool),
        pipeline_statistics_(pipeline_statistics),
        compilation_failed_(false),
        code_(Handle<Code>::null()),
        graph_zone_scope_(zone_pool_),
        graph_zone_(graph_zone_scope_.zone()),
        graph_(nullptr),
        loop_assignment_(nullptr),
        machine_(nullptr),
        common_(nullptr),
        javascript_(nullptr),
        jsgraph_(nullptr),
        js_type_feedback_(nullptr),
        schedule_(nullptr),
        instruction_zone_scope_(zone_pool_),
        instruction_zone_(instruction_zone_scope_.zone()),
        sequence_(nullptr),
        frame_(nullptr),
        register_allocation_zone_scope_(zone_pool_),
        register_allocation_zone_(register_allocation_zone_scope_.zone()),
        register_allocation_data_(nullptr) {
    PhaseScope scope(pipeline_statistics, "init pipeline data");
    graph_ = new (graph_zone_) Graph(graph_zone_);
    source_positions_.Reset(new SourcePositionTable(graph_));
    machine_ = new (graph_zone_) MachineOperatorBuilder(
        graph_zone_, kMachPtr,
        InstructionSelector::SupportedMachineOperatorFlags());
    common_ = new (graph_zone_) CommonOperatorBuilder(graph_zone_);
    javascript_ = new (graph_zone_) JSOperatorBuilder(graph_zone_);
    jsgraph_ = new (graph_zone_)
        JSGraph(isolate_, graph_, common_, javascript_, machine_);
  }

  // For machine graph testing entry point.
  PipelineData(ZonePool* zone_pool, CompilationInfo* info, Graph* graph,
               Schedule* schedule)
      : isolate_(info->isolate()),
        info_(info),
        outer_zone_(nullptr),
        zone_pool_(zone_pool),
        pipeline_statistics_(nullptr),
        compilation_failed_(false),
        code_(Handle<Code>::null()),
        graph_zone_scope_(zone_pool_),
        graph_zone_(nullptr),
        graph_(graph),
        source_positions_(new SourcePositionTable(graph_)),
        loop_assignment_(nullptr),
        machine_(nullptr),
        common_(nullptr),
        javascript_(nullptr),
        jsgraph_(nullptr),
        js_type_feedback_(nullptr),
        schedule_(schedule),
        instruction_zone_scope_(zone_pool_),
        instruction_zone_(instruction_zone_scope_.zone()),
        sequence_(nullptr),
        frame_(nullptr),
        register_allocation_zone_scope_(zone_pool_),
        register_allocation_zone_(register_allocation_zone_scope_.zone()),
        register_allocation_data_(nullptr) {}

  // For register allocation testing entry point.
  PipelineData(ZonePool* zone_pool, CompilationInfo* info,
               InstructionSequence* sequence)
      : isolate_(info->isolate()),
        info_(info),
        outer_zone_(nullptr),
        zone_pool_(zone_pool),
        pipeline_statistics_(nullptr),
        compilation_failed_(false),
        code_(Handle<Code>::null()),
        graph_zone_scope_(zone_pool_),
        graph_zone_(nullptr),
        graph_(nullptr),
        loop_assignment_(nullptr),
        machine_(nullptr),
        common_(nullptr),
        javascript_(nullptr),
        jsgraph_(nullptr),
        js_type_feedback_(nullptr),
        schedule_(nullptr),
        instruction_zone_scope_(zone_pool_),
        instruction_zone_(sequence->zone()),
        sequence_(sequence),
        frame_(nullptr),
        register_allocation_zone_scope_(zone_pool_),
        register_allocation_zone_(register_allocation_zone_scope_.zone()),
        register_allocation_data_(nullptr) {}

  ~PipelineData() {
    DeleteRegisterAllocationZone();
    DeleteInstructionZone();
    DeleteGraphZone();
  }

  Isolate* isolate() const { return isolate_; }
  CompilationInfo* info() const { return info_; }
  ZonePool* zone_pool() const { return zone_pool_; }
  PipelineStatistics* pipeline_statistics() { return pipeline_statistics_; }
  bool compilation_failed() const { return compilation_failed_; }
  void set_compilation_failed() { compilation_failed_ = true; }
  Handle<Code> code() { return code_; }
  void set_code(Handle<Code> code) {
    DCHECK(code_.is_null());
    code_ = code;
  }

  // RawMachineAssembler generally produces graphs which cannot be verified.
  bool MayHaveUnverifiableGraph() const { return outer_zone_ == nullptr; }

  Zone* graph_zone() const { return graph_zone_; }
  Graph* graph() const { return graph_; }
  SourcePositionTable* source_positions() const {
    return source_positions_.get();
  }
  MachineOperatorBuilder* machine() const { return machine_; }
  CommonOperatorBuilder* common() const { return common_; }
  JSOperatorBuilder* javascript() const { return javascript_; }
  JSGraph* jsgraph() const { return jsgraph_; }
  JSTypeFeedbackTable* js_type_feedback() { return js_type_feedback_; }
  void set_js_type_feedback(JSTypeFeedbackTable* js_type_feedback) {
    js_type_feedback_ = js_type_feedback;
  }

  LoopAssignmentAnalysis* loop_assignment() const { return loop_assignment_; }
  void set_loop_assignment(LoopAssignmentAnalysis* loop_assignment) {
    DCHECK(!loop_assignment_);
    loop_assignment_ = loop_assignment;
  }

  Schedule* schedule() const { return schedule_; }
  void set_schedule(Schedule* schedule) {
    DCHECK(!schedule_);
    schedule_ = schedule;
  }

  Zone* instruction_zone() const { return instruction_zone_; }
  InstructionSequence* sequence() const { return sequence_; }
  Frame* frame() const { return frame_; }

  Zone* register_allocation_zone() const { return register_allocation_zone_; }
  RegisterAllocationData* register_allocation_data() const {
    return register_allocation_data_;
  }

  void DeleteGraphZone() {
    // Destroy objects with destructors first.
    source_positions_.Reset(nullptr);
    if (graph_zone_ == nullptr) return;
    // Destroy zone and clear pointers.
    graph_zone_scope_.Destroy();
    graph_zone_ = nullptr;
    graph_ = nullptr;
    loop_assignment_ = nullptr;
    machine_ = nullptr;
    common_ = nullptr;
    javascript_ = nullptr;
    jsgraph_ = nullptr;
    js_type_feedback_ = nullptr;
    schedule_ = nullptr;
  }

  void DeleteInstructionZone() {
    if (instruction_zone_ == nullptr) return;
    instruction_zone_scope_.Destroy();
    instruction_zone_ = nullptr;
    sequence_ = nullptr;
    frame_ = nullptr;
  }

  void DeleteRegisterAllocationZone() {
    if (register_allocation_zone_ == nullptr) return;
    register_allocation_zone_scope_.Destroy();
    register_allocation_zone_ = nullptr;
    register_allocation_data_ = nullptr;
  }

  void InitializeInstructionSequence() {
    DCHECK(sequence_ == nullptr);
    InstructionBlocks* instruction_blocks =
        InstructionSequence::InstructionBlocksFor(instruction_zone(),
                                                  schedule());
    sequence_ = new (instruction_zone()) InstructionSequence(
        info()->isolate(), instruction_zone(), instruction_blocks);
  }

  void InitializeRegisterAllocationData(const RegisterConfiguration* config,
                                        const char* debug_name) {
    DCHECK(frame_ == nullptr);
    DCHECK(register_allocation_data_ == nullptr);
    frame_ = new (instruction_zone()) Frame();
    register_allocation_data_ = new (register_allocation_zone())
        RegisterAllocationData(config, register_allocation_zone(), frame(),
                               sequence(), debug_name);
  }

 private:
  Isolate* isolate_;
  CompilationInfo* info_;
  Zone* outer_zone_;
  ZonePool* const zone_pool_;
  PipelineStatistics* pipeline_statistics_;
  bool compilation_failed_;
  Handle<Code> code_;

  // All objects in the following group of fields are allocated in graph_zone_.
  // They are all set to NULL when the graph_zone_ is destroyed.
  ZonePool::Scope graph_zone_scope_;
  Zone* graph_zone_;
  Graph* graph_;
  // TODO(dcarney): make this into a ZoneObject.
  SmartPointer<SourcePositionTable> source_positions_;
  LoopAssignmentAnalysis* loop_assignment_;
  MachineOperatorBuilder* machine_;
  CommonOperatorBuilder* common_;
  JSOperatorBuilder* javascript_;
  JSGraph* jsgraph_;
  JSTypeFeedbackTable* js_type_feedback_;
  Schedule* schedule_;

  // All objects in the following group of fields are allocated in
  // instruction_zone_.  They are all set to NULL when the instruction_zone_ is
  // destroyed.
  ZonePool::Scope instruction_zone_scope_;
  Zone* instruction_zone_;
  InstructionSequence* sequence_;
  Frame* frame_;

  // All objects in the following group of fields are allocated in
  // register_allocation_zone_.  They are all set to NULL when the zone is
  // destroyed.
  ZonePool::Scope register_allocation_zone_scope_;
  Zone* register_allocation_zone_;
  RegisterAllocationData* register_allocation_data_;

  DISALLOW_COPY_AND_ASSIGN(PipelineData);
};


namespace {

struct TurboCfgFile : public std::ofstream {
  explicit TurboCfgFile(Isolate* isolate)
      : std::ofstream(isolate->GetTurboCfgFileName().c_str(),
                      std::ios_base::app) {}
};


void TraceSchedule(CompilationInfo* info, Schedule* schedule) {
  if (FLAG_trace_turbo) {
    FILE* json_file = OpenVisualizerLogFile(info, NULL, "json", "a+");
    if (json_file != nullptr) {
      OFStream json_of(json_file);
      json_of << "{\"name\":\"Schedule\",\"type\":\"schedule\",\"data\":\"";
      std::stringstream schedule_stream;
      schedule_stream << *schedule;
      std::string schedule_string(schedule_stream.str());
      for (const auto& c : schedule_string) {
        json_of << AsEscapedUC16ForJSON(c);
      }
      json_of << "\"},\n";
      fclose(json_file);
    }
  }
  if (!FLAG_trace_turbo_graph && !FLAG_trace_turbo_scheduler) return;
  OFStream os(stdout);
  os << "-- Schedule --------------------------------------\n" << *schedule;
}


SmartArrayPointer<char> GetDebugName(CompilationInfo* info) {
  if (info->code_stub() != NULL) {
    CodeStub::Major major_key = info->code_stub()->MajorKey();
    const char* major_name = CodeStub::MajorName(major_key, false);
    size_t len = strlen(major_name) + 1;
    SmartArrayPointer<char> name(new char[len]);
    memcpy(name.get(), major_name, len);
    return name;
  } else {
    AllowHandleDereference allow_deref;
    return info->function()->debug_name()->ToCString();
  }
}


class AstGraphBuilderWithPositions final : public AstGraphBuilder {
 public:
  AstGraphBuilderWithPositions(Zone* local_zone, CompilationInfo* info,
                               JSGraph* jsgraph,
                               LoopAssignmentAnalysis* loop_assignment,
                               JSTypeFeedbackTable* js_type_feedback,
                               SourcePositionTable* source_positions)
      : AstGraphBuilder(local_zone, info, jsgraph, loop_assignment,
                        js_type_feedback),
        source_positions_(source_positions),
        start_position_(info->shared_info()->start_position()) {}

  bool CreateGraph(bool stack_check) {
    SourcePositionTable::Scope pos_scope(source_positions_, start_position_);
    return AstGraphBuilder::CreateGraph(stack_check);
  }

#define DEF_VISIT(type)                                               \
  void Visit##type(type* node) override {                             \
    SourcePositionTable::Scope pos(source_positions_,                 \
                                   SourcePosition(node->position())); \
    AstGraphBuilder::Visit##type(node);                               \
  }
  AST_NODE_LIST(DEF_VISIT)
#undef DEF_VISIT

 private:
  SourcePositionTable* const source_positions_;
  SourcePosition const start_position_;
};


class SourcePositionWrapper final : public Reducer {
 public:
  SourcePositionWrapper(Reducer* reducer, SourcePositionTable* table)
      : reducer_(reducer), table_(table) {}
  ~SourcePositionWrapper() final {}

  Reduction Reduce(Node* node) final {
    SourcePosition const pos = table_->GetSourcePosition(node);
    SourcePositionTable::Scope position(table_, pos);
    return reducer_->Reduce(node);
  }

 private:
  Reducer* const reducer_;
  SourcePositionTable* const table_;

  DISALLOW_COPY_AND_ASSIGN(SourcePositionWrapper);
};


class JSGraphReducer final : public GraphReducer {
 public:
  JSGraphReducer(JSGraph* jsgraph, Zone* zone)
      : GraphReducer(zone, jsgraph->graph(), jsgraph->Dead()) {}
  ~JSGraphReducer() final {}
};


void AddReducer(PipelineData* data, GraphReducer* graph_reducer,
                Reducer* reducer) {
  if (data->info()->is_source_positions_enabled()) {
    void* const buffer = data->graph_zone()->New(sizeof(SourcePositionWrapper));
    SourcePositionWrapper* const wrapper =
        new (buffer) SourcePositionWrapper(reducer, data->source_positions());
    graph_reducer->AddReducer(wrapper);
  } else {
    graph_reducer->AddReducer(reducer);
  }
}


class PipelineRunScope {
 public:
  PipelineRunScope(PipelineData* data, const char* phase_name)
      : phase_scope_(
            phase_name == nullptr ? nullptr : data->pipeline_statistics(),
            phase_name),
        zone_scope_(data->zone_pool()) {}

  Zone* zone() { return zone_scope_.zone(); }

 private:
  PhaseScope phase_scope_;
  ZonePool::Scope zone_scope_;
};

}  // namespace


template <typename Phase>
void Pipeline::Run() {
  PipelineRunScope scope(this->data_, Phase::phase_name());
  Phase phase;
  phase.Run(this->data_, scope.zone());
}


template <typename Phase, typename Arg0>
void Pipeline::Run(Arg0 arg_0) {
  PipelineRunScope scope(this->data_, Phase::phase_name());
  Phase phase;
  phase.Run(this->data_, scope.zone(), arg_0);
}


struct LoopAssignmentAnalysisPhase {
  static const char* phase_name() { return "loop assignment analysis"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    AstLoopAssignmentAnalyzer analyzer(data->graph_zone(), data->info());
    LoopAssignmentAnalysis* loop_assignment = analyzer.Analyze();
    data->set_loop_assignment(loop_assignment);
  }
};


struct GraphBuilderPhase {
  static const char* phase_name() { return "graph builder"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    AstGraphBuilderWithPositions graph_builder(
        temp_zone, data->info(), data->jsgraph(), data->loop_assignment(),
        data->js_type_feedback(), data->source_positions());
    bool stack_check = !data->info()->IsStub();
    if (!graph_builder.CreateGraph(stack_check)) {
      data->set_compilation_failed();
    }
  }
};


struct InliningPhase {
  static const char* phase_name() { return "inlining"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    DeadCodeElimination dead_code_elimination(&graph_reducer, data->graph(),
                                              data->common());
    CommonOperatorReducer common_reducer(&graph_reducer, data->graph(),
                                         data->common(), data->machine());
    JSContextSpecialization context_specialization(
        &graph_reducer, data->jsgraph(), data->info()->context());
    JSFrameSpecialization frame_specialization(data->info()->osr_frame(),
                                               data->jsgraph());
    JSInliner inliner(&graph_reducer, data->info()->is_inlining_enabled()
                                          ? JSInliner::kGeneralInlining
                                          : JSInliner::kRestrictedInlining,
                      temp_zone, data->info(), data->jsgraph());
    AddReducer(data, &graph_reducer, &dead_code_elimination);
    AddReducer(data, &graph_reducer, &common_reducer);
    if (data->info()->is_frame_specializing()) {
      AddReducer(data, &graph_reducer, &frame_specialization);
    }
    if (data->info()->is_context_specializing()) {
      AddReducer(data, &graph_reducer, &context_specialization);
    }
    AddReducer(data, &graph_reducer, &inliner);
    graph_reducer.ReduceGraph();
  }
};


struct TyperPhase {
  static const char* phase_name() { return "typer"; }

  void Run(PipelineData* data, Zone* temp_zone, Typer* typer) {
    NodeVector roots(temp_zone);
    data->jsgraph()->GetCachedNodes(&roots);
    typer->Run(roots);
  }
};


struct OsrDeconstructionPhase {
  static const char* phase_name() { return "OSR deconstruction"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    OsrHelper osr_helper(data->info());
    osr_helper.Deconstruct(data->jsgraph(), data->common(), temp_zone);
  }
};


struct JSTypeFeedbackPhase {
  static const char* phase_name() { return "type feedback specializing"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    Handle<Context> native_context(data->info()->context()->native_context());
    TypeFeedbackOracle oracle(data->isolate(), temp_zone,
                              data->info()->unoptimized_code(),
                              data->info()->feedback_vector(), native_context);
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    Handle<GlobalObject> global_object = Handle<GlobalObject>::null();
    if (data->info()->has_global_object()) {
      global_object =
          Handle<GlobalObject>(data->info()->global_object(), data->isolate());
    }
    // TODO(titzer): introduce a specialization mode/flags enum to control
    // specializing to the global object here.
    JSTypeFeedbackSpecializer specializer(
        &graph_reducer, data->jsgraph(), data->js_type_feedback(), &oracle,
        global_object, data->info()->is_deoptimization_enabled()
                           ? JSTypeFeedbackSpecializer::kDeoptimizationEnabled
                           : JSTypeFeedbackSpecializer::kDeoptimizationDisabled,
        data->info()->dependencies());
    AddReducer(data, &graph_reducer, &specializer);
    graph_reducer.ReduceGraph();
  }
};


struct TypedLoweringPhase {
  static const char* phase_name() { return "typed lowering"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    DeadCodeElimination dead_code_elimination(&graph_reducer, data->graph(),
                                              data->common());
    LoadElimination load_elimination(&graph_reducer);
    JSBuiltinReducer builtin_reducer(&graph_reducer, data->jsgraph());
    JSTypedLowering typed_lowering(&graph_reducer, data->jsgraph(), temp_zone);
    JSIntrinsicLowering intrinsic_lowering(
        &graph_reducer, data->jsgraph(),
        data->info()->is_deoptimization_enabled()
            ? JSIntrinsicLowering::kDeoptimizationEnabled
            : JSIntrinsicLowering::kDeoptimizationDisabled);
    CommonOperatorReducer common_reducer(&graph_reducer, data->graph(),
                                         data->common(), data->machine());
    AddReducer(data, &graph_reducer, &dead_code_elimination);
    AddReducer(data, &graph_reducer, &builtin_reducer);
    AddReducer(data, &graph_reducer, &typed_lowering);
    AddReducer(data, &graph_reducer, &intrinsic_lowering);
    AddReducer(data, &graph_reducer, &load_elimination);
    AddReducer(data, &graph_reducer, &common_reducer);
    graph_reducer.ReduceGraph();
  }
};


struct SimplifiedLoweringPhase {
  static const char* phase_name() { return "simplified lowering"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    SimplifiedLowering lowering(data->jsgraph(), temp_zone,
                                data->source_positions());
    lowering.LowerAllNodes();
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    DeadCodeElimination dead_code_elimination(&graph_reducer, data->graph(),
                                              data->common());
    SimplifiedOperatorReducer simple_reducer(data->jsgraph());
    ValueNumberingReducer value_numbering(temp_zone);
    MachineOperatorReducer machine_reducer(data->jsgraph());
    CommonOperatorReducer common_reducer(&graph_reducer, data->graph(),
                                         data->common(), data->machine());
    AddReducer(data, &graph_reducer, &dead_code_elimination);
    AddReducer(data, &graph_reducer, &simple_reducer);
    AddReducer(data, &graph_reducer, &value_numbering);
    AddReducer(data, &graph_reducer, &machine_reducer);
    AddReducer(data, &graph_reducer, &common_reducer);
    graph_reducer.ReduceGraph();
  }
};


struct ControlFlowOptimizationPhase {
  static const char* phase_name() { return "control flow optimization"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    ControlFlowOptimizer optimizer(data->graph(), data->common(),
                                   data->machine(), temp_zone);
    optimizer.Optimize();
  }
};


struct ChangeLoweringPhase {
  static const char* phase_name() { return "change lowering"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    DeadCodeElimination dead_code_elimination(&graph_reducer, data->graph(),
                                              data->common());
    SimplifiedOperatorReducer simple_reducer(data->jsgraph());
    ValueNumberingReducer value_numbering(temp_zone);
    ChangeLowering lowering(data->jsgraph());
    MachineOperatorReducer machine_reducer(data->jsgraph());
    CommonOperatorReducer common_reducer(&graph_reducer, data->graph(),
                                         data->common(), data->machine());
    AddReducer(data, &graph_reducer, &dead_code_elimination);
    AddReducer(data, &graph_reducer, &simple_reducer);
    AddReducer(data, &graph_reducer, &value_numbering);
    AddReducer(data, &graph_reducer, &lowering);
    AddReducer(data, &graph_reducer, &machine_reducer);
    AddReducer(data, &graph_reducer, &common_reducer);
    graph_reducer.ReduceGraph();
  }
};


struct EarlyGraphTrimmingPhase {
  static const char* phase_name() { return "early graph trimming"; }
  void Run(PipelineData* data, Zone* temp_zone) {
    GraphTrimmer trimmer(temp_zone, data->graph());
    NodeVector roots(temp_zone);
    data->jsgraph()->GetCachedNodes(&roots);
    trimmer.TrimGraph(roots.begin(), roots.end());
  }
};


struct LateGraphTrimmingPhase {
  static const char* phase_name() { return "late graph trimming"; }
  void Run(PipelineData* data, Zone* temp_zone) {
    GraphTrimmer trimmer(temp_zone, data->graph());
    NodeVector roots(temp_zone);
    data->jsgraph()->GetCachedNodes(&roots);
    trimmer.TrimGraph(roots.begin(), roots.end());
  }
};


struct StressLoopPeelingPhase {
  static const char* phase_name() { return "stress loop peeling"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    // Peel the first outer loop for testing.
    // TODO(titzer): peel all loops? the N'th loop? Innermost loops?
    LoopTree* loop_tree = LoopFinder::BuildLoopTree(data->graph(), temp_zone);
    if (loop_tree != NULL && loop_tree->outer_loops().size() > 0) {
      LoopPeeler::Peel(data->graph(), data->common(), loop_tree,
                       loop_tree->outer_loops()[0], temp_zone);
    }
  }
};


struct GenericLoweringPhase {
  static const char* phase_name() { return "generic lowering"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    JSGraphReducer graph_reducer(data->jsgraph(), temp_zone);
    DeadCodeElimination dead_code_elimination(&graph_reducer, data->graph(),
                                              data->common());
    CommonOperatorReducer common_reducer(&graph_reducer, data->graph(),
                                         data->common(), data->machine());
    JSGenericLowering generic_lowering(data->info()->is_typing_enabled(),
                                       data->jsgraph());
    SelectLowering select_lowering(data->jsgraph()->graph(),
                                   data->jsgraph()->common());
    TailCallOptimization tco(data->common(), data->graph());
    AddReducer(data, &graph_reducer, &dead_code_elimination);
    AddReducer(data, &graph_reducer, &common_reducer);
    AddReducer(data, &graph_reducer, &generic_lowering);
    AddReducer(data, &graph_reducer, &select_lowering);
    AddReducer(data, &graph_reducer, &tco);
    graph_reducer.ReduceGraph();
  }
};


struct ComputeSchedulePhase {
  static const char* phase_name() { return "scheduling"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    Schedule* schedule = Scheduler::ComputeSchedule(
        temp_zone, data->graph(), data->info()->is_splitting_enabled()
                                      ? Scheduler::kSplitNodes
                                      : Scheduler::kNoFlags);
    if (FLAG_turbo_verify) ScheduleVerifier::Run(schedule);
    data->set_schedule(schedule);
  }
};


struct InstructionSelectionPhase {
  static const char* phase_name() { return "select instructions"; }

  void Run(PipelineData* data, Zone* temp_zone, Linkage* linkage) {
    InstructionSelector selector(
        temp_zone, data->graph()->NodeCount(), linkage, data->sequence(),
        data->schedule(), data->source_positions(),
        data->info()->is_source_positions_enabled()
            ? InstructionSelector::kAllSourcePositions
            : InstructionSelector::kCallSourcePositions);
    selector.SelectInstructions();
  }
};


struct MeetRegisterConstraintsPhase {
  static const char* phase_name() { return "meet register constraints"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    ConstraintBuilder builder(data->register_allocation_data());
    builder.MeetRegisterConstraints();
  }
};


struct ResolvePhisPhase {
  static const char* phase_name() { return "resolve phis"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    ConstraintBuilder builder(data->register_allocation_data());
    builder.ResolvePhis();
  }
};


struct BuildLiveRangesPhase {
  static const char* phase_name() { return "build live ranges"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    LiveRangeBuilder builder(data->register_allocation_data(), temp_zone);
    builder.BuildLiveRanges();
  }
};


template <typename RegAllocator>
struct AllocateGeneralRegistersPhase {
  static const char* phase_name() { return "allocate general registers"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    RegAllocator allocator(data->register_allocation_data(), GENERAL_REGISTERS,
                           temp_zone);
    allocator.AllocateRegisters();
  }
};


template <typename RegAllocator>
struct AllocateDoubleRegistersPhase {
  static const char* phase_name() { return "allocate double registers"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    RegAllocator allocator(data->register_allocation_data(), DOUBLE_REGISTERS,
                           temp_zone);
    allocator.AllocateRegisters();
  }
};


struct LocateSpillSlotsPhase {
  static const char* phase_name() { return "locate spill slots"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    SpillSlotLocator locator(data->register_allocation_data());
    locator.LocateSpillSlots();
  }
};


struct AssignSpillSlotsPhase {
  static const char* phase_name() { return "assign spill slots"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    OperandAssigner assigner(data->register_allocation_data());
    assigner.AssignSpillSlots();
  }
};


struct CommitAssignmentPhase {
  static const char* phase_name() { return "commit assignment"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    OperandAssigner assigner(data->register_allocation_data());
    assigner.CommitAssignment();
  }
};


struct PopulateReferenceMapsPhase {
  static const char* phase_name() { return "populate pointer maps"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    ReferenceMapPopulator populator(data->register_allocation_data());
    populator.PopulateReferenceMaps();
  }
};


struct ConnectRangesPhase {
  static const char* phase_name() { return "connect ranges"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    LiveRangeConnector connector(data->register_allocation_data());
    connector.ConnectRanges(temp_zone);
  }
};


struct ResolveControlFlowPhase {
  static const char* phase_name() { return "resolve control flow"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    LiveRangeConnector connector(data->register_allocation_data());
    connector.ResolveControlFlow(temp_zone);
  }
};


struct OptimizeMovesPhase {
  static const char* phase_name() { return "optimize moves"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    MoveOptimizer move_optimizer(temp_zone, data->sequence());
    move_optimizer.Run();
  }
};


struct FrameElisionPhase {
  static const char* phase_name() { return "frame elision"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    FrameElider(data->sequence()).Run();
  }
};


struct JumpThreadingPhase {
  static const char* phase_name() { return "jump threading"; }

  void Run(PipelineData* data, Zone* temp_zone) {
    ZoneVector<RpoNumber> result(temp_zone);
    if (JumpThreading::ComputeForwarding(temp_zone, result, data->sequence())) {
      JumpThreading::ApplyForwarding(result, data->sequence());
    }
  }
};


struct GenerateCodePhase {
  static const char* phase_name() { return "generate code"; }

  void Run(PipelineData* data, Zone* temp_zone, Linkage* linkage) {
    CodeGenerator generator(data->frame(), linkage, data->sequence(),
                            data->info());
    data->set_code(generator.GenerateCode());
  }
};


struct PrintGraphPhase {
  static const char* phase_name() { return nullptr; }

  void Run(PipelineData* data, Zone* temp_zone, const char* phase) {
    CompilationInfo* info = data->info();
    Graph* graph = data->graph();

    {  // Print dot.
      FILE* dot_file = OpenVisualizerLogFile(info, phase, "dot", "w+");
      if (dot_file == nullptr) return;
      OFStream dot_of(dot_file);
      dot_of << AsDOT(*graph);
      fclose(dot_file);
    }

    {  // Print JSON.
      FILE* json_file = OpenVisualizerLogFile(info, NULL, "json", "a+");
      if (json_file == nullptr) return;
      OFStream json_of(json_file);
      json_of << "{\"name\":\"" << phase << "\",\"type\":\"graph\",\"data\":"
              << AsJSON(*graph, data->source_positions()) << "},\n";
      fclose(json_file);
    }

    if (FLAG_trace_turbo_graph) {  // Simple textual RPO.
      OFStream os(stdout);
      os << "-- Graph after " << phase << " -- " << std::endl;
      os << AsRPO(*graph);
    }
  }
};


struct VerifyGraphPhase {
  static const char* phase_name() { return nullptr; }

  void Run(PipelineData* data, Zone* temp_zone, const bool untyped) {
    Verifier::Run(data->graph(), FLAG_turbo_types && !untyped
                                     ? Verifier::TYPED
                                     : Verifier::UNTYPED);
  }
};


void Pipeline::BeginPhaseKind(const char* phase_kind_name) {
  if (data_->pipeline_statistics() != NULL) {
    data_->pipeline_statistics()->BeginPhaseKind(phase_kind_name);
  }
}


void Pipeline::RunPrintAndVerify(const char* phase, bool untyped) {
  if (FLAG_trace_turbo) {
    Run<PrintGraphPhase>(phase);
  }
  if (FLAG_turbo_verify) {
    Run<VerifyGraphPhase>(untyped);
  }
}


Handle<Code> Pipeline::GenerateCode() {
  // TODO(mstarzinger): This is just a temporary hack to make TurboFan work,
  // the correct solution is to restore the context register after invoking
  // builtins from full-codegen.
  for (int i = 0; i < Builtins::NumberOfJavaScriptBuiltins(); i++) {
    Builtins::JavaScript id = static_cast<Builtins::JavaScript>(i);
    Object* builtin = isolate()->js_builtins_object()->javascript_builtin(id);
    if (*info()->closure() == builtin) return Handle<Code>::null();
  }

  ZonePool zone_pool;
  SmartPointer<PipelineStatistics> pipeline_statistics;

  if (FLAG_turbo_stats) {
    pipeline_statistics.Reset(new PipelineStatistics(info(), &zone_pool));
    pipeline_statistics->BeginPhaseKind("initializing");
  }

  if (FLAG_trace_turbo) {
    FILE* json_file = OpenVisualizerLogFile(info(), NULL, "json", "w+");
    if (json_file != nullptr) {
      OFStream json_of(json_file);
      Handle<Script> script = info()->script();
      FunctionLiteral* function = info()->function();
      SmartArrayPointer<char> function_name =
          info()->shared_info()->DebugName()->ToCString();
      int pos = info()->shared_info()->start_position();
      json_of << "{\"function\":\"" << function_name.get()
              << "\", \"sourcePosition\":" << pos << ", \"source\":\"";
      if (!script->IsUndefined() && !script->source()->IsUndefined()) {
        DisallowHeapAllocation no_allocation;
        int start = function->start_position();
        int len = function->end_position() - start;
        String::SubStringRange source(String::cast(script->source()), start,
                                      len);
        for (const auto& c : source) {
          json_of << AsEscapedUC16ForJSON(c);
        }
      }
      json_of << "\",\n\"phases\":[";
      fclose(json_file);
    }
  }

  PipelineData data(&zone_pool, info(), pipeline_statistics.get());
  this->data_ = &data;

  if (info()->is_type_feedback_enabled()) {
    data.set_js_type_feedback(new (data.graph_zone())
                                  JSTypeFeedbackTable(data.graph_zone()));
  }

  BeginPhaseKind("graph creation");

  if (FLAG_trace_turbo) {
    OFStream os(stdout);
    os << "---------------------------------------------------\n"
       << "Begin compiling method " << GetDebugName(info()).get()
       << " using Turbofan" << std::endl;
    TurboCfgFile tcf(isolate());
    tcf << AsC1VCompilation(info());
  }

  data.source_positions()->AddDecorator();

  if (FLAG_loop_assignment_analysis) {
    Run<LoopAssignmentAnalysisPhase>();
  }

  Run<GraphBuilderPhase>();
  if (data.compilation_failed()) return Handle<Code>::null();
  RunPrintAndVerify("Initial untyped", true);

  // Perform OSR deconstruction.
  if (info()->is_osr()) {
    Run<OsrDeconstructionPhase>();
    RunPrintAndVerify("OSR deconstruction", true);
  }

  // Perform context specialization and inlining (if enabled).
  Run<InliningPhase>();
  RunPrintAndVerify("Inlined", true);

  // Remove dead->live edges from the graph.
  Run<EarlyGraphTrimmingPhase>();
  RunPrintAndVerify("Early trimmed", true);

  if (FLAG_print_turbo_replay) {
    // Print a replay of the initial graph.
    GraphReplayPrinter::PrintReplay(data.graph());
  }

  // Bailout here in case target architecture is not supported.
  if (!SupportedTarget()) return Handle<Code>::null();

  SmartPointer<Typer> typer;
  if (info()->is_typing_enabled()) {
    // Type the graph.
    typer.Reset(new Typer(isolate(), data.graph(), info()->function_type()));
    Run<TyperPhase>(typer.get());
    RunPrintAndVerify("Typed");
  }

  BeginPhaseKind("lowering");

  if (info()->is_typing_enabled()) {
    // Lower JSOperators where we can determine types.
    Run<TypedLoweringPhase>();
    RunPrintAndVerify("Lowered typed");

    if (FLAG_turbo_stress_loop_peeling) {
      Run<StressLoopPeelingPhase>();
      RunPrintAndVerify("Loop peeled");
    }

    if (info()->is_type_feedback_enabled()) {
      Run<JSTypeFeedbackPhase>();
      RunPrintAndVerify("JSType feedback");
    }

    // Lower simplified operators and insert changes.
    Run<SimplifiedLoweringPhase>();
    RunPrintAndVerify("Lowered simplified");

    // Optimize control flow.
    if (FLAG_turbo_cf_optimization) {
      Run<ControlFlowOptimizationPhase>();
      RunPrintAndVerify("Control flow optimized");
    }

    // Lower changes that have been inserted before.
    Run<ChangeLoweringPhase>();
    // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
    RunPrintAndVerify("Lowered changes", true);
  }

  // Lower any remaining generic JSOperators.
  Run<GenericLoweringPhase>();
  // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
  RunPrintAndVerify("Lowered generic", true);

  Run<LateGraphTrimmingPhase>();
  // TODO(jarin, rossberg): Remove UNTYPED once machine typing works.
  RunPrintAndVerify("Late trimmed", true);

  BeginPhaseKind("block building");

  data.source_positions()->RemoveDecorator();

  // Kill the Typer and thereby uninstall the decorator (if any).
  typer.Reset(nullptr);

  return ScheduleAndGenerateCode(
      Linkage::ComputeIncoming(data.instruction_zone(), info()));
}


Handle<Code> Pipeline::GenerateCodeForTesting(CompilationInfo* info,
                                              Graph* graph,
                                              Schedule* schedule) {
  CallDescriptor* call_descriptor =
      Linkage::ComputeIncoming(info->zone(), info);
  return GenerateCodeForTesting(info, call_descriptor, graph, schedule);
}


Handle<Code> Pipeline::GenerateCodeForTesting(Isolate* isolate,
                                              CallDescriptor* call_descriptor,
                                              Graph* graph,
                                              Schedule* schedule) {
  FakeStubForTesting stub(isolate);
  CompilationInfo info(&stub, isolate, graph->zone());
  return GenerateCodeForTesting(&info, call_descriptor, graph, schedule);
}


Handle<Code> Pipeline::GenerateCodeForTesting(CompilationInfo* info,
                                              CallDescriptor* call_descriptor,
                                              Graph* graph,
                                              Schedule* schedule) {
  // Construct a pipeline for scheduling and code generation.
  ZonePool zone_pool;
  PipelineData data(&zone_pool, info, graph, schedule);
  SmartPointer<PipelineStatistics> pipeline_statistics;
  if (FLAG_turbo_stats) {
    pipeline_statistics.Reset(new PipelineStatistics(info, &zone_pool));
    pipeline_statistics->BeginPhaseKind("test codegen");
  }

  Pipeline pipeline(info);
  pipeline.data_ = &data;
  if (data.schedule() == nullptr) {
    // TODO(rossberg): Should this really be untyped?
    pipeline.RunPrintAndVerify("Machine", true);
  }

  return pipeline.ScheduleAndGenerateCode(call_descriptor);
}


bool Pipeline::AllocateRegistersForTesting(const RegisterConfiguration* config,
                                           InstructionSequence* sequence,
                                           bool run_verifier) {
  FakeStubForTesting stub(sequence->isolate());
  CompilationInfo info(&stub, sequence->isolate(), sequence->zone());
  ZonePool zone_pool;
  PipelineData data(&zone_pool, &info, sequence);
  Pipeline pipeline(&info);
  pipeline.data_ = &data;
  pipeline.AllocateRegisters(config, run_verifier);
  return !data.compilation_failed();
}


Handle<Code> Pipeline::ScheduleAndGenerateCode(
    CallDescriptor* call_descriptor) {
  PipelineData* data = this->data_;

  DCHECK_NOT_NULL(data->graph());
  CHECK(SupportedBackend());

  if (data->schedule() == nullptr) Run<ComputeSchedulePhase>();
  TraceSchedule(data->info(), data->schedule());

  BasicBlockProfiler::Data* profiler_data = NULL;
  if (FLAG_turbo_profiling) {
    profiler_data = BasicBlockInstrumentor::Instrument(info(), data->graph(),
                                                       data->schedule());
  }

  data->InitializeInstructionSequence();

  // Select and schedule instructions covering the scheduled graph.
  Linkage linkage(call_descriptor);
  Run<InstructionSelectionPhase>(&linkage);

  if (FLAG_trace_turbo && !data->MayHaveUnverifiableGraph()) {
    TurboCfgFile tcf(isolate());
    tcf << AsC1V("CodeGen", data->schedule(), data->source_positions(),
                 data->sequence());
  }

  std::ostringstream source_position_output;
  if (FLAG_trace_turbo) {
    // Output source position information before the graph is deleted.
    data_->source_positions()->Print(source_position_output);
  }

  data->DeleteGraphZone();

  BeginPhaseKind("register allocation");

  bool run_verifier = FLAG_turbo_verify_allocation;
  // Allocate registers.
  AllocateRegisters(RegisterConfiguration::ArchDefault(), run_verifier);
  if (data->compilation_failed()) {
    info()->AbortOptimization(kNotEnoughVirtualRegistersRegalloc);
    return Handle<Code>();
  }

  BeginPhaseKind("code generation");

  // Optimimize jumps.
  if (FLAG_turbo_jt) {
    Run<JumpThreadingPhase>();
  }

  // Generate final machine code.
  Run<GenerateCodePhase>(&linkage);

  Handle<Code> code = data->code();
  if (profiler_data != NULL) {
#if ENABLE_DISASSEMBLER
    std::ostringstream os;
    code->Disassemble(NULL, os);
    profiler_data->SetCode(&os);
#endif
  }

  info()->SetCode(code);
  v8::internal::CodeGenerator::PrintCode(code, info());

  if (FLAG_trace_turbo) {
    FILE* json_file = OpenVisualizerLogFile(info(), NULL, "json", "a+");
    if (json_file != nullptr) {
      OFStream json_of(json_file);
      json_of
          << "{\"name\":\"disassembly\",\"type\":\"disassembly\",\"data\":\"";
#if ENABLE_DISASSEMBLER
      std::stringstream disassembly_stream;
      code->Disassemble(NULL, disassembly_stream);
      std::string disassembly_string(disassembly_stream.str());
      for (const auto& c : disassembly_string) {
        json_of << AsEscapedUC16ForJSON(c);
      }
#endif  // ENABLE_DISASSEMBLER
      json_of << "\"}\n],\n";
      json_of << "\"nodePositions\":";
      json_of << source_position_output.str();
      json_of << "}";
      fclose(json_file);
    }
    OFStream os(stdout);
    os << "---------------------------------------------------\n"
       << "Finished compiling method " << GetDebugName(info()).get()
       << " using Turbofan" << std::endl;
  }

  return code;
}


void Pipeline::AllocateRegisters(const RegisterConfiguration* config,
                                 bool run_verifier) {
  PipelineData* data = this->data_;

  // Don't track usage for this zone in compiler stats.
  SmartPointer<Zone> verifier_zone;
  RegisterAllocatorVerifier* verifier = nullptr;
  if (run_verifier) {
    verifier_zone.Reset(new Zone());
    verifier = new (verifier_zone.get()) RegisterAllocatorVerifier(
        verifier_zone.get(), config, data->sequence());
  }

  SmartArrayPointer<char> debug_name;
#ifdef DEBUG
  debug_name = GetDebugName(data->info());
#endif

  data->InitializeRegisterAllocationData(config, debug_name.get());
  if (info()->is_osr()) {
    OsrHelper osr_helper(info());
    osr_helper.SetupFrame(data->frame());
  }

  Run<MeetRegisterConstraintsPhase>();
  Run<ResolvePhisPhase>();
  Run<BuildLiveRangesPhase>();
  if (FLAG_trace_turbo_graph) {
    OFStream os(stdout);
    PrintableInstructionSequence printable = {config, data->sequence()};
    os << "----- Instruction sequence before register allocation -----\n"
       << printable;
  }
  if (verifier != nullptr) {
    CHECK(!data->register_allocation_data()->ExistsUseWithoutDefinition());
  }
  if (FLAG_turbo_greedy_regalloc) {
    Run<AllocateGeneralRegistersPhase<GreedyAllocator>>();
    Run<AllocateDoubleRegistersPhase<GreedyAllocator>>();
  } else {
    Run<AllocateGeneralRegistersPhase<LinearScanAllocator>>();
    Run<AllocateDoubleRegistersPhase<LinearScanAllocator>>();
  }

  if (FLAG_turbo_frame_elision) {
    Run<LocateSpillSlotsPhase>();
    Run<FrameElisionPhase>();
  }

  Run<AssignSpillSlotsPhase>();

  Run<CommitAssignmentPhase>();
  Run<PopulateReferenceMapsPhase>();
  Run<ConnectRangesPhase>();
  Run<ResolveControlFlowPhase>();
  if (FLAG_turbo_move_optimization) {
    Run<OptimizeMovesPhase>();
  }

  if (FLAG_trace_turbo_graph) {
    OFStream os(stdout);
    PrintableInstructionSequence printable = {config, data->sequence()};
    os << "----- Instruction sequence after register allocation -----\n"
       << printable;
  }

  if (verifier != nullptr) {
    verifier->VerifyAssignment();
    verifier->VerifyGapMoves();
  }

  if (FLAG_trace_turbo && !data->MayHaveUnverifiableGraph()) {
    TurboCfgFile tcf(data->isolate());
    tcf << AsC1VRegisterAllocationData("CodeGen",
                                       data->register_allocation_data());
  }

  data->DeleteRegisterAllocationZone();
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
