'use strict'

const EventEmitter = require('events').EventEmitter;
// The global variable will be used by ipc for event dispatching
var v8Util = new (process.binding('atom_common_v8_util').v8Util)();
v8Util.setHiddenValue(global, 'ipc', new EventEmitter());

const electron = require('./electron');
require('../common/init.js');

const intlCollator = require('../common/api/intl-collator');

const events = require('events');
const path = require('path');
const Module = require('module');
const timers = require('timers');

let nodeRequire = require;
let nodeProcess = process;

function __mbRequire__(name) {
    var oldProcess = process;
    process = nodeProcess;
    var result = nodeRequire(name);
    process = oldProcess;
    return result;
}

// Export node bindings to global.
window.require = __mbRequire__;
window.require.resolve = nodeRequire.resolve;
window.require.main = nodeRequire.main;
window.require.extensions = nodeRequire.extensions;
window.require.cache = nodeRequire.cache;

window.module = module;
window.miniNodeRequire = __mbRequire__;
window.miniNodeModule = module;
window.setImmediate = timers.setImmediate;
window.Intl = intlCollator;

// Set the __filename to the path of html file if it is file: protocol.
if (window.location.protocol === 'file:') {
    var pathname = process.platform === 'win32' && window.location.pathname[0] === '/' ? window.location.pathname.substr(1) : window.location.pathname;
    window.__filename = path.normalize(decodeURIComponent(pathname));
    window.__dirname = path.dirname(window.__filename);

    // Set module's filename so relative require can work as expected.
    module.filename = window.__filename;

    // Also search for module under the html file.
    module.paths = module.paths.concat(Module._nodeModulePaths(window.__dirname));
}

function outputObj(obj) {  
    var props = "";  
	for (var prop in obj) {
        if (obj.hasOwnProperty(prop)) {
            var propVal = obj[prop];
            props += prop + ' (' + typeof(prop) + ')' + ' = ';
            if (typeof(obj[prop]) == 'string') {
                propVal += obj[prop];
            } else {
                if (propVal != null && propVal.toString) {
                    props += propVal.toString();
                } else {}
            }
            props += '\n';
        }
    }
   	console.log(props);  
}  

global.process.on('exit', function() {
	var activeHandles = global.process._getActiveHandles();
	for (var i in activeHandles) {
		var handle = activeHandles[i];
		if (handle.hasOwnProperty('close') || undefined !== handle['close'])
			handle.close();
		else if (handle.hasOwnProperty('_handle') && (handle._handle.hasOwnProperty('close') || undefined !== handle._handle['close']))
			handle._handle.close();
		else
			console.log("global.process.on exit fail:" + handle + ", " + handle._handle);
	}
});

const resolvePromise = Promise.resolve.bind(Promise);

electron.ipcRenderer.on('ELECTRON_INTERNAL_RENDERER_ASYNC_WEB_FRAME_METHOD', 
    (event, requestId, method, args) => {
    const responseCallback = function (result) {
        resolvePromise(result)
          .then((resolvedResult) => {
              event.sender.send(`ELECTRON_INTERNAL_BROWSER_ASYNC_WEB_FRAME_RESPONSE_${requestId}`, null, resolvedResult);
          })
          .catch((resolvedError) => {
              if (resolvedError instanceof Error) {
                  // Errors get lost, because: JSON.stringify(new Error('Message')) === {}
                  // Take the serializable properties and construct a generic object
                  resolvedError = {
                      message: resolvedError.message,
                      stack: resolvedError.stack,
                      name: resolvedError.name,
                      __ELECTRON_SERIALIZED_ERROR__: true
                  }
              }

              event.sender.send(`ELECTRON_INTERNAL_BROWSER_ASYNC_WEB_FRAME_RESPONSE_${requestId}`, resolvedError);
          })
    }
    args.push(responseCallback);
    electron.webFrame[method](...args);
})