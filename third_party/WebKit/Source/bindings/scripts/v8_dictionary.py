# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Generate template contexts of dictionaries for both v8 bindings and
implementation classes that are used by blink's core/modules.
"""

import operator
from idl_types import IdlType
from v8_globals import includes
import v8_types
import v8_utilities
from v8_utilities import has_extended_attribute_value


DICTIONARY_H_INCLUDES = frozenset([
    'bindings/core/v8/ToV8.h',
    'bindings/core/v8/V8Binding.h',
    'platform/heap/Handle.h',
])

DICTIONARY_CPP_INCLUDES = frozenset([
    'bindings/core/v8/ExceptionState.h',
])


def setter_name_for_dictionary_member(member):
    name = v8_utilities.cpp_name(member)
    return 'set%s' % v8_utilities.capitalize(name)


def null_setter_name_for_dictionary_member(member):
    if member.idl_type.is_nullable:
        name = v8_utilities.cpp_name(member)
        return 'set%sToNull' % v8_utilities.capitalize(name)
    return None


def has_method_name_for_dictionary_member(member):
    name = v8_utilities.cpp_name(member)
    return 'has%s' % v8_utilities.capitalize(name)


def unwrap_nullable_if_needed(idl_type):
    if idl_type.is_nullable:
        return idl_type.inner_type
    return idl_type


# Context for V8 bindings

def dictionary_context(dictionary, interfaces_info):
    includes.clear()
    includes.update(DICTIONARY_CPP_INCLUDES)
    cpp_class = v8_utilities.cpp_name(dictionary)
    context = {
        'cpp_class': cpp_class,
        'header_includes': set(DICTIONARY_H_INCLUDES),
        'members': [member_context(dictionary, member)
                    for member in sorted(dictionary.members,
                                         key=operator.attrgetter('name'))],
        'use_permissive_dictionary_conversion': 'PermissiveDictionaryConversion' in dictionary.extended_attributes,
        'v8_class': v8_types.v8_type(cpp_class),
        'v8_original_class': v8_types.v8_type(dictionary.name),
    }
    if dictionary.parent:
        IdlType(dictionary.parent).add_includes_for_type()
        parent_cpp_class = v8_utilities.cpp_name_from_interfaces_info(
            dictionary.parent, interfaces_info)
        context.update({
            'parent_cpp_class': parent_cpp_class,
            'parent_v8_class': v8_types.v8_type(parent_cpp_class),
        })
    return context


def member_context(dictionary, member):
    extended_attributes = member.extended_attributes
    idl_type = member.idl_type
    idl_type.add_includes_for_type(extended_attributes)
    unwrapped_idl_type = unwrap_nullable_if_needed(idl_type)

    if member.is_required and member.default_value:
        raise Exception(
            'Required member %s must not have a default value.' % member.name)

    def default_values():
        if not member.default_value:
            return None, None
        if member.default_value.is_null:
            return None, 'v8::Null(isolate)'
        cpp_default_value = unwrapped_idl_type.literal_cpp_value(
            member.default_value)
        v8_default_value = unwrapped_idl_type.cpp_value_to_v8_value(
            cpp_value=cpp_default_value, isolate='isolate',
            creation_context='creationContext')
        return cpp_default_value, v8_default_value

    cpp_default_value, v8_default_value = default_values()
    cpp_name = v8_utilities.cpp_name(member)

    return {
        'cpp_default_value': cpp_default_value,
        'cpp_name': cpp_name,
        'cpp_type': unwrapped_idl_type.cpp_type,
        'cpp_value_to_v8_value': unwrapped_idl_type.cpp_value_to_v8_value(
            cpp_value='impl.%s()' % cpp_name, isolate='isolate',
            creation_context='creationContext',
            extended_attributes=extended_attributes),
        'deprecate_as': v8_utilities.deprecate_as(member),
        'enum_type': idl_type.enum_type,
        'enum_values': unwrapped_idl_type.enum_values,
        'has_method_name': has_method_name_for_dictionary_member(member),
        'idl_type': idl_type.base_type,
        'is_interface_type': idl_type.is_interface_type and not idl_type.is_dictionary,
        'is_nullable': idl_type.is_nullable,
        'is_object': unwrapped_idl_type.name == 'Object',
        'is_required': member.is_required,
        'name': member.name,
        'setter_name': setter_name_for_dictionary_member(member),
        'null_setter_name': null_setter_name_for_dictionary_member(member),
        'v8_default_value': v8_default_value,
        'v8_value_to_local_cpp_value': unwrapped_idl_type.v8_value_to_local_cpp_value(
            extended_attributes, member.name + 'Value',
            member.name, isolate='isolate', use_exception_state=True),
    }


# Context for implementation classes

def dictionary_impl_context(dictionary, interfaces_info):
    def remove_duplicate_members(members):
        # When [ImplementedAs] is used, cpp_name can conflict. For example,
        # dictionary D { long foo; [ImplementedAs=foo, DeprecateAs=Foo] long oldFoo; };
        # This function removes such duplications, checking they have the same type.
        members_dict = {}
        for member in members:
            cpp_name = member['cpp_name']
            duplicated_member = members_dict.get(cpp_name)
            if duplicated_member and duplicated_member != member:
                raise Exception('Member name conflict: %s' % cpp_name)
            members_dict[cpp_name] = member
        return sorted(members_dict.values(), key=lambda member: member['cpp_name'])

    includes.clear()
    header_includes = set(['platform/heap/Handle.h'])
    members = [member_impl_context(member, interfaces_info, header_includes)
               for member in dictionary.members]
    members = remove_duplicate_members(members)
    context = {
        'header_includes': header_includes,
        'cpp_class': v8_utilities.cpp_name(dictionary),
        'members': members,
    }
    if dictionary.parent:
        context['parent_cpp_class'] = v8_utilities.cpp_name_from_interfaces_info(
            dictionary.parent, interfaces_info)
        parent_interface_info = interfaces_info.get(dictionary.parent)
        if parent_interface_info:
            context['header_includes'].add(
                parent_interface_info['include_path'])
    return context


def member_impl_context(member, interfaces_info, header_includes):
    idl_type = unwrap_nullable_if_needed(member.idl_type)
    cpp_name = v8_utilities.cpp_name(member)

    def getter_expression():
        if idl_type.impl_should_use_nullable_container:
            return 'm_%s.get()' % cpp_name
        return 'm_%s' % cpp_name

    def has_method_expression():
        if idl_type.impl_should_use_nullable_container or idl_type.is_enum or idl_type.is_string_type or idl_type.is_union_type:
            return '!m_%s.isNull()' % cpp_name
        elif idl_type.name in ['Any', 'Object']:
            return '!(m_{0}.isEmpty() || m_{0}.isNull() || m_{0}.isUndefined())'.format(cpp_name)
        else:
            return 'm_%s' % cpp_name

    def member_cpp_type():
        member_cpp_type = idl_type.cpp_type_args(used_in_cpp_sequence=True)
        if idl_type.impl_should_use_nullable_container:
            return v8_types.cpp_template_type('Nullable', member_cpp_type)
        return member_cpp_type

    cpp_default_value = None
    if member.default_value and not member.default_value.is_null:
        cpp_default_value = idl_type.literal_cpp_value(member.default_value)

    header_includes.update(idl_type.impl_includes_for_type(interfaces_info))
    return {
        'cpp_default_value': cpp_default_value,
        'cpp_name': cpp_name,
        'getter_expression': getter_expression(),
        'has_method_expression': has_method_expression(),
        'has_method_name': has_method_name_for_dictionary_member(member),
        'is_nullable': idl_type.is_nullable,
        'is_traceable': idl_type.is_traceable,
        'member_cpp_type': member_cpp_type(),
        'null_setter_name': null_setter_name_for_dictionary_member(member),
        'rvalue_cpp_type': idl_type.cpp_type_args(used_as_rvalue_type=True),
        'setter_name': setter_name_for_dictionary_member(member),
    }
