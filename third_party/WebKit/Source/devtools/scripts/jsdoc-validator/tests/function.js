function badFuncNoAnnotation() {
    return 1; // ERROR - no @return annotation.
}

/**
 * @return {number}
 */
function badFuncAnnotatedButNoReturn() // ERROR - no @return annotation.
{
}

/**
 * @return {number}
 */
function badFuncAnnotatedButNoReturnValue() // ERROR - no returned value.
{
    return;
}

/**
 * @return {number}
 */
function goodFunc() // OK - annotated @return.
{
    return 1;
}

/**
 * @returns {number}
 */
function badReturnsShouldBeReturnFunc() // ERROR - @returns, should be @return.
{
    return 1;
}

/**
 * @return number
 */
function badReturnShouldBeTypedFunc() // ERROR - number, not {number}.
{
    return 1;
}

/**
 * @param number foo
 * @param bar
 */
function badParamAnnotationsFunc(foo, bar) // ERROR - @param's should be well-formed
{
    return 1;
}


/**
 * @returns {number}
 */
function badReturnsShouldBeReturnNoValueFunc() // ERROR - @returns, should be @return.
{
    return;
}

/**
 * @returns {number}
 */
function badReturnsShouldBeAbsentFunc() // ERROR - @returns, should be absent.
{
}

/**
 * @constructor
 */
function TypeOne() {
    function callback() // OK - not a method.
    {
        return 1;
    }
}

TypeOne.prototype = {
    badApiMethodNoAnnotation: function() // ERROR - public method.
    {
        return 1;
    },

    _privateMethod: function() // OK - non-public method.
    {
        var obj = {};

        /**
         * @constructor
         * @param {number} val
         */
        obj["a"] = obj["b"] = function(val) { // OK - constructor
            this.foo = val;
        }

        /**
         * @param {number} val
         */
        obj["c"] = obj["d"] = function(val) { // ERROR - no @this
            this.foo = val;
        }

        return 1;
    },

    methodTwo: function()
    {
        function callback() // OK - not a method.
        {
            return 1;
        }
    },

    /**
     * @return {number}
     */
    methodThatThrows: function() // OK - throws and should be overridden in subclasses.
    {
        throw "Not implemented";
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturnValue: function() // ERROR - does not return value.
    {
        return;
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturn: function() // ERROR - does not return.
    {
        var foo = 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeReturn: function() // ERROR - @returns, should be @return
    {
        return 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsentToo: function() // ERROR - @returns, should be absent
    {
        return;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsent: function() // ERROR - @returns, should be absent
    {
        var foo = 1;
    }
}


/**
 * @constructor
 */
TypeTwo = function() {
    function callback() // OK - not a method.
    {
        return 1;
    }
}

TypeTwo.prototype = {
    badApiMethodNoAnnotation: function() // ERROR - public method.
    {
        return 1;
    },

    _privateMethod: function() // OK - non-public method.
    {
        return 1;
    },

    methodTwo: function()
    {
        function callback() // OK - not a method.
        {
            return 1;
        }
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturnValue: function() // ERROR - does not return value.
    {
        return;
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturn: function() // ERROR - does not return.
    {
        var foo = 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeReturn: function() // ERROR - @returns, should be @return
    {
        return 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsentToo: function() // ERROR - @returns, should be absent
    {
        return;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsent: function() // ERROR - @returns, should be absent
    {
        var foo = 1;
    }
}

/**
 * @interface
 */
Interface = function() {}

Interface.prototype = {
    /**
     * @return {number}
     */
    interfaceMethod: function() {}, // OK - interface method.

    /**
     * @returns {number}
     */
    badReturnsInterfaceMethod: function() {} // ERROR - @returns instead of return.
}

/**
 * @return {!Object}
 */
function returnConstructedObject() {

/**
 * @constructor
 */
TypeThree = function() {
    function callback() // OK - not a method.
    {
        return 1;
    }
}

TypeThree.prototype = {
    badApiMethodNoAnnotation: function() // ERROR - public method.
    {
        return 1;
    },

    _privateMethod: function() // OK - non-public method.
    {
        return 1;
    },

    methodTwo: function()
    {
        function callback() // OK - not a method.
        {
            return 1;
        }
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturnValue: function() // ERROR - does not return value.
    {
        return;
    },

    /**
     * @return {number}
     */
    badMethodDoesNotReturn: function() // ERROR - does not return.
    {
        var foo = 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeReturn: function() // ERROR - @returns, should be @return
    {
        return 1;
    },

    /**
     * @returns number
     */
    badMethodReturnShouldBeTyped: function() // ERROR - number, not {number}
    {
        return 1;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsentToo: function() // ERROR - @returns, should be absent
    {
        return;
    },

    /**
     * @returns {number}
     */
    badMethodReturnsShouldBeAbsent: function() // ERROR - @returns, should be absent
    {
        var foo = 1;
    },

    /**
     * @param number foo
     * @param bar
     */
    badMethodParamAnnotations: function(foo, bar) // ERROR - @param's should be well-formed
    {
        return 1;
    }
}

return new TypeThree();
}


/**
 * @param {string} a
 * @param {string} b
 * @param {string} c
 */
function funcParamsOK1(a, b, c) {}

function funcParamsOK2(a, b, c) {}

/**
 * @param {string} a
 * @param {string} c
 */
function funcParamsMissingTag1(a, b, c) {}

/**
 * @param {string} a
 */
function funcParamsMissingTag2(a, b, c) {}

/**
 * @interface
 */
function FuncInterface()
{
}

FuncInterface.prototype = {
    /**
     * @return {number}
     */
    returnNumber: function() { }
}
