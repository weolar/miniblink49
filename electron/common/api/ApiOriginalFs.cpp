// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/NodeRegisterHelp.h"
#include "gin/object_template_builder.h"
#include "gin/converter.h"
#include "include/v8-profiler.h"
#include "node/include/nodeblink.h"
#include <string>

namespace {

void initializeCommonOriginalFsApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {

}

}  // namespace

static const char CommonOriginalFsSricpt[] =
"module.exports = require('fs');";

static NodeNative nativeCommonOriginalFsNative{ "original-fs", CommonOriginalFsSricpt, sizeof(CommonOriginalFsSricpt) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_original_fs, initializeCommonOriginalFsApi, &nativeCommonOriginalFsNative)
