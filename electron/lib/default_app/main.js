'use strict';
const app = require('electron').app;
const dialog = require('electron').dialog;
const shell = require('electron').shell;
const Menu = require('electron').Menu;

const fs = require('fs');
const Module = require('module');
const path = require('path');
const url = require('url');

// Parse command line options.
const argv = process.argv.slice(1);
const option = { file: null, help: null, version: null, abi: null, webdriver: null, modules: [] };

for (let i = 1; i < argv.length; i++) {
    if (argv[i] === '--version' || argv[i] === '-v') {
        option.version = true;
        break;
    } else if (argv[i] === '--abi') {
        option.abi = true;
        break;
    } else if (argv[i].match(/^--app=/)) {
        option.file = argv[i].split('=')[1];
        break;
    } else if (argv[i] === '--help' || argv[i] === '-h') {
        option.help = true;
        break;
    } else if (argv[i] === '--interactive' || argv[i] === '-i') {
        option.interactive = true;
    } else if (argv[i] === '--test-type=webdriver') {
        option.webdriver = true;
    } else if (argv[i] === '--require' || argv[i] === '-r') {
        option.modules.push(argv[++i]);
        continue;
    } else if (argv[i][0] === '-') {
        continue;
    } else {
        option.file = argv[i];
        for (let j = option.file.length - 1; j >= 0; --j) {
            const code = option.file.charCodeAt(j);
            if (code === 92 || code === 47 || code === 58) {
                const path = (option.file.slice(0, j));
                process.mainModule.paths = process.mainModule.paths.concat(Module._nodeModulePaths(path));
                process.mainModule.paths = process.mainModule.paths.concat(path);
                process.mainModule.filename = option.file;
                //
                break;
            }
        }
        break;
    }
}

if (null == option.file) {
    let tryPaths = ['../../app.asar/package.json', '../../app.asar/main.js', '../../app/package.json', '/index.html'];
    for (let i = 0; i < tryPaths.length; ++i) {
        let tryPath = tryPaths[i];
        let fileName = path.join(__dirname, tryPath);
        if (!fs.existsSync(fileName))
            continue;
        option.file = fileName;
        break;
    }
    console.log("default_app.js, option.file:" + option.file);
}

// Quit when all windows are closed and no other one is listening to this.
app.on('window-all-closed', () => {
    if (app.listeners('window-all-closed').length === 1 && !option.interactive) {
        app.quit();
    }
})

if (option.modules.length > 0) {
    Module._preloadModules(option.modules);
}

function loadApplicationPackage(packagePath) {
    // Add a flag indicating app is started from default app.
    process.defaultApp = true;

    try {
        // Override app name and version.
        packagePath = path.resolve(packagePath);

        let packageJsonPath = packagePath;
        if (-1 == packagePath.indexOf('package.json'))
            packageJsonPath = path.join(packagePath, 'package.json');
        else
            packagePath = path.dirname(packagePath);

        if (fs.existsSync(packageJsonPath)) {
            let packageJson;
            try {
                packageJson = require(packageJsonPath);
            } catch (e) {
                showErrorMessage(`Unable to parse ${packageJsonPath}\n\n${e.message}`);
                return;
            }

            if (packageJson.version) {
                app.setVersion(packageJson.version);
            }
            if (packageJson.productName) {
                app.setName(packageJson.productName);
            } else if (packageJson.name) {
                app.setName(packageJson.name);
            }
            app.setPath('userData', path.join(app.getPath('appData'), app.getName()));
            app.setPath('userCache', path.join(app.getPath('cache'), app.getName()));
            app.setAppPath(packagePath);
            
            packagePath = path.join(packagePath, packageJson.main);
        }

        try {
            Module._resolveFilename(packagePath, module, true);
        } catch (e) {
            console.log(`default_app/main.js: Unable to find Electron app at ${packagePath}\n\n${e.message}`);
            return;
        }

        // Run the app.
        console.log("default_app main.js packagePath," + packagePath);
        Module._load(packagePath, module, true);
    } catch (e) {
        console.error('App threw an error during load');
        console.error(e.stack || e);
        throw e;
    }
}

function showErrorMessage(message) {
    app.focus();
    dialog.showErrorBox('Error launching app', message);
    process.exit(1);
}

function loadApplicationByUrl(appUrl) {
	console.log("default_app.js, loadApplicationByUrl: " + appUrl);
    require('./default_app').load(appUrl);
}

function loadApplicationByJsUrl(appJsUrl) {
    require(appJsUrl);
}

function startRepl() {
    if (process.platform === 'win32') {
        console.error('Electron REPL not currently supported on Windows');
        process.exit(1);
        return;
    }

    const repl = require('repl');
    repl.start('> ').on('exit', () => {
        process.exit(0);
    })
}

// Start the specified app if there is one specified in command line, otherwise
// start the default app.
if (option.file && !option.webdriver) {
    const file = option.file;
    const protocol = url.parse(file).protocol;
    const extension = path.extname(file);
    
    if (protocol === 'http:' || protocol === 'https:' || protocol === 'file:') {
        loadApplicationByUrl(file);
    } else if (extension === '.html' || extension === '.htm') {
        loadApplicationByUrl('file://' + path.resolve(file));
    } else if (extension === '.js') {
        loadApplicationByJsUrl(path.resolve(file));
    } else {
        loadApplicationPackage(file);
    }
} else if (option.version) {
    console.log('v' + process.versions.electron);
    process.exit(0);
} else if (option.abi) {
    console.log(process.versions.modules);
    process.exit(0);
} else if (option.help) {
    const helpMessage = `Electron ${process.versions.electron} - Build cross platform desktop apps with JavaScript, HTML, and CSS

  Usage: electron [options] [path]

  A path to an Electron app may be specified. The path must be one of the following:

    - index.js file.
    - Folder containing a package.json file.
    - Folder containing an index.js file.
    - .html/.htm file.
    - http://, https://, or file:// URL.

  Options:
    -h, --help            Print this usage message.
    -i, --interactive     Open a REPL to the main process.
    -r, --require         Module to preload (option can be repeated)
    -v, --version         Print the version.
    --abi                 Print the application binary interface.`
    console.log(helpMessage);
    process.exit(0);
} else if (option.interactive) {
    startRepl();
} else {
    const indexPath = path.join(__dirname, '/index.html');
    loadApplicationByUrl(`file://${indexPath}`);
}

// Create default menu.
app.once('ready', () => {
    if (Menu.getApplicationMenu()) return;

    const template = [
      {
          label: 'Edit',
          submenu: [
            {
                role: 'undo'
            },
            {
                role: 'redo'
            },
            {
                type: 'separator'
            },
            {
                role: 'cut'
            },
            {
                role: 'copy'
            },
            {
                role: 'paste'
            },
            {
                role: 'pasteandmatchstyle'
            },
            {
                role: 'delete'
            },
            {
                role: 'selectall'
            }
          ]
      },
      {
          label: 'View',
          submenu: [
            {
                label: 'Reload',
                accelerator: 'CmdOrCtrl+R',
                click (item, focusedWindow) {
                    if (focusedWindow) focusedWindow.reload()
                }
            },
            {
                label: 'Toggle Developer Tools',
                accelerator: process.platform === 'darwin' ? 'Alt+Command+I' : 'Ctrl+Shift+I',
                click (item, focusedWindow) {
                    if (focusedWindow) focusedWindow.toggleDevTools()
                }
            },
            {
                type: 'separator'
            },
            {
                role: 'resetzoom'
            },
            {
                role: 'zoomin'
            },
            {
                role: 'zoomout'
            },
            {
                type: 'separator'
            },
            {
                role: 'togglefullscreen'
            }
          ]
      },
    ]

    //const menu = Menu.buildFromTemplate(template)
    //Menu.setApplicationMenu(menu)
});
////////