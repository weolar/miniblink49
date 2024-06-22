#ifndef V8_TORQUE_PROXY_FROM_DSL_BASE_H__
#define V8_TORQUE_PROXY_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ProxyBuiltinsFromDSLAssembler {
 public:
  explicit ProxyBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<JSProxy> AllocateProxy(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_target, compiler::TNode<JSReceiver> p_handler);
  compiler::TNode<BoolT> IsRevokedProxy(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o);
  compiler::TNode<JSFunction> AllocateProxyRevokeFunction(compiler::TNode<Context> p_context, compiler::TNode<JSProxy> p_proxy);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_PROXY_FROM_DSL_BASE_H__
