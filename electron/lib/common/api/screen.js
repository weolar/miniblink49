const EventEmitter = require('events').EventEmitter;
const binding = process.binding('atom_common_screen');
const Screen = binding.Screen;

Object.setPrototypeOf(Screen.prototype, EventEmitter.prototype); // 把on之类的函数绑定过来

exports.Screen = new Screen();
exports.screen = exports.Screen;