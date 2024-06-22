const binding = process.binding('atom_browser_web_contents');
const WebContents = binding.WebContents;

const electron = require('electron');
const app = electron.app;
const ipcMain = electron.ipcMain;
const EventEmitter = require('events').EventEmitter;
const url = require('url');
const path = require('path');

Object.setPrototypeOf(WebContents.prototype, EventEmitter.prototype); // 把on之类的函数绑定过来

// Add JavaScript wrappers for WebContents class.
WebContents.prototype._init = function () {
    if (this.hasOwnProperty('m_isInited'))
        return;
    this.m_isInited = true;
    
    ///
    this.session = {
        "webRequest" : {
            "onBeforeSendHeaders" : function() {},
            "onBeforeRequest" : function() {},
            "onHeadersReceived" : function() {},
        }
    };
    this.webContents = this; // 兼容vscode 1.23
    ///
    
    // Every remote callback from renderer process would add a listenter to the
    // render-view-deleted event, so ignore the listenters warning.
    this.setMaxListeners(0);
    
    // Dispatch IPC messages to the ipc module.
    // 从electron\lib\renderer\api\ipc-renderer.js的ipcRendererBinding.send过来
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
    this.on('ipc-render-invoke', function (event, channel, ...args) {
        ipcMain.emit(channel, event, ...args); // channel='ScanGames'
    });

    // Handle context menu action request from pepper plugin.
    this.on('pepper-context-menu', function (event, params) {
        //const menu = Menu.buildFromTemplate(params.menu);
        //menu.popup(params.x, params.y);
    });

    // The devtools requests the webContents to reload.
    this.on('devtools-reload-page', function () {
        this.reload();
    });

    // Delays the page-title-updated event to next tick.
    this.on('-page-title-updated', function (...args) {
        setImmediate(() => {
            this.emit.apply(this, ['page-title-updated'].concat(args));
        });
    });
    
    this.on('-new-window', function (event, url) {
        this.onCreateNewWebview(event, url);
    });
}

// WebContents::send(channel, args..)
// WebContents::sendToAll(channel, args..)
WebContents.prototype.send = function (channel, ...args) {
    if (channel == null) throw new Error('Missing required channel argument')
    return this._send(false, channel, args)
}
WebContents.prototype.sendToAll = function (channel, ...args) {
    if (channel == null) throw new Error('Missing required channel argument');
    return this._send(true, channel, args);
}

WebContents.prototype.getURL = function (url) {
    return this._getURL(url);
}

WebContents.prototype.canGoBack = function () {
    return this._canGoBack();
}

WebContents.prototype.canGoForward = function () {
    return this._canGoForward();
}

WebContents.prototype.getZoomLevel = function () {
    return this._getZoomLevel();
}

WebContents.prototype.getZoomLevel = function (z) {
    return this._getZoomLevel(z);
}

WebContents.prototype.loadFile = function (filePath) {
    if (typeof filePath !== 'string')
        throw new Error('Must pass filePath as a string');
    
    return this._loadURL(url.format({
        protocol: 'file',
        slashes: true,
        pathname: path.resolve(app.getAppPath(), filePath)
    }));
}

let nextId = 0;
const getNextId = function () {
    return ++nextId;
}

// Following methods are mapped to webFrame.
const webFrameMethods = [
  'insertCSS',
  'insertText',
  'setLayoutZoomLevelLimits',
  'setVisualZoomLevelLimits'
]
const webFrameMethodsWithResult = []

const errorConstructors = {
    Error,
    EvalError,
    RangeError,
    ReferenceError,
    SyntaxError,
    TypeError,
    URIError
}

const asyncWebFrameMethods = function (requestId, method, callback, ...args) {
    return new Promise((resolve, reject) => {
        this.send('ELECTRON_INTERNAL_RENDERER_ASYNC_WEB_FRAME_METHOD', requestId, method, args)
        ipcMain.once(`ELECTRON_INTERNAL_BROWSER_ASYNC_WEB_FRAME_RESPONSE_${requestId}`, function (event, error, result) {
            if (error == null) {
                if (typeof callback === 'function')
                    callback(result);
                resolve(result);
            } else {
                if (error.__ELECTRON_SERIALIZED_ERROR__ && errorConstructors[error.name]) {
                    const rehydratedError = new errorConstructors[error.name](error.message);
                    rehydratedError.stack = error.stack;

                    reject(rehydratedError);
                } else {
                    reject(error);
                }
            }
        })
    })
}

// Make sure WebContents::executeJavaScript would run the code only when the
// WebContents has been loaded.
WebContents.prototype.executeJavaScript = function (code, hasUserGesture, callback) {
    const requestId = getNextId();

    if (typeof hasUserGesture === 'function') {
        // Shift.
        callback = hasUserGesture;
        hasUserGesture = null;
    }

    if (hasUserGesture == null) {
        hasUserGesture = false;
    }

    if (typeof callback !== 'function') {
        callback = () => { };
    }

    if (true || this.getURL() && !this.isLoadingMainFrame()) {
        return asyncWebFrameMethods.call(this, requestId, 'executeJavaScript', callback, code, hasUserGesture);
    } else {
        return new Promise((resolve, reject) => {
            this.once('did-finish-load', () => {
                asyncWebFrameMethods.call(this, requestId, 'executeJavaScript', callback, code, hasUserGesture)
                    .then(resolve)
                    .catch(reject);
            })
        })
    }
}

WebContents.prototype.setVisualZoomLevelLimits = function (minimumLevel, maximumLevel) {
}

WebContents.prototype.setLayoutZoomLevelLimits = function (minimumLevel, maximumLevel) {
}

module.exports = WebContents;