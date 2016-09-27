// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"use strict";

privateScriptController.installClass("PrivateScriptTest", function(PrivateScriptTestPrototype) {

    PrivateScriptTestPrototype.initialize = function() {
        this.m_shortAttribute = -1;
        this.m_stringAttribute = "xxx";
        this.m_nodeAttribute = null;
        this.m_stringAttributeForPrivateScriptOnly = "yyy";
    }

    PrivateScriptTestPrototype.doNothing = function() {
    }

    PrivateScriptTestPrototype.return123 = function() {
        return 123;
    }

    PrivateScriptTestPrototype.echoInteger = function(value) {
        return value;
    }

    PrivateScriptTestPrototype.echoString = function(value) {
        return value;
    }

    PrivateScriptTestPrototype.echoNode = function(value) {
        return value;
    }

    PrivateScriptTestPrototype.addValues_ = function(value1, value2) {
        return value1 + value2;
    }

    PrivateScriptTestPrototype.addInteger = function(value1, value2) {
        return this.addValues_(value1, value2);
    }

    PrivateScriptTestPrototype.addString = function(value1, value2) {
        return this.addValues_(value1, value2);
    }

    PrivateScriptTestPrototype.setIntegerToDocument = function(document, value) {
        document.integer = value;
    }

    PrivateScriptTestPrototype.getIntegerFromDocument = function(document) {
        return document.integer;
    }

    PrivateScriptTestPrototype.setIntegerToPrototype = function(value) {
        this.integer = value;
    }

    PrivateScriptTestPrototype.getIntegerFromPrototype = function() {
        return this.integer;
    }

    PrivateScriptTestPrototype.createElement = function(document) {
        return document.createElement("div");
    }

    PrivateScriptTestPrototype.appendChild = function(node1, node2) {
        node1.appendChild(node2);
    }

    PrivateScriptTestPrototype.firstChild = function(node) {
        return node.firstChild;
    }

    PrivateScriptTestPrototype.nextSibling = function(node) {
        return node.nextSibling;
    }

    PrivateScriptTestPrototype.innerHTML = function(node) {
        return node.innerHTML;
    }

    PrivateScriptTestPrototype.setInnerHTML = function(node, string) {
        node.innerHTML = string;
    }

    PrivateScriptTestPrototype.addClickListener = function(node) {
        node.addEventListener("click", function () {
            node.innerHTML = "clicked";
        });
    }

    PrivateScriptTestPrototype.clickNode = function(document, node) {
        var event = new MouseEvent("click", { bubbles: true, cancelable: true, view: window });
        node.dispatchEvent(event);
    }

    Object.defineProperty(PrivateScriptTestPrototype, "readonlyShortAttribute", {
        get: function() { return 123; }
    });

    Object.defineProperty(PrivateScriptTestPrototype, "shortAttribute", {
        get: function() { return this.m_shortAttribute; },
        set: function(value) { this.m_shortAttribute = value; }
    });

    Object.defineProperty(PrivateScriptTestPrototype, "stringAttribute", {
        get: function() { return this.m_stringAttribute; },
        set: function(value) { this.m_stringAttribute = value; }
    });

    Object.defineProperty(PrivateScriptTestPrototype, "nodeAttribute", {
        get: function() { return this.m_nodeAttribute; },
        set: function(value) { this.m_nodeAttribute = value; }
    });

    Object.defineProperty(PrivateScriptTestPrototype, "nodeAttributeThrowsIndexSizeError", {
        get: function() { privateScriptController.throwException(privateScriptController.DOMException.IndexSizeError, "getter threw error"); },
        set: function(value) { privateScriptController.throwException(privateScriptController.DOMException.IndexSizeError, "setter threw error"); }
    });

    PrivateScriptTestPrototype.voidMethodThrowsDOMSyntaxError = function() {
        privateScriptController.throwException(privateScriptController.DOMException.SyntaxError, "method threw error");
    }

    PrivateScriptTestPrototype.voidMethodThrowsError = function() {
        privateScriptController.throwException(privateScriptController.JSError.Error, "method threw Error");
    }

    PrivateScriptTestPrototype.voidMethodThrowsTypeError = function() {
        privateScriptController.throwException(privateScriptController.JSError.TypeError, "method threw TypeError");
    }

    PrivateScriptTestPrototype.voidMethodThrowsRangeError = function() {
        privateScriptController.throwException(privateScriptController.JSError.RangeError, "method threw RangeError");
    }

    PrivateScriptTestPrototype.voidMethodThrowsSyntaxError = function() {
        privateScriptController.throwException(privateScriptController.JSError.SyntaxError, "method threw SyntaxError");
    }

    PrivateScriptTestPrototype.voidMethodThrowsReferenceError = function() {
        privateScriptController.throwException(privateScriptController.JSError.ReferenceError, "method threw ReferenceError");
    }

    PrivateScriptTestPrototype.voidMethodThrowsStackOverflowError = function() {
        function f() { f(); }
        f();
    }

    PrivateScriptTestPrototype.addIntegerForPrivateScriptOnly = function(value1, value2) {
        return value1 + value2;
    }

    Object.defineProperty(PrivateScriptTestPrototype, "stringAttributeForPrivateScriptOnly", {
        get: function() { return this.m_stringAttributeForPrivateScriptOnly; },
        set: function(value) { this.m_stringAttributeForPrivateScriptOnly = value; }
    });

    PrivateScriptTestPrototype.addIntegerImplementedInCPP = function(value1, value2) {
        return this.addIntegerImplementedInCPPForPrivateScriptOnly(value1, value2);
    }

    Object.defineProperty(PrivateScriptTestPrototype, "stringAttributeImplementedInCPP", {
        get: function() { return this.m_stringAttributeImplementedInCPPForPrivateScriptOnly; },
        set: function(value) { this.m_stringAttributeImplementedInCPPForPrivateScriptOnly = value; }
    });

    PrivateScriptTestPrototype.dispatchDocumentOnload = function(document) {
        var event = new Event("load", { bubbles: true, cancelable: true });
        event.valueInPrivateScript = "this should not be visible in user's script";
        document.dispatchEvent(event);
    }

});
