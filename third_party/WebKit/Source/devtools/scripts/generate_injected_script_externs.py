#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import re

interface_name_map = {
    'InjectedScriptHost': 'InjectedScriptHostClass'
}

type_map = {
    'any': '*',
    'DOMString': 'string',
    'short': 'number',
    'unsigned short': 'number',
    'long': 'number',
    'unsigned long': 'number',
    'boolean': 'boolean',
    'object': 'Object',
    'void': ''
}

idl_type_exprs = [
    r'any',
    r'DOMString',
    r'short',
    r'unsigned\s+short',
    r'long',
    r'unsigned\s+long',
    r'boolean',
    r'object',
    r'void',
    r'\w+'  # Non IDL-specific object types.
]

# Groups:
# 1: type name
# 2: array (optional)
# 3: nullable (optional)
type_expr = r'\b(' + r'|'.join(idl_type_exprs) + r')\b(\[\])?(\?)?'

# Groups:
# 1: return type
# 2:   array (optional)
# 3:   nullable (optional)
# 4: method name
# 5: method arguments
method_expr = r'^\s*(?:\[.+\])?\s+' + type_expr + r'\s+(\w+)\s*\(([^)]*)\)\s*;\s*$'
method_regex = re.compile(method_expr)

# Groups:
# 1: type name
# 2:   array (optional)
# 3:   nullable (optional)
# 4: attribute name
attribute_expr = r'^\s*(?:\[.+\]\s+)?(?:\breadonly\s+)?\battribute\s+' + type_expr + r'\s+(\w+)\s*;'
attribute_regex = re.compile(attribute_expr)

# Groups:
# 1: optional (optional)
# 2: type name
# 3: array (optional)
# 4: nullable (optional)
# 5: arg name
arg_regex = re.compile(r'\s*(?:\[[^]]+\]\s*)?(\boptional\s+)?' + type_expr + r'\s+(\w+)')

interface_regex = r'\binterface\s+(\w+)'

other_externs = """
/** @type {!Window} */
var inspectedWindow;
/** @type {number} */
var injectedScriptId;
"""


class Type:
    def __init__(self, type_name, is_array, is_nullable):
        self.type_name = re.sub(r'\s+', ' ', type_name)
        self.is_array = is_array
        self.is_nullable = is_nullable

    def as_js_type(self):
        if self.type_name == 'void':
            return ''
        result = ''
        if self.is_nullable:
            result = '?'
        elif self._is_object_type():
            result = '!'
        if self.is_array:
            result += 'Array.<%s>' % Type(self.type_name, False, False).as_js_type()
        else:
            result += type_map.get(self.type_name, self.type_name)
        return result

    def _is_object_type(self):
        return self.is_array or self.type_name == 'object' or not type_map.get(self.type_name)


class Attribute:
    def __init__(self, type, name):
        self.type = type
        self.name = name


class Argument:
    def __init__(self, type, optional, name):
        self.type = type
        self.optional = optional
        self.name = name

    def as_js_param_type(self):
        result = self.type.as_js_type()
        if self.optional:
            result += '='
        return result


class Method:
    def __init__(self, return_type, name, args):
        self.return_type = return_type
        self.name = name
        self.args = args

    def js_argument_names(self):
        result = []
        for arg in self.args:
            result.append(arg.name)
        return ', '.join(result)


class Interface:
    def __init__(self, name, methods, attributes):
        self.name = name
        self.methods = methods
        self.attributes = attributes


def parse_args(text):
    arguments = []
    for (optional, type_name, is_array, is_nullable, arg_name) in re.findall(arg_regex, text):
        arguments.append(Argument(Type(type_name, is_array, is_nullable), optional != '', arg_name))
    return arguments


def read_interface(idl):
    methods = []
    attributes = []
    with open(idl, "r") as input_file:
        for line in input_file.readlines():
            match = re.search(method_regex, line)
            if match:
                return_type = Type(match.group(1), match.group(2) is not None, match.group(3) is not None)
                name = match.group(4)
                methods.append(Method(return_type, name, parse_args(match.group(5))))
                continue
            match = re.search(attribute_regex, line)
            if match:
                type = Type(match.group(1), match.group(2) is not None, match.group(3) is not None)
                name = match.group(4)
                attributes.append(Attribute(type, name))
                continue
            match = re.search(interface_regex, line)
            if match:
                interface_name = match.group(1)
    return Interface(interface_name, methods, attributes)


def generate_injected_script_externs(input_idls, output):
    for idl in input_idls:
        ifc = read_interface(idl)
        interface_name = interface_name_map.get(ifc.name, ifc.name)
        output.write('/** @interface */\nfunction %s()\n{\n' % interface_name)
        for attribute in ifc.attributes:
            output.write('    /** @type {%s} */\n' % attribute.type.as_js_type())
            output.write('    this.%s;\n' % attribute.name)
        output.write('}\n')
        for method in ifc.methods:
            output.write('\n/**\n')
            for arg in method.args:
                output.write(' * @param {%s} %s\n' % (arg.as_js_param_type(), arg.name))
            return_type = method.return_type.as_js_type()
            if return_type:
                output.write(' * @return {%s}\n' % return_type)
            output.write(' */\n')
            output.write('%s.prototype.%s = function(%s) {}\n' % (interface_name, method.name, method.js_argument_names()))
        if interface_name != ifc.name:
            output.write('\n/** @type {!%s} */\nvar %s;\n' % (interface_name, ifc.name))
        output.write('\n')
    output.write(other_externs)


def generate_injected_script_externs_to_file(input_idls, output_name):
    with open(output_name, 'w') as output:
        generate_injected_script_externs(input_idls, output)


def main(argv):
    import os.path
    program_name = os.path.basename(__file__)
    if len(argv) < 3:
        sys.stderr.write("Usage: %s IDL_1 ... IDL_N OUTPUT_FILE\n" % program_name)
        exit(1)
    input_idls = argv[1:-1]
    generate_injected_script_externs_to_file(input_idls, argv[-1])


if __name__ == "__main__":
    import sys
    sys.exit(main(sys.argv))
