
process.binding('atom_browser_web_contents');

const EventEmitter = require('events').EventEmitter;
const BrowserWindow = process.binding('atom_browser_browserwindow').BrowserWindow;
Object.setPrototypeOf(BrowserWindow.prototype, EventEmitter.prototype);

require('./web-contents');

// Helpers.
Object.defineProperty(BrowserWindow.prototype, "webContents", {
    get: function () {
        var webContents =  this._getWebContents();
        webContents._init();
        return webContents;
    },
    configurable : true
});
	
BrowserWindow.prototype.setTouchBar = function() { }

Object.assign(BrowserWindow.prototype, {
    loadURL (...args) {
        var self = this;
        var result = new Promise(function(resole, reject) {
            self.webContents.on("did-finish-load", function() {
                resole();
            });
            self.webContents.on("did-fail-load", function() {
                reject();
            });
            
            self.webContents._loadURL.apply(self.webContents, args);
        });
        return result;
    },
    loadFile (...args) {
        var self = this;
        var result = new Promise(function(resole, reject){
            self.webContents.on("did-finish-load", function() {
                resole();
            });
            self.webContents.on("did-fail-load", function() {
                reject();
            });
            self.webContents.loadFile.apply(self.webContents, args);
        });
        return result;
    },
    getURL (...args) {
        return this.webContents.getURL();
    },
    reload (...args) {
        return this.webContents.reload.apply(this.webContents, args);
    },
    send (...args) {
        return this.webContents.send.apply(this.webContents, args);
    },
    openDevTools (...args) {
        return this.webContents.openDevTools.apply(this.webContents, args);
    },
    closeDevTools () {
        return this.webContents.closeDevTools();
    },
    isDevToolsOpened () {
        return this.webContents.isDevToolsOpened();
    },
    isDevToolsFocused () {
        return this.webContents.isDevToolsFocused();
    },
    toggleDevTools () {
        return this.webContents.toggleDevTools();
    },
    inspectElement (...args) {
        return this.webContents.inspectElement.apply(this.webContents, args);
    },
    inspectServiceWorker () {
        return this.webContents.inspectServiceWorker();
    },
    showDefinitionForSelection () {
        return this.webContents.showDefinitionForSelection();
    },
    capturePage (...args) {
        return this.webContents.capturePage.apply(this.webContents, args);
    }
    

});

module.exports = BrowserWindow;