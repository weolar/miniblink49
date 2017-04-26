const EventEmitter = require('events').EventEmitter;
const Screen = process.binding('atom_common_screen').Screen;

Object.setPrototypeOf(Screen.prototype, EventEmitter.prototype)

module.exports = Screen;
