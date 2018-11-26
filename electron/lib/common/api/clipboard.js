const Clipboard = process.binding('atom_common_clipboard').Clipboard;

Clipboard.prototype.readText = function (type) {
	if (!type)
		type = "";
	return this._readText(type);
}

Clipboard.prototype.readImage = function (type) {
	if (!type)
		type = "";
	return this._readImage(type);
}

Clipboard.prototype.writeText = function (text, type) {
	if (!type)
		type = "";
	return this._writeText(text, type);
}

Clipboard.prototype.writeImage = function (image, type) {
	if (!image)
		image = null;
    if (!type)
		type = "";
	return this._writeImage(image, type);
}

Clipboard.prototype.clear = function (type) {
    if (!type)
		type = "";
	return this._clear(type);
}

Clipboard.prototype.availableFormats = function (type) {
    return [];
}

/*
clipboard.readText = function () {
	console.log("--------------clipboard.readText");
    return "";
}

clipboard.writeText = function (text, type) {
    console.log("--------------clipboard.writeText");
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
*/
const clipboard = new Clipboard();
module.exports = clipboard;
