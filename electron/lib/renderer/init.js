'use strict'

const EventEmitter = require('events').EventEmitter;
// The global variable will be used by ipc for event dispatching
var v8Util = new (process.binding('atom_common_v8_util').v8Util)();
v8Util.setHiddenValue(global, 'ipc', new EventEmitter());

const electron = require('./electron');
require('../common/init.js');

require('./web-view/web-view');

const intlCollator = require('../common/api/intl-collator');

const events = require('events');
const path = require('path');
const Module = require('module');
const timers = require('timers');

// Process command line arguments.
let nodeIntegration = 'false'
let preloadScript = null
let isBackgroundPage = false
for (let arg of process.argv) {
    if (arg.indexOf('--guest-instance-id=') === 0) {
        // This is a guest web view.
        process.guestInstanceId = parseInt(arg.substr(arg.indexOf('=') + 1))
    } else if (arg.indexOf('--opener-id=') === 0) {
        // This is a guest BrowserWindow.
        process.openerId = parseInt(arg.substr(arg.indexOf('=') + 1))
    } else if (arg.indexOf('--node-integration=') === 0) {
        nodeIntegration = arg.substr(arg.indexOf('=') + 1)
    } else if (arg.indexOf('--preload=') === 0) {
        preloadScript = arg.substr(arg.indexOf('=') + 1)
    } else if (arg === '--background-page') {
        isBackgroundPage = true
    }
}

let nodeRequire = require;
let nodeProcess = process;

let isSetModulePaths = false;

// Set the __filename to the path of html file if it is file: protocol.
function addModulePaths() {
	if (isSetModulePaths)
		return;
	
	if (window.location.protocol !== 'file:')
		return;
	
    var pathname = process.platform === 'win32' && window.location.pathname[0] === '/' ? window.location.pathname.substr(1) : window.location.pathname;
    window.__filename = path.normalize(decodeURIComponent(pathname));
    window.__dirname = path.dirname(window.__filename);

    // Set module's filename so relative require can work as expected.
    module.filename = window.__filename;
    
    // Also search for module under the html file.
    module.paths = module.paths.concat(Module._nodeModulePaths(window.__dirname));
    
    isSetModulePaths = true;
}

function __mbRequire__(name) {
	addModulePaths();
	
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

addModulePaths();

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
});

// Load the script specfied by the "preload" attribute.
if (preloadScript) {
    try {
        require(preloadScript)
    } catch (error) {
        console.error('Unable to load preload script: ' + preloadScript)
        console.error(error.stack || error.message)
    }
}