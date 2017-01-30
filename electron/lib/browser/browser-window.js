console.log("BrowserWindow!");
process.binding('atom_browser_web_contents');

const {EventEmitter} = require('events');
const {BrowserWindow} = process.binding('atom_browser_window');
Object.setPrototypeOf(BrowserWindow.prototype, EventEmitter.prototype);
module.exports = BrowserWindow;