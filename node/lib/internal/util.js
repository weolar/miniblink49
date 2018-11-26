'use strict';

const binding = process.binding('util');
const prefix = `(${process.release.name}:${process.pid}) `;

exports.getHiddenValue = binding.getHiddenValue;
exports.setHiddenValue = binding.setHiddenValue;

const createPromise = binding.createPromise;
const promiseResolve = binding.promiseResolve;
const promiseReject = binding.promiseReject;
const kCustomPromisifiedSymbol = Symbol('util.promisify.custom');
const kCustomPromisifyArgsSymbol = Symbol('customPromisifyArgs');

function promisify(original) {

    if (typeof original !== 'function') {
    	throw new ERR_INVALID_ARG_TYPE('original', 'Function', original);
    	console.log("original ERR_INVALID_ARG_TYPE");
    }

    if (original[kCustomPromisifiedSymbol]) {
        const fn = original[kCustomPromisifiedSymbol];
        if (typeof fn !== 'function') {
            //throw new ERR_INVALID_ARG_TYPE('util.promisify.custom', 'Function', fn);
            console.log("util.promisify.custom ERR_INVALID_ARG_TYPE");
        }
        Object.defineProperty(fn, kCustomPromisifiedSymbol, {
            value: fn,
            enumerable: false,
            writable: false,
            configurable: true
        });
        return fn;
    }

    // Names to create an object from in case the callback receives multiple
    // arguments, e.g. ['stdout', 'stderr'] for child_process.exec.
    const argumentNames = original[kCustomPromisifyArgsSymbol];

    function fn(...args) {
        const promise = createPromise();
        try {
        	
            original.call(this, ...args, function (err, ...values) {
            	
                if (err) {
                    promiseReject(promise, err);
                } else if (argumentNames !== undefined && values.length > 1) {
                    const obj = {};
                    for (var i = 0; i < argumentNames.length; i++) obj[argumentNames[i]] = values[i];
                    promiseResolve(promise, obj);
                } else {
                    promiseResolve(promise, values[0]);
                }
                /**/
            });
            
        } catch(err) {
            promiseReject(promise, err);
        }
        return promise;
    }
	
    Object.setPrototypeOf(fn, Object.getPrototypeOf(original));

    Object.defineProperty(fn, kCustomPromisifiedSymbol, {
        value: fn,
        enumerable: false,
        writable: false,
        configurable: true
    });
    return Object.defineProperties(fn, Object.getOwnPropertyDescriptors(original));
    /**/
}


promisify.custom = kCustomPromisifiedSymbol;
exports.promisify = promisify;
	
// The `buffer` module uses this. Defining it here instead of in the public
// `util` module makes it accessible without having to `require('util')` there.
exports.customInspectSymbol = Symbol('util.inspect.custom');

// All the internal deprecations have to use this function only, as this will
// prepend the prefix to the actual message.
exports.deprecate = function(fn, msg) {
  return exports._deprecate(fn, msg);
};

// All the internal deprecations have to use this function only, as this will
// prepend the prefix to the actual message.
exports.printDeprecationMessage = function(msg, warned, ctor) {
  if (warned || process.noDeprecation)
    return true;
  process.emitWarning(msg, 'DeprecationWarning',
                      ctor || exports.printDeprecationMessage);
  return true;
};

exports.error = function(msg) {
  const fmt = `${prefix}${msg}`;
  if (arguments.length > 1) {
    const args = new Array(arguments.length);
    args[0] = fmt;
    for (var i = 1; i < arguments.length; ++i)
      args[i] = arguments[i];
    console.error.apply(console, args);
  } else {
    console.error(fmt);
  }
};

exports.trace = function(msg) {
  console.trace(`${prefix}${msg}`);
};

// Mark that a method should not be used.
// Returns a modified function which warns once by default.
// If --no-deprecation is set, then it is a no-op.
exports._deprecate = function(fn, msg) {
  // Allow for deprecating things in the process of starting up.
  if (global.process === undefined) {
    return function() {
      return exports._deprecate(fn, msg).apply(this, arguments);
    };
  }

  if (process.noDeprecation === true) {
    return fn;
  }

  var warned = false;
  function deprecated() {
    warned = exports.printDeprecationMessage(msg, warned, deprecated);
    if (new.target) {
      return Reflect.construct(fn, arguments, new.target);
    }
    return fn.apply(this, arguments);
  }

  // The wrapper will keep the same prototype as fn to maintain prototype chain
  Object.setPrototypeOf(deprecated, fn);
  if (fn.prototype) {
    // Setting this (rather than using Object.setPrototype, as above) ensures
    // that calling the unwrapped constructor gives an instanceof the wrapped
    // constructor.
    deprecated.prototype = fn.prototype;
  }

  return deprecated;
};

exports.decorateErrorStack = function decorateErrorStack(err) {
  if (!(exports.isError(err) && err.stack) ||
      exports.getHiddenValue(err, 'node:decorated') === true)
    return;

  const arrow = exports.getHiddenValue(err, 'node:arrowMessage');

  if (arrow) {
    err.stack = arrow + err.stack;
    exports.setHiddenValue(err, 'node:decorated', true);
  }
};

exports.isError = function isError(e) {
  return exports.objectToString(e) === '[object Error]' || e instanceof Error;
};

exports.objectToString = function objectToString(o) {
  return Object.prototype.toString.call(o);
};

const noCrypto = !process.versions.openssl;
exports.assertCrypto = function(exports) {
  if (noCrypto)
    throw new Error('Node.js is not compiled with openssl crypto support');
};

// Filters duplicate strings. Used to support functions in crypto and tls
// modules. Implemented specifically to maintain existing behaviors in each.
exports.filterDuplicateStrings = function filterDuplicateStrings(items, low) {
  if (!Array.isArray(items))
    return [];
  const len = items.length;
  if (len <= 1)
    return items;
  const map = new Map();
  for (var i = 0; i < len; i++) {
    const item = items[i];
    const key = item.toLowerCase();
    if (low) {
      map.set(key, key);
    } else {
      if (!map.has(key) || map.get(key) <= item)
        map.set(key, item);
    }
  }
  return Array.from(map.values()).sort();
};

exports.cachedResult = function cachedResult(fn) {
  var result;
  return () => {
    if (result === undefined)
      result = fn();
    return result;
  };
};

exports.kIsEncodingSymbol = Symbol('node.isEncoding');
exports.normalizeEncoding = function normalizeEncoding(enc) {
  if (!enc) return 'utf8';
  var low;
  for (;;) {
    switch (enc) {
      case 'utf8':
      case 'utf-8':
        return 'utf8';
      case 'ucs2':
      case 'utf16le':
      case 'ucs-2':
      case 'utf-16le':
        return 'utf16le';
      case 'binary':
        return 'latin1';
      case 'base64':
      case 'ascii':
      case 'latin1':
      case 'hex':
        return enc;
      default:
        if (low) return; // undefined
        enc = ('' + enc).toLowerCase();
        low = true;
    }
  }
};
