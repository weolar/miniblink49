
const electron = require('electron');
////////////////////////////////////////////////////////////////
const ipcMain = require('./api/ipc-main');
electron.ipcMain = ipcMain;
////////////////////////////////////////////////////////////////
electron.BrowserWindow = require("./../browser/api/browser-window");
////////////////////////////////////////////////////////////////
var App = process.binding('atom_browser_app').App;
electron.app = new App();
const EventEmitter = require('events').EventEmitter;
Object.setPrototypeOf(App.prototype, EventEmitter.prototype);
////////////////////////////////////////////////////////////////
const Menu = require('./api/menu');
electron.Menu = Menu;
////////////////////////////////////////////////////////////////
module.exports = electron;

