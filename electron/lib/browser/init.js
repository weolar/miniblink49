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

let packagePath = __dirname + '/../default_app/';
//packagePath = 'E:\\mycode\\electron-v1.3.3\\electron-v1.3.3-win32-ia32\\demo\\ffftp\\';
//packagePath = 'E:\\mycode\\electron-v1.3.3\\electron-v1.3.3-win32-ia32\\demo\\AiTing\\';

let packageJson = null;
packageJson = require(packagePath + 'package.json');

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

// Set main startup script of the app.
const mainStartupScript = packageJson.main || 'index.js';

// Finally load app's main.js and transfer control to C++.
Module._load(path.join(packagePath, mainStartupScript), Module, true);

app.emit('ready', {});

// test
/*
//var BrowserWindow = require("electron").BrowserWindow;
//var win = new BrowserWindow({x:200, y:300, width: 800, height: 700, title:"test"});
win.on('resize', function (e) {
	console.log("emit resize");
})

// file:///E:/test/weibo/html_link.htm
// file:///E:/test/txmap/drag.htm
// file:///C:/Users/weo/Desktop/wke/3d-cube-loading/index.html
var webContent = win.webContents();
console.log("webContents:" + webContent);
webContent._loadURL("https://segmentfault.com/news");
//win = undefined;
*/
