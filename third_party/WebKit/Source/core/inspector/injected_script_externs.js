// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/** @interface */
function InjectedScriptHostClass()
{
}

/**
 */
InjectedScriptHostClass.prototype.clearConsoleMessages = function() {}

/**
 * @param {*} objectId
 * @param {!Object} hints
 */
InjectedScriptHostClass.prototype.inspect = function(objectId, hints) {}

/**
 * @param {number} num
 * @return {*}
 */
InjectedScriptHostClass.prototype.inspectedObject = function(num) {}

/**
 * @param {*} obj
 * @return {string}
 */
InjectedScriptHostClass.prototype.internalConstructorName = function(obj) {}

/**
 * @param {*} obj
 * @return {boolean}
 */
InjectedScriptHostClass.prototype.isDOMWrapper = function(obj) {}

/**
 * @param {*} obj
 * @return {boolean}
 */
InjectedScriptHostClass.prototype.isHTMLAllCollection = function(obj) {}

/**
 * @param {*} obj
 * @return {boolean}
 */
InjectedScriptHostClass.prototype.isTypedArray = function(obj) {}

/**
 * @param {*} obj
 * @return {string}
 */
InjectedScriptHostClass.prototype.subtype = function(obj) {}

/**
 * @param {!Function} obj
 * @return {*}
 */
InjectedScriptHostClass.prototype.functionDetails = function(obj) {}

/**
 * @param {!Object} obj
 * @return {?Object}
 */
InjectedScriptHostClass.prototype.generatorObjectDetails = function(obj) {}

/**
 * @param {!Object} obj
 * @return {?Array.<*>}
 */
InjectedScriptHostClass.prototype.collectionEntries = function(obj) {}

/**
 * @param {*} obj
 * @return {!Array.<*>}
 */
InjectedScriptHostClass.prototype.getInternalProperties = function(obj) {}

/**
 * @param {!EventTarget} target
 * @return {!Array.<*>}
 */
InjectedScriptHostClass.prototype.getEventListeners = function(target) {}

/**
 * @param {string} text
 * @return {*}
 */
InjectedScriptHostClass.prototype.eval = function(text) {}

/**
 * @param {string} text
 * @return {*}
 */
InjectedScriptHostClass.prototype.evaluateWithExceptionDetails = function(text) {}

/**
 * @param {*} fn
 */
InjectedScriptHostClass.prototype.debugFunction = function(fn) {}

/**
 * @param {*} fn
 */
InjectedScriptHostClass.prototype.undebugFunction = function(fn) {}

/**
 * @param {*} fn
 */
InjectedScriptHostClass.prototype.monitorFunction = function(fn) {}

/**
 * @param {*} fn
 */
InjectedScriptHostClass.prototype.unmonitorFunction = function(fn) {}

/**
 * @param {!Function} fn
 * @param {*} receiver
 * @param {!Array.<*>=} argv
 * @return {*}
 */
InjectedScriptHostClass.prototype.callFunction = function(fn, receiver, argv) {}

/**
 * @param {!Function} fn
 * @param {*} receiver
 * @param {!Array.<*>=} argv
 * @return {*}
 */
InjectedScriptHostClass.prototype.suppressWarningsAndCallFunction = function(fn, receiver, argv) {}

/**
 * @param {!Object} obj
 * @param {string} key
 * @param {*} value
 */
InjectedScriptHostClass.prototype.setNonEnumProperty = function(obj, key, value) {}

/**
 * @param {!Function} functionObject
 * @param {number} scopeIndex
 * @param {string} variableName
 * @param {*} newValue
 * @return {*}
 */
InjectedScriptHostClass.prototype.setFunctionVariableValue = function(functionObject, scopeIndex, variableName, newValue) {}

/**
 * @param {*} value
 * @param {string} groupName
 * @return {number}
 */
InjectedScriptHostClass.prototype.bind = function(value, groupName) {}

/**
 * @param {number} id
 * @return {*}
 */
InjectedScriptHostClass.prototype.objectForId = function(id) {}

/**
 * @param {number} id
 * @return {string}
 */
InjectedScriptHostClass.prototype.idToObjectGroupName = function(id) {}

/** @type {!InjectedScriptHostClass} */
var InjectedScriptHost;

/** @interface */
function JavaScriptCallFrame()
{
    /** @type {!JavaScriptCallFrame} */
    this.caller;
    /** @type {number} */
    this.sourceID;
    /** @type {number} */
    this.line;
    /** @type {number} */
    this.column;
    /** @type {!Array.<!Object>} */
    this.scopeChain;
    /** @type {!Object} */
    this.thisObject;
    /** @type {string} */
    this.stepInPositions;
    /** @type {string} */
    this.functionName;
    /** @type {number} */
    this.functionLine;
    /** @type {number} */
    this.functionColumn;
    /** @type {boolean} */
    this.isAtReturn;
    /** @type {*} */
    this.returnValue;
}

/**
 * @param {string} script
 * @param {!Object=} scopeExtension
 * @return {*}
 */
JavaScriptCallFrame.prototype.evaluateWithExceptionDetails = function(script, scopeExtension) {}

/**
 * @return {*}
 */
JavaScriptCallFrame.prototype.restart = function() {}

/**
 * @param {number=} scopeIndex
 * @param {?string=} variableName
 * @param {*=} newValue
 * @return {*}
 */
JavaScriptCallFrame.prototype.setVariableValue = function(scopeIndex, variableName, newValue) {}

/**
 * @param {number} scopeIndex
 * @return {number}
 */
JavaScriptCallFrame.prototype.scopeType = function(scopeIndex) {}


/** @type {!Window} */
var inspectedGlobalObject;
/** @type {number} */
var injectedScriptId;
