'use strict'

const EventEmitter = require('events').EventEmitter;
const WebFrame = process.binding('atom_renerer_webframe').WebFrame;
const webFrame = new WebFrame();

// WebFrame is an EventEmitter.
Object.setPrototypeOf(WebFrame.prototype, EventEmitter.prototype)

// Lots of webview would subscribe to webFrame's events.
webFrame.setMaxListeners(0);

module.exports = webFrame;
