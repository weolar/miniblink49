/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

"use strict";

/**
 * @param {!InjectedScriptHostClass} InjectedScriptHost
 * @param {!Window|!WorkerGlobalScope} inspectedGlobalObject
 * @param {number} injectedScriptId
 */
(function (InjectedScriptHost, inspectedGlobalObject, injectedScriptId) {

/**
 * Protect against Object overwritten by the user code.
 * @suppress {duplicate}
 */
var Object = /** @type {function(new:Object, *=)} */ ({}.constructor);

/**
 * @param {!Array.<T>} array
 * @param {...} var_args
 * @template T
 */
function push(array, var_args)
{
    for (var i = 1; i < arguments.length; ++i)
        array[array.length] = arguments[i];
}

/**
 * @param {!Arguments.<T>} array
 * @param {number=} index
 * @return {!Array.<T>}
 * @template T
 */
function slice(array, index)
{
    var result = [];
    for (var i = index || 0, j = 0; i < array.length; ++i, ++j)
        result[j] = array[i];
    return result;
}

/**
 * @param {!Array.<T>} array1
 * @param {!Array.<T>} array2
 * @return {!Array.<T>}
 * @template T
 */
function concat(array1, array2)
{
    var result = [];
    for (var i = 0; i < array1.length; ++i)
        push(result, array1[i]);
    for (var i = 0; i < array2.length; ++i)
        push(result, array2[i]);
    return result;
}

/**
 * @param {*} obj
 * @return {string}
 * @suppress {uselessCode}
 */
function toString(obj)
{
    // We don't use String(obj) because String could be overridden.
    // Also the ("" + obj) expression may throw.
    try {
        return "" + obj;
    } catch (e) {
        var name = InjectedScriptHost.internalConstructorName(obj) || InjectedScriptHost.subtype(obj) || (typeof obj);
        return "#<" + name + ">";
    }
}

/**
 * @param {*} obj
 * @return {string}
 */
function toStringDescription(obj)
{
    if (typeof obj === "number" && obj === 0 && 1 / obj < 0)
        return "-0"; // Negative zero.
    return toString(obj);
}

/**
 * Please use this bind, not the one from Function.prototype
 * @param {function(...)} func
 * @param {?Object} thisObject
 * @param {...} var_args
 * @return {function(...)}
 */
function bind(func, thisObject, var_args)
{
    var args = slice(arguments, 2);

    /**
     * @param {...} var_args
     */
    function bound(var_args)
    {
        return InjectedScriptHost.callFunction(func, thisObject, concat(args, slice(arguments)));
    }
    bound.toString = function()
    {
        return "bound: " + toString(func);
    };
    return bound;
}

/**
 * @param {T} obj
 * @return {T}
 * @template T
 */
function nullifyObjectProto(obj)
{
    if (obj && typeof obj === "object")
        obj.__proto__ = null;
    return obj;
}

/**
 * @param {number|string} obj
 * @return {boolean}
 */
function isUInt32(obj)
{
    if (typeof obj === "number")
        return obj >>> 0 === obj && (obj > 0 || 1 / obj > 0);
    return "" + (obj >>> 0) === obj;
}

/**
 * FireBug's array detection.
 * @param {*} obj
 * @return {boolean}
 */
function isArrayLike(obj)
{
    if (typeof obj !== "object")
        return false;
    try {
        if (typeof obj.splice === "function") {
            var len = obj.length;
            return typeof len === "number" && isUInt32(len);
        }
    } catch (e) {
    }
    return false;
}

/**
 * @param {number} a
 * @param {number} b
 * @return {number}
 */
function max(a, b)
{
    return a > b ? a : b;
}

/**
 * FIXME: Remove once ES6 is supported natively by JS compiler.
 * @param {*} obj
 * @return {boolean}
 */
function isSymbol(obj)
{
    var type = typeof obj;
    return (type === "symbol");
}

/**
 * @param {string} str
 * @param {string} searchElement
 * @param {number=} fromIndex
 * @return {number}
 */
function indexOf(str, searchElement, fromIndex)
{
    var len = str.length;
    var n = fromIndex || 0;
    var k = max(n >= 0 ? n : len + n, 0);

    while (k < len) {
        if (str[k] === searchElement)
            return k;
        ++k;
    }
    return -1;
}

/**
 * DOM Attributes which have observable side effect on getter, in the form of
 *   {interfaceName1: {attributeName1: true,
 *                     attributeName2: true,
 *                     ...},
 *    interfaceName2: {...},
 *    ...}
 * @type {!Object<string, !Object<string, boolean>>}
 * @const
 */
var domAttributesWithObservableSideEffectOnGet = nullifyObjectProto({});
domAttributesWithObservableSideEffectOnGet["Request"] = nullifyObjectProto({});
domAttributesWithObservableSideEffectOnGet["Request"]["body"] = true;
domAttributesWithObservableSideEffectOnGet["Response"] = nullifyObjectProto({});
domAttributesWithObservableSideEffectOnGet["Response"]["body"] = true;

/**
 * @param {!Object} object
 * @param {string} attribute
 * @return {boolean}
 */
function doesAttributeHaveObservableSideEffectOnGet(object, attribute)
{
    for (var interfaceName in domAttributesWithObservableSideEffectOnGet) {
        var isInstance = InjectedScriptHost.suppressWarningsAndCallFunction(function(object, interfaceName) {
            return /* suppressBlacklist */ typeof inspectedGlobalObject[interfaceName] === "function" && object instanceof inspectedGlobalObject[interfaceName];
        }, null, [object, interfaceName]);
        if (isInstance) {
            return attribute in domAttributesWithObservableSideEffectOnGet[interfaceName];
        }
    }
    return false;
}

/**
 * @constructor
 */
var InjectedScript = function()
{
    /** @type {!Object.<string, !Object>} */
    this._modules = { __proto__: null };
}

/**
 * @type {!Object.<string, boolean>}
 * @const
 */
InjectedScript.primitiveTypes = {
    "undefined": true,
    "boolean": true,
    "number": true,
    "string": true,
    __proto__: null
}

InjectedScript.prototype = {
    /**
     * @param {*} object
     * @return {boolean}
     */
    isPrimitiveValue: function(object)
    {
        // FIXME(33716): typeof document.all is always 'undefined'.
        return InjectedScript.primitiveTypes[typeof object] && !this._isHTMLAllCollection(object);
    },

    /**
     * @param {*} object
     * @param {string} groupName
     * @param {boolean} canAccessInspectedGlobalObject
     * @param {boolean} generatePreview
     * @return {!RuntimeAgent.RemoteObject}
     */
    wrapObject: function(object, groupName, canAccessInspectedGlobalObject, generatePreview)
    {
        if (canAccessInspectedGlobalObject)
            return this._wrapObject(object, groupName, false, generatePreview);
        return this._fallbackWrapper(object);
    },

    /**
     * @param {*} object
     * @param {string} groupName
     * @param {boolean=} doNotBind
     * @return {!RuntimeAgent.RemoteObject}
     */
    wrapObjectForModule: function(object, groupName, doNotBind)
    {
        return this._wrapObject(object, groupName, false, false, null, false, doNotBind);
    },

    /**
     * @param {*} object
     * @return {!RuntimeAgent.RemoteObject}
     */
    _fallbackWrapper: function(object)
    {
        var result = { __proto__: null };
        result.type = typeof object;
        if (this.isPrimitiveValue(object))
            result.value = object;
        else
            result.description = toString(object);
        return /** @type {!RuntimeAgent.RemoteObject} */ (result);
    },

    /**
     * @param {boolean} canAccessInspectedGlobalObject
     * @param {!Object} table
     * @param {!Array.<string>|string|boolean} columns
     * @return {!RuntimeAgent.RemoteObject}
     */
    wrapTable: function(canAccessInspectedGlobalObject, table, columns)
    {
        if (!canAccessInspectedGlobalObject)
            return this._fallbackWrapper(table);
        var columnNames = null;
        if (typeof columns === "string")
            columns = [columns];
        if (InjectedScriptHost.subtype(columns) === "array") {
            columnNames = [];
            for (var i = 0; i < columns.length; ++i)
                columnNames[i] = toString(columns[i]);
        }
        return this._wrapObject(table, "console", false, true, columnNames, true);
    },

    /**
     * @param {*} object
     * @return {*}
     */
    _inspect: function(object)
    {
        if (arguments.length === 0)
            return;

        var objectId = this._wrapObject(object, "");
        var hints = { __proto__: null };

        InjectedScriptHost.inspect(objectId, hints);
        return object;
    },

    /**
     * This method cannot throw.
     * @param {*} object
     * @param {string=} objectGroupName
     * @param {boolean=} forceValueType
     * @param {boolean=} generatePreview
     * @param {?Array.<string>=} columnNames
     * @param {boolean=} isTable
     * @param {boolean=} doNotBind
     * @param {*=} customObjectConfig
     * @return {!RuntimeAgent.RemoteObject}
     * @suppress {checkTypes}
     */
    _wrapObject: function(object, objectGroupName, forceValueType, generatePreview, columnNames, isTable, doNotBind, customObjectConfig)
    {
        try {
            return new InjectedScript.RemoteObject(object, objectGroupName, doNotBind, forceValueType, generatePreview, columnNames, isTable, undefined, customObjectConfig);
        } catch (e) {
            try {
                var description = injectedScript._describe(e);
            } catch (ex) {
                var description = "<failed to convert exception to string>";
            }
            return new InjectedScript.RemoteObject(description);
        }
    },

    /**
     * @param {!Object|symbol} object
     * @param {string=} objectGroupName
     * @return {string}
     */
    _bind: function(object, objectGroupName)
    {
        var id = InjectedScriptHost.bind(object, objectGroupName || "");
        return "{\"injectedScriptId\":" + injectedScriptId + ",\"id\":" + id + "}";
    },

    /**
     * @param {string} objectId
     * @return {!Object}
     */
    _parseObjectId: function(objectId)
    {
        return nullifyObjectProto(/** @type {!Object} */ (InjectedScriptHost.eval("(" + objectId + ")")));
    },

    clearLastEvaluationResult: function()
    {
        delete this._lastResult;
    },

    /**
     * @param {string} methodName
     * @param {string} args
     * @return {*}
     */
    dispatch: function(methodName, args)
    {
        var argsArray = /** @type {!Array.<*>} */ (InjectedScriptHost.eval("(" + args + ")"));
        var result = InjectedScriptHost.callFunction(this[methodName], this, argsArray);
        if (typeof result === "undefined") {
            inspectedGlobalObject.console.error("Web Inspector error: InjectedScript.%s returns undefined", methodName);
            result = null;
        }
        return result;
    },

    /**
     * @param {string} objectId
     * @param {boolean} ownProperties
     * @param {boolean} accessorPropertiesOnly
     * @param {boolean} generatePreview
     * @return {!Array.<!RuntimeAgent.PropertyDescriptor>|boolean}
     */
    getProperties: function(objectId, ownProperties, accessorPropertiesOnly, generatePreview)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        var object = this._objectForId(parsedObjectId);
        var objectGroupName = InjectedScriptHost.idToObjectGroupName(parsedObjectId.id);

        if (!this._isDefined(object) || isSymbol(object))
            return false;
        object = /** @type {!Object} */ (object);
        var descriptors = [];
        var iter = this._propertyDescriptors(object, ownProperties, accessorPropertiesOnly);
        // Go over properties, wrap object values.
        for (var descriptor of iter) {
            if ("get" in descriptor)
                descriptor.get = this._wrapObject(descriptor.get, objectGroupName);
            if ("set" in descriptor)
                descriptor.set = this._wrapObject(descriptor.set, objectGroupName);
            if ("value" in descriptor)
                descriptor.value = this._wrapObject(descriptor.value, objectGroupName, false, generatePreview);
            if (!("configurable" in descriptor))
                descriptor.configurable = false;
            if (!("enumerable" in descriptor))
                descriptor.enumerable = false;
            if ("symbol" in descriptor)
                descriptor.symbol = this._wrapObject(descriptor.symbol, objectGroupName);
            push(descriptors, descriptor);
        }
        return descriptors;
    },

    /**
     * @param {string} objectId
     * @return {!Array.<!Object>|boolean}
     */
    getInternalProperties: function(objectId)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        var object = this._objectForId(parsedObjectId);
        var objectGroupName = InjectedScriptHost.idToObjectGroupName(parsedObjectId.id);
        if (!this._isDefined(object) || isSymbol(object))
            return false;
        object = /** @type {!Object} */ (object);
        var descriptors = [];
        var internalProperties = InjectedScriptHost.getInternalProperties(object);
        if (internalProperties) {
            for (var i = 0; i < internalProperties.length; i += 2) {
                var descriptor = {
                    name: internalProperties[i],
                    value: this._wrapObject(internalProperties[i + 1], objectGroupName),
                    __proto__: null
                };
                push(descriptors, descriptor);
            }
        }
        return descriptors;
    },

    /**
     * @param {string} functionId
     * @return {!DebuggerAgent.FunctionDetails|string}
     */
    getFunctionDetails: function(functionId)
    {
        var parsedFunctionId = this._parseObjectId(functionId);
        var func = this._objectForId(parsedFunctionId);
        if (typeof func !== "function")
            return "Cannot resolve function by id.";
        var details = nullifyObjectProto(/** @type {!DebuggerAgent.FunctionDetails} */ (InjectedScriptHost.functionDetails(func)));
        if ("rawScopes" in details) {
            var objectGroupName = InjectedScriptHost.idToObjectGroupName(parsedFunctionId.id);
            var rawScopes = details["rawScopes"];
            delete details["rawScopes"];
            var scopes = [];
            for (var i = 0; i < rawScopes.length; ++i)
                scopes[i] = InjectedScript.CallFrameProxy._createScopeJson(rawScopes[i].type, rawScopes[i].object, objectGroupName);
            details.scopeChain = scopes;
        }
        return details;
    },

    /**
     * @param {string} objectId
     * @return {!DebuggerAgent.GeneratorObjectDetails|string}
     */
    getGeneratorObjectDetails: function(objectId)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        var object = this._objectForId(parsedObjectId);
        if (!object || typeof object !== "object")
            return "Could not find object with given id";
        var details = nullifyObjectProto(/** @type {?DebuggerAgent.GeneratorObjectDetails} */ (InjectedScriptHost.generatorObjectDetails(object)));
        if (!details)
            return "Object is not a generator";
        var objectGroupName = InjectedScriptHost.idToObjectGroupName(parsedObjectId.id);
        details["function"] = this._wrapObject(details["function"], objectGroupName);
        return details;
    },

    /**
     * @param {string} objectId
     * @return {!Array.<!Object>|string}
     */
    getCollectionEntries: function(objectId)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        var object = this._objectForId(parsedObjectId);
        if (!object || typeof object !== "object")
            return "Could not find object with given id";
        var entries = InjectedScriptHost.collectionEntries(object);
        if (!entries)
            return "Object with given id is not a collection";
        var objectGroupName = InjectedScriptHost.idToObjectGroupName(parsedObjectId.id);
        for (var i = 0; i < entries.length; ++i) {
            var entry = nullifyObjectProto(entries[i]);
            if ("key" in entry)
                entry.key = this._wrapObject(entry.key, objectGroupName);
            entry.value = this._wrapObject(entry.value, objectGroupName);
            entries[i] = entry;
        }
        return entries;
    },

    /**
     * @param {!Object} object
     * @param {boolean=} ownProperties
     * @param {boolean=} accessorPropertiesOnly
     * @param {?Array.<string>=} propertyNamesOnly
     */
    _propertyDescriptors: function*(object, ownProperties, accessorPropertiesOnly, propertyNamesOnly)
    {
        var propertyProcessed = { __proto__: null };

        /**
         * @param {?Object} o
         * @param {!Iterable.<string|symbol>|!Array.<string|symbol>} properties
         */
        function* process(o, properties)
        {
            for (var property of properties) {
                if (propertyProcessed[property])
                    continue;

                var name = property;
                if (isSymbol(property))
                    name = /** @type {string} */ (injectedScript._describe(property));

                try {
                    propertyProcessed[property] = true;
                    var descriptor = nullifyObjectProto(InjectedScriptHost.suppressWarningsAndCallFunction(Object.getOwnPropertyDescriptor, Object, [o, property]));
                    if (descriptor) {
                        if (accessorPropertiesOnly && !("get" in descriptor || "set" in descriptor))
                            continue;
                        if ("get" in descriptor && "set" in descriptor && name != "__proto__" && InjectedScriptHost.isDOMWrapper(object) && !doesAttributeHaveObservableSideEffectOnGet(object, name)) {
                            descriptor.value = InjectedScriptHost.suppressWarningsAndCallFunction(function(attribute) { return this[attribute]; }, object, [name]);
                            delete descriptor.get;
                            delete descriptor.set;
                        }
                    } else {
                        // Not all bindings provide proper descriptors. Fall back to the writable, configurable property.
                        if (accessorPropertiesOnly)
                            continue;
                        try {
                            descriptor = { name: name, value: o[property], writable: false, configurable: false, enumerable: false, __proto__: null };
                            if (o === object)
                                descriptor.isOwn = true;
                            yield descriptor;
                        } catch (e) {
                            // Silent catch.
                        }
                        continue;
                    }
                } catch (e) {
                    if (accessorPropertiesOnly)
                        continue;
                    var descriptor = { __proto__: null };
                    descriptor.value = e;
                    descriptor.wasThrown = true;
                }

                descriptor.name = name;
                if (o === object)
                    descriptor.isOwn = true;
                if (isSymbol(property))
                    descriptor.symbol = property;
                yield descriptor;
            }
        }

        /**
         * @param {number} length
         */
        function* arrayIndexNames(length)
        {
            for (var i = 0; i < length; ++i)
                yield "" + i;
        }

        if (propertyNamesOnly) {
            for (var i = 0; i < propertyNamesOnly.length; ++i) {
                var name = propertyNamesOnly[i];
                for (var o = object; this._isDefined(o); o = o.__proto__) {
                    if (InjectedScriptHost.suppressWarningsAndCallFunction(Object.prototype.hasOwnProperty, o, [name])) {
                        for (var descriptor of process(o, [name]))
                            yield descriptor;
                        break;
                    }
                    if (ownProperties)
                        break;
                }
            }
            return;
        }

        var skipGetOwnPropertyNames;
        try {
            skipGetOwnPropertyNames = InjectedScriptHost.isTypedArray(object) && object.length > 500000;
        } catch (e) {
        }

        for (var o = object; this._isDefined(o); o = o.__proto__) {
            if (skipGetOwnPropertyNames && o === object) {
                // Avoid OOM crashes from getting all own property names of a large TypedArray.
                for (var descriptor of process(o, arrayIndexNames(o.length)))
                    yield descriptor;
            } else {
                // First call Object.keys() to enforce ordering of the property descriptors.
                for (var descriptor of process(o, Object.keys(/** @type {!Object} */ (o))))
                    yield descriptor;
                for (var descriptor of process(o, Object.getOwnPropertyNames(/** @type {!Object} */ (o))))
                    yield descriptor;
            }
            if (Object.getOwnPropertySymbols) {
                for (var descriptor of process(o, Object.getOwnPropertySymbols(/** @type {!Object} */ (o))))
                    yield descriptor;
            }
            if (ownProperties) {
                if (object.__proto__ && !accessorPropertiesOnly)
                    yield { name: "__proto__", value: object.__proto__, writable: true, configurable: true, enumerable: false, isOwn: true, __proto__: null };
                break;
            }
        }
    },

    /**
     * @param {string} expression
     * @param {string} objectGroup
     * @param {boolean} injectCommandLineAPI
     * @param {boolean} returnByValue
     * @param {boolean} generatePreview
     * @return {*}
     */
    evaluate: function(expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview)
    {
        return this._evaluateAndWrap(null, expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview);
    },

    /**
     * @param {string} objectId
     * @param {string} expression
     * @param {string} args
     * @param {boolean} returnByValue
     * @return {!Object|string}
     */
    callFunctionOn: function(objectId, expression, args, returnByValue)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        var object = this._objectForId(parsedObjectId);
        if (!this._isDefined(object))
            return "Could not find object with given id";

        if (args) {
            var resolvedArgs = [];
            var callArgs = /** @type {!Array.<!RuntimeAgent.CallArgument>} */ (InjectedScriptHost.eval(args));
            for (var i = 0; i < callArgs.length; ++i) {
                try {
                    resolvedArgs[i] = this._resolveCallArgument(callArgs[i]);
                } catch (e) {
                    return toString(e);
                }
            }
        }

        var objectGroup = InjectedScriptHost.idToObjectGroupName(parsedObjectId.id);

        /**
         * @suppressReceiverCheck
         * @param {*} object
         * @param {boolean=} forceValueType
         * @param {boolean=} generatePreview
         * @param {?Array.<string>=} columnNames
         * @param {boolean=} isTable
         * @param {*=} customObjectConfig
         * @return {!RuntimeAgent.RemoteObject}
         * @this {InjectedScript}
         */
        function wrap(object, forceValueType, generatePreview, columnNames, isTable, customObjectConfig)
        {
            return this._wrapObject(object, objectGroup, forceValueType, generatePreview, columnNames, isTable, false, customObjectConfig);
        }

        try {

            var remoteObjectAPI = { bindRemoteObject: bind(wrap, this), __proto__: null};
            InjectedScriptHost.setNonEnumProperty(inspectedGlobalObject, "__remoteObjectAPI", remoteObjectAPI);

            var func = InjectedScriptHost.eval("with (typeof __remoteObjectAPI !== 'undefined' ? __remoteObjectAPI : { __proto__: null }) {(" + expression + ")}");
            if (typeof func !== "function")
                return "Given expression does not evaluate to a function";

            return { wasThrown: false,
                     result: this._wrapObject(InjectedScriptHost.callFunction(func, object, resolvedArgs), objectGroup, returnByValue),
                     __proto__: null };
        } catch (e) {
            return this._createThrownValue(e, objectGroup, false);
        } finally {
            try {
                delete inspectedGlobalObject["__remoteObjectAPI"];
            } catch(e) {
            }
        }
    },

    /**
     * @param {string|undefined} objectGroupName
     * @param {*} jsonMLObject
     * @throws {string} error message
     */
    _substituteObjectTagsInCustomPreview: function(objectGroupName, jsonMLObject)
    {
        var maxCustomPreviewRecursionDepth = 20;
        this._customPreviewRecursionDepth = (this._customPreviewRecursionDepth || 0) + 1
        try {
            if (this._customPreviewRecursionDepth >= maxCustomPreviewRecursionDepth)
                throw new Error("Too deep hierarchy of inlined custom previews");

            if (!isArrayLike(jsonMLObject))
                return;

            if (jsonMLObject[0] === "object") {
                var attributes = jsonMLObject[1];
                var originObject = attributes["object"];
                var config = attributes["config"];
                if (typeof originObject === "undefined")
                    throw new Error("Illegal format: obligatory attribute \"object\" isn't specified");

                jsonMLObject[1] = this._wrapObject(originObject, objectGroupName, false, false, null, false, false, config);
                return;
            }

            for (var i = 0; i < jsonMLObject.length; ++i)
                this._substituteObjectTagsInCustomPreview(objectGroupName, jsonMLObject[i]);
        } finally {
            this._customPreviewRecursionDepth--;
        }
    },

    /**
     * Resolves a value from CallArgument description.
     * @param {!RuntimeAgent.CallArgument} callArgumentJson
     * @return {*} resolved value
     * @throws {string} error message
     */
    _resolveCallArgument: function(callArgumentJson)
    {
        callArgumentJson = nullifyObjectProto(callArgumentJson);
        var objectId = callArgumentJson.objectId;
        if (objectId) {
            var parsedArgId = this._parseObjectId(objectId);
            if (!parsedArgId || parsedArgId["injectedScriptId"] !== injectedScriptId)
                throw "Arguments should belong to the same JavaScript world as the target object.";

            var resolvedArg = this._objectForId(parsedArgId);
            if (!this._isDefined(resolvedArg))
                throw "Could not find object with given id";

            return resolvedArg;
        } else if ("value" in callArgumentJson) {
            var value = callArgumentJson.value;
            if (callArgumentJson.type === "number" && typeof value !== "number")
                value = Number(value);
            return value;
        }
        return undefined;
    },

    /**
     * @param {?JavaScriptCallFrame} callFrame
     * @param {string} expression
     * @param {string} objectGroup
     * @param {boolean} injectCommandLineAPI
     * @param {boolean} returnByValue
     * @param {boolean} generatePreview
     * @param {!Array.<!Object>=} scopeChain
     * @return {!Object}
     */
    _evaluateAndWrap: function(callFrame, expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview, scopeChain)
    {
        var wrappedResult = this._evaluateOn(callFrame, objectGroup, expression, injectCommandLineAPI, scopeChain);
        if (!wrappedResult.exceptionDetails) {
            return { wasThrown: false,
                     result: this._wrapObject(wrappedResult.result, objectGroup, returnByValue, generatePreview),
                     __proto__: null };
        }
        return this._createThrownValue(wrappedResult.result, objectGroup, generatePreview, wrappedResult.exceptionDetails);
    },

    /**
     * @param {*} value
     * @param {string|undefined} objectGroup
     * @param {boolean} generatePreview
     * @param {!DebuggerAgent.ExceptionDetails=} exceptionDetails
     * @return {!Object}
     */
    _createThrownValue: function(value, objectGroup, generatePreview, exceptionDetails)
    {
        var remoteObject = this._wrapObject(value, objectGroup, false, generatePreview && InjectedScriptHost.subtype(value) !== "error");
        if (!remoteObject.description){
            try {
                remoteObject.description = toStringDescription(value);
            } catch (e) {}
        }
        return { wasThrown: true, result: remoteObject, exceptionDetails: exceptionDetails, __proto__: null };
    },

    /**
     * @param {?JavaScriptCallFrame} callFrame
     * @param {string} objectGroup
     * @param {string} expression
     * @param {boolean} injectCommandLineAPI
     * @param {!Array.<!Object>=} scopeChain
     * @return {*}
     */
    _evaluateOn: function(callFrame, objectGroup, expression, injectCommandLineAPI, scopeChain)
    {
        // Only install command line api object for the time of evaluation.
        // Surround the expression in with statements to inject our command line API so that
        // the window object properties still take more precedent than our API functions.

        var scopeExtensionForEval = (callFrame && injectCommandLineAPI) ? new CommandLineAPI(this._commandLineAPIImpl, callFrame) : undefined;

        injectCommandLineAPI = !scopeExtensionForEval && !callFrame && injectCommandLineAPI && !("__commandLineAPI" in inspectedGlobalObject);
        var injectScopeChain = scopeChain && scopeChain.length && !("__scopeChainForEval" in inspectedGlobalObject);

        try {
            var prefix = "";
            var suffix = "";
            if (injectCommandLineAPI) {
                InjectedScriptHost.setNonEnumProperty(inspectedGlobalObject, "__commandLineAPI", new CommandLineAPI(this._commandLineAPIImpl, callFrame));
                prefix = "with (typeof __commandLineAPI !== 'undefined' ? __commandLineAPI : { __proto__: null }) {";
                suffix = "}";
            }
            if (injectScopeChain) {
                InjectedScriptHost.setNonEnumProperty(inspectedGlobalObject, "__scopeChainForEval", scopeChain);
                for (var i = 0; i < scopeChain.length; ++i) {
                    prefix = "with (typeof __scopeChainForEval !== 'undefined' ? __scopeChainForEval[" + i + "] : { __proto__: null }) {" + (suffix ? " " : "") + prefix;
                    if (suffix)
                        suffix += " }";
                    else
                        suffix = "}";
                }
            }

            if (prefix)
                expression = prefix + "\n" + expression + "\n" + suffix;
            var wrappedResult = callFrame ? callFrame.evaluateWithExceptionDetails(expression, scopeExtensionForEval) : InjectedScriptHost.evaluateWithExceptionDetails(expression);
            if (objectGroup === "console" && !wrappedResult.exceptionDetails)
                this._lastResult = wrappedResult.result;
            return wrappedResult;
        } finally {
            if (injectCommandLineAPI) {
                try {
                    delete inspectedGlobalObject["__commandLineAPI"];
                } catch(e) {
                }
            }
            if (injectScopeChain) {
                try {
                    delete inspectedGlobalObject["__scopeChainForEval"];
                } catch(e) {
                }
            }
        }
    },

    /**
     * @param {?Object} callFrame
     * @param {number} asyncOrdinal
     * @return {!Array.<!InjectedScript.CallFrameProxy>|boolean}
     */
    wrapCallFrames: function(callFrame, asyncOrdinal)
    {
        if (!callFrame)
            return false;

        var result = [];
        var depth = 0;
        do {
            result[depth] = new InjectedScript.CallFrameProxy(depth, callFrame, asyncOrdinal);
            callFrame = callFrame.caller;
            ++depth;
        } while (callFrame);
        return result;
    },

    /**
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {!Array.<!JavaScriptCallFrame>} asyncCallStacks
     * @param {string} callFrameId
     * @param {string} expression
     * @param {string} objectGroup
     * @param {boolean} injectCommandLineAPI
     * @param {boolean} returnByValue
     * @param {boolean} generatePreview
     * @return {*}
     */
    evaluateOnCallFrame: function(topCallFrame, asyncCallStacks, callFrameId, expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview)
    {
        var parsedCallFrameId = nullifyObjectProto(/** @type {!Object} */ (InjectedScriptHost.eval("(" + callFrameId + ")")));
        var callFrame = this._callFrameForParsedId(topCallFrame, parsedCallFrameId, asyncCallStacks);
        if (!callFrame)
            return "Could not find call frame with given id";
        if (parsedCallFrameId["asyncOrdinal"])
            return this._evaluateAndWrap(null, expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview, callFrame.scopeChain);
        return this._evaluateAndWrap(callFrame, expression, objectGroup, injectCommandLineAPI, returnByValue, generatePreview);
    },

    /**
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {string} callFrameId
     * @return {*}
     */
    restartFrame: function(topCallFrame, callFrameId)
    {
        var callFrame = this._callFrameForId(topCallFrame, callFrameId);
        if (!callFrame)
            return "Could not find call frame with given id";
        var result = callFrame.restart();
        if (result === false)
            result = "Restart frame is not supported";
        return result;
    },

    /**
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {string} callFrameId
     * @return {*} a stepIn position array ready for protocol JSON or a string error
     */
    getStepInPositions: function(topCallFrame, callFrameId)
    {
        var callFrame = this._callFrameForId(topCallFrame, callFrameId);
        if (!callFrame)
            return "Could not find call frame with given id";
        var stepInPositionsUnpacked = JSON.parse(callFrame.stepInPositions);
        if (typeof stepInPositionsUnpacked !== "object")
            return "Step in positions not available";
        return stepInPositionsUnpacked;
    },

    /**
     * Either callFrameId or functionObjectId must be specified.
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {string|boolean} callFrameId or false
     * @param {string|boolean} functionObjectId or false
     * @param {number} scopeNumber
     * @param {string} variableName
     * @param {string} newValueJsonString RuntimeAgent.CallArgument structure serialized as string
     * @return {string|undefined} undefined if success or an error message
     */
    setVariableValue: function(topCallFrame, callFrameId, functionObjectId, scopeNumber, variableName, newValueJsonString)
    {
        try {
            var newValueJson = /** @type {!RuntimeAgent.CallArgument} */ (InjectedScriptHost.eval("(" + newValueJsonString + ")"));
            var resolvedValue = this._resolveCallArgument(newValueJson);
            if (typeof callFrameId === "string") {
                var callFrame = this._callFrameForId(topCallFrame, callFrameId);
                if (!callFrame)
                    return "Could not find call frame with given id";
                callFrame.setVariableValue(scopeNumber, variableName, resolvedValue)
            } else {
                var parsedFunctionId = this._parseObjectId(/** @type {string} */ (functionObjectId));
                var func = this._objectForId(parsedFunctionId);
                if (typeof func !== "function")
                    return "Could not resolve function by id";
                InjectedScriptHost.setFunctionVariableValue(func, scopeNumber, variableName, resolvedValue);
            }
        } catch (e) {
            return toString(e);
        }
        return undefined;
    },

    /**
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {string} callFrameId
     * @return {?JavaScriptCallFrame}
     */
    _callFrameForId: function(topCallFrame, callFrameId)
    {
        var parsedCallFrameId = nullifyObjectProto(/** @type {!Object} */ (InjectedScriptHost.eval("(" + callFrameId + ")")));
        return this._callFrameForParsedId(topCallFrame, parsedCallFrameId, []);
    },

    /**
     * @param {!JavaScriptCallFrame} topCallFrame
     * @param {!Object} parsedCallFrameId
     * @param {!Array.<!JavaScriptCallFrame>} asyncCallStacks
     * @return {?JavaScriptCallFrame}
     */
    _callFrameForParsedId: function(topCallFrame, parsedCallFrameId, asyncCallStacks)
    {
        var asyncOrdinal = parsedCallFrameId["asyncOrdinal"]; // 1-based index
        if (asyncOrdinal)
            topCallFrame = asyncCallStacks[asyncOrdinal - 1];
        var ordinal = parsedCallFrameId["ordinal"];
        var callFrame = topCallFrame;
        while (--ordinal >= 0 && callFrame)
            callFrame = callFrame.caller;
        return callFrame;
    },

    /**
     * @param {!Object} objectId
     * @return {!Object|symbol|undefined}
     */
    _objectForId: function(objectId)
    {
        return objectId.injectedScriptId === injectedScriptId ? /** @type{!Object|symbol|undefined} */ (InjectedScriptHost.objectForId(objectId.id)) : void 0;
    },

    /**
     * @param {string} objectId
     * @return {!Object|symbol|undefined}
     */
    findObjectById: function(objectId)
    {
        var parsedObjectId = this._parseObjectId(objectId);
        return this._objectForId(parsedObjectId);
    },

    /**
     * @param {string} objectId
     * @return {?Node}
     */
    nodeForObjectId: function(objectId)
    {
        var object = this.findObjectById(objectId);
        if (!object || this._subtype(object) !== "node")
            return null;
        return /** @type {!Node} */ (object);
    },

    /**
     * @param {string} name
     * @return {!Object}
     */
    module: function(name)
    {
        return this._modules[name];
    },

    /**
     * @param {string} name
     * @param {string} source
     * @return {?Object}
     */
    injectModule: function(name, source)
    {
        delete this._modules[name];
        var moduleFunction = InjectedScriptHost.eval("(" + source + ")");
        if (typeof moduleFunction !== "function") {
            inspectedGlobalObject.console.error("Web Inspector error: A function was expected for module %s evaluation", name);
            return null;
        }
        var module = /** @type {!Object} */ (InjectedScriptHost.callFunction(moduleFunction, inspectedGlobalObject, [InjectedScriptHost, inspectedGlobalObject, injectedScriptId, this]));
        this._modules[name] = module;
        return module;
    },

    /**
     * @param {*} object
     * @return {boolean}
     */
    _isDefined: function(object)
    {
        return !!object || this._isHTMLAllCollection(object);
    },

    /**
     * @param {*} object
     * @return {boolean}
     */
    _isHTMLAllCollection: function(object)
    {
        // document.all is reported as undefined, but we still want to process it.
        return (typeof object === "undefined") && InjectedScriptHost.isHTMLAllCollection(object);
    },

    /**
     * @param {*} obj
     * @return {?string}
     */
    _subtype: function(obj)
    {
        if (obj === null)
            return "null";

        if (this.isPrimitiveValue(obj))
            return null;

        var subtype = InjectedScriptHost.subtype(obj);
        if (subtype)
            return subtype;

        if (isArrayLike(obj))
            return "array";

        // If owning frame has navigated to somewhere else window properties will be undefined.
        return null;
    },

    /**
     * @param {*} obj
     * @return {?string}
     */
    _describe: function(obj)
    {
        if (this.isPrimitiveValue(obj))
            return null;

        var subtype = this._subtype(obj);

        if (subtype === "regexp")
            return toString(obj);

        if (subtype === "date")
            return toString(obj);

        if (subtype === "node") {
            var description = obj.nodeName.toLowerCase();
            switch (obj.nodeType) {
            case 1 /* Node.ELEMENT_NODE */:
                description += obj.id ? "#" + obj.id : "";
                var className = obj.className;
                description += (className && typeof className === "string") ? "." + className.trim().replace(/\s+/g, ".") : "";
                break;
            case 10 /*Node.DOCUMENT_TYPE_NODE */:
                description = "<!DOCTYPE " + description + ">";
                break;
            }
            return description;
        }

        var className = InjectedScriptHost.internalConstructorName(obj);
        if (subtype === "array") {
            if (typeof obj.length === "number")
                className += "[" + obj.length + "]";
            return className;
        }

        // NodeList in JSC is a function, check for array prior to this.
        if (typeof obj === "function")
            return toString(obj);

        if (isSymbol(obj)) {
            try {
                return /** @type {string} */ (InjectedScriptHost.callFunction(Symbol.prototype.toString, obj)) || "Symbol";
            } catch (e) {
                return "Symbol";
            }
        }

        if (InjectedScriptHost.subtype(obj) === "error") {
            try {
                var stack = obj.stack;
                var message = obj.message && obj.message.length ? ": " + obj.message : "";
                var firstCallFrame = /^\s+at\s/m.exec(stack);
                var stackMessageEnd = firstCallFrame ? firstCallFrame.index : -1;
                if (stackMessageEnd !== -1) {
                    var stackTrace = stack.substr(stackMessageEnd);
                    return className + message + "\n" + stackTrace;
                }
                return className + message;
            } catch(e) {
            }
        }

        return className;
    },

    /**
     * @param {boolean} enabled
     */
    setCustomObjectFormatterEnabled: function(enabled)
    {
        this._customObjectFormatterEnabled = enabled;
    }
}

/**
 * @type {!InjectedScript}
 * @const
 */
var injectedScript = new InjectedScript();

/**
 * @constructor
 * @param {*} object
 * @param {string=} objectGroupName
 * @param {boolean=} doNotBind
 * @param {boolean=} forceValueType
 * @param {boolean=} generatePreview
 * @param {?Array.<string>=} columnNames
 * @param {boolean=} isTable
 * @param {boolean=} skipEntriesPreview
 * @param {*=} customObjectConfig
 */
InjectedScript.RemoteObject = function(object, objectGroupName, doNotBind, forceValueType, generatePreview, columnNames, isTable, skipEntriesPreview, customObjectConfig)
{
    this.type = typeof object;
    if (this.type === "undefined" && injectedScript._isHTMLAllCollection(object))
        this.type = "object";

    if (injectedScript.isPrimitiveValue(object) || object === null || forceValueType) {
        // We don't send undefined values over JSON.
        if (this.type !== "undefined")
            this.value = object;

        // Null object is object with 'null' subtype.
        if (object === null)
            this.subtype = "null";

        // Provide user-friendly number values.
        if (this.type === "number") {
            this.description = toStringDescription(object);
            // Override "value" property for values that can not be JSON-stringified.
            switch (this.description) {
            case "NaN":
            case "Infinity":
            case "-Infinity":
            case "-0":
                this.value = this.description;
                break;
            }
        }

        return;
    }

    object = /** @type {!Object} */ (object);

    if (!doNotBind)
        this.objectId = injectedScript._bind(object, objectGroupName);
    var subtype = injectedScript._subtype(object);
    if (subtype)
        this.subtype = subtype;
    var className = InjectedScriptHost.internalConstructorName(object);
    if (className)
        this.className = className;
    this.description = injectedScript._describe(object);

    if (generatePreview && this.type === "object" && this.subtype !== "node")
        this.preview = this._generatePreview(object, undefined, columnNames, isTable, skipEntriesPreview);

    if (injectedScript._customObjectFormatterEnabled) {
        var customPreview = this._customPreview(object, objectGroupName, customObjectConfig);
        if (customPreview)
            this.customPreview = customPreview;
    }
}

InjectedScript.RemoteObject.prototype = {

    /**
     * @param {*} object
     * @param {string=} objectGroupName
     * @param {*=} customObjectConfig
     * @return {?RuntimeAgent.CustomPreview}
     */
    _customPreview: function(object, objectGroupName, customObjectConfig)
    {
        /**
         * @param {!Error} error
         */
        function logError(error)
        {
            Promise.resolve().then(inspectedGlobalObject.console.error.bind(inspectedGlobalObject.console, "Custom Formatter Failed: " + error.message));
        }

        try {
            var formatters = inspectedGlobalObject["devtoolsFormatters"];
            if (!formatters || !isArrayLike(formatters))
                return null;

            for (var i = 0; i < formatters.length; ++i) {
                try {
                    var formatted = formatters[i].header(object, customObjectConfig);
                    if (!formatted)
                        continue;

                    var hasBody = formatters[i].hasBody(object, customObjectConfig);
                    injectedScript._substituteObjectTagsInCustomPreview(objectGroupName, formatted);
                    var formatterObjectId = injectedScript._bind(formatters[i], objectGroupName);
                    var result = {header: JSON.stringify(formatted), hasBody: !!hasBody, formatterObjectId: formatterObjectId};
                    if (customObjectConfig)
                        result["configObjectId"] = injectedScript._bind(customObjectConfig, objectGroupName);
                    return result;
                } catch (e) {
                    logError(e);
                }
            }
        } catch (e) {
            logError(e);
        }
        return null;
    },

    /**
     * @return {!RuntimeAgent.ObjectPreview} preview
     */
    _createEmptyPreview: function()
    {
        var preview = {
            type: /** @type {!RuntimeAgent.ObjectPreviewType.<string>} */ (this.type),
            description: this.description || toStringDescription(this.value),
            lossless: true,
            overflow: false,
            properties: [],
            __proto__: null
        };
        if (this.subtype)
            preview.subtype = /** @type {!RuntimeAgent.ObjectPreviewSubtype.<string>} */ (this.subtype);
        return preview;
    },

    /**
     * @param {!Object} object
     * @param {?Array.<string>=} firstLevelKeys
     * @param {?Array.<string>=} secondLevelKeys
     * @param {boolean=} isTable
     * @param {boolean=} skipEntriesPreview
     * @return {!RuntimeAgent.ObjectPreview} preview
     */
    _generatePreview: function(object, firstLevelKeys, secondLevelKeys, isTable, skipEntriesPreview)
    {
        var preview = this._createEmptyPreview();
        var firstLevelKeysCount = firstLevelKeys ? firstLevelKeys.length : 0;

        var propertiesThreshold = {
            properties: isTable ? 1000 : max(5, firstLevelKeysCount),
            indexes: isTable ? 1000 : max(100, firstLevelKeysCount),
            __proto__: null
        };

        try {
            var descriptors = injectedScript._propertyDescriptors(object, undefined, undefined, firstLevelKeys);

            this._appendPropertyDescriptors(preview, descriptors, propertiesThreshold, secondLevelKeys, isTable);
            if (propertiesThreshold.indexes < 0 || propertiesThreshold.properties < 0)
                return preview;

            // Add internal properties to preview.
            var rawInternalProperties = InjectedScriptHost.getInternalProperties(object) || [];
            var internalProperties = [];
            for (var i = 0; i < rawInternalProperties.length; i += 2) {
                push(internalProperties, {
                    name: rawInternalProperties[i],
                    value: rawInternalProperties[i + 1],
                    isOwn: true,
                    enumerable: true,
                    __proto__: null
                });
            }
            this._appendPropertyDescriptors(preview, internalProperties, propertiesThreshold, secondLevelKeys, isTable);

            if (this.subtype === "map" || this.subtype === "set" || this.subtype === "iterator")
                this._appendEntriesPreview(object, preview, skipEntriesPreview);

        } catch (e) {
            preview.lossless = false;
        }

        return preview;
    },

    /**
     * @param {!RuntimeAgent.ObjectPreview} preview
     * @param {!Array.<*>|!Iterable.<*>} descriptors
     * @param {!Object} propertiesThreshold
     * @param {?Array.<string>=} secondLevelKeys
     * @param {boolean=} isTable
     */
    _appendPropertyDescriptors: function(preview, descriptors, propertiesThreshold, secondLevelKeys, isTable)
    {
        for (var descriptor of descriptors) {
            if (propertiesThreshold.indexes < 0 || propertiesThreshold.properties < 0)
                break;
            if (!descriptor)
                continue;
            if (descriptor.wasThrown) {
                preview.lossless = false;
                continue;
            }

            var name = descriptor.name;

            // Ignore __proto__ property, stay lossless.
            if (name === "__proto__")
                continue;

            // Ignore non-enumerable members on prototype, stay lossless.
            if (!descriptor.isOwn && !descriptor.enumerable)
                continue;

            // Ignore length property of array, stay lossless.
            if (this.subtype === "array" && name === "length")
                continue;

            // Ignore size property of map, set, stay lossless.
            if ((this.subtype === "map" || this.subtype === "set") && name === "size")
                continue;

            // Never preview prototype properties, turn lossy.
            if (!descriptor.isOwn) {
                preview.lossless = false;
                continue;
            }

            // Ignore computed properties, turn lossy.
            if (!("value" in descriptor)) {
                preview.lossless = false;
                continue;
            }

            var value = descriptor.value;
            var type = typeof value;

            // Never render functions in object preview, turn lossy
            if (type === "function" && (this.subtype !== "array" || !isUInt32(name))) {
                preview.lossless = false;
                continue;
            }

            // Special-case HTMLAll.
            if (type === "undefined" && injectedScript._isHTMLAllCollection(value))
                type = "object";

            // Render own properties.
            if (value === null) {
                this._appendPropertyPreview(preview, { name: name, type: "object", subtype: "null", value: "null", __proto__: null }, propertiesThreshold);
                continue;
            }

            var maxLength = 100;
            if (InjectedScript.primitiveTypes[type]) {
                if (type === "string" && value.length > maxLength) {
                    value = this._abbreviateString(value, maxLength, true);
                    preview.lossless = false;
                }
                this._appendPropertyPreview(preview, { name: name, type: type, value: toStringDescription(value), __proto__: null }, propertiesThreshold);
                continue;
            }

            var property = { name: name, type: type, __proto__: null };
            var subtype = injectedScript._subtype(value);
            if (subtype)
                property.subtype = subtype;

            if (secondLevelKeys === null || secondLevelKeys) {
                var subPreview = this._generatePreview(value, secondLevelKeys || undefined, undefined, isTable);
                property.valuePreview = subPreview;
                if (!subPreview.lossless)
                    preview.lossless = false;
                if (subPreview.overflow)
                    preview.overflow = true;
            } else {
                var description = "";
                if (type !== "function")
                    description = this._abbreviateString(/** @type {string} */ (injectedScript._describe(value)), maxLength, subtype === "regexp");
                property.value = description;
                preview.lossless = false;
            }
            this._appendPropertyPreview(preview, property, propertiesThreshold);
        }
    },

    /**
     * @param {!RuntimeAgent.ObjectPreview} preview
     * @param {!Object} property
     * @param {!Object} propertiesThreshold
     */
    _appendPropertyPreview: function(preview, property, propertiesThreshold)
    {
        if (toString(property.name >>> 0) === property.name)
            propertiesThreshold.indexes--;
        else
            propertiesThreshold.properties--;
        if (propertiesThreshold.indexes < 0 || propertiesThreshold.properties < 0) {
            preview.overflow = true;
            preview.lossless = false;
        } else {
            push(preview.properties, property);
        }
    },

    /**
     * @param {!Object} object
     * @param {!RuntimeAgent.ObjectPreview} preview
     * @param {boolean=} skipEntriesPreview
     */
    _appendEntriesPreview: function(object, preview, skipEntriesPreview)
    {
        var entries = InjectedScriptHost.collectionEntries(object);
        if (!entries)
            return;
        if (skipEntriesPreview) {
            if (entries.length) {
                preview.overflow = true;
                preview.lossless = false;
            }
            return;
        }
        preview.entries = [];
        var entriesThreshold = 5;
        for (var i = 0; i < entries.length; ++i) {
            if (preview.entries.length >= entriesThreshold) {
                preview.overflow = true;
                preview.lossless = false;
                break;
            }
            var entry = nullifyObjectProto(entries[i]);
            var previewEntry = {
                value: generateValuePreview(entry.value),
                __proto__: null
            };
            if ("key" in entry)
                previewEntry.key = generateValuePreview(entry.key);
            push(preview.entries, previewEntry);
        }

        /**
         * @param {*} value
         * @return {!RuntimeAgent.ObjectPreview}
         */
        function generateValuePreview(value)
        {
            var remoteObject = new InjectedScript.RemoteObject(value, undefined, true, undefined, true, undefined, undefined, true);
            var valuePreview = remoteObject.preview || remoteObject._createEmptyPreview();
            if (!valuePreview.lossless)
                preview.lossless = false;
            return valuePreview;
        }
    },

    /**
     * @param {string} string
     * @param {number} maxLength
     * @param {boolean=} middle
     * @return {string}
     */
    _abbreviateString: function(string, maxLength, middle)
    {
        if (string.length <= maxLength)
            return string;
        if (middle) {
            var leftHalf = maxLength >> 1;
            var rightHalf = maxLength - leftHalf - 1;
            return string.substr(0, leftHalf) + "\u2026" + string.substr(string.length - rightHalf, rightHalf);
        }
        return string.substr(0, maxLength) + "\u2026";
    },

    __proto__: null
}

/**
 * @constructor
 * @param {number} ordinal
 * @param {!JavaScriptCallFrame} callFrame
 * @param {number} asyncOrdinal
 */
InjectedScript.CallFrameProxy = function(ordinal, callFrame, asyncOrdinal)
{
    this.callFrameId = "{\"ordinal\":" + ordinal + ",\"injectedScriptId\":" + injectedScriptId + (asyncOrdinal ? ",\"asyncOrdinal\":" + asyncOrdinal : "") + "}";
    this.functionName = callFrame.functionName;
    this.functionLocation = { scriptId: toString(callFrame.sourceID), lineNumber: callFrame.functionLine, columnNumber: callFrame.functionColumn, __proto__: null };
    this.location = { scriptId: toString(callFrame.sourceID), lineNumber: callFrame.line, columnNumber: callFrame.column, __proto__: null };
    this.scopeChain = this._wrapScopeChain(callFrame);
    this.this = injectedScript._wrapObject(callFrame.thisObject, "backtrace");
    if (callFrame.isAtReturn)
        this.returnValue = injectedScript._wrapObject(callFrame.returnValue, "backtrace");
}

InjectedScript.CallFrameProxy.prototype = {
    /**
     * @param {!JavaScriptCallFrame} callFrame
     * @return {!Array.<!DebuggerAgent.Scope>}
     */
    _wrapScopeChain: function(callFrame)
    {
        var scopeChain = callFrame.scopeChain;
        var scopeChainProxy = [];
        for (var i = 0; i < scopeChain.length; ++i)
            scopeChainProxy[i] = InjectedScript.CallFrameProxy._createScopeJson(callFrame.scopeType(i), scopeChain[i], "backtrace");
        return scopeChainProxy;
    },

    __proto__: null
}

/**
 * @const
 * @type {!Object.<number, !DebuggerAgent.ScopeType>}
 */
InjectedScript.CallFrameProxy._scopeTypeNames = {
    0: "global",
    1: "local",
    2: "with",
    3: "closure",
    4: "catch",
    5: "block",
    6: "script",
    __proto__: null
};

/**
 * @param {number} scopeTypeCode
 * @param {*} scopeObject
 * @param {string} groupId
 * @return {!DebuggerAgent.Scope}
 */
InjectedScript.CallFrameProxy._createScopeJson = function(scopeTypeCode, scopeObject, groupId)
{
    return {
        object: injectedScript._wrapObject(scopeObject, groupId),
        type: InjectedScript.CallFrameProxy._scopeTypeNames[scopeTypeCode],
        __proto__: null
    };
}

/**
 * @constructor
 * @param {!CommandLineAPIImpl} commandLineAPIImpl
 * @param {?JavaScriptCallFrame} callFrame
 */
function CommandLineAPI(commandLineAPIImpl, callFrame)
{
    /**
     * @param {string} member
     * @return {boolean}
     */
    function inScopeVariables(member)
    {
        if (!callFrame)
            return (member in inspectedGlobalObject);

        var scopeChain = callFrame.scopeChain;
        for (var i = 0; i < scopeChain.length; ++i) {
            if (member in scopeChain[i])
                return true;
        }
        return false;
    }

    /**
     * @param {string} name The name of the method for which a toString method should be generated.
     * @return {function():string}
     */
    function customToStringMethod(name)
    {
        return function()
        {
            var funcArgsSyntax = "";
            try {
                var funcSyntax = "" + commandLineAPIImpl[name];
                funcSyntax = funcSyntax.replace(/\n/g, " ");
                funcSyntax = funcSyntax.replace(/^function[^\(]*\(([^\)]*)\).*$/, "$1");
                funcSyntax = funcSyntax.replace(/\s*,\s*/g, ", ");
                funcSyntax = funcSyntax.replace(/\bopt_(\w+)\b/g, "[$1]");
                funcArgsSyntax = funcSyntax.trim();
            } catch (e) {
            }
            return "function " + name + "(" + funcArgsSyntax + ") { [Command Line API] }";
        };
    }

    for (var i = 0; i < CommandLineAPI.members_.length; ++i) {
        var member = CommandLineAPI.members_[i];
        if (inScopeVariables(member))
            continue;

        this[member] = bind(commandLineAPIImpl[member], commandLineAPIImpl);
        this[member].toString = customToStringMethod(member);
    }

    for (var i = 0; i < 5; ++i) {
        var member = "$" + i;
        if (inScopeVariables(member))
            continue;

        this.__defineGetter__("$" + i, bind(commandLineAPIImpl._inspectedObject, commandLineAPIImpl, i));
    }

    this.$_ = injectedScript._lastResult;

    this.__proto__ = null;
}

// NOTE: Please keep the list of API methods below snchronized to that in WebInspector.RuntimeModel!
// NOTE: Argument names of these methods will be printed in the console, so use pretty names!
/**
 * @type {!Array.<string>}
 * @const
 */
CommandLineAPI.members_ = [
    "$", "$$", "$x", "dir", "dirxml", "keys", "values", "profile", "profileEnd",
    "monitorEvents", "unmonitorEvents", "inspect", "copy", "clear", "getEventListeners",
    "debug", "undebug", "monitor", "unmonitor", "table"
];

/**
 * @constructor
 */
function CommandLineAPIImpl()
{
}

CommandLineAPIImpl.prototype = {
    /**
     * @param {string} selector
     * @param {!Node=} opt_startNode
     * @return {*}
     */
    $: function (selector, opt_startNode)
    {
        if (this._canQuerySelectorOnNode(opt_startNode))
            return opt_startNode.querySelector(selector);

        return inspectedGlobalObject.document.querySelector(selector);
    },

    /**
     * @param {string} selector
     * @param {!Node=} opt_startNode
     * @return {*}
     */
    $$: function (selector, opt_startNode)
    {
        if (this._canQuerySelectorOnNode(opt_startNode))
            return opt_startNode.querySelectorAll(selector);
        return inspectedGlobalObject.document.querySelectorAll(selector);
    },

    /**
     * @param {!Node=} node
     * @return {boolean}
     */
    _canQuerySelectorOnNode: function(node)
    {
        return !!node && InjectedScriptHost.subtype(node) === "node" && (node.nodeType === Node.ELEMENT_NODE || node.nodeType === Node.DOCUMENT_NODE || node.nodeType === Node.DOCUMENT_FRAGMENT_NODE);
    },

    /**
     * @param {string} xpath
     * @param {!Node=} opt_startNode
     * @return {*}
     */
    $x: function(xpath, opt_startNode)
    {
        var doc = (opt_startNode && opt_startNode.ownerDocument) || inspectedGlobalObject.document;
        var result = doc.evaluate(xpath, opt_startNode || doc, null, XPathResult.ANY_TYPE, null);
        switch (result.resultType) {
        case XPathResult.NUMBER_TYPE:
            return result.numberValue;
        case XPathResult.STRING_TYPE:
            return result.stringValue;
        case XPathResult.BOOLEAN_TYPE:
            return result.booleanValue;
        default:
            var nodes = [];
            var node;
            while (node = result.iterateNext())
                push(nodes, node);
            return nodes;
        }
    },

    /**
     * @return {*}
     */
    dir: function(var_args)
    {
        return InjectedScriptHost.callFunction(inspectedGlobalObject.console.dir, inspectedGlobalObject.console, slice(arguments));
    },

    /**
     * @return {*}
     */
    dirxml: function(var_args)
    {
        return InjectedScriptHost.callFunction(inspectedGlobalObject.console.dirxml, inspectedGlobalObject.console, slice(arguments));
    },

    /**
     * @return {!Array.<string>}
     */
    keys: function(object)
    {
        return Object.keys(object);
    },

    /**
     * @return {!Array.<*>}
     */
    values: function(object)
    {
        var result = [];
        for (var key in object)
            push(result, object[key]);
        return result;
    },

    /**
     * @return {*}
     */
    profile: function(opt_title)
    {
        return InjectedScriptHost.callFunction(inspectedGlobalObject.console.profile, inspectedGlobalObject.console, slice(arguments));
    },

    /**
     * @return {*}
     */
    profileEnd: function(opt_title)
    {
        return InjectedScriptHost.callFunction(inspectedGlobalObject.console.profileEnd, inspectedGlobalObject.console, slice(arguments));
    },

    /**
     * @param {!Object} object
     * @param {!Array.<string>|string=} opt_types
     */
    monitorEvents: function(object, opt_types)
    {
        if (!object || !object.addEventListener || !object.removeEventListener)
            return;
        var types = this._normalizeEventTypes(opt_types);
        for (var i = 0; i < types.length; ++i) {
            object.removeEventListener(types[i], this._logEvent, false);
            object.addEventListener(types[i], this._logEvent, false);
        }
    },

    /**
     * @param {!Object} object
     * @param {!Array.<string>|string=} opt_types
     */
    unmonitorEvents: function(object, opt_types)
    {
        if (!object || !object.addEventListener || !object.removeEventListener)
            return;
        var types = this._normalizeEventTypes(opt_types);
        for (var i = 0; i < types.length; ++i)
            object.removeEventListener(types[i], this._logEvent, false);
    },

    /**
     * @param {*} object
     * @return {*}
     */
    inspect: function(object)
    {
        return injectedScript._inspect(object);
    },

    copy: function(object)
    {
        var string;
        if (injectedScript._subtype(object) === "node") {
            string = object.outerHTML;
        } else if (injectedScript.isPrimitiveValue(object)) {
            string = toString(object);
        } else {
            try {
                string = JSON.stringify(object, null, "  ");
            } catch (e) {
                string = toString(object);
            }
        }

        var hints = { copyToClipboard: true, __proto__: null };
        var remoteObject = injectedScript._wrapObject(string, "")
        InjectedScriptHost.inspect(remoteObject, hints);
    },

    clear: function()
    {
        InjectedScriptHost.clearConsoleMessages();
    },

    /**
     * @param {!Node} node
     * @return {!Array.<!{type: string, listener: function(), useCapture: boolean, remove: function()}>|undefined}
     */
    getEventListeners: function(node)
    {
        var result = nullifyObjectProto(InjectedScriptHost.getEventListeners(node));
        if (!result)
            return result;
        /** @this {{type: string, listener: function(), useCapture: boolean}} */
        var removeFunc = function()
        {
            node.removeEventListener(this.type, this.listener, this.useCapture);
        }
        for (var type in result) {
            var listeners = result[type];
            for (var i = 0, listener; listener = listeners[i]; ++i) {
                listener["type"] = type;
                listener["remove"] = removeFunc;
            }
        }
        return result;
    },

    debug: function(fn)
    {
        InjectedScriptHost.debugFunction(fn);
    },

    undebug: function(fn)
    {
        InjectedScriptHost.undebugFunction(fn);
    },

    monitor: function(fn)
    {
        InjectedScriptHost.monitorFunction(fn);
    },

    unmonitor: function(fn)
    {
        InjectedScriptHost.unmonitorFunction(fn);
    },

    table: function(data, opt_columns)
    {
        InjectedScriptHost.callFunction(inspectedGlobalObject.console.table, inspectedGlobalObject.console, slice(arguments));
    },

    /**
     * @param {number} num
     */
    _inspectedObject: function(num)
    {
        return InjectedScriptHost.inspectedObject(num);
    },

    /**
     * @param {!Array.<string>|string=} types
     * @return {!Array.<string>}
     */
    _normalizeEventTypes: function(types)
    {
        if (typeof types === "undefined")
            types = ["mouse", "key", "touch", "pointer", "control", "load", "unload", "abort", "error", "select", "input", "change", "submit", "reset", "focus", "blur", "resize", "scroll", "search", "devicemotion", "deviceorientation"];
        else if (typeof types === "string")
            types = [types];

        var result = [];
        for (var i = 0; i < types.length; ++i) {
            if (types[i] === "mouse")
                push(result, "click", "dblclick", "mousedown", "mouseeenter", "mouseleave", "mousemove", "mouseout", "mouseover", "mouseup", "mouseleave", "mousewheel");
            else if (types[i] === "key")
                push(result, "keydown", "keyup", "keypress", "textInput");
            else if (types[i] === "touch")
                push(result, "touchstart", "touchmove", "touchend", "touchcancel");
            else if (types[i] === "pointer")
                push(result, "pointerover", "pointerout", "pointerenter", "pointerleave", "pointerdown", "pointerup", "pointermove", "pointercancel", "gotpointercapture", "lostpointercapture");
            else if (types[i] === "control")
                push(result, "resize", "scroll", "zoom", "focus", "blur", "select", "input", "change", "submit", "reset");
            else
                push(result, types[i]);
        }
        return result;
    },

    /**
     * @param {!Event} event
     */
    _logEvent: function(event)
    {
        inspectedGlobalObject.console.log(event.type, event);
    }
}

injectedScript._commandLineAPIImpl = new CommandLineAPIImpl();
return injectedScript;
})
