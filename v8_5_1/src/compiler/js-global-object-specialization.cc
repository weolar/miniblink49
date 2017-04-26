// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-global-object-specialization.h"

#include "src/compilation-dependencies.h"
#include "src/compiler/access-builder.h"
#include "src/compiler/common-operator.h"
#include "src/compiler/js-graph.h"
#include "src/compiler/js-operator.h"
#include "src/compiler/node-properties.h"
#include "src/compiler/simplified-operator.h"
#include "src/lookup.h"
#include "src/objects-inl.h"  // TODO(mstarzinger): Temporary cycle breaker!
#include "src/type-cache.h"

namespace v8 {
namespace internal {
namespace compiler {

struct JSGlobalObjectSpecialization::ScriptContextTableLookupResult {
  Handle<Context> context;
  bool immutable;
  int index;
};


JSGlobalObjectSpecialization::JSGlobalObjectSpecialization(
    Editor* editor, JSGraph* jsgraph,
    MaybeHandle<Context> native_context, CompilationDependencies* dependencies)
    : AdvancedReducer(editor),
      jsgraph_(jsgraph),
      native_context_(native_context),
      dependencies_(dependencies),
      type_cache_(TypeCache::Get()) {}


Reduction JSGlobalObjectSpecialization::Reduce(Node* node) {
  switch (node->opcode()) {
    case IrOpcode::kJSLoadGlobal:
      return ReduceJSLoadGlobal(node);
    case IrOpcode::kJSStoreGlobal:
      return ReduceJSStoreGlobal(node);
    default:
      break;
  }
  return NoChange();
}

Reduction JSGlobalObjectSpecialization::ReduceJSLoadGlobal(Node* node) {
  DCHECK_EQ(IrOpcode::kJSLoadGlobal, node->opcode());
  Handle<Name> name = LoadGlobalParametersOf(node->op()).name();
  Node* effect = NodeProperties::GetEffectInput(node);
  Node* control = NodeProperties::GetControlInput(node);

  // Retrieve the global object from the given {node}.
  Handle<JSGlobalObject> global_object;
  if (!GetGlobalObject(node).ToHandle(&global_object)) return NoChange();

  // Try to lookup the name on the script context table first (lexical scoping).
  ScriptContextTableLookupResult result;
  if (LookupInScriptContextTable(global_object, name, &result)) {
    if (result.context->is_the_hole(result.index)) return NoChange();
    Node* context = jsgraph()->HeapConstant(result.context);
    Node* value = effect = graph()->NewNode(
        javascript()->LoadContext(0, result.index, result.immutable), context,
        context, effect);
    ReplaceWithValue(node, value, effect);
    return Replace(value);
  }

  // Lookup on the global object instead.  We only deal with own data
  // properties of the global object here (represented as PropertyCell).
  LookupIterator it(global_object, name, LookupIterator::OWN);
  if (it.state() != LookupIterator::DATA) return NoChange();
  Handle<PropertyCell> property_cell = it.GetPropertyCell();
  PropertyDetails property_details = property_cell->property_details();
  Handle<Object> property_cell_value(property_cell->value(), isolate());

  // Load from non-configurable, read-only data property on the global
  // object can be constant-folded, even without deoptimization support.
  if (!property_details.IsConfigurable() && property_details.IsReadOnly()) {
    Node* value = jsgraph()->Constant(property_cell_value);
    ReplaceWithValue(node, value);
    return Replace(value);
  }

  // Record a code dependency on the cell if we can benefit from the
  // additional feedback, or the global property is configurable (i.e.
  // can be deleted or reconfigured to an accessor property).
  if (property_details.cell_type() != PropertyCellType::kMutable ||
      property_details.IsConfigurable()) {
    dependencies()->AssumePropertyCell(property_cell);
  }

  // Load from constant/undefined global property can be constant-folded.
  if (property_details.cell_type() == PropertyCellType::kConstant ||
      property_details.cell_type() == PropertyCellType::kUndefined) {
    Node* value = jsgraph()->Constant(property_cell_value);
    ReplaceWithValue(node, value);
    return Replace(value);
  }

  // Load from constant type cell can benefit from type feedback.
  Type* property_cell_value_type = Type::Tagged();
  if (property_details.cell_type() == PropertyCellType::kConstantType) {
    // Compute proper type based on the current value in the cell.
    if (property_cell_value->IsSmi()) {
      property_cell_value_type = type_cache_.kSmi;
    } else if (property_cell_value->IsNumber()) {
      property_cell_value_type = type_cache_.kHeapNumber;
    } else {
      Handle<Map> property_cell_value_map(
          Handle<HeapObject>::cast(property_cell_value)->map(), isolate());
      property_cell_value_type =
          Type::Class(property_cell_value_map, graph()->zone());
    }
  }
  Node* value = effect = graph()->NewNode(
      simplified()->LoadField(
          AccessBuilder::ForPropertyCellValue(property_cell_value_type)),
      jsgraph()->HeapConstant(property_cell), effect, control);
  ReplaceWithValue(node, value, effect, control);
  return Replace(value);
}


Reduction JSGlobalObjectSpecialization::ReduceJSStoreGlobal(Node* node) {
  DCHECK_EQ(IrOpcode::kJSStoreGlobal, node->opcode());
  Handle<Name> name = StoreGlobalParametersOf(node->op()).name();
  Node* value = NodeProperties::GetValueInput(node, 0);
  Node* frame_state = NodeProperties::GetFrameStateInput(node, 1);
  Node* effect = NodeProperties::GetEffectInput(node);
  Node* control = NodeProperties::GetControlInput(node);

  // Retrieve the global object from the given {node}.
  Handle<JSGlobalObject> global_object;
  if (!GetGlobalObject(node).ToHandle(&global_object)) return NoChange();

  // Try to lookup the name on the script context table first (lexical scoping).
  ScriptContextTableLookupResult result;
  if (LookupInScriptContextTable(global_object, name, &result)) {
    if (result.context->is_the_hole(result.index)) return NoChange();
    if (result.immutable) return NoChange();
    Node* context = jsgraph()->HeapConstant(result.context);
    effect = graph()->NewNode(javascript()->StoreContext(0, result.index),
                              context, value, context, effect, control);
    ReplaceWithValue(node, value, effect, control);
    return Replace(value);
  }

  // Lookup on the global object instead.  We only deal with own data
  // properties of the global object here (represented as PropertyCell).
  LookupIterator it(global_object, name, LookupIterator::OWN);
  if (it.state() != LookupIterator::DATA) return NoChange();
  Handle<PropertyCell> property_cell = it.GetPropertyCell();
  PropertyDetails property_details = property_cell->property_details();
  Handle<Object> property_cell_value(property_cell->value(), isolate());

  // Don't even bother trying to lower stores to read-only data properties.
  if (property_details.IsReadOnly()) return NoChange();
  switch (property_details.cell_type()) {
    case PropertyCellType::kUndefined: {
      return NoChange();
    }
    case PropertyCellType::kConstant: {
      // Record a code dependency on the cell, and just deoptimize if the new
      // value doesn't match the previous value stored inside the cell.
      dependencies()->AssumePropertyCell(property_cell);
      Node* check =
          graph()->NewNode(simplified()->ReferenceEqual(Type::Tagged()), value,
                           jsgraph()->Constant(property_cell_value));
      control = graph()->NewNode(common()->DeoptimizeUnless(), check,
                                 frame_state, effect, control);
      break;
    }
    case PropertyCellType::kConstantType: {
      // Record a code dependency on the cell, and just deoptimize if the new
      // values' type doesn't match the type of the previous value in the cell.
      dependencies()->AssumePropertyCell(property_cell);
      Node* check = graph()->NewNode(simplified()->ObjectIsSmi(), value);
      Type* property_cell_value_type = Type::TaggedSigned();
      if (property_cell_value->IsHeapObject()) {
        // Deoptimize if the {value} is a Smi.
        control = graph()->NewNode(common()->DeoptimizeIf(), check, frame_state,
                                   effect, control);

        // Load the {value} map check against the {property_cell} map.
        Node* value_map = effect =
            graph()->NewNode(simplified()->LoadField(AccessBuilder::ForMap()),
                             value, effect, control);
        Handle<Map> property_cell_value_map(
            Handle<HeapObject>::cast(property_cell_value)->map(), isolate());
        check = graph()->NewNode(
            simplified()->ReferenceEqual(Type::Any()), value_map,
            jsgraph()->HeapConstant(property_cell_value_map));
        property_cell_value_type = Type::TaggedPointer();
      }
      control = graph()->NewNode(common()->DeoptimizeUnless(), check,
                                 frame_state, effect, control);
      effect = graph()->NewNode(
          simplified()->StoreField(
              AccessBuilder::ForPropertyCellValue(property_cell_value_type)),
          jsgraph()->HeapConstant(property_cell), value, effect, control);
      break;
    }
    case PropertyCellType::kMutable: {
      // Store to non-configurable, data property on the global can be lowered
      // to a field store, even without recording a code dependency on the cell,
      // because the property cannot be deleted or reconfigured to an accessor
      // or interceptor property.
      if (property_details.IsConfigurable()) {
        // Protect lowering by recording a code dependency on the cell.
        dependencies()->AssumePropertyCell(property_cell);
      }
      effect = graph()->NewNode(
          simplified()->StoreField(AccessBuilder::ForPropertyCellValue()),
          jsgraph()->HeapConstant(property_cell), value, effect, control);
      break;
    }
  }
  ReplaceWithValue(node, value, effect, control);
  return Replace(value);
}


MaybeHandle<JSGlobalObject> JSGlobalObjectSpecialization::GetGlobalObject(
    Node* node) {
  Node* const context = NodeProperties::GetContextInput(node);
  return NodeProperties::GetSpecializationGlobalObject(context,
                                                       native_context());
}


bool JSGlobalObjectSpecialization::LookupInScriptContextTable(
    Handle<JSGlobalObject> global_object, Handle<Name> name,
    ScriptContextTableLookupResult* result) {
  if (!name->IsString()) return false;
  Handle<ScriptContextTable> script_context_table(
      global_object->native_context()->script_context_table(), isolate());
  ScriptContextTable::LookupResult lookup_result;
  if (!ScriptContextTable::Lookup(script_context_table,
                                  Handle<String>::cast(name), &lookup_result)) {
    return false;
  }
  Handle<Context> script_context = ScriptContextTable::GetContext(
      script_context_table, lookup_result.context_index);
  result->context = script_context;
  result->immutable = IsImmutableVariableMode(lookup_result.mode);
  result->index = lookup_result.slot_index;
  return true;
}


Graph* JSGlobalObjectSpecialization::graph() const {
  return jsgraph()->graph();
}


Isolate* JSGlobalObjectSpecialization::isolate() const {
  return jsgraph()->isolate();
}


CommonOperatorBuilder* JSGlobalObjectSpecialization::common() const {
  return jsgraph()->common();
}


JSOperatorBuilder* JSGlobalObjectSpecialization::javascript() const {
  return jsgraph()->javascript();
}


SimplifiedOperatorBuilder* JSGlobalObjectSpecialization::simplified() const {
  return jsgraph()->simplified();
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
