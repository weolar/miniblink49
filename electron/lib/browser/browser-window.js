console.log("BrowserWindow!");
process.binding('atom_browser_web_contents');

const EventEmitter = require('events').EventEmitter;
const BrowserWindow = process.binding('atom_browser_window').BrowserWindow;
Object.setPrototypeOf(BrowserWindow.prototype, EventEmitter.prototype);
module.exports = BrowserWindow;