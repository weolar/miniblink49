
const binding = process.binding('atom_browser_protocol');
const Protocol = binding.Protocol;
const protocol = new Protocol(onLoadUrlBegin);

var handlerToIdMap = {};
var idGen = 0;

function onLoadUrlBegin(id, request, jobPtr, isCallOnHandlerFinishPtr, typePtr) {
    var handler = handlerToIdMap[id];
    if (!handler)
        return;

    handler(request, function(filePath) {
        var filePathTrim = filePath
        if ("string" != (typeof filePathTrim)) {
            filePathTrim = filePath.path;
        }
        if ("string" != (typeof filePathTrim))
            return;
        
        protocol.onHandlerFinish(filePathTrim, jobPtr, isCallOnHandlerFinishPtr, typePtr);
    });
}

Protocol.prototype.registerProtocol = function(scheme, handler, completion, type) {
    var id = ++idGen;
    handlerToIdMap[id] = handler;
    this._registerProtocol(scheme, id, type);
    if (completion)
        completion(null);
}

Protocol.prototype.registerFileProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion, "file");
}

Protocol.prototype.registerBufferProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion, "buffer");
}


Protocol.prototype.registerStringProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion, "string");
}

Protocol.prototype.registerHttpProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion, "http");
}

Protocol.prototype.unregisterProtocol = function(scheme, completion) {
    delete handlerToIdMap[id];

    this._unregisterProtocol(scheme);
    if (completion)
        completion(null);
}

Protocol.prototype.isProtocolHandled = function(scheme, callback) {
    var b = this._isProtocolHandled(scheme);
    callback(b);
}

Protocol.prototype.interceptFileProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion);
}

Protocol.prototype.interceptStringProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion);
}

Protocol.prototype.interceptBufferProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion);
}

Protocol.prototype.interceptHttpProtocol = function(scheme, handler, completion) {
    this.registerProtocol(scheme, handler, completion);
}

Protocol.prototype.uninterceptProtocol = function(scheme, completion) {
    this.unregisterProtocol(scheme, completion);
}

exports.protocol = protocol;