document.bar();
window.document.bar();
bar.document();
bar.window.document();
document();
var x = window.document;

function a(document) {
    document.bar();
    document();

    function inner() {
        var y = document;
    }
}

function b(bar) {
    document.bar();
    document();
    var document = bar;
}

function c(bar) {
    document.bar();
    document();
    var inner = document + bar;
}

function d(window) {
    window.document();
    window.document.bar = 2;
}

/**
 * @suppressGlobalPropertiesCheck
 * @param {string} param
 */
function e(param) {
    document;
    window.document;
}

/**
 * @param {string} param
 */
function f(param) {
    document;
    window.document;
}

var y = x.document;
window.x.document();
var z = document.document;
var bar = window + window.document;

self.document = bar;
self.addEventListener();
self.removeEventListener();
self.requestAnimationFrame();
window.cancelAnimationFrame();
getSelection();

function g() {
    var self = this;
    self.document;
}

function h() { var a = { document: true }; }
