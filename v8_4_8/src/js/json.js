// Copyright 2009 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(function(global, utils) {

"use strict";

%CheckIsBootstrapping();

// -------------------------------------------------------------------
// Imports

var GlobalJSON = global.JSON;
var InternalArray = utils.InternalArray;
var MakeTypeError;
var MaxSimple;
var MinSimple;
var ObjectHasOwnProperty;
var toStringTagSymbol = utils.ImportNow("to_string_tag_symbol");

utils.Import(function(from) {
  MakeTypeError = from.MakeTypeError;
  MaxSimple = from.MaxSimple;
  MinSimple = from.MinSimple;
  ObjectHasOwnProperty = from.ObjectHasOwnProperty;
});

// -------------------------------------------------------------------

function Revive(holder, name, reviver) {
  var val = holder[name];
  if (IS_OBJECT(val)) {
    if (IS_ARRAY(val)) {
      var length = val.length;
      for (var i = 0; i < length; i++) {
        var newElement = Revive(val, %_NumberToString(i), reviver);
        val[i] = newElement;
      }
    } else {
      for (var p in val) {
        if (HAS_OWN_PROPERTY(val, p)) {
          var newElement = Revive(val, p, reviver);
          if (IS_UNDEFINED(newElement)) {
            delete val[p];
          } else {
            val[p] = newElement;
          }
        }
      }
    }
  }
  return %_Call(reviver, holder, name, val);
}


function JSONParse(text, reviver) {
  var unfiltered = %ParseJson(text);
  if (IS_CALLABLE(reviver)) {
    return Revive({'': unfiltered}, '', reviver);
  } else {
    return unfiltered;
  }
}


function SerializeArray(value, replacer, stack, indent, gap) {
  if (!%PushIfAbsent(stack, value)) throw MakeTypeError(kCircularStructure);
  var stepback = indent;
  indent += gap;
  var partial = new InternalArray();
  var len = value.length;
  for (var i = 0; i < len; i++) {
    var strP = JSONSerialize(%_NumberToString(i), value, replacer, stack,
                             indent, gap);
    if (IS_UNDEFINED(strP)) {
      strP = "null";
    }
    partial.push(strP);
  }
  var final;
  if (gap == "") {
    final = "[" + partial.join(",") + "]";
  } else if (partial.length > 0) {
    var separator = ",\n" + indent;
    final = "[\n" + indent + partial.join(separator) + "\n" +
        stepback + "]";
  } else {
    final = "[]";
  }
  stack.pop();
  return final;
}


function SerializeObject(value, replacer, stack, indent, gap) {
  if (!%PushIfAbsent(stack, value)) throw MakeTypeError(kCircularStructure);
  var stepback = indent;
  indent += gap;
  var partial = new InternalArray();
  if (IS_ARRAY(replacer)) {
    var length = replacer.length;
    for (var i = 0; i < length; i++) {
      if (HAS_OWN_PROPERTY(replacer, i)) {
        var p = replacer[i];
        var strP = JSONSerialize(p, value, replacer, stack, indent, gap);
        if (!IS_UNDEFINED(strP)) {
          var member = %QuoteJSONString(p) + ":";
          if (gap != "") member += " ";
          member += strP;
          partial.push(member);
        }
      }
    }
  } else {
    for (var p in value) {
      if (HAS_OWN_PROPERTY(value, p)) {
        var strP = JSONSerialize(p, value, replacer, stack, indent, gap);
        if (!IS_UNDEFINED(strP)) {
          var member = %QuoteJSONString(p) + ":";
          if (gap != "") member += " ";
          member += strP;
          partial.push(member);
        }
      }
    }
  }
  var final;
  if (gap == "") {
    final = "{" + partial.join(",") + "}";
  } else if (partial.length > 0) {
    var separator = ",\n" + indent;
    final = "{\n" + indent + partial.join(separator) + "\n" +
        stepback + "}";
  } else {
    final = "{}";
  }
  stack.pop();
  return final;
}


function JSONSerialize(key, holder, replacer, stack, indent, gap) {
  var value = holder[key];
  if (IS_SPEC_OBJECT(value)) {
    var toJSON = value.toJSON;
    if (IS_CALLABLE(toJSON)) {
      value = %_Call(toJSON, value, key);
    }
  }
  if (IS_CALLABLE(replacer)) {
    value = %_Call(replacer, holder, key, value);
  }
  if (IS_STRING(value)) {
    return %QuoteJSONString(value);
  } else if (IS_NUMBER(value)) {
    return JSON_NUMBER_TO_STRING(value);
  } else if (IS_BOOLEAN(value)) {
    return value ? "true" : "false";
  } else if (IS_NULL(value)) {
    return "null";
  } else if (IS_SPEC_OBJECT(value) && !IS_CALLABLE(value)) {
    // Non-callable object. If it's a primitive wrapper, it must be unwrapped.
    if (IS_ARRAY(value)) {
      return SerializeArray(value, replacer, stack, indent, gap);
    } else if (IS_NUMBER_WRAPPER(value)) {
      value = TO_NUMBER(value);
      return JSON_NUMBER_TO_STRING(value);
    } else if (IS_STRING_WRAPPER(value)) {
      return %QuoteJSONString(TO_STRING(value));
    } else if (IS_BOOLEAN_WRAPPER(value)) {
      return %_ValueOf(value) ? "true" : "false";
    } else {
      return SerializeObject(value, replacer, stack, indent, gap);
    }
  }
  // Undefined or a callable object.
  return UNDEFINED;
}


function JSONStringify(value, replacer, space) {
  if (%_ArgumentsLength() == 1) {
    return %BasicJSONStringify(value);
  }
  if (IS_ARRAY(replacer)) {
    // Deduplicate replacer array items.
    var property_list = new InternalArray();
    var seen_properties = { __proto__: null };
    var length = replacer.length;
    for (var i = 0; i < length; i++) {
      var v = replacer[i];
      var item;
      if (IS_STRING(v)) {
        item = v;
      } else if (IS_NUMBER(v)) {
        item = %_NumberToString(v);
      } else if (IS_STRING_WRAPPER(v) || IS_NUMBER_WRAPPER(v)) {
        item = TO_STRING(v);
      } else {
        continue;
      }
      if (!seen_properties[item]) {
        property_list.push(item);
        seen_properties[item] = true;
      }
    }
    replacer = property_list;
  }
  if (IS_OBJECT(space)) {
    // Unwrap 'space' if it is wrapped
    if (IS_NUMBER_WRAPPER(space)) {
      space = TO_NUMBER(space);
    } else if (IS_STRING_WRAPPER(space)) {
      space = TO_STRING(space);
    }
  }
  var gap;
  if (IS_NUMBER(space)) {
    space = MaxSimple(0, MinSimple(TO_INTEGER(space), 10));
    gap = %_SubString("          ", 0, space);
  } else if (IS_STRING(space)) {
    if (space.length > 10) {
      gap = %_SubString(space, 0, 10);
    } else {
      gap = space;
    }
  } else {
    gap = "";
  }
  return JSONSerialize('', {'': value}, replacer, new InternalArray(), "", gap);
}

// -------------------------------------------------------------------

%AddNamedProperty(GlobalJSON, toStringTagSymbol, "JSON", READ_ONLY | DONT_ENUM);

// Set up non-enumerable properties of the JSON object.
utils.InstallFunctions(GlobalJSON, DONT_ENUM, [
  "parse", JSONParse,
  "stringify", JSONStringify
]);

// -------------------------------------------------------------------
// JSON Builtins

function JsonSerializeAdapter(key, object) {
  var holder = {};
  holder[key] = object;
  // No need to pass the actual holder since there is no replacer function.
  return JSONSerialize(key, holder, UNDEFINED, new InternalArray(), "", "");
}

%InstallToContext(["json_serialize_adapter", JsonSerializeAdapter]);

})
