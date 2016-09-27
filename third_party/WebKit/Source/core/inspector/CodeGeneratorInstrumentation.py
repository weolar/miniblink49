#!/usr/bin/env python
# Copyright (c) 2013 Google Inc. All rights reserved.
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

import optparse
import re
import string
import sys

template_h = string.Template("""// Code generated from InspectorInstrumentation.idl

#ifndef ${file_name}_h
#define ${file_name}_h

${includes}

namespace blink {

${forward_declarations}

namespace InspectorInstrumentation {

$methods
} // namespace InspectorInstrumentation

} // namespace blink

#endif // !defined(${file_name}_h)
""")

template_inline = string.Template("""
inline void ${name}(${params_public})
{   ${fast_return}
    if (${condition})
        ${name}Impl(${params_impl});
}
""")

template_inline_forward = string.Template("""
inline void ${name}(${params_public})
{   ${fast_return}
    ${name}Impl(${params_impl});
}
""")

template_inline_returns_value = string.Template("""
inline ${return_type} ${name}(${params_public})
{   ${fast_return}
    if (${condition})
        return ${name}Impl(${params_impl});
    return ${default_return_value};
}
""")


template_cpp = string.Template("""// Code generated from InspectorInstrumentation.idl

#include "config.h"

${includes}

namespace blink {
${extra_definitions}

namespace InspectorInstrumentation {
$methods

} // namespace InspectorInstrumentation

} // namespace blink
""")

template_outofline = string.Template("""
${return_type} ${name}Impl(${params_impl})
{${impl_lines}
}""")

template_agent_call = string.Template("""
    if (${agent_class}* agent = ${agent_fetch})
        ${maybe_return}agent->${name}(${params_agent});""")

template_instrumenting_agents_h = string.Template("""// Code generated from InspectorInstrumentation.idl

#ifndef InstrumentingAgentsInl_h
#define InstrumentingAgentsInl_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/FastAllocBase.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

${forward_list}

class CORE_EXPORT InstrumentingAgents : public RefCountedWillBeGarbageCollectedFinalized<InstrumentingAgents> {
    WTF_MAKE_NONCOPYABLE(InstrumentingAgents);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InstrumentingAgents);
public:
    static PassRefPtrWillBeRawPtr<InstrumentingAgents> create()
    {
        return adoptRefWillBeNoop(new InstrumentingAgents());
    }
    ~InstrumentingAgents() { }
    DECLARE_TRACE();
    void reset();

${accessor_list}

private:
    InstrumentingAgents();

${member_list}
};

}

#endif // !defined(InstrumentingAgentsInl_h)
""")

template_instrumenting_agent_accessor = string.Template("""
    ${class_name}* ${getter_name}() const { return ${member_name}; }
    void set${class_name}(${class_name}* agent) { ${member_name} = agent; }""")

template_instrumenting_agents_cpp = string.Template("""
InstrumentingAgents::InstrumentingAgents()
    : $init_list
{
}

DEFINE_TRACE(InstrumentingAgents)
{
    $trace_list
}

void InstrumentingAgents::reset()
{
    $reset_list
}""")



def match_and_consume(pattern, source):
    match = re.match(pattern, source)
    if match:
        return match, source[len(match.group(0)):].strip()
    return None, source


def load_model_from_idl(source):
    source = re.sub("//.*", "", source)  # Remove line comments
    source = re.sub("/\*(.|\n)*?\*/", "", source, re.MULTILINE)  # Remove block comments
    source = re.sub("\]\s*?\n\s*", "] ", source)  # Merge the method annotation with the next line
    source = source.strip()

    model = []

    while len(source):
        match, source = match_and_consume("interface\s(\w*)\s?\{([^\{]*)\}", source)
        if not match:
            sys.stderr.write("Cannot parse %s\n" % source[:100])
            sys.exit(1)
        model.append(File(match.group(1), match.group(2)))

    return model


class File:
    def __init__(self, name, source):
        self.name = name
        self.header_name = self.name + "Inl"
        self.includes = [include_inspector_header("InspectorInstrumentation")]
        self.forward_declarations = []
        self.declarations = []
        for line in map(str.strip, source.split("\n")):
            line = re.sub("\s{2,}", " ", line).strip()  # Collapse whitespace
            if len(line) == 0:
                continue
            if line[0] == "#":
                self.includes.append(line)
            elif line.startswith("class "):
                self.forward_declarations.append(line)
            else:
                self.declarations.append(Method(line))
        self.includes.sort()
        self.forward_declarations.sort()

    def generate(self, cpp_lines, used_agents):
        header_lines = []
        for declaration in self.declarations:
            for agent in set(declaration.agents):
                used_agents.add(agent)
            declaration.generate_header(header_lines)
            declaration.generate_cpp(cpp_lines)

        return template_h.substitute(None,
                                     file_name=self.header_name,
                                     includes="\n".join(self.includes),
                                     forward_declarations="\n".join(self.forward_declarations),
                                     methods="\n".join(header_lines))


class Method:
    def __init__(self, source):
        match = re.match("(\[[\w|,|=|\s]*\])?\s?(\w*\*?) (\w*)\((.*)\)\s?;", source)
        if not match:
            sys.stderr.write("Cannot parse %s\n" % source)
            sys.exit(1)

        self.options = []
        if match.group(1):
            options_str = re.sub("\s", "", match.group(1)[1:-1])
            if len(options_str) != 0:
                self.options = options_str.split(",")

        self.return_type = match.group(2)

        self.name = match.group(3)

        # Splitting parameters by a comma, assuming that attribute lists contain no more than one attribute.
        self.params = map(Parameter, map(str.strip, match.group(4).split(",")))

        self.accepts_cookie = len(self.params) and self.params[0].type == "const InspectorInstrumentationCookie&"
        self.returns_cookie = self.return_type == "InspectorInstrumentationCookie"

        self.returns_value = self.return_type != "void"

        if self.return_type == "bool":
            self.default_return_value = "false"
        elif self.return_type == "int":
            self.default_return_value = "0"
        elif self.return_type == "String":
            self.default_return_value = "\"\""
        else:
            self.default_return_value = self.return_type + "()"

        for param in self.params:
            if "DefaultReturn" in param.options:
                self.default_return_value = param.name

        self.params_impl = self.params
        if not self.accepts_cookie and not "Inline=Forward" in self.options:
            if not "Keep" in self.params_impl[0].options:
                self.params_impl = self.params_impl[1:]
            self.params_impl = [Parameter("InstrumentingAgents* agents")] + self.params_impl

        self.agents = filter(lambda option: not "=" in option, self.options)

    def generate_header(self, header_lines):
        if "Inline=Custom" in self.options:
            return

        header_lines.append("CORE_EXPORT %s %sImpl(%s);" % (
            self.return_type, self.name, ", ".join(map(Parameter.to_str_class, self.params_impl))))

        if "Inline=FastReturn" in self.options or "Inline=Forward" in self.options:
            fast_return = "\n    FAST_RETURN_IF_NO_FRONTENDS(%s);" % self.default_return_value
        else:
            fast_return = ""

        for param in self.params:
            if "FastReturn" in param.options:
                fast_return += "\n    if (!%s)\n        return %s;" % (param.name, self.default_return_value)

        if self.accepts_cookie:
            condition = "%s.isValid()" % self.params_impl[0].name
            template = template_inline
        elif "Inline=Forward" in self.options:
            condition = ""
            template = template_inline_forward
        else:
            condition = "InstrumentingAgents* agents = instrumentingAgentsFor(%s)" % self.params[0].name

            if self.returns_value:
                template = template_inline_returns_value
            else:
                template = template_inline

        header_lines.append(template.substitute(
            None,
            name=self.name,
            fast_return=fast_return,
            return_type=self.return_type,
            default_return_value=self.default_return_value,
            params_public=", ".join(map(Parameter.to_str_full, self.params)),
            params_impl=", ".join(map(Parameter.to_str_name, self.params_impl)),
            condition=condition))

    def generate_cpp(self, cpp_lines):
        if len(self.agents) == 0:
            return

        body_lines = map(self.generate_ref_ptr, self.params)
        body_lines += map(self.generate_agent_call, self.agents)

        if self.returns_cookie:
            body_lines.append("\n    return InspectorInstrumentationCookie(agents);")
        elif self.returns_value:
            body_lines.append("\n    return %s;" % self.default_return_value)

        generated_outofline = template_outofline.substitute(
            None,
            return_type=self.return_type,
            name=self.name,
            params_impl=", ".join(map(Parameter.to_str_class_and_name, self.params_impl)),
            impl_lines="".join(body_lines))
        if generated_outofline not in cpp_lines:
            cpp_lines.append(generated_outofline)

    def generate_agent_call(self, agent):
        agent_class, agent_getter = agent_getter_signature(agent)

        leading_param_name = self.params_impl[0].name
        if not self.accepts_cookie:
            agent_fetch = "%s->%s()" % (leading_param_name, agent_getter)
        else:
            agent_fetch = "%s.instrumentingAgents()->%s()" % (leading_param_name, agent_getter)

        template = template_agent_call

        if not self.returns_value or self.returns_cookie:
            maybe_return = ""
        else:
            maybe_return = "return "

        return template.substitute(
            None,
            name=self.name,
            agent_class=agent_class,
            agent_fetch=agent_fetch,
            maybe_return=maybe_return,
            params_agent=", ".join(map(Parameter.to_str_value, self.params_impl)[1:]))

    def generate_ref_ptr(self, param):
        if param.is_prp:
            return "\n    RefPtr<%s> %s = %s;" % (param.inner_type, param.value, param.name)
        else:
            return ""

class Parameter:
    def __init__(self, source):
        self.options = []
        match, source = match_and_consume("\[(\w*)\]", source)
        if match:
            self.options.append(match.group(1))

        parts = map(str.strip, source.split("="))
        if len(parts) == 1:
            self.default_value = None
        else:
            self.default_value = parts[1]

        param_decl = parts[0]

        if re.match("(const|unsigned long) ", param_decl):
            min_type_tokens = 2
        else:
            min_type_tokens = 1

        if len(param_decl.split(" ")) > min_type_tokens:
            parts = param_decl.split(" ")
            self.type = " ".join(parts[:-1])
            self.name = parts[-1]
        else:
            self.type = param_decl
            self.name = generate_param_name(self.type)

        if re.match("PassRefPtr<", param_decl):
            self.is_prp = True
            self.value = self.name
            self.name = "prp" + self.name[0].upper() + self.name[1:]
            self.inner_type = re.match("PassRefPtr<(.+)>", param_decl).group(1)
        else:
            self.is_prp = False
            self.value = self.name


    def to_str_full(self):
        if self.default_value is None:
            return self.to_str_class_and_name()
        return "%s %s = %s" % (self.type, self.name, self.default_value)

    def to_str_class_and_name(self):
        return "%s %s" % (self.type, self.name)

    def to_str_class(self):
        return self.type

    def to_str_name(self):
        return self.name

    def to_str_value(self):
        return self.value


def generate_param_name(param_type):
    base_name = re.match("(const |PassRefPtr<)?(\w*)", param_type).group(2)
    return "param" + base_name


def agent_class_name(agent):
    custom_agent_names = ["PageDebugger", "PageRuntime", "WorkerRuntime", "PageConsole"]
    if agent in custom_agent_names:
        return "%sAgent" % agent
    if agent == "AsyncCallTracker":
        return agent
    return "Inspector%sAgent" % agent


def agent_getter_signature(agent):
    agent_class = agent_class_name(agent)
    return agent_class, agent_class[0].lower() + agent_class[1:]


def include_header(name):
    return "#include \"%s.h\"" % name


def include_inspector_header(name):
    return include_header("core/inspector/" + name)


def generate_instrumenting_agents(used_agents):
    agents = list(used_agents)

    forward_list = []
    accessor_list = []
    member_list = []
    init_list = []
    trace_list = []
    reset_list = []

    for agent in agents:
        class_name, getter_name = agent_getter_signature(agent)
        member_name = "m_" + getter_name

        forward_list.append("class %s;" % class_name)
        accessor_list.append(template_instrumenting_agent_accessor.substitute(
            None,
            class_name=class_name,
            getter_name=getter_name,
            member_name=member_name))
        member_list.append("    RawPtrWillBeMember<%s> %s;" % (class_name, member_name))
        init_list.append("%s(nullptr)" % member_name)
        trace_list.append("visitor->trace(%s);" % member_name)
        reset_list.append("%s = nullptr;" % member_name)

    forward_list.sort()
    accessor_list.sort()
    member_list.sort()
    init_list.sort()
    trace_list.sort()
    reset_list.sort()

    header_lines = template_instrumenting_agents_h.substitute(
        None,
        forward_list="\n".join(forward_list),
        accessor_list="\n".join(accessor_list),
        member_list="\n".join(member_list))

    cpp_lines = template_instrumenting_agents_cpp.substitute(
        None,
        init_list="\n    , ".join(init_list),
        trace_list="\n    ".join(trace_list),
        reset_list="\n    ".join(reset_list))

    return header_lines, cpp_lines


def generate(input_path, output_dir):
    fin = open(input_path, "r")
    files = load_model_from_idl(fin.read())
    fin.close()

    cpp_includes = []
    cpp_lines = []
    used_agents = set()
    for f in files:
        cpp_includes.append(include_header(f.header_name))

        fout = open(output_dir + "/" + f.header_name + ".h", "w")
        fout.write(f.generate(cpp_lines, used_agents))
        fout.close()

    for agent in used_agents:
        cpp_includes.append(include_inspector_header(agent_class_name(agent)))
    cpp_includes.append(include_header("InstrumentingAgentsInl"))
    cpp_includes.append(include_header("core/CoreExport"))
    cpp_includes.sort()

    instrumenting_agents_header, instrumenting_agents_cpp = generate_instrumenting_agents(used_agents)

    fout = open(output_dir + "/" + "InstrumentingAgentsInl.h", "w")
    fout.write(instrumenting_agents_header)
    fout.close()

    fout = open(output_dir + "/InspectorInstrumentationImpl.cpp", "w")
    fout.write(template_cpp.substitute(None,
                                       includes="\n".join(cpp_includes),
                                       extra_definitions=instrumenting_agents_cpp,
                                       methods="\n".join(cpp_lines)))
    fout.close()


cmdline_parser = optparse.OptionParser()
cmdline_parser.add_option("--output_dir")

try:
    arg_options, arg_values = cmdline_parser.parse_args()
    if (len(arg_values) != 1):
        raise Exception("Exactly one plain argument expected (found %s)" % len(arg_values))
    input_path = arg_values[0]
    output_dirpath = arg_options.output_dir
    if not output_dirpath:
        raise Exception("Output directory must be specified")
except Exception:
    # Work with python 2 and 3 http://docs.python.org/py3k/howto/pyporting.html
    exc = sys.exc_info()[1]
    sys.stderr.write("Failed to parse command-line arguments: %s\n\n" % exc)
    sys.stderr.write("Usage: <script> --output_dir <output_dir> InspectorInstrumentation.idl\n")
    exit(1)

generate(input_path, output_dirpath)
