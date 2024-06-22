
const electron = require('electron');
electron.ipcRenderer = require("./api/ipc-renderer.js");
electron.CallbacksRegistry = require("./../common/api/callbacks-registry.js").CallbacksRegistry;
electron.isPromise = require("./../common/api/is-promise.js").isPromise;
electron.remote = require("./api/remote.js");
electron.shell = require("./../common/api/shell.js").Shell;
electron.screen = require("./../common/api/screen.js").Screen;
electron.webFrame = require("./api/web-frame.js");
electron.clipboard = require("./../common/api/clipboard.js");
electron.contextBridge = require("./../renderer/api/context-bridge.js");

module.exports = electron;