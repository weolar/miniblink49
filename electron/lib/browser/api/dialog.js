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

Dialog.prototype.showSaveDialogSync = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showSaveDialogSync", args);
}

Dialog.prototype.showOpenDialogSync = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showOpenDialogSync", args);
}

Dialog.prototype.showMessageBox = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showMessageBox", args);
}

Dialog.prototype.showMessageBoxSync = function(/* [browserWindow, ]options[, callback] */) {
	if (0 == arguments.length || 4 <= arguments.length)
		return;
	var args = arguments;
	return this._trimArgsCallDialogFunction("showMessageBoxSync", args);
}

Dialog.prototype.showErrorBox = function(title, content) {
	this._showErrorBox(title, content);
}

Dialog.prototype._callDialogFunction = function(funcType, browserWindowID, options, callback) {
	if ("showMessageBoxSync" === funcType)
		return this._showMessageBoxSync(browserWindowID, options, callback);
	else if ("showSaveDialogSync" === funcType)
		return this._showSaveDialogSync(browserWindowID, options, callback);
	else if ("showOpenDialogSync" === funcType)
		return this._showOpenDialogSync(browserWindowID, options, callback);

	var self = this;
	const promise = new Promise(function(resolve, reject) {
		callback = function(canceled, filePaths) {
			console.log("filePaths:" + filePaths);
			resolve({"canceled":canceled, "filePaths":filePaths});
		};
		if ("showSaveDialog" === funcType) {
			var callbackWrap = function(files) {
				var file = (1 <= files.length) ? files[0] : "";
				callback(file);
			}
			/*return*/ self._showSaveDialog(browserWindowID, options, callbackWrap);
		} else if ("showOpenDialog" === funcType)
			/*return*/ self._showOpenDialog(browserWindowID, options, callback);
		else if ("showMessageBox" === funcType)
			/*return*/ self._showMessageBox(browserWindowID, options, callback);
	});
	
	return promise;
}

Dialog.prototype._trimArgsCallDialogFunction = function(funcType, args) {
	var browserWindowID = -1;
	var options = null;
	
	var callback = null;
	var idx = 0;
	if (1 + idx == args.length) {
		options = args[0 + idx];
		return this._callDialogFunction(funcType, null, options, /*null*/callback);
	} else if (2 + idx == args.length) {
		if (args[0 + idx].hasOwnProperty('id')) {
			browserWindowID = args[0 + idx].id;
			options = args[1 + idx];
			return this._callDialogFunction(funcType, browserWindowID, options, /*null*/callback);
		} else {
			options = args[0 + idx];
			//callback = args[1 + idx]; // 新electron已经不允许传回调了
			return this._callDialogFunction(funcType, null, options, callback);
		}
	} else if (3 + idx == args.length) {
		if (args[0 + idx].hasOwnProperty('id'))
			browserWindowID = args[0 + idx].id;
		options = args[1 + idx];
		//callback = args[2 + idx]; // 新electron已经不允许传回调了
		return this._callDialogFunction(funcType, browserWindowID, options, callback);
	}
}
exports.dialog = new Dialog();