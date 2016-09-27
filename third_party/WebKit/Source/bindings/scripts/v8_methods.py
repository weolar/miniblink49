# Copyright (C) 2013 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""Generate template values for methods.

Extends IdlArgument with property |default_cpp_value|.
Extends IdlTypeBase and IdlUnionType with property |union_arguments|.

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

from idl_definitions import IdlArgument, IdlOperation
from idl_types import IdlTypeBase, IdlUnionType, inherits_interface
from v8_globals import includes
import v8_types
import v8_utilities
from v8_utilities import (has_extended_attribute_value, is_unforgeable,
                          is_legacy_interface_type_checking)


# Methods with any of these require custom method registration code in the
# interface's configure*Template() function.
CUSTOM_REGISTRATION_EXTENDED_ATTRIBUTES = frozenset([
    'DoNotCheckSecurity',
    'DoNotCheckSignature',
    'NotEnumerable',
    'Unforgeable',
])


def use_local_result(method):
    extended_attributes = method.extended_attributes
    idl_type = method.idl_type
    return (has_extended_attribute_value(method, 'CallWith', 'ScriptState') or
            'ImplementedInPrivateScript' in extended_attributes or
            'RaisesException' in extended_attributes or
            idl_type.is_union_type or
            idl_type.is_explicit_nullable)


def method_context(interface, method, is_visible=True):
    arguments = method.arguments
    extended_attributes = method.extended_attributes
    idl_type = method.idl_type
    is_static = method.is_static
    name = method.name

    if is_visible:
        idl_type.add_includes_for_type(extended_attributes)

    this_cpp_value = cpp_value(interface, method, len(arguments))

    def function_template():
        if is_static:
            return 'functionTemplate'
        if is_unforgeable(interface, method):
            return 'instanceTemplate'
        return 'prototypeTemplate'

    is_implemented_in_private_script = 'ImplementedInPrivateScript' in extended_attributes
    if is_implemented_in_private_script:
        includes.add('bindings/core/v8/PrivateScriptRunner.h')
        includes.add('core/frame/LocalFrame.h')
        includes.add('platform/ScriptForbiddenScope.h')

    # [OnlyExposedToPrivateScript]
    is_only_exposed_to_private_script = 'OnlyExposedToPrivateScript' in extended_attributes

    is_call_with_script_arguments = has_extended_attribute_value(method, 'CallWith', 'ScriptArguments')
    if is_call_with_script_arguments:
        includes.update(['bindings/core/v8/ScriptCallStackFactory.h',
                         'core/inspector/ScriptArguments.h'])
    is_call_with_script_state = has_extended_attribute_value(method, 'CallWith', 'ScriptState')
    is_call_with_this_value = has_extended_attribute_value(method, 'CallWith', 'ThisValue')
    if is_call_with_script_state or is_call_with_this_value:
        includes.add('bindings/core/v8/ScriptState.h')
    is_check_security_for_node = 'CheckSecurity' in extended_attributes
    if is_check_security_for_node:
        includes.add('bindings/core/v8/BindingSecurity.h')
    is_custom_element_callbacks = 'CustomElementCallbacks' in extended_attributes
    if is_custom_element_callbacks:
        includes.add('core/dom/custom/CustomElementProcessingStack.h')

    is_do_not_check_security = 'DoNotCheckSecurity' in extended_attributes

    is_check_security_for_frame = (
        has_extended_attribute_value(interface, 'CheckSecurity', 'Frame') and
        not is_do_not_check_security)

    is_check_security_for_window = (
        has_extended_attribute_value(interface, 'CheckSecurity', 'Window') and
        not is_do_not_check_security)

    is_raises_exception = 'RaisesException' in extended_attributes
    is_custom_call_prologue = has_extended_attribute_value(method, 'Custom', 'CallPrologue')
    is_custom_call_epilogue = has_extended_attribute_value(method, 'Custom', 'CallEpilogue')
    is_post_message = 'PostMessage' in extended_attributes
    if is_post_message:
        includes.add('bindings/core/v8/SerializedScriptValueFactory.h')
        includes.add('core/dom/DOMArrayBuffer.h')
        includes.add('core/dom/MessagePort.h')

    if 'LenientThis' in extended_attributes:
        raise Exception('[LenientThis] is not supported for operations.')

    return {
        'activity_logging_world_list': v8_utilities.activity_logging_world_list(method),  # [ActivityLogging]
        'arguments': [argument_context(interface, method, argument, index, is_visible=is_visible)
                      for index, argument in enumerate(arguments)],
        'argument_declarations_for_private_script':
            argument_declarations_for_private_script(interface, method),
        'conditional_string': v8_utilities.conditional_string(method),
        'cpp_type': (v8_types.cpp_template_type('Nullable', idl_type.cpp_type)
                     if idl_type.is_explicit_nullable else idl_type.cpp_type),
        'cpp_value': this_cpp_value,
        'cpp_type_initializer': idl_type.cpp_type_initializer,
        'custom_registration_extended_attributes':
            CUSTOM_REGISTRATION_EXTENDED_ATTRIBUTES.intersection(
                extended_attributes.iterkeys()),
        'deprecate_as': v8_utilities.deprecate_as(method),  # [DeprecateAs]
        'exposed_test': v8_utilities.exposed(method, interface),  # [Exposed]
        'function_template': function_template(),
        'has_custom_registration':
            is_static or
            is_unforgeable(interface, method) or
            v8_utilities.has_extended_attribute(
                method, CUSTOM_REGISTRATION_EXTENDED_ATTRIBUTES),
        'has_exception_state':
            is_raises_exception or
            is_check_security_for_frame or
            is_check_security_for_window or
            any(argument for argument in arguments
                if (argument.idl_type.name == 'SerializedScriptValue' or
                    argument_conversion_needs_exception_state(method, argument))),
        'idl_type': idl_type.base_type,
        'is_call_with_execution_context': has_extended_attribute_value(method, 'CallWith', 'ExecutionContext'),
        'is_call_with_script_arguments': is_call_with_script_arguments,
        'is_call_with_script_state': is_call_with_script_state,
        'is_call_with_this_value': is_call_with_this_value,
        'is_check_security_for_frame': is_check_security_for_frame,
        'is_check_security_for_node': is_check_security_for_node,
        'is_check_security_for_window': is_check_security_for_window,
        'is_custom': 'Custom' in extended_attributes and
            not (is_custom_call_prologue or is_custom_call_epilogue),
        'is_custom_call_prologue': is_custom_call_prologue,
        'is_custom_call_epilogue': is_custom_call_epilogue,
        'is_custom_element_callbacks': is_custom_element_callbacks,
        'is_do_not_check_security': is_do_not_check_security,
        'is_do_not_check_signature': 'DoNotCheckSignature' in extended_attributes,
        'is_explicit_nullable': idl_type.is_explicit_nullable,
        'is_implemented_in_private_script': is_implemented_in_private_script,
        'is_partial_interface_member':
            'PartialInterfaceImplementedAs' in extended_attributes,
        'is_per_world_bindings': 'PerWorldBindings' in extended_attributes,
        'is_post_message': is_post_message,
        'is_raises_exception': is_raises_exception,
        'is_read_only': is_unforgeable(interface, method),
        'is_static': is_static,
        'is_variadic': arguments and arguments[-1].is_variadic,
        'measure_as': v8_utilities.measure_as(method, interface),  # [MeasureAs]
        'name': name,
        'number_of_arguments': len(arguments),
        'number_of_required_arguments': len([
            argument for argument in arguments
            if not (argument.is_optional or argument.is_variadic)]),
        'number_of_required_or_variadic_arguments': len([
            argument for argument in arguments
            if not argument.is_optional]),
        'on_instance': v8_utilities.on_instance(interface, method),
        'on_interface': v8_utilities.on_interface(interface, method),
        'on_prototype': v8_utilities.on_prototype(interface, method),
        'only_exposed_to_private_script': is_only_exposed_to_private_script,
        'private_script_v8_value_to_local_cpp_value': idl_type.v8_value_to_local_cpp_value(
            extended_attributes, 'v8Value', 'cppValue', isolate='scriptState->isolate()', bailout_return_value='false'),
        'property_attributes': property_attributes(interface, method),
        'returns_promise': method.returns_promise,
        'runtime_enabled_function': v8_utilities.runtime_enabled_function_name(method),  # [RuntimeEnabled]
        'should_be_exposed_to_script': not (is_implemented_in_private_script and is_only_exposed_to_private_script),
        'signature': 'v8::Local<v8::Signature>()' if is_static or 'DoNotCheckSignature' in extended_attributes else 'defaultSignature',
        'use_output_parameter_for_result': idl_type.use_output_parameter_for_result,
        'use_local_result': use_local_result(method),
        'v8_set_return_value': v8_set_return_value(interface.name, method, this_cpp_value),
        'v8_set_return_value_for_main_world': v8_set_return_value(interface.name, method, this_cpp_value, for_main_world=True),
        'visible': is_visible,
        'world_suffixes': ['', 'ForMainWorld'] if 'PerWorldBindings' in extended_attributes else [''],  # [PerWorldBindings],
    }


def argument_context(interface, method, argument, index, is_visible=True):
    extended_attributes = argument.extended_attributes
    idl_type = argument.idl_type
    if is_visible:
        idl_type.add_includes_for_type(extended_attributes)
    this_cpp_value = cpp_value(interface, method, index)
    is_variadic_wrapper_type = argument.is_variadic and idl_type.is_wrapper_type

    # [TypeChecking=Interface] / [LegacyInterfaceTypeChecking]
    has_type_checking_interface = (
        not is_legacy_interface_type_checking(interface, method) and
        idl_type.is_wrapper_type)

    if ('ImplementedInPrivateScript' in extended_attributes and
        not idl_type.is_wrapper_type and
        not idl_type.is_basic_type):
        raise Exception('Private scripts supports only primitive types and DOM wrappers.')

    set_default_value = argument.set_default_value
    this_cpp_type = idl_type.cpp_type_args(extended_attributes=extended_attributes,
                                           raw_type=True,
                                           used_as_variadic_argument=argument.is_variadic)
    return {
        'cpp_type': (
            v8_types.cpp_template_type('Nullable', this_cpp_type)
            if idl_type.is_explicit_nullable and not argument.is_variadic
            else this_cpp_type),
        'cpp_value': this_cpp_value,
        # FIXME: check that the default value's type is compatible with the argument's
        'set_default_value': set_default_value,
        'enum_type': idl_type.enum_type,
        'enum_values': idl_type.enum_values,
        'handle': '%sHandle' % argument.name,
        # FIXME: remove once [Default] removed and just use argument.default_value
        'has_default': 'Default' in extended_attributes or set_default_value,
        'has_type_checking_interface': has_type_checking_interface,
        # Dictionary is special-cased, but arrays and sequences shouldn't be
        'idl_type': idl_type.base_type,
        'idl_type_object': idl_type,
        'index': index,
        'is_callback_function': idl_type.is_callback_function,
        'is_callback_interface': idl_type.is_callback_interface,
        # FIXME: Remove generic 'Dictionary' special-casing
        'is_dictionary': idl_type.is_dictionary or idl_type.base_type == 'Dictionary',
        'is_explicit_nullable': idl_type.is_explicit_nullable,
        'is_nullable': idl_type.is_nullable,
        'is_optional': argument.is_optional,
        'is_variadic': argument.is_variadic,
        'is_variadic_wrapper_type': is_variadic_wrapper_type,
        'is_wrapper_type': idl_type.is_wrapper_type,
        'name': argument.name,
        'private_script_cpp_value_to_v8_value': idl_type.cpp_value_to_v8_value(
            argument.name, isolate='scriptState->isolate()',
            creation_context='scriptState->context()->Global()'),
        'use_permissive_dictionary_conversion': 'PermissiveDictionaryConversion' in extended_attributes,
        'v8_set_return_value': v8_set_return_value(interface.name, method, this_cpp_value),
        'v8_set_return_value_for_main_world': v8_set_return_value(interface.name, method, this_cpp_value, for_main_world=True),
        'v8_value_to_local_cpp_value': v8_value_to_local_cpp_value(method, argument, index),
    }


def argument_declarations_for_private_script(interface, method):
    argument_declarations = ['LocalFrame* frame']
    argument_declarations.append('%s* holderImpl' % interface.name)
    argument_declarations.extend(['%s %s' % (argument.idl_type.cpp_type_args(
        used_as_rvalue_type=True), argument.name) for argument in method.arguments])
    if method.idl_type.name != 'void':
        argument_declarations.append('%s* %s' % (method.idl_type.cpp_type, 'result'))
    return argument_declarations


################################################################################
# Value handling
################################################################################

def cpp_value(interface, method, number_of_arguments):
    def cpp_argument(argument):
        idl_type = argument.idl_type
        if idl_type.name == 'EventListener':
            return argument.name
        if (idl_type.name in ['NodeFilter', 'NodeFilterOrNull',
                              'XPathNSResolver', 'XPathNSResolverOrNull']):
            # FIXME: remove this special case
            return '%s.release()' % argument.name
        return argument.name

    # Truncate omitted optional arguments
    arguments = method.arguments[:number_of_arguments]
    cpp_arguments = []
    if 'ImplementedInPrivateScript' in method.extended_attributes:
        cpp_arguments.append('toLocalFrame(toFrameIfNotDetached(info.GetIsolate()->GetCurrentContext()))')
        cpp_arguments.append('impl')

    if method.is_constructor:
        call_with_values = interface.extended_attributes.get('ConstructorCallWith')
    else:
        call_with_values = method.extended_attributes.get('CallWith')
    cpp_arguments.extend(v8_utilities.call_with_arguments(call_with_values))

    # Members of IDL partial interface definitions are implemented in C++ as
    # static member functions, which for instance members (non-static members)
    # take *impl as their first argument
    if ('PartialInterfaceImplementedAs' in method.extended_attributes and
        not 'ImplementedInPrivateScript' in method.extended_attributes and
        not method.is_static):
        cpp_arguments.append('*impl')
    cpp_arguments.extend(cpp_argument(argument) for argument in arguments)

    if 'ImplementedInPrivateScript' in method.extended_attributes:
        if method.idl_type.name != 'void':
            cpp_arguments.append('&result')
    elif ('RaisesException' in method.extended_attributes or
        (method.is_constructor and
         has_extended_attribute_value(interface, 'RaisesException', 'Constructor'))):
        cpp_arguments.append('exceptionState')

    # If a method returns an IDL dictionary or union type, the return value is
    # passed as an argument to impl classes.
    idl_type = method.idl_type
    if idl_type and idl_type.use_output_parameter_for_result:
        cpp_arguments.append('result')

    if method.name == 'Constructor':
        base_name = 'create'
    elif method.name == 'NamedConstructor':
        base_name = 'createForJSConstructor'
    elif 'ImplementedInPrivateScript' in method.extended_attributes:
        base_name = '%sMethod' % method.name
    else:
        base_name = v8_utilities.cpp_name(method)

    cpp_method_name = v8_utilities.scoped_name(interface, method, base_name)
    return '%s(%s)' % (cpp_method_name, ', '.join(cpp_arguments))


def v8_set_return_value(interface_name, method, cpp_value, for_main_world=False):
    idl_type = method.idl_type
    extended_attributes = method.extended_attributes
    if not idl_type or idl_type.name == 'void':
        # Constructors and void methods don't have a return type
        return None

    if ('ImplementedInPrivateScript' in extended_attributes and
        not idl_type.is_wrapper_type and
        not idl_type.is_basic_type):
        raise Exception('Private scripts supports only primitive types and DOM wrappers.')

    release = False
    # [CallWith=ScriptState], [RaisesException]
    if use_local_result(method):
        if idl_type.is_explicit_nullable:
            # result is of type Nullable<T>
            cpp_value = 'result.get()'
        else:
            cpp_value = 'result'
        release = idl_type.release

    script_wrappable = 'impl' if inherits_interface(interface_name, 'Node') else ''
    return idl_type.v8_set_return_value(cpp_value, extended_attributes, script_wrappable=script_wrappable, release=release, for_main_world=for_main_world, is_static=method.is_static)


def v8_value_to_local_cpp_variadic_value(method, argument, index, return_promise):
    assert argument.is_variadic
    idl_type = argument.idl_type
    this_cpp_type = idl_type.cpp_type

    if method.returns_promise:
        check_expression = 'exceptionState.hadException()'
    else:
        check_expression = 'exceptionState.throwIfNeeded()'

    if idl_type.is_dictionary or idl_type.is_union_type:
        vector_type = 'HeapVector'
    else:
        vector_type = 'Vector'

    return {
        'assign_expression': 'toImplArguments<%s<%s>>(info, %s, exceptionState)' % (vector_type, this_cpp_type, index),
        'check_expression': check_expression,
        'cpp_type': this_cpp_type,
        'cpp_name': argument.name,
        'declare_variable': False,
    }


def v8_value_to_local_cpp_value(method, argument, index, return_promise=False, restricted_float=False):
    extended_attributes = argument.extended_attributes
    idl_type = argument.idl_type
    name = argument.name
    if argument.is_variadic:
        return v8_value_to_local_cpp_variadic_value(method, argument, index, return_promise)
    return idl_type.v8_value_to_local_cpp_value(extended_attributes, 'info[%s]' % index,
                                                name, index=index, declare_variable=False,
                                                use_exception_state=method.returns_promise,
                                                restricted_float=restricted_float)


################################################################################
# Auxiliary functions
################################################################################

# [NotEnumerable]
def property_attributes(interface, method):
    extended_attributes = method.extended_attributes
    property_attributes_list = []
    if 'NotEnumerable' in extended_attributes:
        property_attributes_list.append('v8::DontEnum')
    if is_unforgeable(interface, method):
        property_attributes_list.append('v8::ReadOnly')
    if property_attributes_list:
        property_attributes_list.insert(0, 'v8::DontDelete')
    return property_attributes_list


def argument_set_default_value(argument):
    idl_type = argument.idl_type
    default_value = argument.default_value
    if not default_value:
        return None
    if idl_type.is_dictionary:
        if not argument.default_value.is_null:
            raise Exception('invalid default value for dictionary type')
        return None
    if idl_type.is_array_or_sequence_type:
        if default_value.value != '[]':
            raise Exception('invalid default value for sequence type: %s' % default_value.value)
        # Nothing to do when we set an empty sequence as default value, but we
        # need to return non-empty value so that we don't generate method calls
        # without this argument.
        return '/* Nothing to do */'
    if idl_type.is_union_type:
        if argument.default_value.is_null:
            if not idl_type.includes_nullable_type:
                raise Exception('invalid default value for union type: null for %s'
                                % idl_type.name)
            # Union container objects are "null" initially.
            return '/* null default value */'
        if isinstance(default_value.value, basestring):
            member_type = idl_type.string_member_type
        elif isinstance(default_value.value, (int, float)):
            member_type = idl_type.numeric_member_type
        elif isinstance(default_value.value, bool):
            member_type = idl_type.boolean_member_type
        else:
            member_type = None
        if member_type is None:
            raise Exception('invalid default value for union type: %r for %s'
                            % (default_value.value, idl_type.name))
        member_type_name = (member_type.inner_type.name
                            if member_type.is_nullable else
                            member_type.name)
        return '%s.set%s(%s)' % (argument.name, member_type_name,
                                 member_type.literal_cpp_value(default_value))
    return '%s = %s' % (argument.name,
                        idl_type.literal_cpp_value(default_value))

IdlArgument.set_default_value = property(argument_set_default_value)


def method_returns_promise(method):
    return method.idl_type and method.idl_type.name == 'Promise'

IdlOperation.returns_promise = property(method_returns_promise)


def argument_conversion_needs_exception_state(method, argument):
    idl_type = argument.idl_type
    return (idl_type.v8_conversion_needs_exception_state or
            argument.is_variadic or
            (method.returns_promise and idl_type.is_string_type))
