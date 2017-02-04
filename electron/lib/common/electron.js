
const electron = require('electron');
electron.BrowserWindow = require("./../browser/browser-window");

var App = process.binding('atom_browser_app').App;
electron.app = new App();

module.exports = electron;

