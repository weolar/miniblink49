'use strict'

const {Buffer} = require('buffer')
const fs = require('fs')
const path = require('path')
const util = require('util')
const Module = require('module')
const v8 = require('v8')

// We modified the original process.argv to let node.js load the atom.js,
// we need to restore it here.
process.argv.splice(1, 1)

// Clear search paths.
//require('../common/reset-search-paths')

// Import common settings.
//require('../common/init')

var globalPaths = Module.globalPaths

// Expose public APIs.
globalPaths.push(path.join(__dirname, 'api', 'exports'))

// Don't quit on fatal error.
process.on('uncaughtException', function (error) {
  // Do nothing if the user has a custom uncaught exception handler.
  var dialog, message, ref, stack
  if (process.listeners('uncaughtException').length > 1) {
    return
  }

  // Show error in GUI.
  dialog = require('electron').dialog
  stack = (ref = error.stack) != null ? ref : error.name + ': ' + error.message
  message = 'Uncaught Exception:\n' + stack
  dialog.showErrorBox('A JavaScript error occurred in the main process', message)
})

// Emit 'exit' event on quit.
const {app} = require('electron')

app.on('quit', function (event, exitCode) {
  process.emit('exit', exitCode)
})

// Map process.exit to app.exit, which quits gracefully.
process.exit = app.exit

// Load the RPC server.
//require('./rpc-server')

// Load the guest view manager.
//require('./guest-view-manager')
//require('./guest-window-manager')

// Now we try to load app's package.json.
let packagePath = null
let packageJson = null
const searchPaths = ['app', 'app.asar', 'default_app.asar']
for (packagePath of searchPaths) {
  try {
    packagePath = path.join(process.resourcesPath, packagePath)
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
  v8.setFlagsFromString(packageJson.v8Flags)
}

// Set the user path according to application's name.
app.setPath('userData', path.join(app.getPath('appData'), app.getName()))
app.setPath('userCache', path.join(app.getPath('cache'), app.getName()))
app.setAppPath(packagePath)

// Load the chrome extension support.
require('./chrome-extension')

// Load internal desktop-capturer module.
require('./desktop-capturer')

// Load protocol module to ensure it is populated on app ready
require('./api/protocol')

// Set main startup script of the app.
const mainStartupScript = packageJson.main || 'index.js'

// Workaround for electron/electron#5050
if (process.platform === 'linux' && process.env.XDG_CURRENT_DESKTOP === 'Pantheon') {
  process.env.XDG_CURRENT_DESKTOP = 'Unity'
}

// Finally load app's main.js and transfer control to C++.
Module._load(path.join(packagePath, mainStartupScript), Module, true)
