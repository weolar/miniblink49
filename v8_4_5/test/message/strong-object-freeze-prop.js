// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --strong-mode

"use strong";

let o = {};
Object.defineProperty(o, "foo", { writable: true });
Object.defineProperty(o, "foo", { writable: false });
