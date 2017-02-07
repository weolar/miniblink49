'use strict';

const EventEmitter = require('events').EventEmitter;
const bindings = process.binding('atom_browser_menu');
const Menu = bindings.Menu;
Object.setPrototypeOf(Menu.prototype, EventEmitter.prototype);

Menu.prototype.popup = function (windowObj, x, y, positioningItem) {
}

Menu.prototype.append = function (item) {
  return null;
}

Menu.prototype.insert = function (pos, item) {
}

Menu.buildFromTemplate = function (template) {
}

Menu.getApplicationMenu = function () {
  return null;
}

Menu.setApplicationMenu = function (menu) {
  return null;
}

module.exports = Menu;
