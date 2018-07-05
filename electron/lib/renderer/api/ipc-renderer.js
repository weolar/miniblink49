'use strict'

var IpcRendererBinding = process.binding('atom_renderer_ipc').ipcRenderer;
const EventEmitter = require('events').EventEmitter;
var v8Util = new (process.binding('atom_common_v8_util').v8Util)();
var binding = new IpcRendererBinding();

// Created by init.js.
const ipcRenderer = v8Util.getHiddenValue(global, 'ipc'); //var ipcRenderer = new EventEmitter();

ipcRenderer.send = function (...args) {
    return binding.send('ipc-message', args);
}

ipcRenderer.sendSync = function (...args) {
    return JSON.parse(binding.sendSync('ipc-message-sync', args));
}

ipcRenderer.sendToHost = function (...args) {
    return binding.send('ipc-message-host', args);
}

ipcRenderer.sendTo = function (webContentsId, channel, ...args) {
    if (typeof webContentsId !== 'number') {
        throw new TypeError('First argument has to be webContentsId');
    }

    ipcRenderer.send('ELECTRON_BROWSER_SEND_TO', false, webContentsId, channel, ...args);
}

ipcRenderer.sendToAll = function (webContentsId, channel, ...args) {
    if (typeof webContentsId !== 'number') {
        throw new TypeError('First argument has to be webContentsId');
    }

    ipcRenderer.send('ELECTRON_BROWSER_SEND_TO', true, webContentsId, channel, ...args);
}

//electron.ipcRenderer = ipcRenderer;
module.exports = ipcRenderer;