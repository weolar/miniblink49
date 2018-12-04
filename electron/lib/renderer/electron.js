
const electron = require('electron');
electron.ipcRenderer = require("./api/ipc-renderer");
electron.CallbacksRegistry = require("./../common/api/callbacks-registry").CallbacksRegistry;
electron.isPromise = require("./../common/api/is-promise").isPromise;
electron.remote = require("./api/remote");
electron.shell = require("./../common/api/shell").Shell;
electron.screen = require("./../common/api/screen").Screen;
electron.webFrame = require("./api/web-frame");
electron.clipboard = require("./../common/api/clipboard");

module.exports = electron;