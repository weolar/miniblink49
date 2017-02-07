
const electron = require('electron');
electron.BrowserWindow = require("./../browser/browser-window");
////////////////////////////////////////////////////////////////
var App = process.binding('atom_browser_app').App;
electron.app = new App();
const EventEmitter = require('events').EventEmitter;
Object.setPrototypeOf(App.prototype, EventEmitter.prototype);
////////////////////////////////////////////////////////////////
const Menu = require('./../browser/menu');
electron.Menu = Menu;
////////////////////////////////////////////////////////////////
module.exports = electron;

