// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "src/v8.h"

#if V8_TARGET_ARCH_MIPS64

#include "src/codegen.h"
#include "src/ic/ic.h"
#include "src/ic/ic-compiler.h"
#include "src/ic/stub-cache.h"

namespace v8 {
namespace internal {


// ----------------------------------------------------------------------------
// Static IC stub generators.
//

#define __ ACCESS_MASM(masm)


static void GenerateGlobalInstanceTypeCheck(MacroAssembler* masm, Register type,
                                            Label* global_object) {
  // Register usage:
  //   type: holds the receiver instance type on entry.
  __ Branch(global_object, eq, type, Operand(JS_GLOBAL_OBJECT_TYPE));
  __ Branch(global_object, eq, type, Operand(JS_BUILTINS_OBJECT_TYPE));
  __ Branch(global_object, eq, type, Operand(JS_GLOBAL_PROXY_TYPE));
}


// Helper function used from LoadIC GenerateNormal.
//
// elements: Property dictionary. It is not clobbered if a jump to the miss
//           label is done.
// name:     Property name. It is not clobbered if a jump to the miss label is
//           done
// result:   Register for the result. It is only updated if a jump to the miss
//           label is not done. Can be the same as elements or name clobbering
//           one of these in the case of not jumping to the miss label.
// The two scratch registers need to be different from elements, name and
// result.
// The generated code assumes that the receiver has slow properties,
// is not a global object and does not have interceptors.
// The address returned from GenerateStringDictionaryProbes() in scratch2
// is used.
static void GenerateDictionaryLoad(MacroAssembler* masm, Label* miss,
                                   Register elements, Register name,
                                   Register result, Register scratch1,
                                   Register scratch2) {
  // Main use of the scratch registers.
  // scratch1: Used as temporary and to hold the capacity of the property
  //           dictionary.
  // scratch2: Used as temporary.
  Label done;

  // Probe the dictionary.
  NameDictionaryLookupStub::GeneratePositiveLookup(masm, miss, &done, elements,
                                                   name, scratch1, scratch2);

  // If probing finds an entry check that the value is a normal
  // property.
  __ bind(&done);  // scratch2 == elements + 4 * index.
  const int kElementsStartOffset =
      NameDictionary::kHeaderSize +
      NameDictionary::kElementsStartIndex * kPointerSize;
  const int kDetailsOffset = kElementsStartOffset + 2 * kPointerSize;
  __ ld(scratch1, FieldMemOperand(scratch2, kDetailsOffset));
  __ And(at, scratch1,
         Operand(Smi::FromInt(PropertyDetails::TypeField::kMask)));
  __ Branch(miss, ne, at, Operand(zero_reg));

  // Get the value at the masked, scaled index and return.
  __ ld(result,
        FieldMemOperand(scratch2, kElementsStartOffset + 1 * kPointerSize));
}


// Helper function used from StoreIC::GenerateNormal.
//
// elements: Property dictionary. It is not clobbered if a jump to the miss
//           label is done.
// name:     Property name. It is not clobbered if a jump to the miss label is
//           done
// value:    The value to store.
// The two scratch registers need to be different from elements, name and
// result.
// The generated code assumes that the receiver has slow properties,
// is not a global object and does not have interceptors.
// The address returned from GenerateStringDictionaryProbes() in scratch2
// is used.
static void GenerateDictionaryStore(MacroAssembler* masm, Label* miss,
                                    Register elements, Register name,
                                    Register value, Register scratch1,
                                    Register scratch2) {
  // Main use of the scratch registers.
  // scratch1: Used as temporary and to hold the capacity of the property
  //           dictionary.
  // scratch2: Used as temporary.
  Label done;

  // Probe the dictionary.
  NameDictionaryLookupStub::GeneratePositiveLookup(masm, miss, &done, elements,
                                                   name, scratch1, scratch2);

  // If probing finds an entry in the dictionary check that the value
  // is a normal property that is not read only.
  __ bind(&done);  // scratch2 == elements + 4 * index.
  const int kElementsStartOffset =
      NameDictionary::kHeaderSize +
      NameDictionary::kElementsStartIndex * kPointerSize;
  const int kDetailsOffset = kElementsStartOffset + 2 * kPointerSize;
  const int kTypeAndReadOnlyMask =
      (PropertyDetails::TypeField::kMask |
       PropertyDetails::AttributesField::encode(READ_ONLY));
  __ ld(scratch1, FieldMemOperand(scratch2, kDetailsOffset));
  __ And(at, scratch1, Operand(Smi::FromInt(kTypeAndReadOnlyMask)));
  __ Branch(miss, ne, at, Operand(zero_reg));

  // Store the value at the masked, scaled index and return.
  const int kValueOffset = kElementsStartOffset + kPointerSize;
  __ Daddu(scratch2, scratch2, Operand(kValueOffset - kHeapObjectTag));
  __ sd(value, MemOperand(scratch2));

  // Update the write barrier. Make sure not to clobber the value.
  __ mov(scratch1, value);
  __ RecordWrite(elements, scratch2, scratch1, kRAHasNotBeenSaved,
                 kDontSaveFPRegs);
}


// Checks the receiver for special cases (value type, slow case bits).
// Falls through for regular JS object.
static void GenerateKeyedLoadReceiverCheck(MacroAssembler* masm,
                                           Register receiver, Register map,
                                           Register scratch,
                                           int interceptor_bit, Label* slow) {
  // Check that the object isn't a smi.
  __ JumpIfSmi(receiver, slow);
  // Get the map of the receiver.
  __ ld(map, FieldMemOperand(receiver, HeapObject::kMapOffset));
  // Check bit field.
  __ lbu(scratch, FieldMemOperand(map, Map::kBitFieldOffset));
  __ And(at, scratch,
         Operand((1 << Map::kIsAccessCheckNeeded) | (1 << interceptor_bit)));
  __ Branch(slow, ne, at, Operand(zero_reg));
  // Check that the object is some kind of JS object EXCEPT JS Value type.
  // In the case that the object is a value-wrapper object,
  // we enter the runtime system to make sure that indexing into string
  // objects work as intended.
  DCHECK(JS_OBJECT_TYPE > JS_VALUE_TYPE);
  __ lbu(scratch, FieldMemOperand(map, Map::kInstanceTypeOffset));
  __ Branch(slow, lt, scratch, Operand(JS_OBJECT_TYPE));
}


// Loads an indexed element from a fast case array.
static void GenerateFastArrayLoad(MacroAssembler* masm, Register receiver,
                                  Register key, Register elements,
                                  Register scratch1, Register scratch2,
                                  Register result, Label* slow,
                                  LanguageMode language_mode) {
  // Register use:
  //
  // receiver - holds the receiver on entry.
  //            Unchanged unless 'result' is the same register.
  //
  // key      - holds the smi key on entry.
  //            Unchanged unless 'result' is the same register.
  //
  // result   - holds the result on exit if the load succeeded.
  //            Allowed to be the the same as 'receiver' or 'key'.
  //            Unchanged on bailout so 'receiver' and 'key' can be safely
  //            used by further computation.
  //
  // Scratch registers:
  //
  // elements - holds the elements of the receiver and its prototypes.
  //
  // scratch1 - used to hold elements length, bit fields, base addresses.
  //
  // scratch2 - used to hold maps, prototypes, and the loaded value.
  Label check_prototypes, check_next_prototype;
  Label done, in_bounds, absent;

  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));
  __ AssertFastElements(elements);

  // Check that the key (index) is within bounds.
  __ ld(scratch1, FieldMemOperand(elements, FixedArray::kLengthOffset));
  __ Branch(&in_bounds, lo, key, Operand(scratch1));
  // Out-of-bounds. Check the prototype chain to see if we can just return
  // 'undefined'.
  // Negative keys can't take the fast OOB path.
  __ Branch(slow, lt, key, Operand(zero_reg));
  __ bind(&check_prototypes);
  __ ld(scratch2, FieldMemOperand(receiver, HeapObject::kMapOffset));
  __ bind(&check_next_prototype);
  __ ld(scratch2, FieldMemOperand(scratch2, Map::kPrototypeOffset));
  // scratch2: current prototype
  __ LoadRoot(at, Heap::kNullValueRootIndex);
  __ Branch(&absent, eq, scratch2, Operand(at));
  __ ld(elements, FieldMemOperand(scratch2, JSObject::kElementsOffset));
  __ ld(scratch2, FieldMemOperand(scratch2, HeapObject::kMapOffset));
  // elements: elements of current prototype
  // scratch2: map of current prototype
  __ lbu(scratch1, FieldMemOperand(scratch2, Map::kInstanceTypeOffset));
  __ Branch(slow, lo, scratch1, Operand(JS_OBJECT_TYPE));
  __ lbu(scratch1, FieldMemOperand(scratch2, Map::kBitFieldOffset));
  __ And(at, scratch1, Operand((1 << Map::kIsAccessCheckNeeded) |
                               (1 << Map::kHasIndexedInterceptor)));
  __ Branch(slow, ne, at, Operand(zero_reg));
  __ LoadRoot(at, Heap::kEmptyFixedArrayRootIndex);
  __ Branch(slow, ne, elements, Operand(at));
  __ Branch(&check_next_prototype);

  __ bind(&absent);
  if (is_strong(language_mode)) {
    __ Branch(slow);
  } else {
    __ LoadRoot(result, Heap::kUndefinedValueRootIndex);
    __ Branch(&done);
  }

  __ bind(&in_bounds);
  // Fast case: Do the load.
  __ Daddu(scratch1, elements,
           Operand(FixedArray::kHeaderSize - kHeapObjectTag));
  // The key is a smi.
  STATIC_ASSERT(kSmiTag == 0 && kSmiTagSize < kPointerSizeLog2);
  __ SmiScale(at, key, kPointerSizeLog2);
  __ daddu(at, at, scratch1);
  __ ld(scratch2, MemOperand(at));

  __ LoadRoot(at, Heap::kTheHoleValueRootIndex);
  // In case the loaded value is the_hole we have to check the prototype chain.
  __ Branch(&check_prototypes, eq, scratch2, Operand(at));
  __ Move(result, scratch2);
  __ bind(&done);
}


// Checks whether a key is an array index string or a unique name.
// Falls through if a key is a unique name.
static void GenerateKeyNameCheck(MacroAssembler* masm, Register key,
                                 Register map, Register hash,
                                 Label* index_string, Label* not_unique) {
  // The key is not a smi.
  Label unique;
  // Is it a name?
  __ GetObjectType(key, map, hash);
  __ Branch(not_unique, hi, hash, Operand(LAST_UNIQUE_NAME_TYPE));
  STATIC_ASSERT(LAST_UNIQUE_NAME_TYPE == FIRST_NONSTRING_TYPE);
  __ Branch(&unique, eq, hash, Operand(LAST_UNIQUE_NAME_TYPE));

  // Is the string an array index, with cached numeric value?
  __ lwu(hash, FieldMemOperand(key, Name::kHashFieldOffset));
  __ And(at, hash, Operand(Name::kContainsCachedArrayIndexMask));
  __ Branch(index_string, eq, at, Operand(zero_reg));

  // Is the string internalized? We know it's a string, so a single
  // bit test is enough.
  // map: key map
  __ lbu(hash, FieldMemOperand(map, Map::kInstanceTypeOffset));
  STATIC_ASSERT(kInternalizedTag == 0);
  __ And(at, hash, Operand(kIsNotInternalizedMask));
  __ Branch(not_unique, ne, at, Operand(zero_reg));

  __ bind(&unique);
}


void LoadIC::GenerateNormal(MacroAssembler* masm, LanguageMode language_mode) {
  Register dictionary = a0;
  DCHECK(!dictionary.is(LoadDescriptor::ReceiverRegister()));
  DCHECK(!dictionary.is(LoadDescriptor::NameRegister()));
  Label slow;

  __ ld(dictionary, FieldMemOperand(LoadDescriptor::ReceiverRegister(),
                                    JSObject::kPropertiesOffset));
  GenerateDictionaryLoad(masm, &slow, dictionary,
                         LoadDescriptor::NameRegister(), v0, a3, a4);
  __ Ret();

  // Dictionary load failed, go slow (but don't miss).
  __ bind(&slow);
  GenerateRuntimeGetProperty(masm, language_mode);
}


// A register that isn't one of the parameters to the load ic.
static const Register LoadIC_TempRegister() { return a3; }


static void LoadIC_PushArgs(MacroAssembler* masm) {
  Register receiver = LoadDescriptor::ReceiverRegister();
  Register name = LoadDescriptor::NameRegister();
  Register slot = LoadDescriptor::SlotRegister();
  Register vector = LoadWithVectorDescriptor::VectorRegister();

  __ Push(receiver, name, slot, vector);
}


void LoadIC::GenerateMiss(MacroAssembler* masm) {
  // The return address is on the stack.
  Isolate* isolate = masm->isolate();

  DCHECK(!AreAliased(a4, a5, LoadWithVectorDescriptor::SlotRegister(),
                     LoadWithVectorDescriptor::VectorRegister()));
  __ IncrementCounter(isolate->counters()->load_miss(), 1, a4, a5);

  LoadIC_PushArgs(masm);

  // Perform tail call to the entry.
  ExternalReference ref = ExternalReference(IC_Utility(kLoadIC_Miss), isolate);
  int arg_count = 4;
  __ TailCallExternalReference(ref, arg_count, 1);
}


void LoadIC::GenerateRuntimeGetProperty(MacroAssembler* masm,
                                        LanguageMode language_mode) {
  // The return address is in ra.

  __ mov(LoadIC_TempRegister(), LoadDescriptor::ReceiverRegister());
  __ Push(LoadIC_TempRegister(), LoadDescriptor::NameRegister());

  // Do tail-call to runtime routine.
  __ TailCallRuntime(is_strong(language_mode) ? Runtime::kGetPropertyStrong
                                              : Runtime::kGetProperty,
                     2, 1);
}


void KeyedLoadIC::GenerateMiss(MacroAssembler* masm) {
  // The return address is in ra.
  Isolate* isolate = masm->isolate();

  DCHECK(!AreAliased(a4, a5, LoadWithVectorDescriptor::SlotRegister(),
                     LoadWithVectorDescriptor::VectorRegister()));
  __ IncrementCounter(isolate->counters()->keyed_load_miss(), 1, a4, a5);

  LoadIC_PushArgs(masm);

  // Perform tail call to the entry.
  ExternalReference ref =
      ExternalReference(IC_Utility(kKeyedLoadIC_Miss), isolate);

  int arg_count = 4;
  __ TailCallExternalReference(ref, arg_count, 1);
}


void KeyedLoadIC::GenerateRuntimeGetProperty(MacroAssembler* masm,
                                             LanguageMode language_mode) {
  // The return address is in ra.

  __ Push(LoadDescriptor::ReceiverRegister(), LoadDescriptor::NameRegister());

  // Do tail-call to runtime routine.
  __ TailCallRuntime(is_strong(language_mode) ? Runtime::kKeyedGetPropertyStrong
                                              : Runtime::kKeyedGetProperty,
                     2, 1);
}


void KeyedLoadIC::GenerateMegamorphic(MacroAssembler* masm,
                                      LanguageMode language_mode) {
  // The return address is in ra.
  Label slow, check_name, index_smi, index_name, property_array_property;
  Label probe_dictionary, check_number_dictionary;

  Register key = LoadDescriptor::NameRegister();
  Register receiver = LoadDescriptor::ReceiverRegister();
  DCHECK(key.is(a2));
  DCHECK(receiver.is(a1));

  Isolate* isolate = masm->isolate();

  // Check that the key is a smi.
  __ JumpIfNotSmi(key, &check_name);
  __ bind(&index_smi);
  // Now the key is known to be a smi. This place is also jumped to from below
  // where a numeric string is converted to a smi.

  GenerateKeyedLoadReceiverCheck(masm, receiver, a0, a3,
                                 Map::kHasIndexedInterceptor, &slow);

  // Check the receiver's map to see if it has fast elements.
  __ CheckFastElements(a0, a3, &check_number_dictionary);

  GenerateFastArrayLoad(masm, receiver, key, a0, a3, a4, v0, &slow,
                        language_mode);
  __ IncrementCounter(isolate->counters()->keyed_load_generic_smi(), 1, a4, a3);
  __ Ret();

  __ bind(&check_number_dictionary);
  __ ld(a4, FieldMemOperand(receiver, JSObject::kElementsOffset));
  __ ld(a3, FieldMemOperand(a4, JSObject::kMapOffset));

  // Check whether the elements is a number dictionary.
  // a3: elements map
  // a4: elements
  __ LoadRoot(at, Heap::kHashTableMapRootIndex);
  __ Branch(&slow, ne, a3, Operand(at));
  __ dsra32(a0, key, 0);
  __ LoadFromNumberDictionary(&slow, a4, key, v0, a0, a3, a5);
  __ Ret();

  // Slow case, key and receiver still in a2 and a1.
  __ bind(&slow);
  __ IncrementCounter(isolate->counters()->keyed_load_generic_slow(), 1, a4,
                      a3);
  GenerateRuntimeGetProperty(masm, language_mode);

  __ bind(&check_name);
  GenerateKeyNameCheck(masm, key, a0, a3, &index_name, &slow);

  GenerateKeyedLoadReceiverCheck(masm, receiver, a0, a3,
                                 Map::kHasNamedInterceptor, &slow);


  // If the receiver is a fast-case object, check the stub cache. Otherwise
  // probe the dictionary.
  __ ld(a3, FieldMemOperand(receiver, JSObject::kPropertiesOffset));
  __ ld(a4, FieldMemOperand(a3, HeapObject::kMapOffset));
  __ LoadRoot(at, Heap::kHashTableMapRootIndex);
  __ Branch(&probe_dictionary, eq, a4, Operand(at));

  // The handlers in the stub cache expect a vector and slot. Since we won't
  // change the IC from any downstream misses, a dummy vector can be used.
  Register vector = LoadWithVectorDescriptor::VectorRegister();
  Register slot = LoadWithVectorDescriptor::SlotRegister();
  DCHECK(!AreAliased(vector, slot, a4, a5, a6, t1));
  Handle<TypeFeedbackVector> dummy_vector = Handle<TypeFeedbackVector>::cast(
      masm->isolate()->factory()->keyed_load_dummy_vector());
  int slot_index = dummy_vector->GetIndex(FeedbackVectorICSlot(0));
  __ LoadRoot(vector, Heap::kKeyedLoadDummyVectorRootIndex);
  __ li(slot, Operand(Smi::FromInt(slot_index)));

  Code::Flags flags = Code::RemoveTypeAndHolderFromFlags(
      Code::ComputeHandlerFlags(Code::LOAD_IC));
  masm->isolate()->stub_cache()->GenerateProbe(
      masm, Code::LOAD_IC, flags, false, receiver, key, a4, a5, a6, t1);
  // Cache miss.
  GenerateMiss(masm);

  // Do a quick inline probe of the receiver's dictionary, if it
  // exists.
  __ bind(&probe_dictionary);
  // a3: elements
  __ ld(a0, FieldMemOperand(receiver, HeapObject::kMapOffset));
  __ lbu(a0, FieldMemOperand(a0, Map::kInstanceTypeOffset));
  GenerateGlobalInstanceTypeCheck(masm, a0, &slow);
  // Load the property to v0.
  GenerateDictionaryLoad(masm, &slow, a3, key, v0, a5, a4);
  __ IncrementCounter(isolate->counters()->keyed_load_generic_symbol(), 1, a4,
                      a3);
  __ Ret();

  __ bind(&index_name);
  __ IndexFromHash(a3, key);
  // Now jump to the place where smi keys are handled.
  __ Branch(&index_smi);
}


static void KeyedStoreGenerateMegamorphicHelper(
    MacroAssembler* masm, Label* fast_object, Label* fast_double, Label* slow,
    KeyedStoreCheckMap check_map, KeyedStoreIncrementLength increment_length,
    Register value, Register key, Register receiver, Register receiver_map,
    Register elements_map, Register elements) {
  Label transition_smi_elements;
  Label finish_object_store, non_double_value, transition_double_elements;
  Label fast_double_without_map_check;

  // Fast case: Do the store, could be either Object or double.
  __ bind(fast_object);
  Register scratch_value = a4;
  Register address = a5;
  if (check_map == kCheckMap) {
    __ ld(elements_map, FieldMemOperand(elements, HeapObject::kMapOffset));
    __ Branch(fast_double, ne, elements_map,
              Operand(masm->isolate()->factory()->fixed_array_map()));
  }

  // HOLECHECK: guards "A[i] = V"
  // We have to go to the runtime if the current value is the hole because
  // there may be a callback on the element.
  Label holecheck_passed1;
  __ Daddu(address, elements, FixedArray::kHeaderSize - kHeapObjectTag);
  __ SmiScale(at, key, kPointerSizeLog2);
  __ daddu(address, address, at);
  __ ld(scratch_value, MemOperand(address));

  __ Branch(&holecheck_passed1, ne, scratch_value,
            Operand(masm->isolate()->factory()->the_hole_value()));
  __ JumpIfDictionaryInPrototypeChain(receiver, elements_map, scratch_value,
                                      slow);

  __ bind(&holecheck_passed1);

  // Smi stores don't require further checks.
  Label non_smi_value;
  __ JumpIfNotSmi(value, &non_smi_value);

  if (increment_length == kIncrementLength) {
    // Add 1 to receiver->length.
    __ Daddu(scratch_value, key, Operand(Smi::FromInt(1)));
    __ sd(scratch_value, FieldMemOperand(receiver, JSArray::kLengthOffset));
  }
  // It's irrelevant whether array is smi-only or not when writing a smi.
  __ Daddu(address, elements,
           Operand(FixedArray::kHeaderSize - kHeapObjectTag));
  __ SmiScale(scratch_value, key, kPointerSizeLog2);
  __ Daddu(address, address, scratch_value);
  __ sd(value, MemOperand(address));
  __ Ret();

  __ bind(&non_smi_value);
  // Escape to elements kind transition case.
  __ CheckFastObjectElements(receiver_map, scratch_value,
                             &transition_smi_elements);

  // Fast elements array, store the value to the elements backing store.
  __ bind(&finish_object_store);
  if (increment_length == kIncrementLength) {
    // Add 1 to receiver->length.
    __ Daddu(scratch_value, key, Operand(Smi::FromInt(1)));
    __ sd(scratch_value, FieldMemOperand(receiver, JSArray::kLengthOffset));
  }
  __ Daddu(address, elements,
           Operand(FixedArray::kHeaderSize - kHeapObjectTag));
  __ SmiScale(scratch_value, key, kPointerSizeLog2);
  __ Daddu(address, address, scratch_value);
  __ sd(value, MemOperand(address));
  // Update write barrier for the elements array address.
  __ mov(scratch_value, value);  // Preserve the value which is returned.
  __ RecordWrite(elements, address, scratch_value, kRAHasNotBeenSaved,
                 kDontSaveFPRegs, EMIT_REMEMBERED_SET, OMIT_SMI_CHECK);
  __ Ret();

  __ bind(fast_double);
  if (check_map == kCheckMap) {
    // Check for fast double array case. If this fails, call through to the
    // runtime.
    __ LoadRoot(at, Heap::kFixedDoubleArrayMapRootIndex);
    __ Branch(slow, ne, elements_map, Operand(at));
  }

  // HOLECHECK: guards "A[i] double hole?"
  // We have to see if the double version of the hole is present. If so
  // go to the runtime.
  __ Daddu(address, elements,
           Operand(FixedDoubleArray::kHeaderSize + sizeof(kHoleNanLower32) -
                   kHeapObjectTag));
  __ SmiScale(at, key, kPointerSizeLog2);
  __ daddu(address, address, at);
  __ lw(scratch_value, MemOperand(address));
  __ Branch(&fast_double_without_map_check, ne, scratch_value,
            Operand(static_cast<int32_t>(kHoleNanUpper32)));
  __ JumpIfDictionaryInPrototypeChain(receiver, elements_map, scratch_value,
                                      slow);

  __ bind(&fast_double_without_map_check);
  __ StoreNumberToDoubleElements(value, key,
                                 elements,  // Overwritten.
                                 a3,        // Scratch regs...
                                 a4, &transition_double_elements);
  if (increment_length == kIncrementLength) {
    // Add 1 to receiver->length.
    __ Daddu(scratch_value, key, Operand(Smi::FromInt(1)));
    __ sd(scratch_value, FieldMemOperand(receiver, JSArray::kLengthOffset));
  }
  __ Ret();

  __ bind(&transition_smi_elements);
  // Transition the array appropriately depending on the value type.
  __ ld(a4, FieldMemOperand(value, HeapObject::kMapOffset));
  __ LoadRoot(at, Heap::kHeapNumberMapRootIndex);
  __ Branch(&non_double_value, ne, a4, Operand(at));

  // Value is a double. Transition FAST_SMI_ELEMENTS ->
  // FAST_DOUBLE_ELEMENTS and complete the store.
  __ LoadTransitionedArrayMapConditional(
      FAST_SMI_ELEMENTS, FAST_DOUBLE_ELEMENTS, receiver_map, a4, slow);
  AllocationSiteMode mode =
      AllocationSite::GetMode(FAST_SMI_ELEMENTS, FAST_DOUBLE_ELEMENTS);
  ElementsTransitionGenerator::GenerateSmiToDouble(masm, receiver, key, value,
                                                   receiver_map, mode, slow);
  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));
  __ jmp(&fast_double_without_map_check);

  __ bind(&non_double_value);
  // Value is not a double, FAST_SMI_ELEMENTS -> FAST_ELEMENTS
  __ LoadTransitionedArrayMapConditional(FAST_SMI_ELEMENTS, FAST_ELEMENTS,
                                         receiver_map, a4, slow);
  mode = AllocationSite::GetMode(FAST_SMI_ELEMENTS, FAST_ELEMENTS);
  ElementsTransitionGenerator::GenerateMapChangeElementsTransition(
      masm, receiver, key, value, receiver_map, mode, slow);
  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));
  __ jmp(&finish_object_store);

  __ bind(&transition_double_elements);
  // Elements are FAST_DOUBLE_ELEMENTS, but value is an Object that's not a
  // HeapNumber. Make sure that the receiver is a Array with FAST_ELEMENTS and
  // transition array from FAST_DOUBLE_ELEMENTS to FAST_ELEMENTS
  __ LoadTransitionedArrayMapConditional(FAST_DOUBLE_ELEMENTS, FAST_ELEMENTS,
                                         receiver_map, a4, slow);
  mode = AllocationSite::GetMode(FAST_DOUBLE_ELEMENTS, FAST_ELEMENTS);
  ElementsTransitionGenerator::GenerateDoubleToObject(
      masm, receiver, key, value, receiver_map, mode, slow);
  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));
  __ jmp(&finish_object_store);
}


void KeyedStoreIC::GenerateMegamorphic(MacroAssembler* masm,
                                       LanguageMode language_mode) {
  // ---------- S t a t e --------------
  //  -- a0     : value
  //  -- a1     : key
  //  -- a2     : receiver
  //  -- ra     : return address
  // -----------------------------------
  Label slow, fast_object, fast_object_grow;
  Label fast_double, fast_double_grow;
  Label array, extra, check_if_double_array, maybe_name_key, miss;

  // Register usage.
  Register value = StoreDescriptor::ValueRegister();
  Register key = StoreDescriptor::NameRegister();
  Register receiver = StoreDescriptor::ReceiverRegister();
  DCHECK(value.is(a0));
  Register receiver_map = a3;
  Register elements_map = a6;
  Register elements = a7;  // Elements array of the receiver.
  // a4 and a5 are used as general scratch registers.

  // Check that the key is a smi.
  __ JumpIfNotSmi(key, &maybe_name_key);
  // Check that the object isn't a smi.
  __ JumpIfSmi(receiver, &slow);
  // Get the map of the object.
  __ ld(receiver_map, FieldMemOperand(receiver, HeapObject::kMapOffset));
  // Check that the receiver does not require access checks and is not observed.
  // The generic stub does not perform map checks or handle observed objects.
  __ lbu(a4, FieldMemOperand(receiver_map, Map::kBitFieldOffset));
  __ And(a4, a4,
         Operand(1 << Map::kIsAccessCheckNeeded | 1 << Map::kIsObserved));
  __ Branch(&slow, ne, a4, Operand(zero_reg));
  // Check if the object is a JS array or not.
  __ lbu(a4, FieldMemOperand(receiver_map, Map::kInstanceTypeOffset));
  __ Branch(&array, eq, a4, Operand(JS_ARRAY_TYPE));
  // Check that the object is some kind of JSObject.
  __ Branch(&slow, lt, a4, Operand(FIRST_JS_OBJECT_TYPE));

  // Object case: Check key against length in the elements array.
  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));
  // Check array bounds. Both the key and the length of FixedArray are smis.
  __ ld(a4, FieldMemOperand(elements, FixedArray::kLengthOffset));
  __ Branch(&fast_object, lo, key, Operand(a4));

  // Slow case, handle jump to runtime.
  __ bind(&slow);
  // Entry registers are intact.
  // a0: value.
  // a1: key.
  // a2: receiver.
  PropertyICCompiler::GenerateRuntimeSetProperty(masm, language_mode);
  // Never returns to here.

  __ bind(&maybe_name_key);
  __ ld(a4, FieldMemOperand(key, HeapObject::kMapOffset));
  __ lb(a4, FieldMemOperand(a4, Map::kInstanceTypeOffset));
  __ JumpIfNotUniqueNameInstanceType(a4, &slow);

  if (FLAG_vector_stores) {
    // The handlers in the stub cache expect a vector and slot. Since we won't
    // change the IC from any downstream misses, a dummy vector can be used.
    Register vector = LoadWithVectorDescriptor::VectorRegister();
    Register slot = LoadWithVectorDescriptor::SlotRegister();
    DCHECK(!AreAliased(vector, slot, a3, a4, a5, a6));
    Handle<TypeFeedbackVector> dummy_vector = Handle<TypeFeedbackVector>::cast(
        masm->isolate()->factory()->keyed_store_dummy_vector());
    int slot_index = dummy_vector->GetIndex(FeedbackVectorICSlot(0));
    __ LoadRoot(vector, Heap::kKeyedStoreDummyVectorRootIndex);
    __ li(slot, Operand(Smi::FromInt(slot_index)));
  }

  Code::Flags flags = Code::RemoveTypeAndHolderFromFlags(
      Code::ComputeHandlerFlags(Code::STORE_IC));
  masm->isolate()->stub_cache()->GenerateProbe(
      masm, Code::STORE_IC, flags, false, receiver, key, a3, a4, a5, a6);
  // Cache miss.
  __ Branch(&miss);

  // Extra capacity case: Check if there is extra capacity to
  // perform the store and update the length. Used for adding one
  // element to the array by writing to array[array.length].
  __ bind(&extra);
  // Condition code from comparing key and array length is still available.
  // Only support writing to array[array.length].
  __ Branch(&slow, ne, key, Operand(a4));
  // Check for room in the elements backing store.
  // Both the key and the length of FixedArray are smis.
  __ ld(a4, FieldMemOperand(elements, FixedArray::kLengthOffset));
  __ Branch(&slow, hs, key, Operand(a4));
  __ ld(elements_map, FieldMemOperand(elements, HeapObject::kMapOffset));
  __ Branch(&check_if_double_array, ne, elements_map,
            Heap::kFixedArrayMapRootIndex);

  __ jmp(&fast_object_grow);

  __ bind(&check_if_double_array);
  __ Branch(&slow, ne, elements_map, Heap::kFixedDoubleArrayMapRootIndex);
  __ jmp(&fast_double_grow);

  // Array case: Get the length and the elements array from the JS
  // array. Check that the array is in fast mode (and writable); if it
  // is the length is always a smi.
  __ bind(&array);
  __ ld(elements, FieldMemOperand(receiver, JSObject::kElementsOffset));

  // Check the key against the length in the array.
  __ ld(a4, FieldMemOperand(receiver, JSArray::kLengthOffset));
  __ Branch(&extra, hs, key, Operand(a4));

  KeyedStoreGenerateMegamorphicHelper(
      masm, &fast_object, &fast_double, &slow, kCheckMap, kDontIncrementLength,
      value, key, receiver, receiver_map, elements_map, elements);
  KeyedStoreGenerateMegamorphicHelper(masm, &fast_object_grow,
                                      &fast_double_grow, &slow, kDontCheckMap,
                                      kIncrementLength, value, key, receiver,
                                      receiver_map, elements_map, elements);

  __ bind(&miss);
  GenerateMiss(masm);
}


void KeyedStoreIC::GenerateMiss(MacroAssembler* masm) {
  // Push receiver, key and value for runtime call.
  __ Push(StoreDescriptor::ReceiverRegister(), StoreDescriptor::NameRegister(),
          StoreDescriptor::ValueRegister());

  ExternalReference ref =
      ExternalReference(IC_Utility(kKeyedStoreIC_Miss), masm->isolate());
  __ TailCallExternalReference(ref, 3, 1);
}


void StoreIC::GenerateMegamorphic(MacroAssembler* masm) {
  Register receiver = StoreDescriptor::ReceiverRegister();
  Register name = StoreDescriptor::NameRegister();
  DCHECK(receiver.is(a1));
  DCHECK(name.is(a2));
  DCHECK(StoreDescriptor::ValueRegister().is(a0));

  // Get the receiver from the stack and probe the stub cache.
  Code::Flags flags = Code::RemoveTypeAndHolderFromFlags(
      Code::ComputeHandlerFlags(Code::STORE_IC));
  masm->isolate()->stub_cache()->GenerateProbe(
      masm, Code::STORE_IC, flags, false, receiver, name, a3, a4, a5, a6);

  // Cache miss: Jump to runtime.
  GenerateMiss(masm);
}


void StoreIC::GenerateMiss(MacroAssembler* masm) {
  __ Push(StoreDescriptor::ReceiverRegister(), StoreDescriptor::NameRegister(),
          StoreDescriptor::ValueRegister());
  // Perform tail call to the entry.
  ExternalReference ref =
      ExternalReference(IC_Utility(kStoreIC_Miss), masm->isolate());
  __ TailCallExternalReference(ref, 3, 1);
}


void StoreIC::GenerateNormal(MacroAssembler* masm) {
  Label miss;
  Register receiver = StoreDescriptor::ReceiverRegister();
  Register name = StoreDescriptor::NameRegister();
  Register value = StoreDescriptor::ValueRegister();
  Register dictionary = a3;
  DCHECK(!AreAliased(value, receiver, name, dictionary, a4, a5));

  __ ld(dictionary, FieldMemOperand(receiver, JSObject::kPropertiesOffset));

  GenerateDictionaryStore(masm, &miss, a3, name, value, a4, a5);
  Counters* counters = masm->isolate()->counters();
  __ IncrementCounter(counters->store_normal_hit(), 1, a4, a5);
  __ Ret();

  __ bind(&miss);
  __ IncrementCounter(counters->store_normal_miss(), 1, a4, a5);
  GenerateMiss(masm);
}


#undef __


Condition CompareIC::ComputeCondition(Token::Value op) {
  switch (op) {
    case Token::EQ_STRICT:
    case Token::EQ:
      return eq;
    case Token::LT:
      return lt;
    case Token::GT:
      return gt;
    case Token::LTE:
      return le;
    case Token::GTE:
      return ge;
    default:
      UNREACHABLE();
      return kNoCondition;
  }
}


bool CompareIC::HasInlinedSmiCode(Address address) {
  // The address of the instruction following the call.
  Address andi_instruction_address =
      address + Assembler::kCallTargetAddressOffset;

  // If the instruction following the call is not a andi at, rx, #yyy, nothing
  // was inlined.
  Instr instr = Assembler::instr_at(andi_instruction_address);
  return Assembler::IsAndImmediate(instr) &&
         Assembler::GetRt(instr) == static_cast<uint32_t>(zero_reg.code());
}


void PatchInlinedSmiCode(Address address, InlinedSmiCheck check) {
  Address andi_instruction_address =
      address + Assembler::kCallTargetAddressOffset;

  // If the instruction following the call is not a andi at, rx, #yyy, nothing
  // was inlined.
  Instr instr = Assembler::instr_at(andi_instruction_address);
  if (!(Assembler::IsAndImmediate(instr) &&
        Assembler::GetRt(instr) == static_cast<uint32_t>(zero_reg.code()))) {
    return;
  }

  // The delta to the start of the map check instruction and the
  // condition code uses at the patched jump.
  int delta = Assembler::GetImmediate16(instr);
  delta += Assembler::GetRs(instr) * kImm16Mask;
  // If the delta is 0 the instruction is andi at, zero_reg, #0 which also
  // signals that nothing was inlined.
  if (delta == 0) {
    return;
  }

  if (FLAG_trace_ic) {
    PrintF("[  patching ic at %p, andi=%p, delta=%d\n", address,
           andi_instruction_address, delta);
  }

  Address patch_address =
      andi_instruction_address - delta * Instruction::kInstrSize;
  Instr instr_at_patch = Assembler::instr_at(patch_address);
  Instr branch_instr =
      Assembler::instr_at(patch_address + Instruction::kInstrSize);
  // This is patching a conditional "jump if not smi/jump if smi" site.
  // Enabling by changing from
  //   andi at, rx, 0
  //   Branch <target>, eq, at, Operand(zero_reg)
  // to:
  //   andi at, rx, #kSmiTagMask
  //   Branch <target>, ne, at, Operand(zero_reg)
  // and vice-versa to be disabled again.
  CodePatcher patcher(patch_address, 2);
  Register reg = Register::from_code(Assembler::GetRs(instr_at_patch));
  if (check == ENABLE_INLINED_SMI_CHECK) {
    DCHECK(Assembler::IsAndImmediate(instr_at_patch));
    DCHECK_EQ(0u, Assembler::GetImmediate16(instr_at_patch));
    patcher.masm()->andi(at, reg, kSmiTagMask);
  } else {
    DCHECK_EQ(check, DISABLE_INLINED_SMI_CHECK);
    DCHECK(Assembler::IsAndImmediate(instr_at_patch));
    patcher.masm()->andi(at, reg, 0);
  }
  DCHECK(Assembler::IsBranch(branch_instr));
  if (Assembler::IsBeq(branch_instr)) {
    patcher.ChangeBranchCondition(ne);
  } else {
    DCHECK(Assembler::IsBne(branch_instr));
    patcher.ChangeBranchCondition(eq);
  }
}
}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_MIPS64
