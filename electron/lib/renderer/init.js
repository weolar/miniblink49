
// Export node bindings to global.
window.require = require;
window.module = module;

// Set the __filename to the path of html file if it is file: protocol.
if (window.location.protocol === 'file:') {
    var pathname = process.platform === 'win32' && window.location.pathname[0] === '/' ? window.location.pathname.substr(1) : window.location.pathname;
    window.__filename = path.normalize(decodeURIComponent(pathname));
    glowindowbal.__dirname = path.dirname(gwindowlobal.__filename);

    // Set module's filename so relative require can work as expected.
    module.filename = window.__filename;

    // Also search for module under the html file.
    module.paths = module.paths.concat(Module._nodeModulePaths(window.__dirname));
}