// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_JS_NATIVE_CONTEXT_SPECIALIZATION_H_
#define V8_COMPILER_JS_NATIVE_CONTEXT_SPECIALIZATION_H_

#include "src/base/flags.h"
#include "src/compiler/access-info.h"
#include "src/compiler/graph-reducer.h"

namespace v8 {
namespace internal {

// Forward declarations.
class CompilationDependencies;
class Factory;
class FeedbackNexus;
class TypeCache;


namespace compiler {

// Forward declarations.
class CommonOperatorBuilder;
class JSGraph;
class JSOperatorBuilder;
class MachineOperatorBuilder;
class SimplifiedOperatorBuilder;


// Specializes a given JSGraph to a given native context, potentially constant
// folding some {LoadGlobal} nodes or strength reducing some {StoreGlobal}
// nodes.  And also specializes {LoadNamed} and {StoreNamed} nodes according
// to type feedback (if available).
class JSNativeContextSpecialization final : public AdvancedReducer {
 public:
  // Flags that control the mode of operation.
  enum Flag {
    kNoFlags = 0u,
    kDeoptimizationEnabled = 1u << 0,
  };
  typedef base::Flags<Flag> Flags;

  JSNativeContextSpecialization(Editor* editor, JSGraph* jsgraph, Flags flags,
                                Handle<Context> native_context,
                                CompilationDependencies* dependencies,
                                Zone* zone);

  Reduction Reduce(Node* node) final;

 private:
  Reduction ReduceJSCallFunction(Node* node);
  Reduction ReduceJSLoadNamed(Node* node);
  Reduction ReduceJSStoreNamed(Node* node);
  Reduction ReduceJSLoadProperty(Node* node);
  Reduction ReduceJSStoreProperty(Node* node);

  Reduction ReduceElementAccess(Node* node, Node* index, Node* value,
                                MapHandleList const& receiver_maps,
                                AccessMode access_mode,
                                LanguageMode language_mode);
  Reduction ReduceKeyedAccess(Node* node, Node* index, Node* value,
                              FeedbackNexus const& nexus,
                              AccessMode access_mode,
                              LanguageMode language_mode);
  Reduction ReduceNamedAccess(Node* node, Node* value,
                              MapHandleList const& receiver_maps,
                              Handle<Name> name, AccessMode access_mode,
                              LanguageMode language_mode,
                              Node* index = nullptr);

  // Adds stability dependencies on all prototypes of every class in
  // {receiver_type} up to (and including) the {holder}.
  void AssumePrototypesStable(Type* receiver_type, Handle<JSObject> holder);

  // Assuming that {if_projection} is either IfTrue or IfFalse, adds a hint on
  // the dominating Branch that {if_projection} is the unlikely (deferred) case.
  void MarkAsDeferred(Node* if_projection);

  Graph* graph() const;
  JSGraph* jsgraph() const { return jsgraph_; }
  Isolate* isolate() const;
  Factory* factory() const;
  CommonOperatorBuilder* common() const;
  JSOperatorBuilder* javascript() const;
  SimplifiedOperatorBuilder* simplified() const;
  MachineOperatorBuilder* machine() const;
  Flags flags() const { return flags_; }
  Handle<Context> native_context() const { return native_context_; }
  CompilationDependencies* dependencies() const { return dependencies_; }
  Zone* zone() const { return zone_; }
  AccessInfoFactory& access_info_factory() { return access_info_factory_; }

  JSGraph* const jsgraph_;
  Flags const flags_;
  Handle<Context> native_context_;
  CompilationDependencies* const dependencies_;
  Zone* const zone_;
  TypeCache const& type_cache_;
  AccessInfoFactory access_info_factory_;

  DISALLOW_COPY_AND_ASSIGN(JSNativeContextSpecialization);
};

DEFINE_OPERATORS_FOR_FLAGS(JSNativeContextSpecialization::Flags)

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_JS_NATIVE_CONTEXT_SPECIALIZATION_H_
