'use strict';

const EventEmitter = require('events').EventEmitter;
const bindings = process.binding('atom_browser_menu');
const Menu = bindings.Menu;
Object.setPrototypeOf(Menu.prototype, EventEmitter.prototype);

Menu.prototype.popup = function (windowObj, x, y, positioningItem) {
	console.log('Menu.popup');
}

Menu.prototype.append = function (item) {
	//console.log('Menu.append');
    return null;
}

Menu.prototype.insert = function (pos, item) {
	console.log('Menu.insert');
}

Menu.prototype.items = [];

Menu.buildFromTemplate = function (template) {
	console.log('Menu.buildFromTemplate');
	var result = (new Menu())._buildFromTemplate(template);
	return result;
}

Menu.getApplicationMenu = function () {
	console.log('Menu.getApplicationMenu');
    return null;
}

Menu.setApplicationMenu = function (menu) {
	console.log('Menu.setApplicationMenu');
    return null;
}

module.exports = Menu;
