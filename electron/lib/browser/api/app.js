
var App = process.binding('atom_browser_app').App;

App.prototype.commandLine = {
	appendSwitch: function(switchVal, value) {
	},
	
	appendArgument: function(value) {
	}
};

let appPath = null;

App.prototype.getAppPath = function() {
	return appPath;
}

App.prototype.setAppPath = function(path) {
	appPath = path
}

App.prototype.getApplicationMenu = function() {
	return Menu.getApplicationMenu()
}

App.prototype.relaunch = function(options) {
	if (!options)
		options = {};
	if (!options.args || Object.prototype.toString.call(options.args) != '[object Array]')
		options.args = [""];
	
	if (!options.execPath || typeof (execPath) != "string")
		options.execPath = "";
	this._relaunch(options);
}

var singleInstanceCallbackMap = [];
var singleInstanceCallbackMapIdGen = 0;

function singleInstancCallback(callback) {
	return function(argString) {
	    try {
	        var argJson = JSON.parse(argString);
	        if (argJson.length == 0)
	            return;

	        if (argJson.length == 1)
	            argJson.append("");

	        var workingDirectory = argJson.pop();
	        callback(argJson, workingDirectory);

	    } catch (e) {
	    }
	}
}

App.prototype.makeSingleInstance = function(callback) {
	//if (callback)
	//	return this.makeSingleInstanceImpl(-1);
	
	//singleInstanceCallbackMapIdGen++;
	//singleInstanceCallbackMap[singleInstanceCallbackMapIdGen] = callback;
	return this.makeSingleInstanceImpl(singleInstancCallback(callback));
}


exports.App = App;