// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef/common/CefCommandLineImpl.h"
#include "libcef/common/StringUtil.h"
#include "include/base/cef_logging.h"
#include "base/file_path.h"

#define CEF_VALUE_VERIFY_RETURN(modify, errorVal) 
#define CEF_VALUE_VERIFY_RETURN_VOID(modify) 

static const WCHAR kSwitchTerminator[] = FILE_PATH_LITERAL("--");
static const WCHAR kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");
static const WCHAR* const kSwitchPrefixes[] = { L"--", L"-", L"/" };
static const size_t switch_prefix_count = arraysize(kSwitchPrefixes);

CefCommandLineImpl::CefCommandLineImpl(bool willDelete, bool readOnly) {
    m_bReadOnly = readOnly;
    m_argv = new WTF::Vector<String>();
    m_switches = new WTF::HashMap<String, String>();
}

void CefCommandLineImpl::Shutdown() {
    delete m_argv;
    m_argv = nullptr;

    delete m_switches;
    m_switches = nullptr;
}

bool CefCommandLineImpl::IsValid() {
    return m_bIsValid;
}

bool CefCommandLineImpl::IsReadOnly() {
    return m_bReadOnly;
}

CefRefPtr<CefCommandLine> CefCommandLineImpl::Copy() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    CefCommandLineImpl* newSelf = new CefCommandLineImpl(true, false);
    newSelf->m_bIsValid = this->m_bIsValid;
    newSelf->m_bReadOnly = this->m_bReadOnly;
    newSelf->m_argv = this->m_argv;
    newSelf->m_switches = this->m_switches;
    return newSelf;
}

void CefCommandLineImpl::InitFromArgv(int argc, const char* const* argv) {
    NOTREACHED() << "method not supported on this platform";
}

static size_t GetSwitchPrefixLength(const WTF::String& string)
{
    for (size_t i = 0; i < switch_prefix_count; ++i) {
        WTF::String prefix(kSwitchPrefixes[i]);
        if (string.startsWith(prefix))
            return prefix.length();
    }
    return 0;
}

void CefCommandLineImpl::AppendSwitchNative(const String& switch_string, const String& value) {
    String switch_key(switch_string.lower());
    String combined_switch_string(switch_key);

    size_t prefix_length = GetSwitchPrefixLength(combined_switch_string);
    m_switches->set(switch_key.substring(prefix_length), value);
    // Preserve existing switch prefixes in |argv_|; only append one if necessary.
    if (prefix_length == 0)
        combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
    if (!value.isEmpty())
        combined_switch_string.append(kSwitchValueSeparator + value);
    // Append the switch and update the switches/arguments divider |begin_args_|.
    m_argv->append(combined_switch_string);
}

void CefCommandLineImpl::AppendArgNative(const String& value) {
    m_argv->append(value);
}

static bool IsSwitch(const WTF::String& string,
    WTF::String* switch_string,
    WTF::String* switch_value) {
    *switch_string = StringImpl::empty();
    *switch_value = StringImpl::empty();
    size_t prefix_length = GetSwitchPrefixLength(string);
    if (prefix_length == 0 || prefix_length == string.length())
        return false;

    const size_t equals_position = string.find(kSwitchValueSeparator);
    *switch_string = string.substring(0, equals_position);
    if (equals_position != WTF::kNotFound)
        *switch_value = string.substring(equals_position + 1);
    return true;
}

void CefCommandLineImpl::AppendSwitchesAndArguments(const WTF::Vector<String>& argv) {
    bool parse_switches = true;
    for (size_t i = 1; i < argv.size(); ++i) {
        String arg = argv[i];
        //TrimWhitespace(arg, TRIM_ALL, &arg);

        String switch_string;
        String switch_value;
        parse_switches &= (arg != String(kSwitchTerminator));
        if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
            AppendSwitchNative(switch_string, switch_value);
        } else {
            AppendArgNative(arg);
        }
    }
}

void CefCommandLineImpl::InitFromArgvV(const WTF::Vector<String>& argv) {
    m_argv->clear();
    m_switches->clear();

    m_argv->append(argv.isEmpty() ? StringImpl::empty() : argv[0]);
    AppendSwitchesAndArguments(argv);
}

void CefCommandLineImpl::InitFromArgvW(int argc, wchar_t** argv) {
    WTF::Vector<String> newArgv;
    for (int i = 0; i < argc; ++i)
        newArgv.append(String(argv[i]));
    InitFromArgvV(newArgv);
}

void CefCommandLineImpl::InitFromString(const CefString& commandLine) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //TrimWhitespace(command_line, TRIM_ALL, &command_line_string);
    if (commandLine.empty())
        return;

    int numArgs = 0;
    wchar_t** args = NULL;
    args = ::CommandLineToArgvW(commandLine.c_str(), &numArgs);
    InitFromArgvW(numArgs, args);
    ::LocalFree(args);
}

void CefCommandLineImpl::InitFromLPWSTR(LPWSTR cmdLine) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //TrimWhitespace(command_line, TRIM_ALL, &command_line_string);
    String commandLine((const UChar*)cmdLine);
    if (commandLine.isEmpty())
        return;

    int numArgs = 0;
    wchar_t** args = NULL;
    args = ::CommandLineToArgvW(commandLine.charactersWithNullTermination().data(), &numArgs);
    InitFromArgvW(numArgs, args);
    ::LocalFree(args);
}

void CefCommandLineImpl::Reset() {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    String argv;
    if (m_argv->size() >= 1)
        argv = m_argv->at(0);
    m_argv->clear();
    m_switches->clear();

    m_argv->append(argv);
}

void CefCommandLineImpl::GetArgv(std::vector<CefString>& argv) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);

    const WTF::Vector<String>& cmdArgv = *m_argv;
    WTF::Vector<String>::const_iterator it = cmdArgv.begin();
    for (; it != cmdArgv.end(); ++it) {
        cef_string_utf16_t* value = cef_string_userfree_utf16_alloc();
        Vector<UChar> cmd = it->charactersWithNullTermination();
        CefString cmdCefString;
        cmdCefString.FromString(cmd.data(), cmd.size(), true);
        argv.push_back(cmdCefString);
    }
}

WTF::String CefCommandLineImpl::GetArgumentsString() const {
    WTF::String params;
    // Append switches and arguments.
    bool parse_switches = true;
    for (size_t i = 1; i < m_argv->size(); ++i) {
        WTF::String arg = m_argv->at(i);
        WTF::String switch_string;
        WTF::String switch_value;
        parse_switches &= arg != WTF::String(kSwitchTerminator);
        if (i > 1)
            params.append((FILE_PATH_LITERAL(" ")));
        if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
            params.append(switch_string);
            if (!switch_value.isEmpty()) {
                // #if defined(OS_WIN)
                //                 switch_value = QuoteForCommandLineToArgvW(switch_value);
                // #endif
                params.append(kSwitchValueSeparator + switch_value);
            }
        } else {
            // #if defined(OS_WIN)
            //             arg = QuoteForCommandLineToArgvW(arg);
            // #endif
            params.append(arg);
        }
    }
    return params;
}

CefString CefCommandLineImpl::GetCommandLineString() {
    CEF_VALUE_VERIFY_RETURN(false, CefString());

    WTF::String string = "";
    if (m_argv->size() > 0)
        string = m_argv->at(0);
    // #if defined(OS_WIN)
    //     string = QuoteForCommandLineToArgvW(string);
    // #endif
    WTF::String params(GetArgumentsString());
    if (!params.isEmpty()) {
        string.append(WTF::String(FILE_PATH_LITERAL(" ")));
        string.append(params);
    }

    Vector<UChar> str = string.charactersWithNullTermination();
    CefString retVal;
    retVal.FromString(str.data(), str.size(), true);
    return retVal;
}

CefString CefCommandLineImpl::GetProgram() {
    CEF_VALUE_VERIFY_RETURN(false, CefString());

    CefString retVal;
    if (m_argv->size() != 0) {
        Vector<UChar> str = m_argv->at(0).charactersWithNullTermination();
        retVal.FromString(str.data(), str.size(), true);
    }
    return retVal;
}

void CefCommandLineImpl::SetProgram(const CefString& program) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);

    String programStr(program.c_str(), program.length());
    if (m_argv->size() == 0)
        m_argv->append(programStr);
    else
        m_argv->at(0) = programStr;
}

bool CefCommandLineImpl::HasSwitches() {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return m_switches->size() > 0;
}

bool CefCommandLineImpl::HasSwitch(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    String str(name.c_str(), name.length());
    WTF::HashMap<String, String>::const_iterator it = m_switches->find(str.lower());
    return it != m_switches->end();
}

CefString CefCommandLineImpl::GetSwitchValue(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN(false, CefString());

    String str(name.c_str(), name.length());
    WTF::HashMap<String, String>::const_iterator it = m_switches->find(str.lower());

    CefString retVal;
    if (it != m_switches->end()) {
        Vector<UChar> str = it->value.charactersWithNullTermination();
        retVal.FromString(str.data(), str.size(), true);
    }

    return retVal;
}

void CefCommandLineImpl::GetSwitches(SwitchMap& switches) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);
//     const base::CommandLine::SwitchMap& map = const_value().GetSwitches();
//     base::CommandLine::SwitchMap::const_iterator it = map.begin();
//     for (; it != map.end(); ++it)
//         switches.insert(std::make_pair(it->first, it->second));
    DebugBreak();
}

void CefCommandLineImpl::AppendSwitch(const CefString& name) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //mutable_value()->AppendSwitch(name);
    DebugBreak();
}

void CefCommandLineImpl::AppendSwitchWithValue(const CefString& name,
    const CefString& value) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //mutable_value()->AppendSwitchNative(name, value);
    DebugBreak();
}

bool CefCommandLineImpl::HasArguments() {
    CEF_VALUE_VERIFY_RETURN(false, false);
    //return (const_value().GetArgs().size() > 0);
    if (!m_argv)
        return false;
    return m_argv->size() > 0;
}

void CefCommandLineImpl::GetArguments(ArgumentList& arguments) {
    CEF_VALUE_VERIFY_RETURN_VOID(false);
//     const base::CommandLine::StringVector& vec = const_value().GetArgs();
//     base::CommandLine::StringVector::const_iterator it = vec.begin();
//     for (; it != vec.end(); ++it)
//         arguments.push_back(*it);
    if (!m_argv)
        return;
    
    WTF::Vector<String>::const_iterator it = m_argv->begin();
    for (; it != m_argv->end(); ++it) {
        CefString output;
        cef::WTFStringToCefString(*it, output);
        arguments.push_back(output);
    }
}

void CefCommandLineImpl::AppendArgument(const CefString& argument) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //mutable_value()->AppendArgNative(argument);
    DebugBreak();
}

void CefCommandLineImpl::PrependWrapper(const CefString& wrapper) {
    CEF_VALUE_VERIFY_RETURN_VOID(true);
    //mutable_value()->PrependWrapper(wrapper);
    DebugBreak();
}


// CefCommandLine implementation.

// static
CefRefPtr<CefCommandLine> CefCommandLine::CreateCommandLine() {
    return new CefCommandLineImpl(true, false);
}

// static
CefRefPtr<CefCommandLine> CefCommandLine::GetGlobalCommandLine() {
    // Uses a singleton reference object.
    static CefRefPtr<CefCommandLineImpl> commandLinePtr;
    if (!commandLinePtr.get()) {
        commandLinePtr = new CefCommandLineImpl(false, true);
        commandLinePtr->InitFromLPWSTR(::GetCommandLineW());
    }
    
    return commandLinePtr.get();
}
