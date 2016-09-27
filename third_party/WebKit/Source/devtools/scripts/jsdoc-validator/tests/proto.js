/**
 * @constructor
 */
Base = function() {}

/**
 * @constructor
 * @extends {Base}
 */
DerivedNoProto = function() {}

/**
 * @constructor
 * @extends {Base}
 */
DerivedBadProto = function() {}

DerivedBadProto.prototype = {
    __proto__: Base
}

/**
 * @interface
 */
InterfaceWithProto = function() {}

InterfaceWithProto.prototype = {
    __proto__: Base.prototype
}

/**
 * @constructor
 */
ProtoNoExtends = function() {}

ProtoNoExtends.prototype = {
    __proto__: Base.prototype
}

/**
 * @constructor
 * @extends {Base}
 */
ProtoNotSameAsExtends = function() {}

ProtoNotSameAsExtends.prototype = {
    __proto__: Object.prototype
}

/**
 * @constructor
 * @extends {Base}
 */
ProtoNotObjectLiteral = function() {}

ProtoNotObjectLiteral.prototype = Object;

/**
 * @constructor
 * @extends {Base}
 */
DerivedNoSuperCall = function() {
}

DerivedNoSuperCall.prototype = {
    __proto__: Base.prototype
}

/**
 * @constructor
 * @extends {Base}
 */
DerivedBadSuperCall = function() {
    Base.call(1);
}

DerivedBadSuperCall.prototype = {
    __proto__: Base.prototype
}

/**
 * @extends {Base}
 */
GoodDerived = function() {
    Base.call(this);
}

GoodDerived.prototype = {
    __proto__: Base.prototype
}

/**
 * @constructor
 * @template T
 */
var Set = function() {}

Set.prototype = {
    add: function(item) {},
    remove: function(item) {},
    /** @return {boolean} */
    contains: function(item) { return true; }
}

/**
 * @constructor
 * @extends {Set.<T>}
 * @template T
 */
var GoodSetSubclass = function()
{
    Set.call(this);
}

GoodSetSubclass.prototype = {
    __proto__: Set.prototype
}

/**
 * @constructor
 * @extends {Set.<T>}
 * @template T
 */
var BadSetSubclass = function()
{
}

BadSetSubclass.prototype = {
}

var NS = {};

/**
 * @constructor
 * @extends {Base}
 */
NS.BadSubClass = function() {}
