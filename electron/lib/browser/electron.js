
const electron = require('electron');
const ipcMain = require('./api/ipc-main');
electron.ipcMain = ipcMain;

var App = require("./../browser/api/app").App;
electron.app = new App();

electron.BrowserWindow = require("./../browser/api/browser-window.js");
electron.BrowserView = require("./../browser/api/browser-view.js");
electron.webContents = require("./../browser/api/web-contents.js");
electron.session = require("./../browser/api/session.js").session;

var s_map = {};
var s_idGen = 0;

electron.webContents.prototype.onCreateNewWebview = function(event, url) {
    const win = new electron.BrowserWindow({width: 800, height: 600, show: true});
    win.show();
    
    var id = ++s_idGen;
    s_map[id] = win;
    win.webContents.once("destroyed", function() { console.log('onCreateNewWebview, destroy'); delete s_map[id]; });
    console.log("electron.webContents.prototype.onCreateNewWebview!!!!!!!!!!!");
    event.newGuest = win;
}

const EventEmitter = require('events').EventEmitter;
Object.setPrototypeOf(App.prototype, EventEmitter.prototype);

const MenuItem = require('./api/menu-item.js');
electron.MenuItem = MenuItem;

const Menu = require('./api/menu.js');
electron.Menu = Menu;

const isPromise = require('./../common/api/is-promise.js').isPromise;
electron.isPromise = isPromise;

const dialog = require('./api/dialog.js').dialog;
electron.dialog = dialog;

const net = require('./api/net.js').net;
electron.net = net;

electron.shell = require("./../common/api/shell.js").Shell;
electron.screen = require("./../common/api/screen.js").Screen;
electron.tray = require("./../common/api/screen.js").Tray;
electron.clipboard = require("./../common/api/clipboard.js");
electron.nativeImage = require("./../common/api/native-image.js").NativeImage;

function SystemPreferences () {}
SystemPreferences.prototype.isDarkMode = function() { return false; }
SystemPreferences.prototype.isSwipeTrackingFromScrollEventsEnabled = function() { return false; }
SystemPreferences.prototype.isAeroGlassEnabled = function() { return false; }
SystemPreferences.prototype.isInvertedColorScheme = function() { return false; }
Object.setPrototypeOf(SystemPreferences.prototype, EventEmitter.prototype);
electron.systemPreferences = new SystemPreferences();

////////////////////////////////////////////////////////////////

electron.protocol = require("./api/protocol.js").protocol;

// function Protocol() {}
// Protocol.prototype.registerStandardSchemes = function(schemes) {}
// Protocol.prototype.registerServiceWorkerSchemes = function(scheme) {}
// Protocol.prototype.registerFileProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.registerBufferProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.registerStringProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.registerHttpProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.unregisterProtocol = function(scheme, completion) {}
// Protocol.prototype.isProtocolHandled = function(scheme, callback) {}
// Protocol.prototype.interceptFileProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.interceptStringProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.interceptBufferProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.interceptHttpProtocol = function(scheme, handler, completion) {}
// Protocol.prototype.uninterceptProtocol = function(scheme, completion) {}

// electron.protocol = new Protocol();
////////////////////////////////////////////////////////////////

var TouchBar = function (){};
TouchBar.TouchBarButton = function() {};
TouchBar.TouchBarColorPicker = function() {};
TouchBar.TouchBarGroup = function() {};
TouchBar.TouchBarLabel = function() {};
TouchBar.TouchBarPopover = function() {};
TouchBar.TouchBarScrubber = function() {};
TouchBar.TouchBarSegmentedControl = function() {};
TouchBar.TouchBarSlider = function() {};
TouchBar.TouchBarSpacer = function() {};

electron.TouchBar = TouchBar;

////////////////////////////////////////////////////////////////

function AutoUpdater() {}
AutoUpdater.prototype.on = function(evt, callback) {}
AutoUpdater.prototype.setFeedURL= function(url) {}
AutoUpdater.prototype.checkForUpdates = function() {}
AutoUpdater.prototype.quitAndInstall = function() {}

electron.autoUpdater = new AutoUpdater();
////////////////////////////////////////////////////////////////
/*
function Tray() {}
Tray.prototype.on = function(evt, callback) {}
Tray.prototype.destroy = function() {}
Tray.prototype.setImage= function(image) {}
Tray.prototype.setPressedImage = function(image) {}
Tray.prototype.setToolTip = function(toolTip) {}
Tray.prototype.setTitle = function(title) {}
Tray.prototype.displayBalloon = function(options) {}
Tray.prototype.popUpContextMenu = function(menu, position) {}
Tray.prototype.setContextMenu = function(menu) {}

electron.Tray = Tray;
*/
electron.Tray = require("./api/tray").Tray;
////////////////////////////////////////////////////////////////
function GlobalShortcut() {}
GlobalShortcut.prototype.register = function(accelerator, callback) {}
GlobalShortcut.prototype.isRegistered = function(accelerator) { return true; }
GlobalShortcut.prototype.unregister= function() {}
GlobalShortcut.prototype.unregisterAll = function() {}
electron.globalShortcut = new GlobalShortcut();
////////////////////////////////////////////////////////////////
function PowerMonitor() {}
PowerMonitor.prototype.on = function(evtName, callback) {}
electron.powerMonitor = new PowerMonitor();
////////////////////////////////////////////////////////////////
function PowerSaveBlocker() {}
PowerSaveBlocker.prototype.start = function(type) { return 0; }
PowerSaveBlocker.prototype.stop = function(id) {}
PowerSaveBlocker.prototype.isStarted = function(id) { return false; }
electron.powerSaveBlocker = new PowerSaveBlocker();
////////////////////////////////////////////////////////////////
function CrashReporter () {}
CrashReporter.prototype.start = function(options) { return; }
CrashReporter.prototype.getLastCrashReport = function() { return null; }
CrashReporter.prototype.getUploadedReports = function() { return 0; }
electron.crashReporter = new CrashReporter();
////////////////////////////////////////////////////////////////

module.exports = electron;

