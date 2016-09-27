// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_
#define CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_

#include "include/cef_command_line.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/text/StringHash.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"

// CefCommandLine implementation
class CefCommandLineImpl : public CefCommandLine {
public:
    CefCommandLineImpl(bool willDelete, bool read_only);
    void Shutdown();

    // CefCommandLine methods.
    bool IsValid() override;
    bool IsReadOnly() override;
    CefRefPtr<CefCommandLine> Copy() override;
    void InitFromArgv(int argc, const char* const* argv) override;
    void InitFromString(const CefString& command_line) override;
    void Reset() override;
    void GetArgv(std::vector<CefString>& argv) override;
    CefString GetCommandLineString() override;
    CefString GetProgram() override;
    void SetProgram(const CefString& program) override;
    bool HasSwitches() override;
    bool HasSwitch(const CefString& name) override;
    CefString GetSwitchValue(const CefString& name) override;
    void GetSwitches(SwitchMap& switches) override;
    void AppendSwitch(const CefString& name) override;
    void AppendSwitchWithValue(const CefString& name,
        const CefString& value) override;
    bool HasArguments() override;
    void GetArguments(ArgumentList& arguments) override;
    void AppendArgument(const CefString& argument) override;
    void PrependWrapper(const CefString& wrapper) override;

    void InitFromArgvW(int argc, wchar_t** argv);
    void InitFromLPWSTR(LPWSTR cmdLine);

private:
    void InitFromArgvV(const WTF::Vector<String>& argv);
    void AppendSwitchNative(const String& switch_string, const String& value);
    void AppendArgNative(const String& value);
    void AppendSwitchesAndArguments(const WTF::Vector<String>& argv);
    WTF::String GetArgumentsString() const;

    bool m_bIsValid;
    bool m_bReadOnly;
    WTF::Vector<String>* m_argv;
    WTF::HashMap<String, String>* m_switches;

    DISALLOW_COPY_AND_ASSIGN(CefCommandLineImpl);
    IMPLEMENT_REFCOUNTING(CefCommandLineImpl);
};

#endif  // CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_
