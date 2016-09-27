// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"use strict";

privateScriptController.installClass("PrivateScriptTest", function(PrivateScriptTestPrototype) {

    PrivateScriptTestPrototype.addIntegerInPartial = function(value1, value2) {
        return value1 + value2;
    }

    PrivateScriptTestPrototype.addInteger2InPartial = function(value1, value2) {
        // addValue_ is a method defined in PrivateScriptTest.js.
        // Partial interfaces should be able to use methods defined in the base interface.
        return this.addValues_(value1, value2);
    }

    Object.defineProperty(PrivateScriptTestPrototype, "stringAttributeInPartial", {
        get: function() { return this.m_stringAttributeInPartial; },
        set: function(value) { this.m_stringAttributeInPartial = value; }
    });

});
