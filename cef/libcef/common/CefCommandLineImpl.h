// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_
#define CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_
#pragma once

#include "include/cef_command_line.h"
#include "libcef/common/CefValueBase.h"

#include "base/command_line.h"

// CefCommandLine implementation
class CefCommandLineImpl :
    public CefValueBase<CefCommandLine, base::CommandLine> {
 public:
  CefCommandLineImpl(base::CommandLine* value,
                     bool will_delete,
                     bool read_only);

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

  // Must hold the controller lock while using this value.
  const base::CommandLine& command_line() { return const_value(); }

  DISALLOW_COPY_AND_ASSIGN(CefCommandLineImpl);
};

#endif  // CEF_LIBCEF_COMMON_COMMAND_LINE_IMPL_H_
