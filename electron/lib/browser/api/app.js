
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

exports.App = App;