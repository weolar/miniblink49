const binding = process.binding('atom_browser_web_contents');
const WebContents = binding.WebContents;

const app = require('electron').app;
const ipcMain = require('electron').ipcMain;
const EventEmitter = require('events').EventEmitter;

Object.setPrototypeOf(WebContents.prototype, EventEmitter.prototype);

// Add JavaScript wrappers for WebContents class.
WebContents.prototype._init = function () {
	if (this.hasOwnProperty('m_isInited'))
		return;
	this.m_isInited = true;
	
	// Every remote callback from renderer process would add a listenter to the
	// render-view-deleted event, so ignore the listenters warning.
	this.setMaxListeners(0);
	
	// Dispatch IPC messages to the ipc module.
	this.on('ipc-message', function (event, channel, ...args) {
		ipcMain.emit(channel, event, ...args);
	});
	this.on('ipc-message-sync', function (event, channel, ...args) {
		Object.defineProperty(event, 'returnValue', {
			set: function (value) {
				return event.sendReply(JSON.stringify(value));
			},
		get: function () {}
		});
		ipcMain.emit(channel, event, ...args);
	});

	// Handle context menu action request from pepper plugin.
	this.on('pepper-context-menu', function (event, params) {
		//const menu = Menu.buildFromTemplate(params.menu);
		//menu.popup(params.x, params.y);
	});

	// The devtools requests the webContents to reload.
	this.on('devtools-reload-page', function () {
		this.reload();
	})

	// Delays the page-title-updated event to next tick.
	this.on('-page-title-updated', function (...args) {
		setImmediate(() => {
			this.emit.apply(this, ['page-title-updated'].concat(args));
		})
	});
}

// WebContents::send(channel, args..)
// WebContents::sendToAll(channel, args..)
WebContents.prototype.send = function (channel, ...args) {
  if (channel == null) throw new Error('Missing required channel argument')
  return this._send(false, channel, args)
}
WebContents.prototype.sendToAll = function (channel, ...args) {
  if (channel == null) throw new Error('Missing required channel argument')
  return this._send(true, channel, args)
}

module.exports = WebContents;