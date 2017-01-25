'use strict';

const {Buffer} = require('buffer')
const fs = require('fs')
const path = require('path')
const util = require('util')
const Module = require('module')
const v8 = require('v8')

process.binding('atom_browser_web_contents');
const binding = process.binding('atom_browser_window');
var i = 0;

var win = new binding.BrowserWindow({x:200, y:300, width: 800, height: 700, title:"test"});

/*
win.emit = function(e) {
	console.log("emit 2");
}

binding.BrowserWindow.emit = function(e) {
	console.log("emit 1");
}*/

const {EventEmitter} = require('events')
Object.setPrototypeOf(binding.BrowserWindow.prototype, EventEmitter.prototype)

win.on('resize', function (e) {
  console.log("emit resize");
})

// file:///E:/test/weibo/html_link.htm
// file:///E:/test/txmap/drag.htm
// file:///C:/Users/weo/Desktop/wke/3d-cube-loading/index.html
var webContent = win.webContents();
console.log("webContents:" + webContent);
webContent._loadURL("http://bjorker.blog.163.com/blog/static/28878284201122304146605/");
//win = undefined;
