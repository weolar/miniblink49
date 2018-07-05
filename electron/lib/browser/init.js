'use strict';

//process.binding('atom_browser_electron');
require('./electron');

const fs = require('fs');
const path = require('path');
const util = require('util');
const Module = require('module');
const v8 = require('v8');
const app = require('electron').app;

// Import common settings. 
require('./../common/init');
require('./rpc-server');

/*
let packageJson = null;
let packagePath = __dirname + '/../../app.asar/'; //__dirname + '/../default_app/';
if (!fs.existsSync(packagePath))
    packagePath = __dirname + '/../default_app/';
packageJson = require(packagePath + 'package.json');
*/
let packageJson = null;
let packagePath = null;
const searchPaths = ['/../default_app/', 'app', 'app.asar', 'default_app.asar']
for (packagePath of searchPaths) {
	try {
		packagePath = path.join(__dirname, packagePath)
		packageJson = require(path.join(packagePath, 'package.json'))
		break
	} catch (error) {
		continue
	}
}

if (packageJson == null) {
	process.nextTick(function () {
		return process.exit(1)
	})
	throw new Error('Unable to find a valid app')
}

// Set application's version.
if (packageJson.version != null) {
	app.setVersion(packageJson.version)
}

// Set application's name.
if (packageJson.productName != null) {
	app.setName(packageJson.productName)
} else if (packageJson.name != null) {
	app.setName(packageJson.name)
}

// Set application's desktop name.
if (packageJson.desktopName != null) {
	app.setDesktopName(packageJson.desktopName)
} else {
	app.setDesktopName((app.getName()) + '.desktop')
}

// Set v8 flags
if (packageJson.v8Flags != null) {
	v8.setFlagsFromString(packageJson.v8Flags);
}

// Set the user path according to application's name.
app.setPath('userData', path.join(app.getPath('appData'), app.getName()));
app.setPath('userCache', path.join(app.getPath('cache'), app.getName()));
app.setAppPath(packagePath);

// Set main startup script of the app.
const mainStartupScript = packageJson.main || 'index.js';

console.log("browser.init.js.packagePath:" + packagePath);

// Finally load app's main.js and transfer control to C++.
Module._load(path.join(packagePath, mainStartupScript), Module, true);

app.emit('will-finish-launching', {});
app.emit('ready', {});