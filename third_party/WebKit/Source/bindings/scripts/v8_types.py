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

"""Functions for type handling and type conversion (Blink/C++ <-> V8/JS).

Extends IdlType and IdlUnionType with V8-specific properties, methods, and
class methods.

Spec:
http://www.w3.org/TR/WebIDL/#es-type-mapping

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

import posixpath

from idl_types import IdlTypeBase, IdlType, IdlUnionType, IdlArrayOrSequenceType, IdlNullableType
import v8_attributes  # for IdlType.constructor_type_name
from v8_globals import includes


################################################################################
# V8-specific handling of IDL types
################################################################################

NON_WRAPPER_TYPES = frozenset([
    'Dictionary',
    'EventHandler',
    'EventListener',
    'NodeFilter',
    'SerializedScriptValue',
])
TYPED_ARRAY_TYPES = frozenset([
    'Float32Array',
    'Float64Array',
    'Int8Array',
    'Int16Array',
    'Int32Array',
    'Uint8Array',
    'Uint8ClampedArray',
    'Uint16Array',
    'Uint32Array',
])
ARRAY_BUFFER_AND_VIEW_TYPES = TYPED_ARRAY_TYPES.union(frozenset([
    'ArrayBuffer',
    'ArrayBufferView',
    'DataView',
]))

IdlType.is_array_buffer_or_view = property(
    lambda self: self.base_type in ARRAY_BUFFER_AND_VIEW_TYPES)

IdlType.is_typed_array = property(
    lambda self: self.base_type in TYPED_ARRAY_TYPES)

IdlType.is_wrapper_type = property(
    lambda self: (self.is_interface_type and
                  not self.is_callback_interface and
                  self.base_type not in NON_WRAPPER_TYPES))


################################################################################
# C++ types
################################################################################

CPP_TYPE_SAME_AS_IDL_TYPE = set([
    'double',
    'float',
    'long long',
    'unsigned long long',
])
CPP_INT_TYPES = set([
    'byte',
    'long',
    'short',
])
CPP_UNSIGNED_TYPES = set([
    'octet',
    'unsigned int',
    'unsigned long',
    'unsigned short',
])
CPP_SPECIAL_CONVERSION_RULES = {
    'Date': 'double',
    'Dictionary': 'Dictionary',
    'EventHandler': 'EventListener*',
    'NodeFilter': 'RefPtrWillBeRawPtr<NodeFilter>',
    'Promise': 'ScriptPromise',
    'ScriptValue': 'ScriptValue',
    # FIXME: Eliminate custom bindings for XPathNSResolver  http://crbug.com/345529
    'XPathNSResolver': 'RawPtr<XPathNSResolver>',
    'boolean': 'bool',
    'unrestricted double': 'double',
    'unrestricted float': 'float',
}


def cpp_type(idl_type, extended_attributes=None, raw_type=False, used_as_rvalue_type=False, used_as_variadic_argument=False, used_in_cpp_sequence=False):
    """Returns C++ type corresponding to IDL type.

    |idl_type| argument is of type IdlType, while return value is a string

    Args:
        idl_type:
            IdlType
        raw_type:
            bool, True if idl_type's raw/primitive C++ type should be returned.
        used_as_rvalue_type:
            bool, True if the C++ type is used as an argument or the return
            type of a method.
        used_as_variadic_argument:
            bool, True if the C++ type is used as a variadic argument of a method.
        used_in_cpp_sequence:
            bool, True if the C++ type is used as an element of a container.
            Containers can be an array, a sequence or a dictionary.
    """
    def string_mode():
        if extended_attributes.get('TreatNullAs') == 'EmptyString':
            return 'TreatNullAsEmptyString'
        if idl_type.is_nullable or extended_attributes.get('TreatNullAs') == 'NullString':
            if extended_attributes.get('TreatUndefinedAs') == 'NullString':
                return 'TreatNullAndUndefinedAsNullString'
            return 'TreatNullAsNullString'
        return ''

    extended_attributes = extended_attributes or {}
    idl_type = idl_type.preprocessed_type

    # Array or sequence types
    if used_as_variadic_argument:
        native_array_element_type = idl_type
    else:
        native_array_element_type = idl_type.native_array_element_type
    if native_array_element_type:
        vector_type = cpp_ptr_type('Vector', 'HeapVector', native_array_element_type.gc_type)
        vector_template_type = cpp_template_type(vector_type, native_array_element_type.cpp_type_args(used_in_cpp_sequence=True))
        if used_as_rvalue_type:
            return 'const %s&' % vector_template_type
        return vector_template_type

    # Simple types
    base_idl_type = idl_type.base_type

    if base_idl_type in CPP_TYPE_SAME_AS_IDL_TYPE:
        return base_idl_type
    if base_idl_type in CPP_INT_TYPES:
        return 'int'
    if base_idl_type in CPP_UNSIGNED_TYPES:
        return 'unsigned'
    if base_idl_type in CPP_SPECIAL_CONVERSION_RULES:
        return CPP_SPECIAL_CONVERSION_RULES[base_idl_type]

    if base_idl_type in NON_WRAPPER_TYPES:
        return ('PassRefPtr<%s>' if used_as_rvalue_type else 'RefPtr<%s>') % base_idl_type
    if idl_type.is_string_type:
        if not raw_type:
            return 'String'
        return 'V8StringResource<%s>' % string_mode()

    if idl_type.is_array_buffer_or_view and raw_type:
        return idl_type.implemented_as + '*'
    if idl_type.is_interface_type:
        implemented_as_class = idl_type.implemented_as
        if raw_type or (used_as_rvalue_type and idl_type.is_garbage_collected):
            return implemented_as_class + '*'
        new_type = 'Member' if used_in_cpp_sequence else 'RawPtr'
        ptr_type = cpp_ptr_type(('PassRefPtr' if used_as_rvalue_type else 'RefPtr'), new_type, idl_type.gc_type)
        return cpp_template_type(ptr_type, implemented_as_class)
    if idl_type.is_dictionary:
        return base_idl_type
    if idl_type.is_union_type:
        # Avoid "AOrNullOrB" for cpp type of (A? or B) because we generate
        # V8AOrBOrNull to handle nulle for (A? or B), (A or B?) and (A or B)?
        def member_cpp_name(idl_type):
            if idl_type.is_nullable:
                return idl_type.inner_type.name
            return idl_type.name
        idl_type_name = "Or".join(member_cpp_name(member)
                                  for member in idl_type.member_types)
        return 'const %s&' % idl_type_name if used_as_rvalue_type else idl_type_name

    # Default, assume native type is a pointer with same type name as idl type
    return base_idl_type + '*'


def cpp_type_initializer(idl_type):
    """Returns a string containing a C++ initialization statement for the
    corresponding type.

    |idl_type| argument is of type IdlType.
    """

    base_idl_type = idl_type.base_type

    if idl_type.native_array_element_type:
        return ''
    if idl_type.is_numeric_type:
        return ' = 0'
    if base_idl_type == 'boolean':
        return ' = false'
    if (base_idl_type in NON_WRAPPER_TYPES or
        base_idl_type in CPP_SPECIAL_CONVERSION_RULES or
        base_idl_type == 'any' or
        idl_type.is_string_type or
        idl_type.is_enum):
        return ''
    return ' = nullptr'


# Allow access as idl_type.cpp_type if no arguments
IdlTypeBase.cpp_type = property(cpp_type)
IdlTypeBase.cpp_type_initializer = property(cpp_type_initializer)
IdlTypeBase.cpp_type_args = cpp_type
IdlUnionType.cpp_type_initializer = ''


IdlArrayOrSequenceType.native_array_element_type = property(
    lambda self: self.element_type)


def cpp_template_type(template, inner_type):
    """Returns C++ template specialized to type."""
    format_string = '{template}<{inner_type}>'
    return format_string.format(template=template, inner_type=inner_type)


def cpp_ptr_type(old_type, new_type, gc_type):
    if gc_type == 'GarbageCollectedObject':
        return new_type
    if gc_type == 'WillBeGarbageCollectedObject':
        if old_type == 'Vector':
            return 'WillBe' + new_type
        return old_type + 'WillBe' + new_type
    return old_type


def v8_type(interface_name):
    return 'V8' + interface_name


# [ImplementedAs]
# This handles [ImplementedAs] on interface types, not [ImplementedAs] in the
# interface being generated. e.g., given:
#   Foo.idl: interface Foo {attribute Bar bar};
#   Bar.idl: [ImplementedAs=Zork] interface Bar {};
# when generating bindings for Foo, the [ImplementedAs] on Bar is needed.
# This data is external to Foo.idl, and hence computed as global information in
# compute_interfaces_info.py to avoid having to parse IDLs of all used interfaces.
IdlType.implemented_as_interfaces = {}


def implemented_as(idl_type):
    base_idl_type = idl_type.base_type
    if base_idl_type in IdlType.implemented_as_interfaces:
        return IdlType.implemented_as_interfaces[base_idl_type]
    return base_idl_type


IdlType.implemented_as = property(implemented_as)

IdlType.set_implemented_as_interfaces = classmethod(
    lambda cls, new_implemented_as_interfaces:
        cls.implemented_as_interfaces.update(new_implemented_as_interfaces))


# [GarbageCollected]
IdlType.garbage_collected_types = set()

IdlType.is_garbage_collected = property(
    lambda self: self.base_type in IdlType.garbage_collected_types)

IdlType.set_garbage_collected_types = classmethod(
    lambda cls, new_garbage_collected_types:
        cls.garbage_collected_types.update(new_garbage_collected_types))


# [WillBeGarbageCollected]
IdlType.will_be_garbage_collected_types = set()

IdlType.is_will_be_garbage_collected = property(
    lambda self: self.base_type in IdlType.will_be_garbage_collected_types)

IdlType.set_will_be_garbage_collected_types = classmethod(
    lambda cls, new_will_be_garbage_collected_types:
        cls.will_be_garbage_collected_types.update(new_will_be_garbage_collected_types))


def gc_type(idl_type):
    if idl_type.is_garbage_collected or idl_type.is_dictionary or idl_type.is_union_type:
        return 'GarbageCollectedObject'
    if idl_type.is_will_be_garbage_collected:
        return 'WillBeGarbageCollectedObject'
    return 'RefCountedObject'

IdlTypeBase.gc_type = property(gc_type)


def is_traceable(idl_type):
    return (idl_type.is_garbage_collected
            or idl_type.is_will_be_garbage_collected
            or idl_type.is_dictionary)

IdlTypeBase.is_traceable = property(is_traceable)
IdlUnionType.is_traceable = property(lambda self: True)
IdlArrayOrSequenceType.is_traceable = property(
    lambda self: self.element_type.is_traceable)


################################################################################
# Includes
################################################################################

def includes_for_cpp_class(class_name, relative_dir_posix):
    return set([posixpath.join('bindings', relative_dir_posix, class_name + '.h')])


INCLUDES_FOR_TYPE = {
    'object': set(),
    'Dictionary': set(['bindings/core/v8/Dictionary.h']),
    'EventHandler': set(['bindings/core/v8/V8AbstractEventListener.h',
                         'bindings/core/v8/V8EventListenerList.h']),
    'EventListener': set(['bindings/core/v8/BindingSecurity.h',
                          'bindings/core/v8/V8EventListenerList.h',
                          'core/frame/LocalDOMWindow.h']),
    'HTMLCollection': set(['bindings/core/v8/V8HTMLCollection.h',
                           'core/dom/ClassCollection.h',
                           'core/dom/TagCollection.h',
                           'core/html/HTMLCollection.h',
                           'core/html/HTMLDataListOptionsCollection.h',
                           'core/html/HTMLFormControlsCollection.h',
                           'core/html/HTMLTableRowsCollection.h']),
    'NodeList': set(['bindings/core/v8/V8NodeList.h',
                     'core/dom/NameNodeList.h',
                     'core/dom/NodeList.h',
                     'core/dom/StaticNodeList.h',
                     'core/html/LabelsNodeList.h']),
    'Promise': set(['bindings/core/v8/ScriptPromise.h']),
    'SerializedScriptValue': set(['bindings/core/v8/SerializedScriptValue.h',
                                  'bindings/core/v8/SerializedScriptValueFactory.h']),
    'ScriptValue': set(['bindings/core/v8/ScriptValue.h']),
}


def includes_for_type(idl_type, extended_attributes=None):
    idl_type = idl_type.preprocessed_type
    extended_attributes = extended_attributes or {}

    # Simple types
    base_idl_type = idl_type.base_type
    if base_idl_type in INCLUDES_FOR_TYPE:
        return INCLUDES_FOR_TYPE[base_idl_type]
    if idl_type.is_basic_type:
        return set()
    if base_idl_type.endswith('ConstructorConstructor'):
        # FIXME: rename to NamedConstructor
        # FIXME: replace with a [NamedConstructorAttribute] extended attribute
        # Ending with 'ConstructorConstructor' indicates a named constructor,
        # and these do not have header files, as they are part of the generated
        # bindings for the interface
        return set()
    if base_idl_type.endswith('Constructor'):
        # FIXME: replace with a [ConstructorAttribute] extended attribute
        base_idl_type = idl_type.constructor_type_name
    if base_idl_type not in component_dir:
        return set()
    return set(['bindings/%s/v8/V8%s.h' % (component_dir[base_idl_type],
                                           base_idl_type)])

IdlType.includes_for_type = includes_for_type


def includes_for_union_type(idl_type, extended_attributes=None):
    return set.union(*[member_type.includes_for_type(extended_attributes)
                       for member_type in idl_type.member_types])

IdlUnionType.includes_for_type = includes_for_union_type


def includes_for_array_or_sequence_type(idl_type, extended_attributes=None):
    return idl_type.element_type.includes_for_type(extended_attributes)

IdlArrayOrSequenceType.includes_for_type = includes_for_array_or_sequence_type


def add_includes_for_type(idl_type, extended_attributes=None):
    includes.update(idl_type.includes_for_type(extended_attributes))

IdlTypeBase.add_includes_for_type = add_includes_for_type


def includes_for_interface(interface_name):
    return IdlType(interface_name).includes_for_type()


def add_includes_for_interface(interface_name):
    includes.update(includes_for_interface(interface_name))


def impl_should_use_nullable_container(idl_type):
    return not(idl_type.cpp_type_has_null_value)

IdlTypeBase.impl_should_use_nullable_container = property(
    impl_should_use_nullable_container)


def impl_includes_for_type(idl_type, interfaces_info):
    includes_for_type = set()
    if idl_type.impl_should_use_nullable_container:
        includes_for_type.add('bindings/core/v8/Nullable.h')

    idl_type = idl_type.preprocessed_type
    native_array_element_type = idl_type.native_array_element_type
    if native_array_element_type:
        includes_for_type.update(impl_includes_for_type(
                native_array_element_type, interfaces_info))
        includes_for_type.add('wtf/Vector.h')

    base_idl_type = idl_type.base_type
    if idl_type.is_string_type:
        includes_for_type.add('wtf/text/WTFString.h')
    if base_idl_type in interfaces_info:
        interface_info = interfaces_info[idl_type.base_type]
        if interface_info['include_path']:
            includes_for_type.add(interface_info['include_path'])
    if base_idl_type in INCLUDES_FOR_TYPE:
        includes_for_type.update(INCLUDES_FOR_TYPE[base_idl_type])
    if idl_type.is_typed_array:
        return set(['core/dom/DOMTypedArray.h'])
    return includes_for_type


def impl_includes_for_type_union(idl_type, interfaces_info):
    includes_for_type = set()
    for member_type in idl_type.member_types:
        includes_for_type.update(member_type.impl_includes_for_type(interfaces_info))
    return includes_for_type

IdlTypeBase.impl_includes_for_type = impl_includes_for_type
IdlUnionType.impl_includes_for_type = impl_includes_for_type_union


component_dir = {}


def set_component_dirs(new_component_dirs):
    component_dir.update(new_component_dirs)


################################################################################
# V8 -> C++
################################################################################

V8_VALUE_TO_CPP_VALUE = {
    # Basic
    'Date': 'toCoreDate({isolate}, {v8_value})',
    'DOMString': '{v8_value}',
    'ByteString': 'toByteString({isolate}, {arguments})',
    'USVString': 'toUSVString({isolate}, {arguments})',
    'boolean': 'toBoolean({isolate}, {arguments})',
    'float': 'toRestrictedFloat({isolate}, {arguments})',
    'unrestricted float': 'toFloat({isolate}, {arguments})',
    'double': 'toRestrictedDouble({isolate}, {arguments})',
    'unrestricted double': 'toDouble({isolate}, {arguments})',
    'byte': 'toInt8({isolate}, {arguments})',
    'octet': 'toUInt8({isolate}, {arguments})',
    'short': 'toInt16({isolate}, {arguments})',
    'unsigned short': 'toUInt16({isolate}, {arguments})',
    'long': 'toInt32({isolate}, {arguments})',
    'unsigned long': 'toUInt32({isolate}, {arguments})',
    'long long': 'toInt64({isolate}, {arguments})',
    'unsigned long long': 'toUInt64({isolate}, {arguments})',
    # Interface types
    'Dictionary': 'Dictionary({v8_value}, {isolate}, exceptionState)',
    'EventTarget': 'toEventTarget({isolate}, {v8_value})',
    'NodeFilter': 'toNodeFilter({v8_value}, info.Holder(), ScriptState::current({isolate}))',
    'Promise': 'ScriptPromise::cast(ScriptState::current({isolate}), {v8_value})',
    'SerializedScriptValue': 'SerializedScriptValueFactory::instance().create({isolate}, {v8_value}, 0, 0, exceptionState)',
    'ScriptValue': 'ScriptValue(ScriptState::current({isolate}), {v8_value})',
    'Window': 'toDOMWindow({isolate}, {v8_value})',
    'XPathNSResolver': 'toXPathNSResolver(ScriptState::current({isolate}), {v8_value})',
}


def v8_conversion_needs_exception_state(idl_type):
    return (idl_type.is_numeric_type or
            idl_type.is_enum or
            idl_type.is_dictionary or
            idl_type.name in ('Boolean', 'ByteString', 'Dictionary', 'USVString', 'SerializedScriptValue'))

IdlType.v8_conversion_needs_exception_state = property(v8_conversion_needs_exception_state)
IdlArrayOrSequenceType.v8_conversion_needs_exception_state = True
IdlUnionType.v8_conversion_needs_exception_state = True


TRIVIAL_CONVERSIONS = frozenset([
    'any',
    'boolean',
    'Date',
    'Dictionary',
    'NodeFilter',
    'XPathNSResolver',
    'Promise'
])


def v8_conversion_is_trivial(idl_type):
    # The conversion is a simple expression that returns the converted value and
    # cannot raise an exception.
    return (idl_type.base_type in TRIVIAL_CONVERSIONS or
            idl_type.is_wrapper_type)

IdlType.v8_conversion_is_trivial = property(v8_conversion_is_trivial)


def v8_value_to_cpp_value(idl_type, extended_attributes, v8_value, variable_name, index, isolate, restricted_float=False):
    if idl_type.name == 'void':
        return ''

    # Array or sequence types
    native_array_element_type = idl_type.native_array_element_type
    if native_array_element_type:
        return v8_value_to_cpp_value_array_or_sequence(native_array_element_type, v8_value, index, isolate)

    # Simple types
    idl_type = idl_type.preprocessed_type
    base_idl_type = idl_type.as_union_type.name if idl_type.is_union_type else idl_type.base_type

    if idl_type.is_integer_type:
        configuration = 'NormalConversion'
        if 'EnforceRange' in extended_attributes:
            configuration = 'EnforceRange'
        elif 'Clamp' in extended_attributes:
            configuration = 'Clamp'
        arguments = ', '.join([v8_value, configuration, 'exceptionState'])
    elif idl_type.v8_conversion_needs_exception_state:
        arguments = ', '.join([v8_value, 'exceptionState'])
    else:
        arguments = v8_value
    if base_idl_type in V8_VALUE_TO_CPP_VALUE:
        cpp_expression_format = V8_VALUE_TO_CPP_VALUE[base_idl_type]
    elif idl_type.is_array_buffer_or_view:
        cpp_expression_format = (
            '{v8_value}->Is{idl_type}() ? '
            'V8{idl_type}::toImpl(v8::Local<v8::{idl_type}>::Cast({v8_value})) : 0')
    elif idl_type.use_output_parameter_for_result:
        if idl_type.includes_nullable_type:
            base_idl_type = idl_type.cpp_type + 'OrNull'
        cpp_expression_format = 'V8{idl_type}::toImpl({isolate}, {v8_value}, {variable_name}, exceptionState)'
    else:
        cpp_expression_format = (
            'V8{idl_type}::toImplWithTypeCheck({isolate}, {v8_value})')

    return cpp_expression_format.format(arguments=arguments, idl_type=base_idl_type, v8_value=v8_value, variable_name=variable_name, isolate=isolate)


def v8_value_to_cpp_value_array_or_sequence(native_array_element_type, v8_value, index, isolate='info.GetIsolate()'):
    # Index is None for setters, index (starting at 0) for method arguments,
    # and is used to provide a human-readable exception message
    if index is None:
        index = 0  # special case, meaning "setter"
    else:
        index += 1  # human-readable index
    if (native_array_element_type.is_interface_type and
        native_array_element_type.name != 'Dictionary'):
        this_cpp_type = None
        ref_ptr_type = cpp_ptr_type('RefPtr', 'Member', native_array_element_type.gc_type)
        expression_format = '(to{ref_ptr_type}NativeArray<{native_array_element_type}, V8{native_array_element_type}>({v8_value}, {index}, {isolate}, exceptionState))'
    else:
        ref_ptr_type = None
        this_cpp_type = native_array_element_type.cpp_type
        if native_array_element_type.is_dictionary or native_array_element_type.is_union_type:
            vector_type = 'HeapVector'
        else:
            vector_type = 'Vector'
        expression_format = 'toImplArray<%s<{cpp_type}>>({v8_value}, {index}, {isolate}, exceptionState)' % vector_type
    expression = expression_format.format(native_array_element_type=native_array_element_type.name, cpp_type=this_cpp_type, index=index, ref_ptr_type=ref_ptr_type, v8_value=v8_value, isolate=isolate)
    return expression


# FIXME: this function should be refactored, as this takes too many flags.
def v8_value_to_local_cpp_value(idl_type, extended_attributes, v8_value, variable_name, index=None, declare_variable=True, isolate='info.GetIsolate()', bailout_return_value=None, use_exception_state=False, restricted_float=False):
    """Returns an expression that converts a V8 value to a C++ value and stores it as a local value."""

    this_cpp_type = idl_type.cpp_type_args(extended_attributes=extended_attributes, raw_type=True)
    idl_type = idl_type.preprocessed_type

    cpp_value = v8_value_to_cpp_value(idl_type, extended_attributes, v8_value, variable_name, index, isolate, restricted_float=restricted_float)

    # Optional expression that returns a value to be assigned to the local variable.
    assign_expression = None
    # Optional void expression executed unconditionally.
    set_expression = None
    # Optional expression that returns true if the conversion fails.
    check_expression = None
    # Optional expression used as the return value when returning. Only
    # meaningful if 'check_expression' is not None.
    return_expression = bailout_return_value

    if idl_type.is_string_type or idl_type.v8_conversion_needs_exception_state:
        # Types for which conversion can fail and that need error handling.

        if use_exception_state:
            check_expression = 'exceptionState.hadException()'
        else:
            check_expression = 'exceptionState.throwIfNeeded()'

        if idl_type.is_dictionary or idl_type.is_union_type:
            set_expression = cpp_value
        else:
            assign_expression = cpp_value
            # Note: 'not idl_type.v8_conversion_needs_exception_state' implies
            # 'idl_type.is_string_type', but there are types for which both are
            # true (ByteString and USVString), so using idl_type.is_string_type
            # as the condition here would be wrong.
            if not idl_type.v8_conversion_needs_exception_state:
                if use_exception_state:
                    check_expression = '!%s.prepare(exceptionState)' % variable_name
                else:
                    check_expression = '!%s.prepare()' % variable_name
    elif not idl_type.v8_conversion_is_trivial:
        return {
            'error_message': 'no V8 -> C++ conversion for IDL type: %s' % idl_type.name
        }
    else:
        assign_expression = cpp_value

    # Types that don't need error handling, and simply assign a value to the
    # local variable.

    return {
        'assign_expression': assign_expression,
        'check_expression': check_expression,
        'cpp_type': this_cpp_type,
        'cpp_name': variable_name,
        'declare_variable': declare_variable,
        'return_expression': bailout_return_value,
        'set_expression': set_expression,
    }


IdlTypeBase.v8_value_to_local_cpp_value = v8_value_to_local_cpp_value


def use_output_parameter_for_result(idl_type):
    """True when methods/getters which return the given idl_type should
    take the output argument.
    """
    return idl_type.is_dictionary or idl_type.is_union_type

IdlTypeBase.use_output_parameter_for_result = property(use_output_parameter_for_result)


################################################################################
# C++ -> V8
################################################################################

def preprocess_idl_type(idl_type):
    if idl_type.is_nullable:
        return IdlNullableType(idl_type.inner_type.preprocessed_type)
    if idl_type.is_enum:
        # Enumerations are internally DOMStrings
        return IdlType('DOMString')
    if idl_type.base_type in ['any', 'object'] or idl_type.is_callback_function:
        return IdlType('ScriptValue')
    return idl_type

IdlTypeBase.preprocessed_type = property(preprocess_idl_type)


def preprocess_idl_type_and_value(idl_type, cpp_value, extended_attributes):
    """Returns IDL type and value, with preliminary type conversions applied."""
    idl_type = idl_type.preprocessed_type
    if idl_type.name == 'Promise':
        idl_type = IdlType('ScriptValue')
    if idl_type.base_type in ['long long', 'unsigned long long']:
        # long long and unsigned long long are not representable in ECMAScript;
        # we represent them as doubles.
        is_nullable = idl_type.is_nullable
        idl_type = IdlType('double')
        if is_nullable:
            idl_type = IdlNullableType(idl_type)
        cpp_value = 'static_cast<double>(%s)' % cpp_value
    # HTML5 says that unsigned reflected attributes should be in the range
    # [0, 2^31). When a value isn't in this range, a default value (or 0)
    # should be returned instead.
    extended_attributes = extended_attributes or {}
    if ('Reflect' in extended_attributes and
        idl_type.base_type in ['unsigned long', 'unsigned short']):
        cpp_value = cpp_value.replace('getUnsignedIntegralAttribute',
                                      'getIntegralAttribute')
        cpp_value = 'std::max(0, static_cast<int>(%s))' % cpp_value
    return idl_type, cpp_value


def v8_conversion_type(idl_type, extended_attributes):
    """Returns V8 conversion type, adding any additional includes.

    The V8 conversion type is used to select the C++ -> V8 conversion function
    or v8SetReturnValue* function; it can be an idl_type, a cpp_type, or a
    separate name for the type of conversion (e.g., 'DOMWrapper').
    """
    extended_attributes = extended_attributes or {}

    # Nullable dictionaries need to be handled differently than either
    # non-nullable dictionaries or unions.
    if idl_type.is_dictionary and idl_type.is_nullable:
        return 'NullableDictionary'

    if idl_type.is_dictionary or idl_type.is_union_type:
        return 'DictionaryOrUnion'

    # Array or sequence types
    native_array_element_type = idl_type.native_array_element_type
    if native_array_element_type:
        return 'array'

    # Simple types
    base_idl_type = idl_type.base_type
    # Basic types, without additional includes
    if base_idl_type in CPP_INT_TYPES:
        return 'int'
    if base_idl_type in CPP_UNSIGNED_TYPES:
        return 'unsigned'
    if idl_type.is_string_type:
        if idl_type.is_nullable:
            return 'StringOrNull'
        if 'TreatReturnedNullStringAs' not in extended_attributes:
            return base_idl_type
        treat_returned_null_string_as = extended_attributes['TreatReturnedNullStringAs']
        if treat_returned_null_string_as == 'Null':
            return 'StringOrNull'
        if treat_returned_null_string_as == 'Undefined':
            return 'StringOrUndefined'
        raise 'Unrecognized TreatReturnedNullStringAs value: "%s"' % treat_returned_null_string_as
    if idl_type.is_basic_type or base_idl_type == 'ScriptValue':
        return base_idl_type
    # Generic dictionary type
    if base_idl_type == 'Dictionary':
        return 'Dictionary'

    # Data type with potential additional includes
    if base_idl_type in V8_SET_RETURN_VALUE:  # Special v8SetReturnValue treatment
        return base_idl_type

    # Pointer type
    return 'DOMWrapper'

IdlTypeBase.v8_conversion_type = v8_conversion_type


V8_SET_RETURN_VALUE = {
    'boolean': 'v8SetReturnValueBool(info, {cpp_value})',
    'int': 'v8SetReturnValueInt(info, {cpp_value})',
    'unsigned': 'v8SetReturnValueUnsigned(info, {cpp_value})',
    'DOMString': 'v8SetReturnValueString(info, {cpp_value}, info.GetIsolate())',
    'ByteString': 'v8SetReturnValueString(info, {cpp_value}, info.GetIsolate())',
    'USVString': 'v8SetReturnValueString(info, {cpp_value}, info.GetIsolate())',
    # [TreatReturnedNullStringAs]
    'StringOrNull': 'v8SetReturnValueStringOrNull(info, {cpp_value}, info.GetIsolate())',
    'StringOrUndefined': 'v8SetReturnValueStringOrUndefined(info, {cpp_value}, info.GetIsolate())',
    'void': '',
    # No special v8SetReturnValue* function (set value directly)
    'float': 'v8SetReturnValue(info, {cpp_value})',
    'unrestricted float': 'v8SetReturnValue(info, {cpp_value})',
    'double': 'v8SetReturnValue(info, {cpp_value})',
    'unrestricted double': 'v8SetReturnValue(info, {cpp_value})',
    # No special v8SetReturnValue* function, but instead convert value to V8
    # and then use general v8SetReturnValue.
    'array': 'v8SetReturnValue(info, {cpp_value})',
    'Date': 'v8SetReturnValue(info, {cpp_value})',
    'EventHandler': 'v8SetReturnValue(info, {cpp_value})',
    'ScriptValue': 'v8SetReturnValue(info, {cpp_value})',
    'SerializedScriptValue': 'v8SetReturnValue(info, {cpp_value})',
    # DOMWrapper
    'DOMWrapperForMainWorld': 'v8SetReturnValueForMainWorld(info, WTF::getPtr({cpp_value}))',
    'DOMWrapperFast': 'v8SetReturnValueFast(info, WTF::getPtr({cpp_value}), {script_wrappable})',
    'DOMWrapperDefault': 'v8SetReturnValue(info, {cpp_value})',
    # Note that static attributes and operations do not check whether |this| is
    # an instance of the interface nor |this|'s creation context is the same as
    # the current context.  So we must always use the current context as the
    # creation context of the DOM wrapper for the return value.
    'DOMWrapperStatic': 'v8SetReturnValue(info, {cpp_value}, info.GetIsolate()->GetCurrentContext()->Global())',
    # Generic dictionary type
    'Dictionary': 'v8SetReturnValue(info, {cpp_value})',
    'DictionaryStatic': '#error not implemented yet',
    # Nullable dictionaries
    'NullableDictionary': 'v8SetReturnValue(info, result.get())',
    'NullableDictionaryStatic': '#error not implemented yet',
    # Union types or dictionaries
    'DictionaryOrUnion': 'v8SetReturnValue(info, result)',
    'DictionaryOrUnionStatic': '#error not implemented yet',
}


def v8_set_return_value(idl_type, cpp_value, extended_attributes=None, script_wrappable='', release=False, for_main_world=False, is_static=False):
    """Returns a statement that converts a C++ value to a V8 value and sets it as a return value.

    """
    def dom_wrapper_conversion_type():
        if is_static:
            return 'DOMWrapperStatic'
        if not script_wrappable:
            return 'DOMWrapperDefault'
        if for_main_world:
            return 'DOMWrapperForMainWorld'
        return 'DOMWrapperFast'

    idl_type, cpp_value = preprocess_idl_type_and_value(idl_type, cpp_value, extended_attributes)
    this_v8_conversion_type = idl_type.v8_conversion_type(extended_attributes)
    # SetReturn-specific overrides
    if this_v8_conversion_type in ['Date', 'EventHandler', 'ScriptValue', 'SerializedScriptValue', 'array']:
        # Convert value to V8 and then use general v8SetReturnValue
        cpp_value = idl_type.cpp_value_to_v8_value(cpp_value, extended_attributes=extended_attributes)
    if this_v8_conversion_type == 'DOMWrapper':
        this_v8_conversion_type = dom_wrapper_conversion_type()
    if is_static and this_v8_conversion_type in ('Dictionary', 'NullableDictionary', 'DictionaryOrUnion'):
        this_v8_conversion_type += 'Static'

    format_string = V8_SET_RETURN_VALUE[this_v8_conversion_type]
    # FIXME: oilpan: Remove .release() once we remove all RefPtrs from generated code.
    if release:
        cpp_value = '%s.release()' % cpp_value
    statement = format_string.format(cpp_value=cpp_value, script_wrappable=script_wrappable)
    return statement


IdlTypeBase.v8_set_return_value = v8_set_return_value

IdlType.release = property(lambda self: self.is_interface_type)
IdlUnionType.release = False


CPP_VALUE_TO_V8_VALUE = {
    # Built-in types
    'Date': 'v8DateOrNaN({isolate}, {cpp_value})',
    'DOMString': 'v8String({isolate}, {cpp_value})',
    'ByteString': 'v8String({isolate}, {cpp_value})',
    'USVString': 'v8String({isolate}, {cpp_value})',
    'boolean': 'v8Boolean({cpp_value}, {isolate})',
    'int': 'v8::Integer::New({isolate}, {cpp_value})',
    'unsigned': 'v8::Integer::NewFromUnsigned({isolate}, {cpp_value})',
    'float': 'v8::Number::New({isolate}, {cpp_value})',
    'unrestricted float': 'v8::Number::New({isolate}, {cpp_value})',
    'double': 'v8::Number::New({isolate}, {cpp_value})',
    'unrestricted double': 'v8::Number::New({isolate}, {cpp_value})',
    'void': 'v8Undefined()',
    # [TreatReturnedNullStringAs]
    'StringOrNull': '{cpp_value}.isNull() ? v8::Local<v8::Value>(v8::Null({isolate})) : v8String({isolate}, {cpp_value})',
    'StringOrUndefined': '{cpp_value}.isNull() ? v8Undefined() : v8String({isolate}, {cpp_value})',
    # Special cases
    'Dictionary': '{cpp_value}.v8Value()',
    'EventHandler': '{cpp_value} ? v8::Local<v8::Value>(V8AbstractEventListener::cast({cpp_value})->getListenerObject(impl->executionContext())) : v8::Local<v8::Value>(v8::Null({isolate}))',
    'ScriptValue': '{cpp_value}.v8Value()',
    'SerializedScriptValue': '{cpp_value} ? {cpp_value}->deserialize() : v8::Local<v8::Value>(v8::Null({isolate}))',
    # General
    'array': 'toV8({cpp_value}, {creation_context}, {isolate})',
    'DOMWrapper': 'toV8({cpp_value}, {creation_context}, {isolate})',
    # Passing nullable dictionaries isn't a pattern currently used
    # anywhere in the web platform, and more work would be needed in
    # the code generator to distinguish between passing null, and
    # passing an object which happened to not contain any of the
    # dictionary's defined attributes. For now, don't define
    # NullableDictionary here, which will cause an exception to be
    # thrown during code generation if an argument to a method is a
    # nullable dictionary type.
    #
    # Union types or dictionaries
    'DictionaryOrUnion': 'toV8({cpp_value}, {creation_context}, {isolate})',
}


def cpp_value_to_v8_value(idl_type, cpp_value, isolate='info.GetIsolate()', creation_context='info.Holder()', extended_attributes=None):
    """Returns an expression that converts a C++ value to a V8 value."""
    # the isolate parameter is needed for callback interfaces
    idl_type, cpp_value = preprocess_idl_type_and_value(idl_type, cpp_value, extended_attributes)
    this_v8_conversion_type = idl_type.v8_conversion_type(extended_attributes)
    format_string = CPP_VALUE_TO_V8_VALUE[this_v8_conversion_type]
    statement = format_string.format(cpp_value=cpp_value, isolate=isolate, creation_context=creation_context)
    return statement

IdlTypeBase.cpp_value_to_v8_value = cpp_value_to_v8_value


def literal_cpp_value(idl_type, idl_literal):
    """Converts an expression that is a valid C++ literal for this type."""
    # FIXME: add validation that idl_type and idl_literal are compatible
    if idl_type.base_type in ('any', 'object') and idl_literal.is_null:
        return 'ScriptValue()'
    literal_value = str(idl_literal)
    if idl_type.base_type in CPP_UNSIGNED_TYPES:
        return literal_value + 'u'
    return literal_value


def union_literal_cpp_value(idl_type, idl_literal):
    if idl_literal.is_null:
        return idl_type.name + '()'
    elif idl_literal.idl_type == 'DOMString':
        member_type = idl_type.string_member_type
    elif idl_literal.idl_type in ('integer', 'float'):
        member_type = idl_type.numeric_member_type
    elif idl_literal.idl_type == 'boolean':
        member_type = idl_type.boolean_member_type
    else:
        raise ValueError('Unsupported literal type: ' + idl_literal.idl_type)

    return '%s::from%s(%s)' % (idl_type.name, member_type.name,
                               member_type.literal_cpp_value(idl_literal))


def array_or_sequence_literal_cpp_value(idl_type, idl_literal):
    # Only support empty arrays.
    if idl_literal.value == '[]':
        return cpp_type(idl_type) + '()'
    raise ValueError('Unsupported literal type: ' + idl_literal.idl_type)


IdlType.literal_cpp_value = literal_cpp_value
IdlUnionType.literal_cpp_value = union_literal_cpp_value
IdlArrayOrSequenceType.literal_cpp_value = array_or_sequence_literal_cpp_value


################################################################################
# Utility properties for nullable types
################################################################################


def cpp_type_has_null_value(idl_type):
    # - String types (String/AtomicString) represent null as a null string,
    #   i.e. one for which String::isNull() returns true.
    # - Enum types, as they are implemented as Strings.
    # - Interface types (raw pointer or RefPtr/PassRefPtr) represent null as
    #   a null pointer.
    # - Union types, as thier container classes can represent null value.
    # - 'Object' and 'any' type. We use ScriptValue for object type.
    return (idl_type.is_string_type or idl_type.is_interface_type or
            idl_type.is_enum or idl_type.is_union_type
            or idl_type.base_type == 'object' or idl_type.base_type == 'any'
            or idl_type.is_callback_function or idl_type.is_callback_interface)

IdlTypeBase.cpp_type_has_null_value = property(cpp_type_has_null_value)


def is_implicit_nullable(idl_type):
    # Nullable type where the corresponding C++ type supports a null value.
    return idl_type.is_nullable and idl_type.cpp_type_has_null_value


def is_explicit_nullable(idl_type):
    # Nullable type that isn't implicit nullable (see above.) For such types,
    # we use Nullable<T> or similar explicit ways to represent a null value.
    return idl_type.is_nullable and not idl_type.is_implicit_nullable

IdlTypeBase.is_implicit_nullable = property(is_implicit_nullable)
IdlUnionType.is_implicit_nullable = False
IdlTypeBase.is_explicit_nullable = property(is_explicit_nullable)


def number_of_nullable_member_types_union(idl_type):
    # http://heycam.github.io/webidl/#dfn-number-of-nullable-member-types
    count = 0
    for member in idl_type.member_types:
        if member.is_nullable:
            count += 1
            member = member.inner_type
        if member.is_union_type:
            count += number_of_nullable_member_types_union(member)
    return count

IdlUnionType.number_of_nullable_member_types = property(
    number_of_nullable_member_types_union)


def includes_nullable_type_union(idl_type):
    # http://heycam.github.io/webidl/#dfn-includes-a-nullable-type
    return idl_type.number_of_nullable_member_types == 1

IdlTypeBase.includes_nullable_type = False
IdlNullableType.includes_nullable_type = True
IdlUnionType.includes_nullable_type = property(includes_nullable_type_union)
