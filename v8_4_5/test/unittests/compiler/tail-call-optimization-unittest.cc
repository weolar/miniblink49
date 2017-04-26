// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/linkage.h"
#include "src/compiler/tail-call-optimization.h"
#include "test/unittests/compiler/graph-unittest.h"
#include "test/unittests/compiler/node-test-utils.h"

namespace v8 {
namespace internal {
namespace compiler {

class TailCallOptimizationTest : public GraphTest {
 public:
  explicit TailCallOptimizationTest(int num_parameters = 1)
      : GraphTest(num_parameters) {}
  ~TailCallOptimizationTest() override {}

 protected:
  Reduction Reduce(Node* node) {
    TailCallOptimization tco(common(), graph());
    return tco.Reduce(node);
  }
};


TEST_F(TailCallOptimizationTest, CallCodeObject0) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallCodeObject, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kNoFlags);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Reduction r = Reduce(ret);
  ASSERT_FALSE(r.Changed());
}


TEST_F(TailCallOptimizationTest, CallCodeObject1) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallCodeObject, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kSupportsTailCalls);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* if_exception = graph()->NewNode(
      common()->IfException(IfExceptionHint::kLocallyUncaught), call, call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Node* end = graph()->NewNode(common()->End(1), if_exception);
  graph()->SetEnd(end);
  Reduction r = Reduce(ret);
  ASSERT_FALSE(r.Changed());
}


TEST_F(TailCallOptimizationTest, CallCodeObject2) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallCodeObject, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kSupportsTailCalls);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Reduction r = Reduce(ret);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsTailCall(kCallDescriptor, p0, p1,
                                          graph()->start(), graph()->start()));
}


TEST_F(TailCallOptimizationTest, CallJSFunction0) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallJSFunction, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kNoFlags);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Reduction r = Reduce(ret);
  ASSERT_FALSE(r.Changed());
}


TEST_F(TailCallOptimizationTest, CallJSFunction1) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallJSFunction, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kSupportsTailCalls);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* if_exception = graph()->NewNode(
      common()->IfException(IfExceptionHint::kLocallyUncaught), call, call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Node* end = graph()->NewNode(common()->End(1), if_exception);
  graph()->SetEnd(end);
  Reduction r = Reduce(ret);
  ASSERT_FALSE(r.Changed());
}


TEST_F(TailCallOptimizationTest, CallJSFunction2) {
  MachineType kMachineSignature[] = {kMachAnyTagged, kMachAnyTagged};
  LinkageLocation kLocationSignature[] = {LinkageLocation(0),
                                          LinkageLocation(1)};
  const CallDescriptor* kCallDescriptor = new (zone()) CallDescriptor(
      CallDescriptor::kCallJSFunction, kMachAnyTagged, LinkageLocation(0),
      new (zone()) MachineSignature(1, 1, kMachineSignature),
      new (zone()) LocationSignature(1, 1, kLocationSignature), 0,
      Operator::kNoProperties, 0, 0, CallDescriptor::kSupportsTailCalls);
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* call = graph()->NewNode(common()->Call(kCallDescriptor), p0, p1,
                                graph()->start(), graph()->start());
  Node* if_success = graph()->NewNode(common()->IfSuccess(), call);
  Node* ret = graph()->NewNode(common()->Return(), call, call, if_success);
  Reduction r = Reduce(ret);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsTailCall(kCallDescriptor, p0, p1,
                                          graph()->start(), graph()->start()));
}


}  // namespace compiler
}  // namespace internal
}  // namespace v8
