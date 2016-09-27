// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"use strict";

function PrivateScriptController()
{
    this._installedClasses = {};
    this._DOMException = {};
    this._JSError = {};
    // This list must be in sync with the enum in ExceptionCode.h. The order matters.
    var domExceptions = [
        "IndexSizeError",
        "HierarchyRequestError",
        "WrongDocumentError",
        "InvalidCharacterError",
        "NoModificationAllowedError",
        "NotFoundError",
        "NotSupportedError",
        "InUseAttributeError", // Historical. Only used in setAttributeNode etc which have been removed from the DOM specs.

        // Introduced in DOM Level 2:
        "InvalidStateError",
        "SyntaxError",
        "InvalidModificationError",
        "NamespaceError",
        "InvalidAccessError",

        // Introduced in DOM Level 3:
        "TypeMismatchError", // Historical; use TypeError instead

        // XMLHttpRequest extension:
        "SecurityError",

        // Others introduced in HTML5:
        "NetworkError",
        "AbortError",
        "URLMismatchError",
        "QuotaExceededError",
        "TimeoutError",
        "InvalidNodeTypeError",
        "DataCloneError",

        // These are IDB-specific.
        "UnknownError",
        "ConstraintError",
        "DataError",
        "TransactionInactiveError",
        "ReadOnlyError",
        "VersionError",

        // File system
        "NotReadableError",
        "EncodingError",
        "PathExistsError",

        // SQL
        "SQLDatabaseError", // Naming conflict with DatabaseError class.

        // Web Crypto
        "OperationError",
    ];

    // This list must be in sync with the enum in ExceptionCode.h. The order matters.
    var jsErrors = [
        "Error",
        "TypeError",
        "RangeError",
        "SyntaxError",
        "ReferenceError",
    ];

    var code = 1;
    domExceptions.forEach(function (exception) {
        this._DOMException[exception] = code;
        ++code;
    }.bind(this));

    var code = 1000;
    jsErrors.forEach(function (exception) {
        this._JSError[exception] = code;
        ++code;
    }.bind(this));
}

PrivateScriptController.prototype = {
    get installedClasses()
    {
        return this._installedClasses;
    },

    get DOMException()
    {
        return this._DOMException;
    },

    get JSError()
    {
        return this._JSError;
    },

    installClass: function(className, implementation)
    {
        function PrivateScriptClass()
        {
        }

        if (!(className in this._installedClasses))
            this._installedClasses[className] = new PrivateScriptClass();
        implementation(this._installedClasses[className]);
    },

    // Private scripts can throw JS errors and DOM exceptions as follows:
    //     throwException(privateScriptController.DOMException.IndexSizeError, "...");
    //     throwException(privateScriptController.JSError.TypeError, "...");
    //
    // Note that normal JS errors thrown by private scripts are treated
    // as real JS errors caused by programming mistake and the execution crashes.
    // If you want to intentially throw JS errors from private scripts,
    // you need to use throwException(privateScriptController.JSError.TypeError, "...").
    throwException: function(code, message)
    {
        function PrivateScriptException()
        {
        }

        var exception = new PrivateScriptException();
        exception.code = code;
        exception.message = message;
        exception.name = "PrivateScriptException";
        throw exception;
    },
}

if (typeof window.privateScriptController === 'undefined')
    window.privateScriptController = new PrivateScriptController();

// This line must be the last statement of this JS file.
// A parenthesis is needed, because the caller of this script (PrivateScriptRunner.cpp)
// is depending on the completion value of this script.
(privateScriptController.installedClasses);
