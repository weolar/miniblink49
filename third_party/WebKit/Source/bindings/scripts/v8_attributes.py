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

"""Generate template values for attributes.

Extends IdlType with property |constructor_type_name|.

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

import idl_types
from idl_types import inherits_interface
from v8_globals import includes, interfaces
import v8_types
import v8_utilities
from v8_utilities import (cpp_name_or_partial, capitalize, cpp_name, has_extended_attribute,
                          has_extended_attribute_value, scoped_name, strip_suffix,
                          uncapitalize, extended_attribute_value_as_list, is_unforgeable,
                          is_legacy_interface_type_checking)


def attribute_context(interface, attribute):
    idl_type = attribute.idl_type
    base_idl_type = idl_type.base_type
    extended_attributes = attribute.extended_attributes

    idl_type.add_includes_for_type(extended_attributes)
    if idl_type.enum_values:
        includes.add('core/inspector/ConsoleMessage.h')

    # [CheckSecurity]
    is_do_not_check_security = 'DoNotCheckSecurity' in extended_attributes
    is_check_security_for_frame = (
        has_extended_attribute_value(interface, 'CheckSecurity', 'Frame') and
        not is_do_not_check_security)
    is_check_security_for_node = (
        has_extended_attribute_value(attribute, 'CheckSecurity', 'Node'))
    is_check_security_for_window = (
        has_extended_attribute_value(interface, 'CheckSecurity', 'Window') and
        not is_do_not_check_security)
    if is_check_security_for_frame or is_check_security_for_node or is_check_security_for_window:
        includes.add('bindings/core/v8/BindingSecurity.h')
    # [CustomElementCallbacks], [Reflect]
    is_custom_element_callbacks = 'CustomElementCallbacks' in extended_attributes
    is_reflect = 'Reflect' in extended_attributes
    if is_custom_element_callbacks or is_reflect:
        includes.add('core/dom/custom/CustomElementProcessingStack.h')
    # [ImplementedInPrivateScript]
    is_implemented_in_private_script = 'ImplementedInPrivateScript' in extended_attributes
    if is_implemented_in_private_script:
        includes.add('bindings/core/v8/PrivateScriptRunner.h')
        includes.add('core/frame/LocalFrame.h')
        includes.add('platform/ScriptForbiddenScope.h')
    # [OnlyExposedToPrivateScript]
    is_only_exposed_to_private_script = 'OnlyExposedToPrivateScript' in extended_attributes
    # [PerWorldBindings]
    if 'PerWorldBindings' in extended_attributes:
        assert idl_type.is_wrapper_type or 'LogActivity' in extended_attributes, '[PerWorldBindings] should only be used with wrapper types: %s.%s' % (interface.name, attribute.name)

    if (base_idl_type == 'EventHandler' and
        interface.name in ['Window', 'WorkerGlobalScope'] and
        attribute.name == 'onerror'):
        includes.add('bindings/core/v8/V8ErrorHandler.h')

    cached_attribute_validation_method = extended_attributes.get('CachedAttribute')
    keep_alive_for_gc = is_keep_alive_for_gc(interface, attribute)
    if cached_attribute_validation_method or keep_alive_for_gc:
        includes.add('bindings/core/v8/V8HiddenValue.h')

    context = {
        'access_control_list': access_control_list(interface, attribute),
        'activity_logging_world_list_for_getter': v8_utilities.activity_logging_world_list(attribute, 'Getter'),  # [ActivityLogging]
        'activity_logging_world_list_for_setter': v8_utilities.activity_logging_world_list(attribute, 'Setter'),  # [ActivityLogging]
        'activity_logging_world_check': v8_utilities.activity_logging_world_check(attribute),  # [ActivityLogging]
        'argument_cpp_type': idl_type.cpp_type_args(used_as_rvalue_type=True),
        'cached_attribute_validation_method': cached_attribute_validation_method,
        'conditional_string': v8_utilities.conditional_string(attribute),
        'constructor_type': idl_type.constructor_type_name
                            if is_constructor_attribute(attribute) else None,
        'cpp_name': cpp_name(attribute),
        'cpp_type': idl_type.cpp_type,
        'cpp_type_initializer': idl_type.cpp_type_initializer,
        'deprecate_as': v8_utilities.deprecate_as(attribute),  # [DeprecateAs]
        'enum_type': idl_type.enum_type,
        'enum_values': idl_type.enum_values,
        'exposed_test': v8_utilities.exposed(attribute, interface),  # [Exposed]
        'has_custom_getter': has_custom_getter(attribute),
        'has_custom_setter': has_custom_setter(attribute),
        'has_setter': has_setter(attribute),
        'idl_type': str(idl_type),  # need trailing [] on array for Dictionary::ConversionContext::setConversionType
        'is_call_with_execution_context': has_extended_attribute_value(attribute, 'CallWith', 'ExecutionContext'),
        'is_call_with_script_state': has_extended_attribute_value(attribute, 'CallWith', 'ScriptState'),
        'is_check_security_for_frame': is_check_security_for_frame,
        'is_check_security_for_node': is_check_security_for_node,
        'is_check_security_for_window': is_check_security_for_window,
        'is_custom_element_callbacks': is_custom_element_callbacks,
        'is_expose_js_accessors': is_expose_js_accessors(interface, attribute),
        'is_getter_raises_exception':  # [RaisesException]
            'RaisesException' in extended_attributes and
            extended_attributes['RaisesException'] in (None, 'Getter'),
        'is_implemented_in_private_script': is_implemented_in_private_script,
        'is_keep_alive_for_gc': keep_alive_for_gc,
        'is_lenient_this': 'LenientThis' in extended_attributes,
        'is_nullable': idl_type.is_nullable,
        'is_explicit_nullable': idl_type.is_explicit_nullable,
        'is_partial_interface_member':
            'PartialInterfaceImplementedAs' in extended_attributes,
        'is_per_world_bindings': 'PerWorldBindings' in extended_attributes,
        'is_put_forwards': 'PutForwards' in extended_attributes,
        'is_read_only': attribute.is_read_only,
        'is_reflect': is_reflect,
        'is_replaceable': 'Replaceable' in attribute.extended_attributes,
        'is_static': attribute.is_static,
        'is_url': 'URL' in extended_attributes,
        'is_unforgeable': is_unforgeable(interface, attribute),
        'on_instance': v8_utilities.on_instance(interface, attribute),
        'on_interface': v8_utilities.on_interface(interface, attribute),
        'on_prototype': v8_utilities.on_prototype(interface, attribute),
        'use_output_parameter_for_result': idl_type.use_output_parameter_for_result,
        'measure_as': v8_utilities.measure_as(attribute, interface),  # [MeasureAs]
        'name': attribute.name,
        'only_exposed_to_private_script': is_only_exposed_to_private_script,
        'private_script_v8_value_to_local_cpp_value': idl_type.v8_value_to_local_cpp_value(
            extended_attributes, 'v8Value', 'cppValue', bailout_return_value='false', isolate='scriptState->isolate()'),
        'property_attributes': property_attributes(interface, attribute),
        'reflect_empty': extended_attributes.get('ReflectEmpty'),
        'reflect_invalid': extended_attributes.get('ReflectInvalid', ''),
        'reflect_missing': extended_attributes.get('ReflectMissing'),
        'reflect_only': extended_attribute_value_as_list(attribute, 'ReflectOnly'),
        'runtime_enabled_function': v8_utilities.runtime_enabled_function_name(attribute),  # [RuntimeEnabled]
        'should_be_exposed_to_script': not (is_implemented_in_private_script and is_only_exposed_to_private_script),
        'world_suffixes': ['', 'ForMainWorld']
                          if 'PerWorldBindings' in extended_attributes
                          else [''],  # [PerWorldBindings]
    }

    if is_constructor_attribute(attribute):
        update_constructor_attribute_context(interface, attribute, context)
    if not has_custom_getter(attribute):
        getter_context(interface, attribute, context)
    if not has_custom_setter(attribute) and has_setter(attribute):
        setter_context(interface, attribute, context)

    return context


################################################################################
# Getter
################################################################################

def getter_context(interface, attribute, context):
    idl_type = attribute.idl_type
    base_idl_type = idl_type.base_type
    extended_attributes = attribute.extended_attributes

    cpp_value = getter_expression(interface, attribute, context)
    # Normally we can inline the function call into the return statement to
    # avoid the overhead of using a Ref<> temporary, but for some cases
    # (nullable types, EventHandler, [CachedAttribute], or if there are
    # exceptions), we need to use a local variable.
    # FIXME: check if compilers are smart enough to inline this, and if so,
    # always use a local variable (for readability and CG simplicity).
    release = False
    if 'ImplementedInPrivateScript' in extended_attributes:
        if (not idl_type.is_wrapper_type and
            not idl_type.is_basic_type and
            not idl_type.is_enum):
            raise Exception('Private scripts supports only primitive types and DOM wrappers.')

        context['cpp_value_original'] = cpp_value
        cpp_value = 'result'
        # EventHandler has special handling
        if base_idl_type != 'EventHandler':
            release = idl_type.release
    elif (idl_type.is_explicit_nullable or
        base_idl_type == 'EventHandler' or
        'CachedAttribute' in extended_attributes or
        'ReflectOnly' in extended_attributes or
        context['is_keep_alive_for_gc'] or
        context['is_getter_raises_exception']):
        context['cpp_value_original'] = cpp_value
        cpp_value = 'cppValue'
        # EventHandler has special handling
        if base_idl_type != 'EventHandler':
            release = idl_type.release

    def v8_set_return_value_statement(for_main_world=False):
        if context['is_keep_alive_for_gc'] or 'CachedAttribute' in extended_attributes:
            return 'v8SetReturnValue(info, v8Value)'
        return idl_type.v8_set_return_value(
            cpp_value, extended_attributes=extended_attributes, script_wrappable='impl',
            release=release, for_main_world=for_main_world, is_static=attribute.is_static)

    context.update({
        'cpp_value': cpp_value,
        'cpp_value_to_v8_value': idl_type.cpp_value_to_v8_value(
            cpp_value=cpp_value, creation_context='holder',
            extended_attributes=extended_attributes),
        'v8_set_return_value_for_main_world': v8_set_return_value_statement(for_main_world=True),
        'v8_set_return_value': v8_set_return_value_statement(),
    })

def getter_expression(interface, attribute, context):
    arguments = []
    this_getter_base_name = getter_base_name(interface, attribute, arguments)
    getter_name = scoped_name(interface, attribute, this_getter_base_name)

    if 'ImplementedInPrivateScript' in attribute.extended_attributes:
        arguments.append('toLocalFrame(toFrameIfNotDetached(info.GetIsolate()->GetCurrentContext()))')
        arguments.append('impl')
        arguments.append('&result')
    arguments.extend(v8_utilities.call_with_arguments(
        attribute.extended_attributes.get('CallWith')))
    # Members of IDL partial interface definitions are implemented in C++ as
    # static member functions, which for instance members (non-static members)
    # take *impl as their first argument
    if ('PartialInterfaceImplementedAs' in attribute.extended_attributes and
        not 'ImplementedInPrivateScript' in attribute.extended_attributes and
        not attribute.is_static):
        arguments.append('*impl')
    if attribute.idl_type.is_explicit_nullable:
        arguments.append('isNull')
    if context['is_getter_raises_exception']:
        arguments.append('exceptionState')
    if attribute.idl_type.use_output_parameter_for_result:
        arguments.append('result')
    return '%s(%s)' % (getter_name, ', '.join(arguments))


CONTENT_ATTRIBUTE_GETTER_NAMES = {
    'boolean': 'fastHasAttribute',
    'long': 'getIntegralAttribute',
    'unsigned long': 'getUnsignedIntegralAttribute',
}


def getter_base_name(interface, attribute, arguments):
    extended_attributes = attribute.extended_attributes

    if 'ImplementedInPrivateScript' in extended_attributes:
        return '%sAttributeGetter' % uncapitalize(cpp_name(attribute))

    if 'Reflect' not in extended_attributes:
        return uncapitalize(cpp_name(attribute))

    content_attribute_name = extended_attributes['Reflect'] or attribute.name.lower()
    if content_attribute_name in ['class', 'id', 'name']:
        # Special-case for performance optimization.
        return 'get%sAttribute' % content_attribute_name.capitalize()

    arguments.append(scoped_content_attribute_name(interface, attribute))

    base_idl_type = attribute.idl_type.base_type
    if base_idl_type in CONTENT_ATTRIBUTE_GETTER_NAMES:
        return CONTENT_ATTRIBUTE_GETTER_NAMES[base_idl_type]
    if 'URL' in attribute.extended_attributes:
        return 'getURLAttribute'
    return 'fastGetAttribute'


def is_keep_alive_for_gc(interface, attribute):
    idl_type = attribute.idl_type
    base_idl_type = idl_type.base_type
    extended_attributes = attribute.extended_attributes
    return (
        # For readonly attributes, for performance reasons we keep the attribute
        # wrapper alive while the owner wrapper is alive, because the attribute
        # never changes.
        (attribute.is_read_only and
         idl_type.is_wrapper_type and
         # There are some exceptions, however:
         not(
             # Node lifetime is managed by object grouping.
             inherits_interface(interface.name, 'Node') or
             inherits_interface(base_idl_type, 'Node') or
             # A self-reference is unnecessary.
             attribute.name == 'self' or
             # FIXME: Remove these hard-coded hacks.
             base_idl_type in ['EventTarget', 'Window'] or
             base_idl_type.startswith(('HTML', 'SVG')))))


################################################################################
# Setter
################################################################################

def setter_context(interface, attribute, context):
    if 'PutForwards' in attribute.extended_attributes:
        # Use target interface and attribute in place of original interface and
        # attribute from this point onwards.
        target_interface_name = attribute.idl_type.base_type
        target_attribute_name = attribute.extended_attributes['PutForwards']
        interface = interfaces[target_interface_name]
        try:
            attribute = next(candidate
                             for candidate in interface.attributes
                             if candidate.name == target_attribute_name)
        except StopIteration:
            raise Exception('[PutForward] target not found:\n'
                            'Attribute "%s" is not present in interface "%s"' %
                            (target_attribute_name, target_interface_name))

    if ('Replaceable' in attribute.extended_attributes or
            is_constructor_attribute(attribute)):
        context['cpp_setter'] = '%sCreateDataProperty(propertyName, v8Value, info)' % cpp_name(interface)
        return

    extended_attributes = attribute.extended_attributes
    idl_type = attribute.idl_type

    # [RaisesException], [RaisesException=Setter]
    is_setter_raises_exception = (
        'RaisesException' in extended_attributes and
        extended_attributes['RaisesException'] in [None, 'Setter'])
    # [TypeChecking=Interface] / [LegacyInterfaceTypeChecking]
    has_type_checking_interface = (
        not is_legacy_interface_type_checking(interface, attribute) and
        idl_type.is_wrapper_type)

    context.update({
        'has_setter_exception_state':
            is_setter_raises_exception or has_type_checking_interface or
            idl_type.v8_conversion_needs_exception_state,
        'has_type_checking_interface': has_type_checking_interface,
        'is_setter_call_with_execution_context': has_extended_attribute_value(
            attribute, 'SetterCallWith', 'ExecutionContext'),
        'is_setter_raises_exception': is_setter_raises_exception,
        'private_script_cpp_value_to_v8_value': idl_type.cpp_value_to_v8_value(
            'cppValue', isolate='scriptState->isolate()',
            creation_context='scriptState->context()->Global()'),
        'v8_value_to_local_cpp_value': idl_type.v8_value_to_local_cpp_value(
            extended_attributes, 'v8Value', 'cppValue'),
    })

    # setter_expression() depends on context values we set above.
    context['cpp_setter'] = setter_expression(interface, attribute, context)


def setter_expression(interface, attribute, context):
    extended_attributes = attribute.extended_attributes
    arguments = v8_utilities.call_with_arguments(
        extended_attributes.get('SetterCallWith') or
        extended_attributes.get('CallWith'))

    this_setter_base_name = setter_base_name(interface, attribute, arguments)
    setter_name = scoped_name(interface, attribute, this_setter_base_name)

    # Members of IDL partial interface definitions are implemented in C++ as
    # static member functions, which for instance members (non-static members)
    # take *impl as their first argument
    if ('PartialInterfaceImplementedAs' in extended_attributes and
        not 'ImplementedInPrivateScript' in extended_attributes and
        not attribute.is_static):
        arguments.append('*impl')
    idl_type = attribute.idl_type
    if 'ImplementedInPrivateScript' in extended_attributes:
        arguments.append('toLocalFrame(toFrameIfNotDetached(info.GetIsolate()->GetCurrentContext()))')
        arguments.append('impl')
        arguments.append('cppValue')
    elif idl_type.base_type == 'EventHandler':
        getter_name = scoped_name(interface, attribute, cpp_name(attribute))
        context['event_handler_getter_expression'] = '%s(%s)' % (
            getter_name, ', '.join(arguments))
        if (interface.name in ['Window', 'WorkerGlobalScope'] and
            attribute.name == 'onerror'):
            includes.add('bindings/core/v8/V8ErrorHandler.h')
            arguments.append('V8EventListenerList::findOrCreateWrapper<V8ErrorHandler>(v8Value, true, ScriptState::current(info.GetIsolate()))')
        else:
            arguments.append('V8EventListenerList::getEventListener(ScriptState::current(info.GetIsolate()), v8Value, true, ListenerFindOrCreate)')
    elif idl_type.is_interface_type:
        # FIXME: should be able to eliminate WTF::getPtr in most or all cases
        arguments.append('WTF::getPtr(cppValue)')
    else:
        arguments.append('cppValue')
    if context['is_setter_raises_exception']:
        arguments.append('exceptionState')

    return '%s(%s)' % (setter_name, ', '.join(arguments))


CONTENT_ATTRIBUTE_SETTER_NAMES = {
    'boolean': 'setBooleanAttribute',
    'long': 'setIntegralAttribute',
    'unsigned long': 'setUnsignedIntegralAttribute',
}


def setter_base_name(interface, attribute, arguments):
    if 'ImplementedInPrivateScript' in attribute.extended_attributes:
        return '%sAttributeSetter' % uncapitalize(cpp_name(attribute))

    if 'Reflect' not in attribute.extended_attributes:
        return 'set%s' % capitalize(cpp_name(attribute))
    arguments.append(scoped_content_attribute_name(interface, attribute))

    base_idl_type = attribute.idl_type.base_type
    if base_idl_type in CONTENT_ATTRIBUTE_SETTER_NAMES:
        return CONTENT_ATTRIBUTE_SETTER_NAMES[base_idl_type]
    return 'setAttribute'


def scoped_content_attribute_name(interface, attribute):
    content_attribute_name = attribute.extended_attributes['Reflect'] or attribute.name.lower()
    if interface.name.startswith('SVG'):
        namespace = 'SVGNames'
    else:
        namespace = 'HTMLNames'
    includes.add('core/%s.h' % namespace)
    return '%s::%sAttr' % (namespace, content_attribute_name)


################################################################################
# Attribute configuration
################################################################################

# [PutForwards], [Replaceable]
def has_setter(attribute):
    return (not attribute.is_read_only or
            'PutForwards' in attribute.extended_attributes or
            'Replaceable' in attribute.extended_attributes)


# [DoNotCheckSecurity], [Unforgeable]
def access_control_list(interface, attribute):
    extended_attributes = attribute.extended_attributes
    access_control = []
    if 'DoNotCheckSecurity' in extended_attributes:
        do_not_check_security = extended_attributes['DoNotCheckSecurity']
        if do_not_check_security == 'Setter':
            access_control.append('v8::ALL_CAN_WRITE')
        else:
            access_control.append('v8::ALL_CAN_READ')
            if has_setter(attribute):
                access_control.append('v8::ALL_CAN_WRITE')
    if is_unforgeable(interface, attribute):
        access_control.append('v8::PROHIBITS_OVERWRITING')
    return access_control or ['v8::DEFAULT']


# [NotEnumerable], [Unforgeable]
def property_attributes(interface, attribute):
    extended_attributes = attribute.extended_attributes
    property_attributes_list = []
    if ('NotEnumerable' in extended_attributes or
        is_constructor_attribute(attribute)):
        property_attributes_list.append('v8::DontEnum')
    if is_unforgeable(interface, attribute):
        property_attributes_list.append('v8::DontDelete')
    return property_attributes_list or ['v8::None']


# [Custom], [Custom=Getter]
def has_custom_getter(attribute):
    extended_attributes = attribute.extended_attributes
    return ('Custom' in extended_attributes and
            extended_attributes['Custom'] in [None, 'Getter'])


# [Custom], [Custom=Setter]
def has_custom_setter(attribute):
    extended_attributes = attribute.extended_attributes
    return (not attribute.is_read_only and
            'Custom' in extended_attributes and
            extended_attributes['Custom'] in [None, 'Setter'])


# [ExposeJSAccessors]
def is_expose_js_accessors(interface, attribute):
    # Default behavior
    is_accessor = True

    if ('ExposeJSAccessors' in interface.extended_attributes and
            'DoNotExposeJSAccessors' in interface.extended_attributes):
        raise Exception('Both of ExposeJSAccessors and DoNotExposeJSAccessors are specified at a time in an interface: ' + interface.name)
    if 'ExposeJSAccessors' in interface.extended_attributes:
        is_accessor = True
    if 'DoNotExposeJSAccessors' in interface.extended_attributes:
        is_accessor = False

    # Note that ExposeJSAccessors and DoNotExposeJSAccessors are more powerful
    # than 'static', [Unforgeable] and [OverrideBuiltins].
    if ('ExposeJSAccessors' in attribute.extended_attributes and
            'DoNotExposeJSAccessors' in attribute.extended_attributes):
        raise Exception('Both of ExposeJSAccessors and DoNotExposeJSAccessors are specified at a time on an attribute: ' + attribute.name + ' in an interface: ' + interface.name)
    if 'ExposeJSAccessors' in attribute.extended_attributes:
        return True
    if 'DoNotExposeJSAccessors' in attribute.extended_attributes:
        return False

    # These attributes must not be accessors on prototype chains.
    if (is_constructor_attribute(attribute) or
            attribute.is_static or
            is_unforgeable(interface, attribute) or
            'OverrideBuiltins' in interface.extended_attributes):
        return False

    # The members of Window interface must be placed on the instance object.
    if interface.name == 'Window':
        return False

    return is_accessor


################################################################################
# Constructors
################################################################################

idl_types.IdlType.constructor_type_name = property(
    # FIXME: replace this with a [ConstructorAttribute] extended attribute
    lambda self: strip_suffix(self.base_type, 'Constructor'))


def is_constructor_attribute(attribute):
    # FIXME: replace this with [ConstructorAttribute] extended attribute
    return attribute.idl_type.name.endswith('Constructor')


def update_constructor_attribute_context(interface, attribute, context):
    context['needs_constructor_getter_callback'] = context['measure_as'] or context['deprecate_as']
    # When the attribute name is the same as the interface name, do not generate
    # callback functions for each attribute and use
    # {{cpp_class}}ConstructorAttributeSetterCallback.  Otherwise, generate
    # a callback function in order to hard-code the attribute name.
    context['needs_constructor_setter_callback'] = context['name'] != context['constructor_type']
