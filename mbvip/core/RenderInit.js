
const timers = require('timers');
const Module = require('module');
const path = require('path');

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
    if (module.paths)
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
//window.require = __mbRequire__;
//window.require.resolve = nodeRequire.resolve;
//window.require.main = nodeRequire.main;
//window.require.extensions = nodeRequire.extensions;
//window.require.cache = nodeRequire.cache;

//window.module = module;
window.miniNodeRequire = __mbRequire__;
window.miniNodeModule = module;
window.setImmediate = timers.setImmediate;
//window.Intl = intlCollator;

addModulePaths();
/**/