const clipboard = {}; // process.atomBinding('clipboard');

clipboard.readText = function () {
    return "";
}

clipboard.writeText = function (text, type) {
    ;
}

clipboard.readImage = function (type) {
    ;
}

clipboard.writeImage = function (image, type) {
    ;
}

clipboard.clear = function (type) {
    ;
}

clipboard.availableFormats = function (type) {
    return [];
}

clipboard.has = function (data, type) {
    return false;
}

clipboard.read = function (data, type) {
    return "";
}

clipboard.write = function (data, type) {
    return;
}

// TODO(codebytere): remove in 3.0
clipboard.readHtml = function () {
    //return clipboard.readHTML();
    return "";
}

// TODO(codebytere): remove in 3.0
clipboard.writeHtml = function () {
    //return clipboard.writeHTML();
}

// TODO(codebytere): remove in 3.0
clipboard.readRtf = function () {
    return "";
    //return clipboard.readRTF();
}

// TODO(codebytere): remove in 3.0
clipboard.writeRtf = function () {
    //return clipboard.writeRTF();
}

module.exports = clipboard;
