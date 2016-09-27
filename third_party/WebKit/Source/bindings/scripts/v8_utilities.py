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

"""Functions shared by various parts of the code generator.

Design doc: http://www.chromium.org/developers/design-documents/idl-compiler
"""

import re

from idl_types import IdlTypeBase
import idl_types
from idl_definitions import Exposure, IdlInterface
from v8_globals import includes

ACRONYMS = [
    'CSSOM',  # must come *before* CSS to match full acronym
    'CSS',
    'HTML',
    'IME',
    'JS',
    'SVG',
    'URL',
    'WOFF',
    'XML',
    'XSLT',
]


################################################################################
# Extended attribute parsing
################################################################################

def extended_attribute_value_contains(extended_attribute_value, key):
    return (extended_attribute_value == key or
            (isinstance(extended_attribute_value, list) and
             key in extended_attribute_value))


def has_extended_attribute(definition_or_member, extended_attribute_list):
    return any(extended_attribute in definition_or_member.extended_attributes
               for extended_attribute in extended_attribute_list)


def has_extended_attribute_value(definition_or_member, name, value):
    extended_attributes = definition_or_member.extended_attributes
    return (name in extended_attributes and
            extended_attribute_value_contains(extended_attributes[name], value))


def extended_attribute_value_as_list(definition_or_member, name):
    extended_attributes = definition_or_member.extended_attributes
    if name not in extended_attributes:
        return None
    value = extended_attributes[name]
    if isinstance(value, list):
        return value
    return [value]


################################################################################
# String handling
################################################################################

def capitalize(name):
    """Capitalize first letter or initial acronym (used in setter names)."""
    for acronym in ACRONYMS:
        if name.startswith(acronym.lower()):
            return name.replace(acronym.lower(), acronym)
    return name[0].upper() + name[1:]


def strip_suffix(string, suffix):
    if not suffix or not string.endswith(suffix):
        return string
    return string[:-len(suffix)]


def uncapitalize(name):
    """Uncapitalizes first letter or initial acronym (used in method names).

    E.g., 'SetURL' becomes 'setURL', but 'URLFoo' becomes 'urlFoo'.
    """
    for acronym in ACRONYMS:
        if name.startswith(acronym):
            return name.replace(acronym, acronym.lower())
    return name[0].lower() + name[1:]


################################################################################
# C++
################################################################################

def scoped_name(interface, definition, base_name):
    if 'ImplementedInPrivateScript' in definition.extended_attributes:
        return '%s::PrivateScript::%s' % (v8_class_name(interface), base_name)
    # partial interfaces are implemented as separate classes, with their members
    # implemented as static member functions
    partial_interface_implemented_as = definition.extended_attributes.get('PartialInterfaceImplementedAs')
    if partial_interface_implemented_as:
        return '%s::%s' % (partial_interface_implemented_as, base_name)
    if (definition.is_static or
        definition.name in ('Constructor', 'NamedConstructor')):
        return '%s::%s' % (cpp_name(interface), base_name)
    return 'impl->%s' % base_name


def v8_class_name(interface):
    return 'V8' + interface.name


def v8_class_name_or_partial(interface):
    class_name = v8_class_name(interface)
    if interface.is_partial:
        return ''.join([class_name, 'Partial'])
    return class_name


################################################################################
# Specific extended attributes
################################################################################

# [ActivityLogging]
def activity_logging_world_list(member, access_type=''):
    """Returns a set of world suffixes for which a definition member has activity logging, for specified access type.

    access_type can be 'Getter' or 'Setter' if only checking getting or setting.
    """
    extended_attributes = member.extended_attributes
    if 'LogActivity' not in extended_attributes:
        return set()
    log_activity = extended_attributes['LogActivity']
    if log_activity and not log_activity.startswith(access_type):
        return set()

    includes.add('bindings/core/v8/V8DOMActivityLogger.h')
    if 'LogAllWorlds' in extended_attributes:
        return set(['', 'ForMainWorld'])
    return set([''])  # At minimum, include isolated worlds.


# [ActivityLogging]
def activity_logging_world_check(member):
    """Returns if an isolated world check is required when generating activity
    logging code.

    The check is required when there is no per-world binding code and logging is
    required only for isolated world.
    """
    extended_attributes = member.extended_attributes
    if 'LogActivity' not in extended_attributes:
        return False
    if ('PerWorldBindings' not in extended_attributes and
        'LogAllWorlds' not in extended_attributes):
        return True
    return False


# [CallWith]
CALL_WITH_ARGUMENTS = {
    'ScriptState': 'scriptState',
    'ExecutionContext': 'executionContext',
    'ScriptArguments': 'scriptArguments.release()',
    'ActiveWindow': 'callingDOMWindow(info.GetIsolate())',
    'FirstWindow': 'enteredDOMWindow(info.GetIsolate())',
    'Document': 'document',
    'ThisValue': 'ScriptValue(scriptState, info.This())',
}
# List because key order matters, as we want arguments in deterministic order
CALL_WITH_VALUES = [
    'ScriptState',
    'ExecutionContext',
    'ScriptArguments',
    'ActiveWindow',
    'FirstWindow',
    'Document',
    'ThisValue',
]


def call_with_arguments(call_with_values):
    if not call_with_values:
        return []
    return [CALL_WITH_ARGUMENTS[value]
            for value in CALL_WITH_VALUES
            if extended_attribute_value_contains(call_with_values, value)]


# [Conditional]
DELIMITER_TO_OPERATOR = {
    '|': '||',
    ',': '&&',
}


def conditional_string(definition_or_member):
    extended_attributes = definition_or_member.extended_attributes
    if 'Conditional' not in extended_attributes:
        return None
    return 'ENABLE(%s)' % extended_attributes['Conditional']


# [Constructor], [NamedConstructor]
def is_constructor_attribute(member):
    # TODO(yukishiino): replace this with [Constructor] and [NamedConstructor] extended attribute
    return member.idl_type.name.endswith('Constructor')


# [DeprecateAs]
def deprecate_as(member):
    extended_attributes = member.extended_attributes
    if 'DeprecateAs' not in extended_attributes:
        return None
    includes.add('core/frame/UseCounter.h')
    return extended_attributes['DeprecateAs']


# [Exposed]
EXPOSED_EXECUTION_CONTEXT_METHOD = {
    'CompositorWorker': 'isCompositorWorkerGlobalScope',
    'DedicatedWorker': 'isDedicatedWorkerGlobalScope',
    'ServiceWorker': 'isServiceWorkerGlobalScope',
    'SharedWorker': 'isSharedWorkerGlobalScope',
    'Window': 'isDocument',
    'Worker': 'isWorkerGlobalScope',
}


EXPOSED_WORKERS = set([
    'CompositorWorker',
    'DedicatedWorker',
    'SharedWorker',
    'ServiceWorker',
])


class ExposureSet:
    """An ExposureSet is a collection of Exposure instructions."""
    def __init__(self, exposures=None):
        self.exposures = set(exposures) if exposures else set()

    def issubset(self, other):
        """Returns true if |self|'s exposure set is a subset of
        |other|'s exposure set. This function doesn't care about
        RuntimeEnabled."""
        self_set = self._extended(set(e.exposed for e in self.exposures))
        other_set = self._extended(set(e.exposed for e in other.exposures))
        return self_set.issubset(other_set)

    @staticmethod
    def _extended(target):
        if EXPOSED_WORKERS.issubset(target):
            return target | set(['Worker'])
        elif 'Worker' in target:
            return target | EXPOSED_WORKERS
        return target

    def add(self, exposure):
        self.exposures.add(exposure)

    def __len__(self):
        return len(self.exposures)

    def __iter__(self):
        return self.exposures.__iter__()

    @staticmethod
    def _code(exposure):
        exposed = ('context->%s()' %
                   EXPOSED_EXECUTION_CONTEXT_METHOD[exposure.exposed])
        if exposure.runtime_enabled is not None:
            runtime_enabled = ('RuntimeEnabledFeatures::%sEnabled()' %
                               uncapitalize(exposure.runtime_enabled))
            return '({0} && {1})'.format(exposed, runtime_enabled)
        return exposed

    def code(self):
        if len(self.exposures) == 0:
            return None
        # We use sorted here to deflake output.
        return ' || '.join(sorted(self._code(e) for e in self.exposures))


def exposed(member, interface):
    """Returns a C++ code that checks if a method/attribute/etc is exposed.

    When the Exposed attribute contains RuntimeEnabledFeatures (i.e.
    Exposed(Arguments) form is given), the code contains check for them as
    well.

    EXAMPLE: [Exposed=Window, RuntimeEnabledFeature=Feature1]
      => context->isDocument()

    EXAMPLE: [Exposed(Window Feature1, Window Feature2)]
      => context->isDocument() && RuntimeEnabledFeatures::feature1Enabled() ||
         context->isDocument() && RuntimeEnabledFeatures::feature2Enabled()
    """
    exposure_set = ExposureSet(
        extended_attribute_value_as_list(member, 'Exposed'))
    interface_exposure_set = ExposureSet(
        extended_attribute_value_as_list(interface, 'Exposed'))
    for e in exposure_set:
        if e.exposed not in EXPOSED_EXECUTION_CONTEXT_METHOD:
            raise ValueError('Invalid execution context: %s' % e.exposed)

    # Methods must not be exposed to a broader scope than their interface.
    if not exposure_set.issubset(interface_exposure_set):
        raise ValueError('Interface members\' exposure sets must be a subset of the interface\'s.')

    return exposure_set.code()


# [GarbageCollected], [WillBeGarbageCollected]
def gc_type(definition):
    extended_attributes = definition.extended_attributes
    if 'GarbageCollected' in extended_attributes:
        return 'GarbageCollectedObject'
    elif 'WillBeGarbageCollected' in extended_attributes:
        return 'WillBeGarbageCollectedObject'
    return 'RefCountedObject'


# [ImplementedAs]
def cpp_name(definition_or_member):
    extended_attributes = definition_or_member.extended_attributes
    if 'ImplementedAs' not in extended_attributes:
        return definition_or_member.name
    return extended_attributes['ImplementedAs']


def cpp_name_from_interfaces_info(name, interfaces_info):
    return interfaces_info.get(name, {}).get('implemented_as') or name


def cpp_name_or_partial(interface):
    cpp_class_name = cpp_name(interface)
    if interface.is_partial:
        return ''.join([cpp_class_name, 'Partial'])
    return cpp_class_name


# [MeasureAs]
def measure_as(definition_or_member, interface):
    extended_attributes = definition_or_member.extended_attributes
    if 'MeasureAs' in extended_attributes:
        includes.add('core/frame/UseCounter.h')
        return lambda suffix: extended_attributes['MeasureAs']
    if 'Measure' in extended_attributes:
        includes.add('core/frame/UseCounter.h')
        measure_as_name = capitalize(definition_or_member.name)
        if interface is not None:
            measure_as_name = '%s_%s' % (capitalize(interface.name), measure_as_name)
        return lambda suffix: 'V8%s_%s' % (measure_as_name, suffix)
    return None


# [RuntimeEnabled]
def runtime_enabled_function_name(definition_or_member):
    """Returns the name of the RuntimeEnabledFeatures function.

    The returned function checks if a method/attribute is enabled.
    Given extended attribute RuntimeEnabled=FeatureName, return:
        RuntimeEnabledFeatures::{featureName}Enabled
    """
    extended_attributes = definition_or_member.extended_attributes
    if 'RuntimeEnabled' not in extended_attributes:
        return None
    feature_name = extended_attributes['RuntimeEnabled']
    return 'RuntimeEnabledFeatures::%sEnabled' % uncapitalize(feature_name)


# [Unforgeable]
def is_unforgeable(interface, member):
    return (('Unforgeable' in interface.extended_attributes or
             'Unforgeable' in member.extended_attributes) and
            not member.is_static)


# [TypeChecking=Interface] / [LegacyInterfaceTypeChecking]
def is_legacy_interface_type_checking(interface, member):
    if not ('TypeChecking' in interface.extended_attributes or
            'TypeChecking' in member.extended_attributes):
        return True
    if 'LegacyInterfaceTypeChecking' in member.extended_attributes:
        return True
    return False


# [Unforgeable], [Global], [PrimaryGlobal] and [DoNotExposeJSAccessors]
def on_instance(interface, member):
    """Returns True if the interface's member needs to be defined on every
    instance object.

    The following members must be defiend on an instance object.
    - [Unforgeable] members
    - regular members of [Global] or [PrimaryGlobal] interfaces
    - members on which [DoNotExposeJSAccessors] is specified
    """
    # TODO(yukishiino): Implement this function following the spec.
    return not on_prototype(interface, member)


# [ExposeJSAccessors]
def on_prototype(interface, member):
    """Returns True if the interface's member needs to be defined on the
    prototype object.

    Most members are defined on the prototype object.  Exceptions are as
    follows.
    - constant members
    - static members (optional)
    - [Unforgeable] members
    - members of [Global] or [PrimaryGlobal] interfaces
    - named properties of [Global] or [PrimaryGlobal] interfaces
    However, if [ExposeJSAccessors] is specified, the member is defined on the
    prototype object.
    """
    # TODO(yukishiino): Implement this function following the spec.

    if ('ExposeJSAccessors' in interface.extended_attributes and
            'DoNotExposeJSAccessors' in interface.extended_attributes):
        raise Exception('Both of ExposeJSAccessors and DoNotExposeJSAccessors are specified at a time in an interface: ' + interface.name)
    if ('ExposeJSAccessors' in member.extended_attributes and
            'DoNotExposeJSAccessors' in member.extended_attributes):
        raise Exception('Both of ExposeJSAccessors and DoNotExposeJSAccessors are specified at a time on a member: ' + member.name + ' in an interface: ' + interface.name)

    # Note that ExposeJSAccessors and DoNotExposeJSAccessors are more powerful
    # than 'static', [Unforgeable] and [OverrideBuiltins].
    if 'ExposeJSAccessors' in member.extended_attributes:
        return True
    if 'DoNotExposeJSAccessors' in member.extended_attributes:
        return False

    # These members must not be placed on prototype chains.
    if (is_constructor_attribute(member) or
            member.is_static or
            is_unforgeable(interface, member) or
            'OverrideBuiltins' in interface.extended_attributes):
        return False

    # TODO(yukishiino): We should handle [Global] and [PrimaryGlobal] instead of
    # Window.
    if (interface.name == 'Window'):
        return member.idl_type.name == 'EventHandler'

    if 'ExposeJSAccessors' in interface.extended_attributes:
        return True
    if 'DoNotExposeJSAccessors' in interface.extended_attributes:
        return False

    return True


# static, const
def on_interface(interface, member):
    """Returns True if the interface's member needs to be defined on the
    interface object.

    The following members must be defiend on an interface object.
    - constant members
    - static members
    """
    # TODO(yukishiino): Implement this function following the spec.
    return False


################################################################################
# Indexed properties
# http://heycam.github.io/webidl/#idl-indexed-properties
################################################################################

def indexed_property_getter(interface):
    try:
        # Find indexed property getter, if present; has form:
        # getter TYPE [OPTIONAL_IDENTIFIER](unsigned long ARG1)
        return next(
            method
            for method in interface.operations
            if ('getter' in method.specials and
                len(method.arguments) == 1 and
                str(method.arguments[0].idl_type) == 'unsigned long'))
    except StopIteration:
        return None


def indexed_property_setter(interface):
    try:
        # Find indexed property setter, if present; has form:
        # setter RETURN_TYPE [OPTIONAL_IDENTIFIER](unsigned long ARG1, ARG_TYPE ARG2)
        return next(
            method
            for method in interface.operations
            if ('setter' in method.specials and
                len(method.arguments) == 2 and
                str(method.arguments[0].idl_type) == 'unsigned long'))
    except StopIteration:
        return None


def indexed_property_deleter(interface):
    try:
        # Find indexed property deleter, if present; has form:
        # deleter TYPE [OPTIONAL_IDENTIFIER](unsigned long ARG)
        return next(
            method
            for method in interface.operations
            if ('deleter' in method.specials and
                len(method.arguments) == 1 and
                str(method.arguments[0].idl_type) == 'unsigned long'))
    except StopIteration:
        return None


################################################################################
# Named properties
# http://heycam.github.io/webidl/#idl-named-properties
################################################################################

def named_property_getter(interface):
    try:
        # Find named property getter, if present; has form:
        # getter TYPE [OPTIONAL_IDENTIFIER](DOMString ARG1)
        getter = next(
            method
            for method in interface.operations
            if ('getter' in method.specials and
                len(method.arguments) == 1 and
                str(method.arguments[0].idl_type) == 'DOMString'))
        getter.name = getter.name or 'anonymousNamedGetter'
        return getter
    except StopIteration:
        return None


def named_property_setter(interface):
    try:
        # Find named property setter, if present; has form:
        # setter RETURN_TYPE [OPTIONAL_IDENTIFIER](DOMString ARG1, ARG_TYPE ARG2)
        return next(
            method
            for method in interface.operations
            if ('setter' in method.specials and
                len(method.arguments) == 2 and
                str(method.arguments[0].idl_type) == 'DOMString'))
    except StopIteration:
        return None


def named_property_deleter(interface):
    try:
        # Find named property deleter, if present; has form:
        # deleter TYPE [OPTIONAL_IDENTIFIER](DOMString ARG)
        return next(
            method
            for method in interface.operations
            if ('deleter' in method.specials and
                len(method.arguments) == 1 and
                str(method.arguments[0].idl_type) == 'DOMString'))
    except StopIteration:
        return None


IdlInterface.indexed_property_getter = property(indexed_property_getter)
IdlInterface.indexed_property_setter = property(indexed_property_setter)
IdlInterface.indexed_property_deleter = property(indexed_property_deleter)
IdlInterface.named_property_getter = property(named_property_getter)
IdlInterface.named_property_setter = property(named_property_setter)
IdlInterface.named_property_deleter = property(named_property_deleter)
