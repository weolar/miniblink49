const EventEmitter = require('events').EventEmitter;
var ipcMain = new EventEmitter();
ipcMain.handle = function (channel, listener) {
    ipcMain.on(channel, function (event, ...args) {
        // TODO: Need channel ?
        var promise = listener(/*channel,*/ event, ...args);
        if (!promise || promise.toString() != "[object Promise]")
            return;
        promise.then(function(result) {
            event.sender.send('ipc-main-handle-reply-' + channel, result);
        });
    });
}

module.exports = ipcMain;

// Do not throw exception when channel name is "error".
module.exports.on('error', () => {})
