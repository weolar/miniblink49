// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

define(function() {
  // Equality function based on isEqual in
  // Underscore.js 1.5.2
  // http://underscorejs.org
  // (c) 2009-2013 Jeremy Ashkenas,
  //               DocumentCloud,
  //               and Investigative Reporters & Editors
  // Underscore may be freely distributed under the MIT license.
  //
  function has(obj, key) {
    return obj.hasOwnProperty(key);
  }
  function isFunction(obj) {
    return typeof obj === 'function';
  }
  function isArrayBufferClass(className) {
    return className == '[object ArrayBuffer]' ||
        className.match(/\[object \w+\d+(Clamped)?Array\]/);
  }
  // Internal recursive comparison function for `isEqual`.
  function eq(a, b, aStack, bStack) {
    // Identical objects are equal. `0 === -0`, but they aren't identical.
    // See the Harmony `egal` proposal:
    // http://wiki.ecmascript.org/doku.php?id=harmony:egal.
    if (a === b)
      return a !== 0 || 1 / a == 1 / b;
    // A strict comparison is necessary because `null == undefined`.
    if (a == null || b == null)
      return a === b;
    // Compare `[[Class]]` names.
    var className = toString.call(a);
    if (className != toString.call(b))
      return false;
    switch (className) {
      // Strings, numbers, dates, and booleans are compared by value.
      case '[object String]':
        // Primitives and their corresponding object wrappers are equivalent;
        // thus, `"5"` is equivalent to `new String("5")`.
        return a == String(b);
      case '[object Number]':
        // `NaN`s are equivalent, but non-reflexive. An `egal` comparison is
        // performed for other numeric values.
        return a != +a ? b != +b : (a == 0 ? 1 / a == 1 / b : a == +b);
      case '[object Date]':
      case '[object Boolean]':
        // Coerce dates and booleans to numeric primitive values. Dates are
        // compared by their millisecond representations. Note that invalid
        // dates with millisecond representations of `NaN` are not equivalent.
        return +a == +b;
      // RegExps are compared by their source patterns and flags.
      case '[object RegExp]':
        return a.source == b.source &&
               a.global == b.global &&
               a.multiline == b.multiline &&
               a.ignoreCase == b.ignoreCase;
    }
    if (typeof a != 'object' || typeof b != 'object')
      return false;
    // Assume equality for cyclic structures. The algorithm for detecting
    // cyclic structures is adapted from ES 5.1 section 15.12.3, abstract
    // operation `JO`.
    var length = aStack.length;
    while (length--) {
      // Linear search. Performance is inversely proportional to the number of
      // unique nested structures.
      if (aStack[length] == a)
        return bStack[length] == b;
    }
    // Objects with different constructors are not equivalent, but `Object`s
    // from different frames are.
    var aCtor = a.constructor, bCtor = b.constructor;
    if (aCtor !== bCtor && !(isFunction(aCtor) && (aCtor instanceof aCtor) &&
                             isFunction(bCtor) && (bCtor instanceof bCtor))
                        && ('constructor' in a && 'constructor' in b)) {
      return false;
    }
    // Add the first object to the stack of traversed objects.
    aStack.push(a);
    bStack.push(b);
    var size = 0, result = true;
    // Recursively compare Maps, objects and arrays.
    if (className == '[object Array]' || isArrayBufferClass(className)) {
      // Compare array lengths to determine if a deep comparison is necessary.
      size = a.length;
      result = size == b.length;
      if (result) {
        // Deep compare the contents, ignoring non-numeric properties.
        while (size--) {
          if (!(result = eq(a[size], b[size], aStack, bStack)))
            break;
        }
      }
    } else if (className == '[object Map]') {
      result = a.size == b.size;
      if (result) {
        var entries = a.entries();
        for (var e = entries.next(); result && !e.done; e = entries.next()) {
          var key = e.value[0];
          var value = e.value[1];
          result = b.has(key) && eq(value, b.get(key), aStack, bStack);
        }
      }
    } else {
      // Deep compare objects.
      for (var key in a) {
        if (has(a, key)) {
          // Count the expected number of properties.
          size++;
          // Deep compare each member.
          if (!(result = has(b, key) && eq(a[key], b[key], aStack, bStack)))
            break;
        }
      }
      // Ensure that both objects contain the same number of properties.
      if (result) {
        for (key in b) {
          if (has(b, key) && !(size--))
            break;
        }
        result = !size;
      }
    }
    // Remove the first object from the stack of traversed objects.
    aStack.pop();
    bStack.pop();
    return result;
  };

  function describe(subjects) {
    var descriptions = [];
    Object.getOwnPropertyNames(subjects).forEach(function(name) {
      if (name === "Description")
        descriptions.push(subjects[name]);
      else
        descriptions.push(name + ": " + JSON.stringify(subjects[name]));
    });
    return descriptions.join(" ");
  }

  var predicates = {};

  predicates.toBe = function(actual, expected) {
    return {
      "result": actual === expected,
      "message": describe({
        "Actual": actual,
        "Expected": expected,
      }),
    };
  };

  predicates.toEqual = function(actual, expected) {
    return {
      "result": eq(actual, expected, [], []),
      "message": describe({
        "Actual": actual,
        "Expected": expected,
      }),
    };
  };

  predicates.toBeDefined = function(actual) {
    return {
      "result": typeof actual !== "undefined",
      "message": describe({
        "Actual": actual,
        "Description": "Expected a defined value",
      }),
    };
  };

  predicates.toBeUndefined = function(actual) {
    // Recall: undefined is just a global variable. :)
    return {
      "result": typeof actual === "undefined",
      "message": describe({
        "Actual": actual,
        "Description": "Expected an undefined value",
      }),
    };
  };

  predicates.toBeNull = function(actual) {
    // Recall: typeof null === "object".
    return {
      "result": actual === null,
      "message": describe({
        "Actual": actual,
        "Expected": null,
      }),
    };
  };

  predicates.toBeTruthy = function(actual) {
    return {
      "result": !!actual,
      "message": describe({
        "Actual": actual,
        "Description": "Expected a truthy value",
      }),
    };
  };

  predicates.toBeFalsy = function(actual) {
    return {
      "result": !!!actual,
      "message": describe({
        "Actual": actual,
        "Description": "Expected a falsy value",
      }),
    };
  };

  predicates.toContain = function(actual, element) {
    return {
      "result": (function () {
        for (var i = 0; i < actual.length; ++i) {
          if (eq(actual[i], element, [], []))
            return true;
        }
        return false;
      })(),
      "message": describe({
        "Actual": actual,
        "Element": element,
      }),
    };
  };

  predicates.toBeLessThan = function(actual, reference) {
    return {
      "result": actual < reference,
      "message": describe({
        "Actual": actual,
        "Reference": reference,
      }),
    };
  };

  predicates.toBeGreaterThan = function(actual, reference) {
    return {
      "result": actual > reference,
      "message": describe({
        "Actual": actual,
        "Reference": reference,
      }),
    };
  };

  predicates.toThrow = function(actual) {
    return {
      "result": (function () {
        if (!isFunction(actual))
          throw new TypeError;
        try {
          actual();
        } catch (ex) {
          return true;
        }
        return false;
      })(),
      "message": "Expected function to throw",
    };
  }

  function negate(predicate) {
    return function() {
      var outcome = predicate.apply(null, arguments);
      outcome.result = !outcome.result;
      return outcome;
    }
  }

  function check(predicate) {
    return function() {
      var outcome = predicate.apply(null, arguments);
      if (outcome.result)
        return;
      throw outcome.message;
    };
  }

  function Condition(actual) {
    this.not = {};
    Object.getOwnPropertyNames(predicates).forEach(function(name) {
      var bound = predicates[name].bind(null, actual);
      this[name] = check(bound);
      this.not[name] = check(negate(bound));
    }, this);
  }

  return function(actual) {
    return new Condition(actual);
  };
});
