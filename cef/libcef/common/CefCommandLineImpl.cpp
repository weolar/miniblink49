// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/CefCommandLineImpl.h"

#include "libcef/common/StringUtil.h"
#include "include/base/cef_logging.h"
#include "base/file_path.h"

CefCommandLineImpl::CefCommandLineImpl(base::CommandLine* value,
    bool will_delete,
    bool read_only)
    : CefValueBase<CefCommandLine, base::CommandLine>(value, NULL, will_delete ? kOwnerWillDelete : kOwnerNoDelete, read_only, NULL) {
}

bool CefCommandLineImpl::IsValid() {
    return !detached();
}

bool CefCommandLineImpl::IsReadOnly() {
    return read_only();
}

CefRefPtr<CefCommandLine> CefCommandLineImpl::Copy() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);
    return new CefCommandLineImpl(new base::CommandLine(const_value().argv()), true, false);
}

void CefCommandLineImpl::InitFromArgv(int argc, const char* const* argv) {
#if !defined(OS_WIN)
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->InitFromArgv(argc, argv);
#else
    NOTREACHED() << "method not supported on this platform";
#endif
}

void CefCommandLineImpl::InitFromString(const CefString& command_line) {
#if defined(OS_WIN)
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->ParseFromString(command_line);
#else
    NOTREACHED() << "method not supported on this platform";
#endif
}

void CefCommandLineImpl::Reset() {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    base::CommandLine::StringVector argv;
    argv.push_back(mutable_value()->GetProgram());
    mutable_value()->InitFromArgv(argv);

    const base::CommandLine::SwitchMap& map = mutable_value()->GetSwitches();
    const_cast<base::CommandLine::SwitchMap*>(&map)->clear();
}

void CefCommandLineImpl::GetArgv(std::vector<CefString>& argv) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);
    const base::CommandLine::StringVector& cmd_argv = const_value().argv();
    base::CommandLine::StringVector::const_iterator it = cmd_argv.begin();
    for (; it != cmd_argv.end(); ++it)
        argv.push_back(*it);
}

CefString CefCommandLineImpl::GetCommandLineString() {
    CEF_VALUE_VERIFY_RETURN(false, CefString());
    return const_value().GetCommandLineString();
}

CefString CefCommandLineImpl::GetProgram() {
    CEF_VALUE_VERIFY_RETURN(false, CefString());
    return const_value().GetProgram()/*.value()*/;
}

void CefCommandLineImpl::SetProgram(const CefString& program) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->SetProgram(/*base::FilePath*/(program));
}

bool CefCommandLineImpl::HasSwitches() {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return (const_value().GetSwitches().size() > 0);
}

bool CefCommandLineImpl::HasSwitch(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return const_value().HasSwitch(name.ToString());
}

CefString CefCommandLineImpl::GetSwitchValue(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN(false, CefString());
    return const_value().GetSwitchValueNative(name.ToString());
}

void CefCommandLineImpl::GetSwitches(SwitchMap& switches) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);
    const base::CommandLine::SwitchMap& map = const_value().GetSwitches();
    base::CommandLine::SwitchMap::const_iterator it = map.begin();
    for (; it != map.end(); ++it)
        switches.insert(std::make_pair(it->first, it->second));
}

void CefCommandLineImpl::AppendSwitch(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->AppendSwitch(name);
}

void CefCommandLineImpl::AppendSwitchWithValue(const CefString& name, const CefString& value) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->AppendSwitchNative(name, value);
}

bool CefCommandLineImpl::HasArguments() {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return (const_value().GetArgs().size() > 0);
}

void CefCommandLineImpl::GetArguments(ArgumentList& arguments) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);
    const base::CommandLine::StringVector& vec = const_value().GetArgs();
    base::CommandLine::StringVector::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it)
        arguments.push_back(*it);
}

void CefCommandLineImpl::AppendArgument(const CefString& argument) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->AppendArgNative(argument);
}

void CefCommandLineImpl::PrependWrapper(const CefString& wrapper) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    mutable_value()->PrependWrapper(wrapper);
}


// CefCommandLine implementation.

// static
CefRefPtr<CefCommandLine> CefCommandLine::CreateCommandLine() {
    return new CefCommandLineImpl(new base::CommandLine(base::CommandLine::NO_PROGRAM), true, false);
}

// static
CefRefPtr<CefCommandLine> CefCommandLine::GetGlobalCommandLine() {
    // Uses a singleton reference object.
    static CefCommandLineImpl* s_commandLinePtr = nullptr;
    if (!s_commandLinePtr) {
        base::CommandLine* commandLine = base::CommandLine::ForCurrentProcess();
        if (commandLine) {
            s_commandLinePtr = new CefCommandLineImpl(commandLine, false, true);  
            s_commandLinePtr->AddRef();
        }
    }

    return s_commandLinePtr;
}
#endif
