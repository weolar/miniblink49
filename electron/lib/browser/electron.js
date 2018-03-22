
const electron = require('electron');
const ipcMain = require('./api/ipc-main');
electron.ipcMain = ipcMain;

electron.BrowserWindow = require("./../browser/api/browser-window");

var App = require("./../browser/api/app").App;
electron.app = new App();

const EventEmitter = require('events').EventEmitter;
Object.setPrototypeOf(App.prototype, EventEmitter.prototype);

const MenuItem = require('./api/menu-item');
electron.MenuItem = MenuItem;

const Menu = require('./api/menu');
electron.Menu = Menu;

const isPromise = require('./../common/api/is-promise').isPromise;
electron.isPromise = isPromise;

const dialog = require('./api/dialog').dialog;
electron.dialog = dialog;

electron.shell = require("./../common/api/shell").Shell;
electron.screen = require("./../common/api/screen").Screen;

function SystemPreferences () {}
SystemPreferences.prototype.isDarkMode = function() { return false; }
SystemPreferences.prototype.isSwipeTrackingFromScrollEventsEnabled = function() { return false; }
SystemPreferences.prototype.isAeroGlassEnabled = function() { return false; }
SystemPreferences.prototype.isInvertedColorScheme = function() { return false; }
Object.setPrototypeOf(SystemPreferences.prototype, EventEmitter.prototype);
electron.systemPreferences = new SystemPreferences();
////////////////////////////////////////////////////////////////

module.exports = electron;

