
const EventEmitter = require('events').EventEmitter;
const binding = process.binding('atom_browser_tray');
const Tray = binding.Tray;
Object.setPrototypeOf(Tray.prototype, EventEmitter.prototype);

const Menu = require('./menu');

Tray.prototype._init = function () {
	this.menu = null;
}

Tray.prototype.onNativeMessage = function(msg) {
	if ("right-click" == msg) {
		console.log("Tray.prototype.onNativeMessage:" + this.menu);
		if (this.menu)
			this.menu.popup();
		this.emit("right-click");
	} else if ("click" == msg)
		this.emit("click");
}

Tray.prototype.setContextMenu = function(menu) {
	var self = this;
	this.menu = menu;
	this._setIsContextMenu(true);
	this._setNativeMessageCallback(function(msg) { self.onNativeMessage(msg); });
}

Tray.prototype.popUpContextMenu = function(menu, position) {
	if (this.menu)
		return;
	menu.popup(position);
}

exports.Tray = Tray;