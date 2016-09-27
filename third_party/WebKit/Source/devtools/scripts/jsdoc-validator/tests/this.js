this.foo = this.foo + 1; // OK - outside of function.

function f() {
    this.foo = this.foo + 1; // OK - global |this|.
}

/**
 * @constructor
 */
function TypeOne() {
    this.foo = this.foo + 1; // OK - object field in ctor.

    /**
     * @this {TypeOne}
     */
    function callbackOne() {
        this.foo = this.foo + 1; // OK - @this declared.

        function badInnerCallback() {
            this.foo = this.foo + 2; // ERROR - @this not declared.
        }
    }

    function badCallbackInCtor() {
        this.foo = this.foo + 1; // ERROR - @this not declared.
    }
}

TypeOne.prototype = {
    addListener: function(callback)
    {
        if (typeof callback !== "function")
            throw "addListener: callback is not a function";
        if (this._listeners.length === 0)
            extensionServer.sendRequest({ command: commands.Subscribe, type: this._type });
        this._listeners.push(callback);
        extensionServer.registerHandler("notify-" + this._type, this._dispatch.bind(this));
    },

    funcOne: function() {
        this.foo = this.foo + 1; // OK - in method.
    },

    funcTwo: function() {
        /**
         * @this {TypeOne}
         */
        function callback() {
            this.foo = this.foo + 1; // OK - @this declared.
        }
    },

    funcThree: function() {
        function badCallbackInMethod() {
            this.foo = this.foo + 1; // ERROR - @this not declared.
        }
    }
}


/**
 * @constructor
 */
TypeTwo = function() {
    this.bar = this.bar + 1; // OK - object field in ctor.

    /**
     * @this {TypeTwo}
     */
    function callbackOne() {
        this.bar = this.bar + 1; // OK - @this declared.

        function badInnerCallback() {
            this.bar = this.bar + 2; // ERROR - @this not declared.
        }
    }

    function badCallbackInCtor() {
        this.bar = this.bar + 1; // ERROR - @this not declared.
    }
}

TypeTwo.prototype = {
    funcOne: function() {
        this.bar = this.bar + 1; // OK - in method.
    },

    funcTwo: function() {
        /**
         * @this {TypeTwo}
         */
        function callback() {
            this.bar = this.bar + 1; // OK - @this declared.
        }
    },

    funcThree: function() {
        function badCallbackInMethod() {
            this.bar = this.bar + 1; // ERROR - @this not declared.
        }
    }
}

/**
 * @return {!Object}
 */
function returnConstructedObject() {

/**
 * @constructor
 */
TypeThree = function() {
    this.bar = this.bar + 1; // OK - object field in ctor.

    /**
     * @this {TypeThree}
     */
    function callbackOne() {
        this.bar = this.bar + 1; // OK - @this declared.

        function badInnerCallback() {
            this.bar = this.bar + 2; // ERROR - @this not declared.
        }
    }

    function badCallbackInCtor() {
        this.bar = this.bar + 1; // ERROR - @this not declared.
    }
}

TypeThree.prototype = {
    funcOne: function() {
        this.bar = this.bar + 1; // OK - in method.
    },

    funcTwo: function() {
        /**
         * @this {TypeThree}
         */
        function callback() {
            this.bar = this.bar + 1; // OK - @this declared.
        }
    },

    funcThree: function() {
        function badCallbackInMethod() {
            this.bar = this.bar + 1; // ERROR - @this not declared.
        }

        /**
         * @this {TypeOne}
         */
        function callbackNotReferencingThis() {
            return 3; // ERROR - @this for a function not referencing |this|.
        }
    }
}

return new TypeThree();
}

var object = {
    /**
     * @this {MyType}
     */
    value: function()
    {
        this.foo = 1; // OK - @this annotated.
    }
};

(function() {
    var object = {
        /**
         * @this {MyType}
         */
        value: function()
        {
            this.foo = 1; // OK - @this annotated.
        }
    };
})();

/**
 * @constructor
 */
var ReceiverTest = function() {}

ReceiverTest.prototype = {
    memberOne: function() {
        var badMemberBinding1 = this.memberTwo.bind(null); // ERROR - Member not bound to |this| receiver.
        var badMemberBinding2 = this.memberTwo.bind(bar); // ERROR - Member not bound to |this| receiver.
        var goodMemberBinding = this.memberTwo.bind(this);

        /** @this {ReceiverTest} */
        function callbackWithThis()
        {
            this.memberTwo();
        }

        function callbackNoThis()
        {
            return 42;
        }

        callbackWithThis.call(this);
        callbackWithThis.call(foo);
        callbackNoThis();
        callbackNoThis.call(null, 1);
        callbackNoThis.apply(null, [2]);
        callbackNoThis.bind(null, 1);
        this.memberTwo(callbackWithThis.bind(this, 1));
        this.memberTwo(callbackWithThis.bind(foo, 1));
        this.memberTwo(callbackNoThis);
        this.memberTwo(callbackNoThis.bind(null));

        callbackWithThis(); // ERROR - No receiver.
        callbackWithThis.call(); // ERROR - No receiver.
        callbackWithThis.call(null); // ERROR - No receiver.
        callbackWithThis.apply(); // ERROR - No receiver.
        callbackWithThis.apply(null); // ERROR - No receiver.
        callbackNoThis.call(this); // ERROR - Function has no @this annotation.
        callbackNoThis.call(foo); // ERROR - Function has no @this annotation.
        callbackNoThis.apply(this); // ERROR - Function has no @this annotation.
        callbackNoThis.bind(this); // ERROR - Function has no @this annotation.

        this.memberTwo(callbackWithThis); // ERROR - Used as argument with no bound receiver.
        this.memberTwo(callbackWithThis.bind(null, 2)); // ERROR - Used as argument with no bound receiver (null means "no receiver").
        this.memberTwo(callbackNoThis.bind(this)); // ERROR - Bound to a receiver but has no @this annotation.
        this.memberTwo(callbackNoThis.bind(foo)); // ERROR - Bound to a receiver but has no @this annotation.

        // Callback receivers specified as arguments.

        array.forEach(callbackWithThis, this);
        array.forEach(callbackNoThis);

        array.forEach(callbackWithThis); // ERROR - No receiver.
        array.forEach(callbackNoThis, this); // ERROR - Receiver for callback with no @this annotation.

        var isMultiline = false;

        element.addEventListener("click", callbackNoThis);
        element.addEventListener("click", callbackNoThis, true);
        element.addEventListener("click", callbackNoThis, false);
        element.addEventListener("click", callbackNoThis, isMultiline); // OK - ignored.

        element.addEventListener("click", callbackNoThis, this); // ERROR.

        element.addEventListener("click", callbackWithThis, this);
        element.addEventListener("click", callbackWithThis, foo); // OK - ignored.
        element.addEventListener("click", callbackWithThis, isMultiline); // OK - ignored.

        element.addEventListener("click", callbackWithThis, true); // ERROR.
        element.addEventListener("click", callbackWithThis, false); // ERROR.

        // DevTools-specific.

        /**
         * @suppressReceiverCheck
         * @this {Object}
         */
        function ignoredCallbackWithThis()
        {
            this.foo = 1;
        }
        object.callFunction(func, [], ignoredCallbackWithThis); // OK - ignored.

        function callbackReferencingThisNotAnnotated()
        {
            this.foo = 2;
        }
        this.memberTwo(callbackReferencingThisNotAnnotated.bind(this)); // OK - No @this annotation, but references |this|.

        /**
         * @this {Object}
         */
        function callbackNotReferencingThisAnnotated()
        {
        }
        this.memberTwo(callbackNotReferencingThisAnnotated); // OK - Has @this annotation, but does not reference |this|.
    },

    memberTwo: function(arg) {}
}
