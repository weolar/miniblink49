'use strict';

const bindings = process.binding('atom_browser_dialog');
const Dialog = bindings.Dialog;

Dialog.prototype.showSaveDialog = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showSaveDialog", args);
}

Dialog.prototype.showOpenDialog = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showOpenDialog", args);
}

Dialog.prototype.showMessageBox = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showMessageBox", args);
}

Dialog.prototype.showErrorBox = function(title, content) {
	this._showErrorBox(title, content);
}

Dialog.prototype._callDialogFunction = function(funcType, browserWindowID, options, callback) {
	if ("showSaveDialog" === funcType)
		return this._showSaveDialog(browserWindowID, options, callback);
	else if ("showOpenDialog" === funcType)
		return this._showOpenDialog(browserWindowID, options, callback);
	else if ("showMessageBox" === funcType)
		return this._showMessageBox(browserWindowID, options, callback);
}

Dialog.prototype._trimArgsCallDialogFunction = function(funcType, args) {
	var browserWindowID = -1;
	var options = null;
	var callback = null;
	
	var idx = 0;
	if (1 + idx == args.length) {
		options = args[0 + idx];
		return this._callDialogFunction(funcType, null, options, null);
	} else if (2 + idx == args.length) {
		if (args[0 + idx].hasOwnProperty('id')) {
			browserWindowID = args[0 + idx].id;
			options = args[1 + idx];
			return this._callDialogFunction(funcType, browserWindowID, options, null);
		} else {
			options = args[0 + idx];
			callback = args[1 + idx];
			return this._callDialogFunction(funcType, null, options, callback);
		}
	} else if (3 + idx == args.length) {
		if (args[0 + idx].hasOwnProperty('id'))
			browserWindowID = args[0 + idx].id;
		options = args[1 + idx];
		callback = args[2 + idx];
		return this._callDialogFunction(funcType, browserWindowID, options, callback);
	}
}
exports.dialog = new Dialog();