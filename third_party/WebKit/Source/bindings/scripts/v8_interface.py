# Copyright (C) 2013 Google Inc. All rights reserved.
# coding=utf-8
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

"""Generate template values for an interface.

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

from collections import defaultdict
import itertools
from operator import itemgetter

import idl_definitions
from idl_definitions import IdlOperation, IdlArgument
import idl_types
from idl_types import IdlType, inherits_interface
import v8_attributes
from v8_globals import includes
import v8_methods
import v8_types
from v8_types import cpp_ptr_type, cpp_template_type
import v8_utilities
from v8_utilities import (cpp_name_or_partial, capitalize, conditional_string, cpp_name, gc_type,
                          has_extended_attribute_value, runtime_enabled_function_name,
                          extended_attribute_value_as_list, is_legacy_interface_type_checking)


INTERFACE_H_INCLUDES = frozenset([
    'bindings/core/v8/ScriptWrappable.h',
    'bindings/core/v8/ToV8.h',
    'bindings/core/v8/V8Binding.h',
    'bindings/core/v8/V8DOMWrapper.h',
    'bindings/core/v8/WrapperTypeInfo.h',
    'platform/heap/Handle.h',
])
INTERFACE_CPP_INCLUDES = frozenset([
    'bindings/core/v8/ExceptionState.h',
    'bindings/core/v8/V8DOMConfiguration.h',
    'bindings/core/v8/V8ObjectConstructor.h',
    'core/dom/ContextFeatures.h',
    'core/dom/Document.h',
    'platform/RuntimeEnabledFeatures.h',
    'platform/TraceEvent.h',
    'wtf/GetPtr.h',
    'wtf/RefPtr.h',
])


def interface_context(interface):
    includes.clear()
    includes.update(INTERFACE_CPP_INCLUDES)
    header_includes = set(INTERFACE_H_INCLUDES)

    if interface.is_partial:
        # A partial interface definition cannot specify that the interface
        # inherits from another interface. Inheritance must be specified on
        # the original interface definition.
        parent_interface = None
        is_event_target = False
        # partial interface needs the definition of its original interface.
        includes.add('bindings/core/v8/V8%s.h' % interface.name)
    else:
        parent_interface = interface.parent
        if parent_interface:
            header_includes.update(v8_types.includes_for_interface(parent_interface))
        is_event_target = inherits_interface(interface.name, 'EventTarget')

    extended_attributes = interface.extended_attributes

    is_array_buffer_or_view = interface.idl_type.is_array_buffer_or_view
    is_typed_array_type = interface.idl_type.is_typed_array
    if is_array_buffer_or_view:
        includes.add('bindings/core/v8/V8ArrayBuffer.h')
    if interface.name == 'ArrayBufferView':
        includes.update((
            'bindings/core/v8/V8Int8Array.h',
            'bindings/core/v8/V8Int16Array.h',
            'bindings/core/v8/V8Int32Array.h',
            'bindings/core/v8/V8Uint8Array.h',
            'bindings/core/v8/V8Uint8ClampedArray.h',
            'bindings/core/v8/V8Uint16Array.h',
            'bindings/core/v8/V8Uint32Array.h',
            'bindings/core/v8/V8Float32Array.h',
            'bindings/core/v8/V8Float64Array.h',
            'bindings/core/v8/V8DataView.h'))

    # [ActiveDOMObject]
    is_active_dom_object = 'ActiveDOMObject' in extended_attributes

    # [CheckSecurity]
    is_check_security = 'CheckSecurity' in extended_attributes
    if is_check_security:
        includes.add('bindings/core/v8/BindingSecurity.h')

    # [DependentLifetime]
    is_dependent_lifetime = 'DependentLifetime' in extended_attributes

    # [MeasureAs]
    is_measure_as = 'MeasureAs' in extended_attributes
    if is_measure_as:
        includes.add('core/frame/UseCounter.h')

    # [SetWrapperReferenceFrom]
    set_wrapper_reference_from = extended_attributes.get('SetWrapperReferenceFrom')
    if set_wrapper_reference_from:
        includes.update(['bindings/core/v8/V8GCController.h',
                         'core/dom/Element.h'])

    # [SetWrapperReferenceTo]
    set_wrapper_reference_to_argument = extended_attributes.get('SetWrapperReferenceTo')
    set_wrapper_reference_to = None
    if set_wrapper_reference_to_argument:
        set_wrapper_reference_to = {
            'name': set_wrapper_reference_to_argument.name,
            # FIXME: properly should be:
            # 'cpp_type': set_wrapper_reference_to_argument.idl_type.cpp_type_args(raw_type=True),
            # (if type is non-wrapper type like NodeFilter, normally RefPtr)
            # Raw pointers faster though, and NodeFilter hacky anyway.
            'cpp_type': set_wrapper_reference_to_argument.idl_type.implemented_as + '*',
            'idl_type': set_wrapper_reference_to_argument.idl_type,
            'v8_type': v8_types.v8_type(set_wrapper_reference_to_argument.idl_type.name),
        }
        set_wrapper_reference_to['idl_type'].add_includes_for_type()

    # [SetWrapperReferenceFrom]
    has_visit_dom_wrapper = (
        has_extended_attribute_value(interface, 'Custom', 'VisitDOMWrapper') or
        set_wrapper_reference_from or set_wrapper_reference_to)

    this_gc_type = gc_type(interface)

    wrapper_class_id = ('NodeClassId' if inherits_interface(interface.name, 'Node') else 'ObjectClassId')

    v8_class_name = v8_utilities.v8_class_name(interface)
    cpp_class_name = cpp_name(interface)
    cpp_class_name_or_partial = cpp_name_or_partial(interface)
    v8_class_name_or_partial = v8_utilities.v8_class_name_or_partial(interface)

    context = {
        'conditional_string': conditional_string(interface),  # [Conditional]
        'cpp_class': cpp_class_name,
        'cpp_class_or_partial': cpp_class_name_or_partial,
        'event_target_inheritance': 'InheritFromEventTarget' if is_event_target else 'NotInheritFromEventTarget',
        'gc_type': this_gc_type,
        # FIXME: Remove 'EventTarget' special handling, http://crbug.com/383699
        'has_access_check_callbacks': (is_check_security and
                                       interface.name != 'Window' and
                                       interface.name != 'EventTarget'),
        'has_custom_legacy_call_as_function': has_extended_attribute_value(interface, 'Custom', 'LegacyCallAsFunction'),  # [Custom=LegacyCallAsFunction]
        'has_partial_interface': len(interface.partial_interfaces) > 0,
        'has_visit_dom_wrapper': has_visit_dom_wrapper,
        'header_includes': header_includes,
        'interface_name': interface.name,
        'is_active_dom_object': is_active_dom_object,
        'is_array_buffer_or_view': is_array_buffer_or_view,
        'is_check_security': is_check_security,
        'is_event_target': is_event_target,
        'is_exception': interface.is_exception,
        'is_node': inherits_interface(interface.name, 'Node'),
        'is_partial': interface.is_partial,
        'is_typed_array_type': is_typed_array_type,
        'lifetime': 'Dependent'
            if (has_visit_dom_wrapper or
                is_active_dom_object or
                is_dependent_lifetime)
            else 'Independent',
        'measure_as': v8_utilities.measure_as(interface, None),  # [MeasureAs]
        'parent_interface': parent_interface,
        'pass_cpp_type': cpp_template_type(
            cpp_ptr_type('PassRefPtr', 'RawPtr', this_gc_type),
            cpp_name(interface)),
        'runtime_enabled_function': runtime_enabled_function_name(interface),  # [RuntimeEnabled]
        'set_wrapper_reference_from': set_wrapper_reference_from,
        'set_wrapper_reference_to': set_wrapper_reference_to,
        'v8_class': v8_class_name,
        'v8_class_or_partial': v8_class_name_or_partial,
        'wrapper_class_id': wrapper_class_id,
    }

    # Constructors
    constructors = [constructor_context(interface, constructor)
                    for constructor in interface.constructors
                    # FIXME: shouldn't put named constructors with constructors
                    # (currently needed for Perl compatibility)
                    # Handle named constructors separately
                    if constructor.name == 'Constructor']
    if len(constructors) > 1:
        context['constructor_overloads'] = overloads_context(interface, constructors)

    # [CustomConstructor]
    custom_constructors = [{  # Only needed for computing interface length
        'number_of_required_arguments':
            number_of_required_arguments(constructor),
    } for constructor in interface.custom_constructors]

    # [NamedConstructor]
    named_constructor = named_constructor_context(interface)

    if constructors or custom_constructors or named_constructor:
        if interface.is_partial:
            raise Exception('[Constructor] and [NamedConstructor] MUST NOT be'
                            ' specified on partial interface definitions:'
                            '%s' % interface.name)

        includes.add('bindings/core/v8/V8ObjectConstructor.h')
        includes.add('core/frame/LocalDOMWindow.h')

    # [Unscopeable] attributes and methods
    unscopeables = []
    for attribute in interface.attributes:
        if 'Unscopeable' in attribute.extended_attributes:
            unscopeables.append((attribute.name, v8_utilities.runtime_enabled_function_name(attribute)))
    for method in interface.operations:
        if 'Unscopeable' in method.extended_attributes:
            unscopeables.append((method.name, v8_utilities.runtime_enabled_function_name(method)))

    context.update({
        'constructors': constructors,
        'has_custom_constructor': bool(custom_constructors),
        'interface_length':
            interface_length(interface, constructors + custom_constructors),
        'is_constructor_raises_exception': extended_attributes.get('RaisesException') == 'Constructor',  # [RaisesException=Constructor]
        'named_constructor': named_constructor,
        'unscopeables': sorted(unscopeables),
    })

    constants = [constant_context(constant, interface) for constant in interface.constants]

    special_getter_constants = []
    runtime_enabled_constants = []
    constant_configuration_constants = []

    for constant in constants:
        if constant['measure_as'] or constant['deprecate_as']:
            special_getter_constants.append(constant)
            continue
        if constant['runtime_enabled_function']:
            runtime_enabled_constants.append(constant)
            continue
        constant_configuration_constants.append(constant)

    # Constants
    context.update({
        'constant_configuration_constants': constant_configuration_constants,
        'constants': constants,
        'do_not_check_constants': 'DoNotCheckConstants' in extended_attributes,
        'has_constant_configuration': any(
            not constant['runtime_enabled_function']
            for constant in constants),
        'runtime_enabled_constants': runtime_enabled_constants,
        'special_getter_constants': special_getter_constants,
    })

    # Attributes
    attributes = [v8_attributes.attribute_context(interface, attribute)
                  for attribute in interface.attributes]

    has_conditional_attributes = any(attribute['exposed_test'] for attribute in attributes)
    if has_conditional_attributes and interface.is_partial:
        raise Exception('Conditional attributes between partial interfaces in modules and the original interfaces(%s) in core are not allowed.' % interface.name)

    context.update({
        'attributes': attributes,
        'has_accessor_configuration': any(
            attribute['is_expose_js_accessors'] and
            not (attribute['is_static'] or
                 attribute['runtime_enabled_function']) and
            attribute['should_be_exposed_to_script']
            for attribute in attributes),
        'has_attribute_configuration': any(
             not (attribute['is_expose_js_accessors'] or
                  attribute['is_static'] or
                  attribute['runtime_enabled_function'])
             and attribute['should_be_exposed_to_script']
             for attribute in attributes),
        'has_constructor_attributes': any(attribute['constructor_type'] for attribute in attributes),
        'has_replaceable_attributes': any(attribute['is_replaceable'] for attribute in attributes),
    })

    # Methods
    methods = []
    if interface.original_interface:
        methods.extend([v8_methods.method_context(interface, operation, is_visible=False)
                        for operation in interface.original_interface.operations
                        if operation.name])
    methods.extend([v8_methods.method_context(interface, method)
                    for method in interface.operations
                    if method.name])  # Skip anonymous special operations (methods)
    if interface.partial_interfaces:
        assert len(interface.partial_interfaces) == len(set(interface.partial_interfaces))
        for partial_interface in interface.partial_interfaces:
            methods.extend([v8_methods.method_context(interface, operation, is_visible=False)
                            for operation in partial_interface.operations
                            if operation.name])
    compute_method_overloads_context(interface, methods)

    def generated_method(return_type, name, arguments=None, extended_attributes=None, implemented_as=None):
        operation = IdlOperation(interface.idl_name)
        operation.idl_type = return_type
        operation.name = name
        if arguments:
            operation.arguments = arguments
        if extended_attributes:
            operation.extended_attributes.update(extended_attributes)
        if implemented_as is None:
            implemented_as = name + 'ForBinding'
        operation.extended_attributes['ImplementedAs'] = implemented_as
        return v8_methods.method_context(interface, operation)

    def generated_argument(idl_type, name, is_optional=False, extended_attributes=None):
        argument = IdlArgument(interface.idl_name)
        argument.idl_type = idl_type
        argument.name = name
        argument.is_optional = is_optional
        if extended_attributes:
            argument.extended_attributes.update(extended_attributes)
        return argument

    # [Iterable], iterable<>, maplike<> and setlike<>
    iterator_method = None
    # FIXME: support Iterable in partial interfaces. However, we don't
    # need to support iterator overloads between interface and
    # partial interface definitions.
    # http://heycam.github.io/webidl/#idl-overloading
    if (not interface.is_partial
        and (interface.iterable or interface.maplike or interface.setlike
             or 'Iterable' in extended_attributes)):

        used_extended_attributes = {}

        if interface.iterable:
            used_extended_attributes.update(interface.iterable.extended_attributes)
        elif interface.maplike:
            used_extended_attributes.update(interface.maplike.extended_attributes)
        elif interface.setlike:
            used_extended_attributes.update(interface.setlike.extended_attributes)

        if 'RaisesException' in used_extended_attributes:
            raise ValueError('[RaisesException] is implied for iterable<>/maplike<>/setlike<>')
        if 'CallWith' in used_extended_attributes:
            raise ValueError('[CallWith=ScriptState] is implied for iterable<>/maplike<>/setlike<>')

        used_extended_attributes.update({
            'RaisesException': None,
            'CallWith': 'ScriptState',
        })

        forEach_extended_attributes = used_extended_attributes.copy()
        forEach_extended_attributes.update({
            'CallWith': ['ScriptState', 'ThisValue'],
        })

        def generated_iterator_method(name, implemented_as=None):
            return generated_method(
                return_type=IdlType('Iterator'),
                name=name,
                extended_attributes=used_extended_attributes,
                implemented_as=implemented_as)

        iterator_method = generated_iterator_method('iterator', implemented_as='iterator')

        if interface.iterable or interface.maplike or interface.setlike:
            implicit_methods = [
                generated_iterator_method('keys'),
                generated_iterator_method('values'),
                generated_iterator_method('entries'),

                # void forEach(Function callback, [Default=Undefined] optional any thisArg)
                generated_method(IdlType('void'), 'forEach',
                                 arguments=[generated_argument(IdlType('Function'), 'callback'),
                                            generated_argument(IdlType('any'), 'thisArg',
                                                               is_optional=True,
                                                               extended_attributes={'Default': 'Undefined'})],
                                 extended_attributes=forEach_extended_attributes),
            ]

            if interface.maplike:
                key_argument = generated_argument(interface.maplike.key_type, 'key')
                value_argument = generated_argument(interface.maplike.value_type, 'value')

                implicit_methods.extend([
                    generated_method(IdlType('boolean'), 'has',
                                     arguments=[key_argument],
                                     extended_attributes=used_extended_attributes),
                    generated_method(IdlType('any'), 'get',
                                     arguments=[key_argument],
                                     extended_attributes=used_extended_attributes),
                ])

                if not interface.maplike.is_read_only:
                    implicit_methods.extend([
                        generated_method(IdlType('void'), 'clear',
                                         extended_attributes=used_extended_attributes),
                        generated_method(IdlType('boolean'), 'delete',
                                         arguments=[key_argument],
                                         extended_attributes=used_extended_attributes),
                        generated_method(IdlType(interface.name), 'set',
                                         arguments=[key_argument, value_argument],
                                         extended_attributes=used_extended_attributes),
                    ])

            if interface.setlike:
                value_argument = generated_argument(interface.setlike.value_type, 'value')

                implicit_methods.extend([
                    generated_method(IdlType('boolean'), 'has',
                                     arguments=[value_argument],
                                     extended_attributes=used_extended_attributes),
                ])

                if not interface.setlike.is_read_only:
                    implicit_methods.extend([
                        generated_method(IdlType(interface.name), 'add',
                                         arguments=[value_argument],
                                         extended_attributes=used_extended_attributes),
                        generated_method(IdlType('void'), 'clear',
                                         extended_attributes=used_extended_attributes),
                        generated_method(IdlType('boolean'), 'delete',
                                         arguments=[value_argument],
                                         extended_attributes=used_extended_attributes),
                    ])

            methods_by_name = {}
            for method in methods:
                methods_by_name.setdefault(method['name'], []).append(method)

            for implicit_method in implicit_methods:
                if implicit_method['name'] in methods_by_name:
                    # FIXME: Check that the existing method is compatible.
                    continue
                methods.append(implicit_method)

        # FIXME: maplike<> and setlike<> should also imply the presence of a
        # 'size' attribute.

    # Serializer
    if interface.serializer:
        serializer = interface.serializer
        serializer_ext_attrs = serializer.extended_attributes.copy()
        if serializer.operation:
            return_type = serializer.operation.idl_type
            implemented_as = serializer.operation.name
        else:
            return_type = IdlType('any')
            implemented_as = None
            if 'CallWith' not in serializer_ext_attrs:
                serializer_ext_attrs['CallWith'] = 'ScriptState'
        methods.append(generated_method(
            return_type=return_type,
            name='toJSON',
            extended_attributes=serializer_ext_attrs,
            implemented_as=implemented_as))

    # Stringifier
    if interface.stringifier:
        stringifier = interface.stringifier
        stringifier_ext_attrs = stringifier.extended_attributes.copy()
        if stringifier.attribute:
            implemented_as = stringifier.attribute.name
        elif stringifier.operation:
            implemented_as = stringifier.operation.name
        else:
            implemented_as = 'toString'
        methods.append(generated_method(
            return_type=IdlType('DOMString'),
            name='toString',
            extended_attributes=stringifier_ext_attrs,
            implemented_as=implemented_as))

    conditionally_enabled_methods = []
    custom_registration_methods = []
    method_configuration_methods = []

    for method in methods:
        # Skip all but one method in each set of overloaded methods.
        if 'overload_index' in method and 'overloads' not in method:
            continue

        if 'overloads' in method:
            overloads = method['overloads']
            if not overloads['visible']:
                continue
            # original interface will register instead of partial interface.
            if overloads['has_partial_overloads'] and interface.is_partial:
                continue
            conditionally_exposed_function = overloads['exposed_test_all']
            runtime_enabled_function = overloads['runtime_enabled_function_all']
            has_custom_registration = (overloads['has_custom_registration_all'] or
                                       overloads['runtime_determined_lengths'])
        else:
            if not method['visible']:
                continue
            conditionally_exposed_function = method['exposed_test']
            runtime_enabled_function = method['runtime_enabled_function']
            has_custom_registration = method['has_custom_registration']

        if has_custom_registration:
            custom_registration_methods.append(method)
            continue
        if conditionally_exposed_function:
            conditionally_enabled_methods.append(method)
            continue
        if runtime_enabled_function:
            custom_registration_methods.append(method)
            continue
        if method['should_be_exposed_to_script']:
            method_configuration_methods.append(method)

    for method in methods:
        # The value of the Function object’s “length” property is a Number
        # determined as follows:
        # 1. Let S be the effective overload set for regular operations (if the
        # operation is a regular operation) or for static operations (if the
        # operation is a static operation) with identifier id on interface I and
        # with argument count 0.
        # 2. Return the length of the shortest argument list of the entries in S.
        # FIXME: This calculation doesn't take into account whether runtime
        # enabled overloads are actually enabled, so length may be incorrect.
        # E.g., [RuntimeEnabled=Foo] void f(); void f(long x);
        # should have length 1 if Foo is not enabled, but length 0 if it is.
        method['length'] = (method['overloads']['length'] if 'overloads' in method else
                            method['number_of_required_arguments'])

    context.update({
        'conditionally_enabled_methods': conditionally_enabled_methods,
        'custom_registration_methods': custom_registration_methods,
        'has_origin_safe_method_setter': any(
            method['is_check_security_for_frame'] and not method['is_read_only']
            for method in methods),
        'has_private_script': any(attribute['is_implemented_in_private_script'] for attribute in attributes) or
            any(method['is_implemented_in_private_script'] for method in methods),
        'iterator_method': iterator_method,
        'method_configuration_methods': method_configuration_methods,
        'methods': methods,
    })

    # Conditionally enabled members
    has_conditional_attributes_on_instance = any(
        attribute['exposed_test'] and attribute['on_instance']
        for attribute in attributes)
    has_conditional_attributes_on_prototype = any(
        attribute['exposed_test'] and attribute['on_prototype']
        for attribute in attributes)
    context.update({
        'has_conditional_attributes_on_instance':
            has_conditional_attributes_on_instance,
        'has_conditional_attributes_on_prototype':
            has_conditional_attributes_on_prototype,
    })

    context.update({
        'indexed_property_getter': property_getter(interface.indexed_property_getter, ['index']),
        'indexed_property_setter': property_setter(interface.indexed_property_setter, interface),
        'indexed_property_deleter': property_deleter(interface.indexed_property_deleter),
        'is_override_builtins': 'OverrideBuiltins' in extended_attributes,
        'named_property_getter': property_getter(interface.named_property_getter, ['propertyName']),
        'named_property_setter': property_setter(interface.named_property_setter, interface),
        'named_property_deleter': property_deleter(interface.named_property_deleter),
    })

    return context


# [DeprecateAs], [Reflect], [RuntimeEnabled]
def constant_context(constant, interface):
    extended_attributes = constant.extended_attributes
    return {
        'cpp_class': extended_attributes.get('PartialInterfaceImplementedAs'),
        'deprecate_as': v8_utilities.deprecate_as(constant),  # [DeprecateAs]
        'idl_type': constant.idl_type.name,
        'measure_as': v8_utilities.measure_as(constant, interface),  # [MeasureAs]
        'name': constant.name,
        # FIXME: use 'reflected_name' as correct 'name'
        'reflected_name': extended_attributes.get('Reflect', constant.name),
        'runtime_enabled_function': runtime_enabled_function_name(constant),
        'value': constant.value,
    }


################################################################################
# Overloads
################################################################################

def compute_method_overloads_context(interface, methods):
    # Regular methods
    compute_method_overloads_context_by_type(
        interface, [method for method in methods if not method['is_static']])
    # Static methods
    compute_method_overloads_context_by_type(
        interface, [method for method in methods if method['is_static']])


def compute_method_overloads_context_by_type(interface, methods):
    """Computes |method.overload*| template values.

    Called separately for static and non-static (regular) methods,
    as these are overloaded separately.
    Modifies |method| in place for |method| in |methods|.
    Doesn't change the |methods| list itself (only the values, i.e. individual
    methods), so ok to treat these separately.
    """
    # Add overload information only to overloaded methods, so template code can
    # easily verify if a function is overloaded
    for name, overloads in method_overloads_by_name(methods):
        # Resolution function is generated after last overloaded function;
        # package necessary information into |method.overloads| for that method.
        overloads[-1]['overloads'] = overloads_context(interface, overloads)
        overloads[-1]['overloads']['name'] = name


def method_overloads_by_name(methods):
    """Returns generator of overloaded methods by name: [name, [method]]"""
    # Filter to only methods that are actually overloaded
    method_counts = Counter(method['name'] for method in methods)
    overloaded_method_names = set(name
                                  for name, count in method_counts.iteritems()
                                  if count > 1)
    overloaded_methods = [method for method in methods
                          if method['name'] in overloaded_method_names]

    # Group by name (generally will be defined together, but not necessarily)
    return sort_and_groupby(overloaded_methods, itemgetter('name'))


def overloads_context(interface, overloads):
    """Returns |overloads| template values for a single name.

    Sets |method.overload_index| in place for |method| in |overloads|
    and returns dict of overall overload template values.
    """
    assert len(overloads) > 1  # only apply to overloaded names
    for index, method in enumerate(overloads, 1):
        method['overload_index'] = index

    effective_overloads_by_length = effective_overload_set_by_length(overloads)
    lengths = [length for length, _ in effective_overloads_by_length]
    name = overloads[0].get('name', '<constructor>')

    runtime_determined_lengths = None
    function_length = lengths[0]
    runtime_determined_maxargs = None
    maxarg = lengths[-1]

    # The special case handling below is not needed if all overloads are
    # runtime enabled by the same feature.
    if not common_value(overloads, 'runtime_enabled_function'):
        # Check if all overloads with the shortest acceptable arguments list are
        # runtime enabled, in which case we need to have a runtime determined
        # Function.length.
        shortest_overloads = effective_overloads_by_length[0][1]
        if (all(method.get('runtime_enabled_function')
                for method, _, _ in shortest_overloads)):
            # Generate a list of (length, runtime_enabled_functions) tuples.
            runtime_determined_lengths = []
            for length, effective_overloads in effective_overloads_by_length:
                runtime_enabled_functions = set(
                    method['runtime_enabled_function']
                    for method, _, _ in effective_overloads
                    if method.get('runtime_enabled_function'))
                if not runtime_enabled_functions:
                    # This "length" is unconditionally enabled, so stop here.
                    runtime_determined_lengths.append((length, [None]))
                    break
                runtime_determined_lengths.append(
                    (length, sorted(runtime_enabled_functions)))
            function_length = ('%sV8Internal::%sMethodLength()'
                               % (cpp_name_or_partial(interface), name))

        # Check if all overloads with the longest required arguments list are
        # runtime enabled, in which case we need to have a runtime determined
        # maximum distinguishing argument index.
        longest_overloads = effective_overloads_by_length[-1][1]
        if (not common_value(overloads, 'runtime_enabled_function') and
            all(method.get('runtime_enabled_function')
                for method, _, _ in longest_overloads)):
            # Generate a list of (length, runtime_enabled_functions) tuples.
            runtime_determined_maxargs = []
            for length, effective_overloads in reversed(effective_overloads_by_length):
                runtime_enabled_functions = set(
                    method['runtime_enabled_function']
                    for method, _, _ in effective_overloads
                    if method.get('runtime_enabled_function'))
                if not runtime_enabled_functions:
                    # This "length" is unconditionally enabled, so stop here.
                    runtime_determined_maxargs.append((length, [None]))
                    break
                runtime_determined_maxargs.append(
                    (length, sorted(runtime_enabled_functions)))
            maxarg = ('%sV8Internal::%sMethodMaxArg()'
                      % (cpp_name_or_partial(interface), name))

    # Check and fail if overloads disagree on any of the extended attributes
    # that affect how the method should be registered.
    # Skip the check for overloaded constructors, since they don't support any
    # of the extended attributes in question.
    if not overloads[0].get('is_constructor'):
        overload_extended_attributes = [
            method['custom_registration_extended_attributes']
            for method in overloads]
        for extended_attribute in v8_methods.CUSTOM_REGISTRATION_EXTENDED_ATTRIBUTES:
            if common_key(overload_extended_attributes, extended_attribute) is None:
                raise ValueError('Overloads of %s have conflicting extended attribute %s'
                                 % (name, extended_attribute))

    # Check and fail if overloads disagree about whether the return type
    # is a Promise or not.
    promise_overload_count = sum(1 for method in overloads if method.get('returns_promise'))
    if promise_overload_count not in (0, len(overloads)):
        raise ValueError('Overloads of %s have conflicting Promise/non-Promise types'
                         % (name))

    has_overload_visible = False
    has_overload_not_visible = False
    for overload in overloads:
        if overload.get('visible', True):
            # If there exists an overload which is visible, need to generate
            # overload_resolution, i.e. overlods_visible should be True.
            has_overload_visible = True
        else:
            has_overload_not_visible = True

    # If some overloads are not visible and others are visible,
    # the method is overloaded between core and modules.
    has_partial_overloads = has_overload_visible and has_overload_not_visible

    return {
        'deprecate_all_as': common_value(overloads, 'deprecate_as'),  # [DeprecateAs]
        'exposed_test_all': common_value(overloads, 'exposed_test'),  # [Exposed]
        'has_custom_registration_all': common_value(overloads, 'has_custom_registration'),
        'length': function_length,
        'length_tests_methods': length_tests_methods(effective_overloads_by_length),
        # 1. Let maxarg be the length of the longest type list of the
        # entries in S.
        'maxarg': maxarg,
        'measure_all_as': common_value(overloads, 'measure_as'),  # [MeasureAs]
        'returns_promise_all': promise_overload_count > 0,
        'runtime_determined_lengths': runtime_determined_lengths,
        'runtime_determined_maxargs': runtime_determined_maxargs,
        'runtime_enabled_function_all': common_value(overloads, 'runtime_enabled_function'),  # [RuntimeEnabled]
        'valid_arities': lengths
            # Only need to report valid arities if there is a gap in the
            # sequence of possible lengths, otherwise invalid length means
            # "not enough arguments".
            if lengths[-1] - lengths[0] != len(lengths) - 1 else None,
        'visible': has_overload_visible,
        'has_partial_overloads': has_partial_overloads,
    }


def effective_overload_set(F):
    """Returns the effective overload set of an overloaded function.

    An effective overload set is the set of overloaded functions + signatures
    (type list of arguments, with optional and variadic arguments included or
    not), and is used in the overload resolution algorithm.

    For example, given input [f1(optional long x), f2(DOMString s)], the output
    is informally [f1(), f1(long), f2(DOMString)], and formally
    [(f1, [], []), (f1, [long], [optional]), (f2, [DOMString], [required])].

    Currently the optionality list is a list of |is_optional| booleans (True
    means optional, False means required); to support variadics this needs to
    be tri-valued as required, optional, or variadic.

    Formally:
    An effective overload set represents the allowable invocations for a
    particular operation, constructor (specified with [Constructor] or
    [NamedConstructor]), legacy caller or callback function.

    An additional argument N (argument count) is needed when overloading
    variadics, but we don't use that currently.

    Spec: http://heycam.github.io/webidl/#dfn-effective-overload-set

    Formally the input and output lists are sets, but methods are stored
    internally as dicts, which can't be stored in a set because they are not
    hashable, so we use lists instead.

    Arguments:
        F: list of overloads for a given callable name.

    Returns:
        S: list of tuples of the form (callable, type list, optionality list).
    """
    # Code closely follows the algorithm in the spec, for clarity and
    # correctness, and hence is not very Pythonic.

    # 1. Initialize S to ∅.
    # (We use a list because we can't use a set, as noted above.)
    S = []

    # 2. Let F be a set with elements as follows, according to the kind of
    # effective overload set:
    # (Passed as argument, nothing to do.)

    # 3. & 4. (maxarg, m) are only needed for variadics, not used.

    # 5. For each operation, extended attribute or callback function X in F:
    for X in F:  # X is the "callable", F is the overloads.
        arguments = X['arguments']
        # 1. Let n be the number of arguments X is declared to take.
        n = len(arguments)
        # 2. Let t0..n−1 be a list of types, where ti is the type of X’s
        # argument at index i.
        # (“type list”)
        t = tuple(argument['idl_type_object'] for argument in arguments)
        # 3. Let o0..n−1 be a list of optionality values, where oi is “variadic”
        # if X’s argument at index i is a final, variadic argument, “optional”
        # if the argument is optional, and “required” otherwise.
        # (“optionality list”)
        # (We’re just using a boolean for optional/variadic vs. required.)
        o = tuple(argument['is_optional'] or argument['is_variadic']
                  for argument in arguments)
        # 4. Add to S the tuple <X, t0..n−1, o0..n−1>.
        S.append((X, t, o))
        # 5. If X is declared to be variadic, then:
        # (Not used, so not implemented.)
        # 6. Initialize i to n−1.
        i = n - 1
        # 7. While i ≥ 0:
        # Spec bug (fencepost error); should be “While i > 0:”
        # https://www.w3.org/Bugs/Public/show_bug.cgi?id=25590
        while i > 0:
            # 1. If argument i of X is not optional, then break this loop.
            if not o[i]:
                break
            # 2. Otherwise, add to S the tuple <X, t0..i−1, o0..i−1>.
            S.append((X, t[:i], o[:i]))
            # 3. Set i to i−1.
            i = i - 1
        # 8. If n > 0 and all arguments of X are optional, then add to S the
        # tuple <X, (), ()> (where “()” represents the empty list).
        if n > 0 and all(oi for oi in o):
            S.append((X, [], []))
    # 6. The effective overload set is S.
    return S


def effective_overload_set_by_length(overloads):
    def type_list_length(entry):
        # Entries in the effective overload set are 3-tuples:
        # (callable, type list, optionality list)
        return len(entry[1])

    effective_overloads = effective_overload_set(overloads)
    return list(sort_and_groupby(effective_overloads, type_list_length))


def distinguishing_argument_index(entries):
    """Returns the distinguishing argument index for a sequence of entries.

    Entries are elements of the effective overload set with the same number
    of arguments (formally, same type list length), each a 3-tuple of the form
    (callable, type list, optionality list).

    Spec: http://heycam.github.io/webidl/#dfn-distinguishing-argument-index

    If there is more than one entry in an effective overload set that has a
    given type list length, then for those entries there must be an index i
    such that for each pair of entries the types at index i are
    distinguishable.
    The lowest such index is termed the distinguishing argument index for the
    entries of the effective overload set with the given type list length.
    """
    # Only applicable “If there is more than one entry”
    assert len(entries) > 1
    type_lists = [tuple(idl_type.name for idl_type in entry[1])
                  for entry in entries]
    type_list_length = len(type_lists[0])
    # Only applicable for entries that “[have] a given type list length”
    assert all(len(type_list) == type_list_length for type_list in type_lists)
    name = entries[0][0].get('name', 'Constructor')  # for error reporting

    # The spec defines the distinguishing argument index by conditions it must
    # satisfy, but does not give an algorithm.
    #
    # We compute the distinguishing argument index by first computing the
    # minimum index where not all types are the same, and then checking that
    # all types in this position are distinguishable (and the optionality lists
    # up to this point are identical), since "minimum index where not all types
    # are the same" is a *necessary* condition, and more direct to check than
    # distinguishability.
    types_by_index = (set(types) for types in zip(*type_lists))
    try:
        # “In addition, for each index j, where j is less than the
        #  distinguishing argument index for a given type list length, the types
        #  at index j in all of the entries’ type lists must be the same”
        index = next(i for i, types in enumerate(types_by_index)
                     if len(types) > 1)
    except StopIteration:
        raise ValueError('No distinguishing index found for %s, length %s:\n'
                         'All entries have the same type list:\n'
                         '%s' % (name, type_list_length, type_lists[0]))
    # Check optionality
    # “and the booleans in the corresponding list indicating argument
    #  optionality must be the same.”
    # FIXME: spec typo: optionality value is no longer a boolean
    # https://www.w3.org/Bugs/Public/show_bug.cgi?id=25628
    initial_optionality_lists = set(entry[2][:index] for entry in entries)
    if len(initial_optionality_lists) > 1:
        raise ValueError(
            'Invalid optionality lists for %s, length %s:\n'
            'Optionality lists differ below distinguishing argument index %s:\n'
            '%s'
            % (name, type_list_length, index, set(initial_optionality_lists)))

    # Check distinguishability
    # http://heycam.github.io/webidl/#dfn-distinguishable
    # Use names to check for distinct types, since objects are distinct
    # FIXME: check distinguishability more precisely, for validation
    distinguishing_argument_type_names = [type_list[index]
                                          for type_list in type_lists]
    if (len(set(distinguishing_argument_type_names)) !=
        len(distinguishing_argument_type_names)):
        raise ValueError('Types in distinguishing argument are not distinct:\n'
                         '%s' % distinguishing_argument_type_names)

    return index


def length_tests_methods(effective_overloads_by_length):
    """Returns sorted list of resolution tests and associated methods, by length.

    This builds the main data structure for the overload resolution loop.
    For a given argument length, bindings test argument at distinguishing
    argument index, in order given by spec: if it is compatible with
    (optionality or) type required by an overloaded method, resolve to that
    method.

    Returns:
        [(length, [(test, method)])]
    """
    return [(length, list(resolution_tests_methods(effective_overloads)))
            for length, effective_overloads in effective_overloads_by_length]


def resolution_tests_methods(effective_overloads):
    """Yields resolution test and associated method, in resolution order, for effective overloads of a given length.

    This is the heart of the resolution algorithm.
    http://heycam.github.io/webidl/#dfn-overload-resolution-algorithm

    Note that a given method can be listed multiple times, with different tests!
    This is to handle implicit type conversion.

    Returns:
        [(test, method)]
    """
    methods = [effective_overload[0]
               for effective_overload in effective_overloads]
    if len(methods) == 1:
        # If only one method with a given length, no test needed
        yield 'true', methods[0]
        return

    # 6. If there is more than one entry in S, then set d to be the
    # distinguishing argument index for the entries of S.
    index = distinguishing_argument_index(effective_overloads)
    # (7-9 are for handling |undefined| values for optional arguments before
    # the distinguishing argument (as “missing”), so you can specify only some
    # optional arguments. We don’t support this, so we skip these steps.)
    # 10. If i = d, then:
    # (d is the distinguishing argument index)
    # 1. Let V be argi.
    #     Note: This is the argument that will be used to resolve which
    #           overload is selected.
    cpp_value = 'info[%s]' % index

    # Extract argument and IDL type to simplify accessing these in each loop.
    arguments = [method['arguments'][index] for method in methods]
    arguments_methods = zip(arguments, methods)
    idl_types = [argument['idl_type_object'] for argument in arguments]
    idl_types_methods = zip(idl_types, methods)

    # We can’t do a single loop through all methods or simply sort them, because
    # a method may be listed in multiple steps of the resolution algorithm, and
    # which test to apply differs depending on the step.
    #
    # Instead, we need to go through all methods at each step, either finding
    # first match (if only one test is allowed) or filtering to matches (if
    # multiple tests are allowed), and generating an appropriate tests.

    # 2. If V is undefined, and there is an entry in S whose list of
    # optionality values has “optional” at index i, then remove from S all
    # other entries.
    try:
        method = next(method for argument, method in arguments_methods
                      if argument['is_optional'])
        test = '%s->IsUndefined()' % cpp_value
        yield test, method
    except StopIteration:
        pass

    # 3. Otherwise: if V is null or undefined, and there is an entry in S that
    # has one of the following types at position i of its type list,
    # • a nullable type
    try:
        method = next(method for idl_type, method in idl_types_methods
                      if idl_type.is_nullable)
        test = 'isUndefinedOrNull(%s)' % cpp_value
        yield test, method
    except StopIteration:
        pass

    # 4. Otherwise: if V is a platform object – but not a platform array
    # object – and there is an entry in S that has one of the following
    # types at position i of its type list,
    # • an interface type that V implements
    # (Unlike most of these tests, this can return multiple methods, since we
    #  test if it implements an interface. Thus we need a for loop, not a next.)
    # (We distinguish wrapper types from built-in interface types.)
    for idl_type, method in ((idl_type, method)
                             for idl_type, method in idl_types_methods
                             if idl_type.is_wrapper_type):
        test = 'V8{idl_type}::hasInstance({cpp_value}, info.GetIsolate())'.format(idl_type=idl_type.base_type, cpp_value=cpp_value)
        yield test, method

    # 13. Otherwise: if IsCallable(V) is true, and there is an entry in S that
    # has one of the following types at position i of its type list,
    # • a callback function type
    # ...
    #
    # FIXME:
    # We test for functions rather than callability, which isn't strictly the
    # same thing.
    try:
        method = next(method for idl_type, method in idl_types_methods
                      if idl_type.is_callback_function)
        test = '%s->IsFunction()' % cpp_value
        yield test, method
    except StopIteration:
        pass

    # 14. Otherwise: if V is any kind of object except for a native Date object,
    # a native RegExp object, and there is an entry in S that has one of the
    # following types at position i of its type list,
    # • a sequence type
    # ...
    #
    # 15. Otherwise: if V is any kind of object except for a native Date object,
    # a native RegExp object, and there is an entry in S that has one of the
    # following types at position i of its type list,
    # • an array type
    # ...
    # • a dictionary
    #
    # FIXME:
    # We don't strictly follow the algorithm here. The algorithm says "remove
    # all other entries" if there is "one entry" matching, but we yield all
    # entries to support following constructors:
    # [constructor(sequence<DOMString> arg), constructor(Dictionary arg)]
    # interface I { ... }
    # (Need to check array types before objects because an array is an object)
    for idl_type, method in idl_types_methods:
        if idl_type.native_array_element_type:
            # (We test for Array instead of generic Object to type-check.)
            # FIXME: test for Object during resolution, then have type check for
            # Array in overloaded method: http://crbug.com/262383
            yield '%s->IsArray()' % cpp_value, method
    for idl_type, method in idl_types_methods:
        if idl_type.is_dictionary or idl_type.name == 'Dictionary':
            # FIXME: should be '{1}->IsObject() && !{1}->IsDate() && !{1}->IsRegExp()'.format(cpp_value)
            # FIXME: the IsDate and IsRegExp checks can be skipped if we've
            # already generated tests for them.
            yield '%s->IsObject()' % cpp_value, method

    # (Check for exact type matches before performing automatic type conversion;
    # only needed if distinguishing between primitive types.)
    if len([idl_type.is_primitive_type for idl_type in idl_types]) > 1:
        # (Only needed if match in step 11, otherwise redundant.)
        if any(idl_type.is_string_type or idl_type.is_enum
               for idl_type in idl_types):
            # 10. Otherwise: if V is a Number value, and there is an entry in S
            # that has one of the following types at position i of its type
            # list,
            # • a numeric type
            try:
                method = next(method for idl_type, method in idl_types_methods
                              if idl_type.is_numeric_type)
                test = '%s->IsNumber()' % cpp_value
                yield test, method
            except StopIteration:
                pass

    # (Perform automatic type conversion, in order. If any of these match,
    # that’s the end, and no other tests are needed.) To keep this code simple,
    # we rely on the C++ compiler's dead code elimination to deal with the
    # redundancy if both cases below trigger.

    # 11. Otherwise: if there is an entry in S that has one of the following
    # types at position i of its type list,
    # • DOMString
    # • ByteString
    # • USVString
    # • an enumeration type
    try:
        method = next(method for idl_type, method in idl_types_methods
                      if idl_type.is_string_type or idl_type.is_enum)
        yield 'true', method
    except StopIteration:
        pass

    # 12. Otherwise: if there is an entry in S that has one of the following
    # types at position i of its type list,
    # • a numeric type
    try:
        method = next(method for idl_type, method in idl_types_methods
                      if idl_type.is_numeric_type)
        yield 'true', method
    except StopIteration:
        pass


################################################################################
# Utility functions
################################################################################

def Counter(iterable):
    # Once using Python 2.7, using collections.Counter
    counter = defaultdict(lambda: 0)
    for item in iterable:
        counter[item] += 1
    return counter


def common(dicts, f):
    """Returns common result of f across an iterable of dicts, or None.

    Call f for each dict and return its result if the same across all dicts.
    """
    values = (f(d) for d in dicts)
    first_value = next(values)
    if all(value == first_value for value in values):
        return first_value
    return None


def common_key(dicts, key):
    """Returns common presence of a key across an iterable of dicts, or None.

    True if all dicts have the key, False if none of the dicts have the key,
    and None if some but not all dicts have the key.
    """
    return common(dicts, lambda d: key in d)


def common_value(dicts, key):
    """Returns common value of a key across an iterable of dicts, or None.

    Auxiliary function for overloads, so can consolidate an extended attribute
    that appears with the same value on all items in an overload set.
    """
    return common(dicts, lambda d: d.get(key))


def sort_and_groupby(l, key=None):
    """Returns a generator of (key, list), sorting and grouping list by key."""
    l.sort(key=key)
    return ((k, list(g)) for k, g in itertools.groupby(l, key))


################################################################################
# Constructors
################################################################################

# [Constructor]
def constructor_context(interface, constructor):
    # [RaisesException=Constructor]
    is_constructor_raises_exception = \
        interface.extended_attributes.get('RaisesException') == 'Constructor'

    return {
        'arguments': [v8_methods.argument_context(interface, constructor, argument, index)
                      for index, argument in enumerate(constructor.arguments)],
        'cpp_type': cpp_template_type(
            cpp_ptr_type('RefPtr', 'RawPtr', gc_type(interface)),
            cpp_name(interface)),
        'cpp_value': v8_methods.cpp_value(
            interface, constructor, len(constructor.arguments)),
        'has_exception_state':
            is_constructor_raises_exception or
            any(argument for argument in constructor.arguments
                if argument.idl_type.name == 'SerializedScriptValue' or
                   argument.idl_type.v8_conversion_needs_exception_state),
        'is_call_with_document':
            # [ConstructorCallWith=Document]
            has_extended_attribute_value(interface,
                'ConstructorCallWith', 'Document'),
        'is_call_with_execution_context':
            # [ConstructorCallWith=ExecutionContext]
            has_extended_attribute_value(interface,
                'ConstructorCallWith', 'ExecutionContext'),
        'is_call_with_script_state':
            # [ConstructorCallWith=ScriptState]
            has_extended_attribute_value(
                interface, 'ConstructorCallWith', 'ScriptState'),
        'is_constructor': True,
        'is_named_constructor': False,
        'is_raises_exception': is_constructor_raises_exception,
        'number_of_required_arguments':
            number_of_required_arguments(constructor),
    }


# [NamedConstructor]
def named_constructor_context(interface):
    extended_attributes = interface.extended_attributes
    if 'NamedConstructor' not in extended_attributes:
        return None
    # FIXME: parser should return named constructor separately;
    # included in constructors (and only name stored in extended attribute)
    # for Perl compatibility
    idl_constructor = interface.constructors[-1]
    assert idl_constructor.name == 'NamedConstructor'
    context = constructor_context(interface, idl_constructor)
    context.update({
        'name': extended_attributes['NamedConstructor'],
        'is_named_constructor': True,
    })
    return context


def number_of_required_arguments(constructor):
    return len([argument for argument in constructor.arguments
                if not argument.is_optional])


def interface_length(interface, constructors):
    # Docs: http://heycam.github.io/webidl/#es-interface-call
    if not constructors:
        return 0
    return min(constructor['number_of_required_arguments']
               for constructor in constructors)


################################################################################
# Special operations (methods)
# http://heycam.github.io/webidl/#idl-special-operations
################################################################################

def property_getter(getter, cpp_arguments):
    if not getter:
        return None

    def is_null_expression(idl_type):
        if idl_type.use_output_parameter_for_result:
            return 'result.isNull()'
        if idl_type.is_string_type:
            return 'result.isNull()'
        if idl_type.is_interface_type:
            return '!result'
        if idl_type.base_type in ('any', 'object'):
            return 'result.isEmpty()'
        return ''

    extended_attributes = getter.extended_attributes
    idl_type = getter.idl_type
    idl_type.add_includes_for_type(extended_attributes)
    is_call_with_script_state = v8_utilities.has_extended_attribute_value(getter, 'CallWith', 'ScriptState')
    is_raises_exception = 'RaisesException' in extended_attributes
    use_output_parameter_for_result = idl_type.use_output_parameter_for_result

    # FIXME: make more generic, so can use v8_methods.cpp_value
    cpp_method_name = 'impl->%s' % cpp_name(getter)

    if is_call_with_script_state:
        cpp_arguments.insert(0, 'scriptState')
    if is_raises_exception:
        cpp_arguments.append('exceptionState')
    if use_output_parameter_for_result:
        cpp_arguments.append('result')

    cpp_value = '%s(%s)' % (cpp_method_name, ', '.join(cpp_arguments))

    return {
        'cpp_type': idl_type.cpp_type,
        'cpp_value': cpp_value,
        'do_not_check_security': 'DoNotCheckSecurity' in extended_attributes,
        'is_call_with_script_state': is_call_with_script_state,
        'is_custom':
            'Custom' in extended_attributes and
            (not extended_attributes['Custom'] or
             has_extended_attribute_value(getter, 'Custom', 'PropertyGetter')),
        'is_custom_property_enumerator': has_extended_attribute_value(
            getter, 'Custom', 'PropertyEnumerator'),
        'is_custom_property_query': has_extended_attribute_value(
            getter, 'Custom', 'PropertyQuery'),
        'is_enumerable': 'NotEnumerable' not in extended_attributes,
        'is_null_expression': is_null_expression(idl_type),
        'is_raises_exception': is_raises_exception,
        'name': cpp_name(getter),
        'use_output_parameter_for_result': use_output_parameter_for_result,
        'v8_set_return_value': idl_type.v8_set_return_value('result', extended_attributes=extended_attributes, script_wrappable='impl', release=idl_type.release),
    }


def property_setter(setter, interface):
    if not setter:
        return None

    extended_attributes = setter.extended_attributes
    idl_type = setter.arguments[1].idl_type
    idl_type.add_includes_for_type(extended_attributes)
    is_call_with_script_state = v8_utilities.has_extended_attribute_value(setter, 'CallWith', 'ScriptState')
    is_raises_exception = 'RaisesException' in extended_attributes

    # [TypeChecking=Interface] / [LegacyInterfaceTypeChecking]
    has_type_checking_interface = (
        not is_legacy_interface_type_checking(interface, setter) and
        idl_type.is_wrapper_type)

    return {
        'has_exception_state': (is_raises_exception or
                                idl_type.v8_conversion_needs_exception_state),
        'has_type_checking_interface': has_type_checking_interface,
        'idl_type': idl_type.base_type,
        'is_call_with_script_state': is_call_with_script_state,
        'is_custom': 'Custom' in extended_attributes,
        'is_nullable': idl_type.is_nullable,
        'is_raises_exception': is_raises_exception,
        'name': cpp_name(setter),
        'v8_value_to_local_cpp_value': idl_type.v8_value_to_local_cpp_value(
            extended_attributes, 'v8Value', 'propertyValue'),
    }


def property_deleter(deleter):
    if not deleter:
        return None

    extended_attributes = deleter.extended_attributes
    idl_type = deleter.idl_type
    is_call_with_script_state = v8_utilities.has_extended_attribute_value(deleter, 'CallWith', 'ScriptState')
    return {
        'is_call_with_script_state': is_call_with_script_state,
        'is_custom': 'Custom' in extended_attributes,
        'is_raises_exception': 'RaisesException' in extended_attributes,
        'name': cpp_name(deleter),
    }
