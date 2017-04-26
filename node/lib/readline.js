// Inspiration for this code comes from Salvatore Sanfilippo's linenoise.
// https://github.com/antirez/linenoise
// Reference:
// * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// * http://www.3waylabs.com/nw/WWW/products/wizcon/vt220.html

'use strict';

const kHistorySize = 30;
const kMincrlfDelay = 100;
const kMaxcrlfDelay = 2000;

const util = require('util');
const debug = util.debuglog('readline');
const internalUtil = require('internal/util');
const inherits = util.inherits;
const Buffer = require('buffer').Buffer;
const EventEmitter = require('events');
const internalReadline = require('internal/readline');
const emitKeys = internalReadline.emitKeys;
const getStringWidth = internalReadline.getStringWidth;
const isFullWidthCodePoint = internalReadline.isFullWidthCodePoint;
const stripVTControlCharacters = internalReadline.stripVTControlCharacters;


exports.createInterface = function(input, output, completer, terminal) {
  var rl;
  if (arguments.length === 1) {
    rl = new Interface(input);
  } else {
    rl = new Interface(input, output, completer, terminal);
  }
  return rl;
};


function Interface(input, output, completer, terminal) {
  if (!(this instanceof Interface)) {
    // call the constructor preserving original number of arguments
    const self = Object.create(Interface.prototype);
    Interface.apply(self, arguments);
    return self;
  }

  this._sawReturnAt = 0;
  this.isCompletionEnabled = true;
  this._sawKeyPress = false;

  EventEmitter.call(this);
  var historySize;
  let crlfDelay;
  let prompt = '> ';

  if (arguments.length === 1) {
    // an options object was given
    output = input.output;
    completer = input.completer;
    terminal = input.terminal;
    historySize = input.historySize;
    if (input.prompt !== undefined) {
      prompt = input.prompt;
    }
    crlfDelay = input.crlfDelay;
    input = input.input;
  }

  if (completer && typeof completer !== 'function') {
    throw new TypeError('Argument "completer" must be a function');
  }

  if (historySize === undefined) {
    historySize = kHistorySize;
  }

  if (typeof historySize !== 'number' ||
      isNaN(historySize) ||
      historySize < 0) {
    throw new TypeError('Argument "historySize" must be a positive number');
  }

  // backwards compat; check the isTTY prop of the output stream
  //  when `terminal` was not specified
  if (terminal === undefined && !(output === null || output === undefined)) {
    terminal = !!output.isTTY;
  }

  var self = this;

  this.output = output;
  this.input = input;
  this.historySize = historySize;
  this.crlfDelay = Math.max(kMincrlfDelay,
                            Math.min(kMaxcrlfDelay, crlfDelay >>> 0));

  // Check arity, 2 - for async, 1 for sync
  if (typeof completer === 'function') {
    this.completer = completer.length === 2 ? completer : function(v, cb) {
      cb(null, completer(v));
    };
  }

  this.setPrompt(prompt);

  this.terminal = !!terminal;

  function ondata(data) {
    self._normalWrite(data);
  }

  function onend() {
    if (typeof self._line_buffer === 'string' &&
        self._line_buffer.length > 0) {
      self.emit('line', self._line_buffer);
    }
    self.close();
  }

  function ontermend() {
    if (typeof self.line === 'string' && self.line.length > 0) {
      self.emit('line', self.line);
    }
    self.close();
  }

  function onkeypress(s, key) {
    self._ttyWrite(s, key);
  }

  function onresize() {
    self._refreshLine();
  }

  if (!this.terminal) {
    input.on('data', ondata);
    input.on('end', onend);
    self.once('close', function() {
      input.removeListener('data', ondata);
      input.removeListener('end', onend);
    });
    var StringDecoder = require('string_decoder').StringDecoder; // lazy load
    this._decoder = new StringDecoder('utf8');

  } else {

    emitKeypressEvents(input, this);

    // input usually refers to stdin
    input.on('keypress', onkeypress);
    input.on('end', ontermend);

    // Current line
    this.line = '';

    this._setRawMode(true);
    this.terminal = true;

    // Cursor position on the line.
    this.cursor = 0;

    this.history = [];
    this.historyIndex = -1;

    if (output !== null && output !== undefined)
      output.on('resize', onresize);

    self.once('close', function() {
      input.removeListener('keypress', onkeypress);
      input.removeListener('end', ontermend);
      if (output !== null && output !== undefined) {
        output.removeListener('resize', onresize);
      }
    });
  }

  input.resume();
}

inherits(Interface, EventEmitter);

Object.defineProperty(Interface.prototype, 'columns', {
  configurable: true,
  enumerable: true,
  get: function() {
    var columns = Infinity;
    if (this.output && this.output.columns)
      columns = this.output.columns;
    return columns;
  }
});

Interface.prototype.setPrompt = function(prompt) {
  this._prompt = prompt;
};


Interface.prototype._setRawMode = function(mode) {
  const wasInRawMode = this.input.isRaw;

  if (typeof this.input.setRawMode === 'function') {
    this.input.setRawMode(mode);
  }

  return wasInRawMode;
};


Interface.prototype.prompt = function(preserveCursor) {
  if (this.paused) this.resume();
  if (this.terminal) {
    if (!preserveCursor) this.cursor = 0;
    this._refreshLine();
  } else {
    this._writeToOutput(this._prompt);
  }
};


Interface.prototype.question = function(query, cb) {
  if (typeof cb === 'function') {
    if (this._questionCallback) {
      this.prompt();
    } else {
      this._oldPrompt = this._prompt;
      this.setPrompt(query);
      this._questionCallback = cb;
      this.prompt();
    }
  }
};


Interface.prototype._onLine = function(line) {
  if (this._questionCallback) {
    var cb = this._questionCallback;
    this._questionCallback = null;
    this.setPrompt(this._oldPrompt);
    cb(line);
  } else {
    this.emit('line', line);
  }
};

Interface.prototype._writeToOutput = function _writeToOutput(stringToWrite) {
  if (typeof stringToWrite !== 'string')
    throw new TypeError('"stringToWrite" argument must be a string');

  if (this.output !== null && this.output !== undefined)
    this.output.write(stringToWrite);
};

Interface.prototype._addHistory = function() {
  if (this.line.length === 0) return '';

  // if the history is disabled then return the line
  if (this.historySize === 0) return this.line;

  // if the trimmed line is empty then return the line
  if (this.line.trim().length === 0) return this.line;

  if (this.history.length === 0 || this.history[0] !== this.line) {
    this.history.unshift(this.line);

    // Only store so many
    if (this.history.length > this.historySize) this.history.pop();
  }

  this.historyIndex = -1;
  return this.history[0];
};


Interface.prototype._refreshLine = function() {
  // line length
  var line = this._prompt + this.line;
  var dispPos = this._getDisplayPos(line);
  var lineCols = dispPos.cols;
  var lineRows = dispPos.rows;

  // cursor position
  var cursorPos = this._getCursorPos();

  // first move to the bottom of the current line, based on cursor pos
  var prevRows = this.prevRows || 0;
  if (prevRows > 0) {
    exports.moveCursor(this.output, 0, -prevRows);
  }

  // Cursor to left edge.
  exports.cursorTo(this.output, 0);
  // erase data
  exports.clearScreenDown(this.output);

  // Write the prompt and the current buffer content.
  this._writeToOutput(line);

  // Force terminal to allocate a new line
  if (lineCols === 0) {
    this._writeToOutput(' ');
  }

  // Move cursor to original position.
  exports.cursorTo(this.output, cursorPos.cols);

  var diff = lineRows - cursorPos.rows;
  if (diff > 0) {
    exports.moveCursor(this.output, 0, -diff);
  }

  this.prevRows = cursorPos.rows;
};


Interface.prototype.close = function() {
  if (this.closed) return;
  this.pause();
  if (this.terminal) {
    this._setRawMode(false);
  }
  this.closed = true;
  this.emit('close');
};


Interface.prototype.pause = function() {
  if (this.paused) return;
  this.input.pause();
  this.paused = true;
  this.emit('pause');
  return this;
};


Interface.prototype.resume = function() {
  if (!this.paused) return;
  this.input.resume();
  this.paused = false;
  this.emit('resume');
  return this;
};


Interface.prototype.write = function(d, key) {
  if (this.paused) this.resume();
  this.terminal ? this._ttyWrite(d, key) : this._normalWrite(d);
};

// \r\n, \n, or \r followed by something other than \n
const lineEnding = /\r?\n|\r(?!\n)/;
Interface.prototype._normalWrite = function(b) {
  if (b === undefined) {
    return;
  }
  var string = this._decoder.write(b);
  if (this._sawReturnAt &&
      Date.now() - this._sawReturnAt <= this.crlfDelay) {
    string = string.replace(/^\n/, '');
    this._sawReturnAt = 0;
  }

  // Run test() on the new string chunk, not on the entire line buffer.
  var newPartContainsEnding = lineEnding.test(string);

  if (this._line_buffer) {
    string = this._line_buffer + string;
    this._line_buffer = null;
  }
  if (newPartContainsEnding) {
    this._sawReturnAt = string.endsWith('\r') ? Date.now() : 0;

    // got one or more newlines; process into "line" events
    var lines = string.split(lineEnding);
    // either '' or (conceivably) the unfinished portion of the next line
    string = lines.pop();
    this._line_buffer = string;
    lines.forEach(function(line) {
      this._onLine(line);
    }, this);
  } else if (string) {
    // no newlines this time, save what we have for next time
    this._line_buffer = string;
  }
};

Interface.prototype._insertString = function(c) {
  if (this.cursor < this.line.length) {
    var beg = this.line.slice(0, this.cursor);
    var end = this.line.slice(this.cursor, this.line.length);
    this.line = beg + c + end;
    this.cursor += c.length;
    this._refreshLine();
  } else {
    this.line += c;
    this.cursor += c.length;

    if (this._getCursorPos().cols === 0) {
      this._refreshLine();
    } else {
      this._writeToOutput(c);
    }

    // a hack to get the line refreshed if it's needed
    this._moveCursor(0);
  }
};

Interface.prototype._tabComplete = function() {
  var self = this;

  self.pause();
  self.completer(self.line.slice(0, self.cursor), function(err, rv) {
    self.resume();

    if (err) {
      debug('tab completion error %j', err);
      return;
    }

    const completions = rv[0];
    const completeOn = rv[1];  // the text that was completed
    if (completions && completions.length) {
      // Apply/show completions.
      if (completions.length === 1) {
        self._insertString(completions[0].slice(completeOn.length));
      } else {
        self._writeToOutput('\r\n');
        var width = completions.reduce(function(a, b) {
          return a.length > b.length ? a : b;
        }).length + 2;  // 2 space padding
        var maxColumns = Math.floor(self.columns / width);
        if (!maxColumns || maxColumns === Infinity) {
          maxColumns = 1;
        }
        var group = [], c;
        for (var i = 0, compLen = completions.length; i < compLen; i++) {
          c = completions[i];
          if (c === '') {
            handleGroup(self, group, width, maxColumns);
            group = [];
          } else {
            group.push(c);
          }
        }
        handleGroup(self, group, width, maxColumns);

        // If there is a common prefix to all matches, then apply that
        // portion.
        var f = completions.filter(function(e) { if (e) return e; });
        var prefix = commonPrefix(f);
        if (prefix.length > completeOn.length) {
          self._insertString(prefix.slice(completeOn.length));
        }

      }
      self._refreshLine();
    }
  });
};

// this = Interface instance
function handleGroup(self, group, width, maxColumns) {
  if (group.length == 0) {
    return;
  }
  var minRows = Math.ceil(group.length / maxColumns);
  for (var row = 0; row < minRows; row++) {
    for (var col = 0; col < maxColumns; col++) {
      var idx = row * maxColumns + col;
      if (idx >= group.length) {
        break;
      }
      var item = group[idx];
      self._writeToOutput(item);
      if (col < maxColumns - 1) {
        for (var s = 0, itemLen = item.length; s < width - itemLen;
             s++) {
          self._writeToOutput(' ');
        }
      }
    }
    self._writeToOutput('\r\n');
  }
  self._writeToOutput('\r\n');
}

function commonPrefix(strings) {
  if (!strings || strings.length == 0) {
    return '';
  }
  var sorted = strings.slice().sort();
  var min = sorted[0];
  var max = sorted[sorted.length - 1];
  for (var i = 0, len = min.length; i < len; i++) {
    if (min[i] != max[i]) {
      return min.slice(0, i);
    }
  }
  return min;
}


Interface.prototype._wordLeft = function() {
  if (this.cursor > 0) {
    var leading = this.line.slice(0, this.cursor);
    var match = leading.match(/([^\w\s]+|\w+|)\s*$/);
    this._moveCursor(-match[0].length);
  }
};


Interface.prototype._wordRight = function() {
  if (this.cursor < this.line.length) {
    var trailing = this.line.slice(this.cursor);
    var match = trailing.match(/^(\s+|\W+|\w+)\s*/);
    this._moveCursor(match[0].length);
  }
};


Interface.prototype._deleteLeft = function() {
  if (this.cursor > 0 && this.line.length > 0) {
    this.line = this.line.slice(0, this.cursor - 1) +
                this.line.slice(this.cursor, this.line.length);

    this.cursor--;
    this._refreshLine();
  }
};


Interface.prototype._deleteRight = function() {
  this.line = this.line.slice(0, this.cursor) +
              this.line.slice(this.cursor + 1, this.line.length);
  this._refreshLine();
};


Interface.prototype._deleteWordLeft = function() {
  if (this.cursor > 0) {
    var leading = this.line.slice(0, this.cursor);
    var match = leading.match(/([^\w\s]+|\w+|)\s*$/);
    leading = leading.slice(0, leading.length - match[0].length);
    this.line = leading + this.line.slice(this.cursor, this.line.length);
    this.cursor = leading.length;
    this._refreshLine();
  }
};


Interface.prototype._deleteWordRight = function() {
  if (this.cursor < this.line.length) {
    var trailing = this.line.slice(this.cursor);
    var match = trailing.match(/^(\s+|\W+|\w+)\s*/);
    this.line = this.line.slice(0, this.cursor) +
                trailing.slice(match[0].length);
    this._refreshLine();
  }
};


Interface.prototype._deleteLineLeft = function() {
  this.line = this.line.slice(this.cursor);
  this.cursor = 0;
  this._refreshLine();
};


Interface.prototype._deleteLineRight = function() {
  this.line = this.line.slice(0, this.cursor);
  this._refreshLine();
};


Interface.prototype.clearLine = function() {
  this._moveCursor(+Infinity);
  this._writeToOutput('\r\n');
  this.line = '';
  this.cursor = 0;
  this.prevRows = 0;
};


Interface.prototype._line = function() {
  var line = this._addHistory();
  this.clearLine();
  this._onLine(line);
};


Interface.prototype._historyNext = function() {
  if (this.historyIndex > 0) {
    this.historyIndex--;
    this.line = this.history[this.historyIndex];
    this.cursor = this.line.length; // set cursor to end of line.
    this._refreshLine();

  } else if (this.historyIndex === 0) {
    this.historyIndex = -1;
    this.cursor = 0;
    this.line = '';
    this._refreshLine();
  }
};


Interface.prototype._historyPrev = function() {
  if (this.historyIndex + 1 < this.history.length) {
    this.historyIndex++;
    this.line = this.history[this.historyIndex];
    this.cursor = this.line.length; // set cursor to end of line.

    this._refreshLine();
  }
};


// Returns the last character's display position of the given string
Interface.prototype._getDisplayPos = function(str) {
  var offset = 0;
  var col = this.columns;
  var row = 0;
  var code;
  str = stripVTControlCharacters(str);
  for (var i = 0, len = str.length; i < len; i++) {
    code = str.codePointAt(i);
    if (code >= 0x10000) { // surrogates
      i++;
    }
    if (code === 0x0a) { // new line \n
      offset = 0;
      row += 1;
      continue;
    }
    if (isFullWidthCodePoint(code)) {
      if ((offset + 1) % col === 0) {
        offset++;
      }
      offset += 2;
    } else {
      offset++;
    }
  }
  var cols = offset % col;
  var rows = row + (offset - cols) / col;
  return {cols: cols, rows: rows};
};


// Returns current cursor's position and line
Interface.prototype._getCursorPos = function() {
  var columns = this.columns;
  var strBeforeCursor = this._prompt + this.line.substring(0, this.cursor);
  var dispPos = this._getDisplayPos(stripVTControlCharacters(strBeforeCursor));
  var cols = dispPos.cols;
  var rows = dispPos.rows;
  // If the cursor is on a full-width character which steps over the line,
  // move the cursor to the beginning of the next line.
  if (cols + 1 === columns &&
      this.cursor < this.line.length &&
      isFullWidthCodePoint(this.line.codePointAt(this.cursor))) {
    rows++;
    cols = 0;
  }
  return {cols: cols, rows: rows};
};


// This function moves cursor dx places to the right
// (-dx for left) and refreshes the line if it is needed
Interface.prototype._moveCursor = function(dx) {
  var oldcursor = this.cursor;
  var oldPos = this._getCursorPos();
  this.cursor += dx;

  // bounds check
  if (this.cursor < 0) this.cursor = 0;
  else if (this.cursor > this.line.length) this.cursor = this.line.length;

  var newPos = this._getCursorPos();

  // check if cursors are in the same line
  if (oldPos.rows === newPos.rows) {
    var diffCursor = this.cursor - oldcursor;
    var diffWidth;
    if (diffCursor < 0) {
      diffWidth = -getStringWidth(
          this.line.substring(this.cursor, oldcursor)
          );
    } else if (diffCursor > 0) {
      diffWidth = getStringWidth(
          this.line.substring(this.cursor, oldcursor)
          );
    }
    exports.moveCursor(this.output, diffWidth, 0);
    this.prevRows = newPos.rows;
  } else {
    this._refreshLine();
  }
};


// handle a write from the tty
Interface.prototype._ttyWrite = function(s, key) {
  key = key || {};

  // Ignore escape key - Fixes #2876
  if (key.name == 'escape') return;

  if (key.ctrl && key.shift) {
    /* Control and shift pressed */
    switch (key.name) {
      case 'backspace':
        this._deleteLineLeft();
        break;

      case 'delete':
        this._deleteLineRight();
        break;
    }

  } else if (key.ctrl) {
    /* Control key pressed */

    switch (key.name) {
      case 'c':
        if (this.listenerCount('SIGINT') > 0) {
          this.emit('SIGINT');
        } else {
          // This readline instance is finished
          this.close();
        }
        break;

      case 'h': // delete left
        this._deleteLeft();
        break;

      case 'd': // delete right or EOF
        if (this.cursor === 0 && this.line.length === 0) {
          // This readline instance is finished
          this.close();
        } else if (this.cursor < this.line.length) {
          this._deleteRight();
        }
        break;

      case 'u': // delete the whole line
        this.cursor = 0;
        this.line = '';
        this._refreshLine();
        break;

      case 'k': // delete from current to end of line
        this._deleteLineRight();
        break;

      case 'a': // go to the start of the line
        this._moveCursor(-Infinity);
        break;

      case 'e': // go to the end of the line
        this._moveCursor(+Infinity);
        break;

      case 'b': // back one character
        this._moveCursor(-1);
        break;

      case 'f': // forward one character
        this._moveCursor(+1);
        break;

      case 'l': // clear the whole screen
        exports.cursorTo(this.output, 0, 0);
        exports.clearScreenDown(this.output);
        this._refreshLine();
        break;

      case 'n': // next history item
        this._historyNext();
        break;

      case 'p': // previous history item
        this._historyPrev();
        break;

      case 'z':
        if (process.platform == 'win32') break;
        if (this.listenerCount('SIGTSTP') > 0) {
          this.emit('SIGTSTP');
        } else {
          process.once('SIGCONT', (function(self) {
            return function() {
              // Don't raise events if stream has already been abandoned.
              if (!self.paused) {
                // Stream must be paused and resumed after SIGCONT to catch
                // SIGINT, SIGTSTP, and EOF.
                self.pause();
                self.emit('SIGCONT');
              }
              // explicitly re-enable "raw mode" and move the cursor to
              // the correct position.
              // See https://github.com/joyent/node/issues/3295.
              self._setRawMode(true);
              self._refreshLine();
            };
          })(this));
          this._setRawMode(false);
          process.kill(process.pid, 'SIGTSTP');
        }
        break;

      case 'w': // delete backwards to a word boundary
      case 'backspace':
        this._deleteWordLeft();
        break;

      case 'delete': // delete forward to a word boundary
        this._deleteWordRight();
        break;

      case 'left':
        this._wordLeft();
        break;

      case 'right':
        this._wordRight();
        break;
    }

  } else if (key.meta) {
    /* Meta key pressed */

    switch (key.name) {
      case 'b': // backward word
        this._wordLeft();
        break;

      case 'f': // forward word
        this._wordRight();
        break;

      case 'd': // delete forward word
      case 'delete':
        this._deleteWordRight();
        break;

      case 'backspace': // delete backwards to a word boundary
        this._deleteWordLeft();
        break;
    }

  } else {
    /* No modifier keys used */

    // \r bookkeeping is only relevant if a \n comes right after.
    if (this._sawReturnAt && key.name !== 'enter')
      this._sawReturnAt = 0;

    switch (key.name) {
      case 'return':  // carriage return, i.e. \r
        this._sawReturnAt = Date.now();
        this._line();
        break;

      case 'enter':
        // When key interval > crlfDelay
        if (this._sawReturnAt === 0 ||
            Date.now() - this._sawReturnAt > this.crlfDelay) {
          this._line();
        }
        this._sawReturnAt = 0;
        break;

      case 'backspace':
        this._deleteLeft();
        break;

      case 'delete':
        this._deleteRight();
        break;

      case 'left':
        this._moveCursor(-1);
        break;

      case 'right':
        this._moveCursor(+1);
        break;

      case 'home':
        this._moveCursor(-Infinity);
        break;

      case 'end':
        this._moveCursor(+Infinity);
        break;

      case 'up':
        this._historyPrev();
        break;

      case 'down':
        this._historyNext();
        break;

      case 'tab':
        // If tab completion enabled, do that...
        if (typeof this.completer === 'function' && this.isCompletionEnabled) {
          this._tabComplete();
          break;
        }
        // falls through

      default:
        if (s instanceof Buffer)
          s = s.toString('utf-8');

        if (s) {
          var lines = s.split(/\r\n|\n|\r/);
          for (var i = 0, len = lines.length; i < len; i++) {
            if (i > 0) {
              this._line();
            }
            this._insertString(lines[i]);
          }
        }
    }
  }
};


exports.Interface = Interface;


/**
 * accepts a readable Stream instance and makes it emit "keypress" events
 */

const KEYPRESS_DECODER = Symbol('keypress-decoder');
const ESCAPE_DECODER = Symbol('escape-decoder');

// GNU readline library - keyseq-timeout is 500ms (default)
const ESCAPE_CODE_TIMEOUT = 500;

function emitKeypressEvents(stream, iface) {
  if (stream[KEYPRESS_DECODER]) return;
  var StringDecoder = require('string_decoder').StringDecoder; // lazy load
  stream[KEYPRESS_DECODER] = new StringDecoder('utf8');

  stream[ESCAPE_DECODER] = emitKeys(stream);
  stream[ESCAPE_DECODER].next();

  const escapeCodeTimeout = () => stream[ESCAPE_DECODER].next('');
  let timeoutId;

  function onData(b) {
    if (stream.listenerCount('keypress') > 0) {
      var r = stream[KEYPRESS_DECODER].write(b);
      if (r) {
        clearTimeout(timeoutId);

        if (iface) {
          iface._sawKeyPress = r.length === 1;
        }

        for (var i = 0; i < r.length; i++) {
          if (r[i] === '\t' && typeof r[i + 1] === 'string' && iface) {
            iface.isCompletionEnabled = false;
          }

          try {
            stream[ESCAPE_DECODER].next(r[i]);
            // Escape letter at the tail position
            if (r[i] === '\x1b' && i + 1 === r.length) {
              timeoutId = setTimeout(escapeCodeTimeout, ESCAPE_CODE_TIMEOUT);
            }
          } catch (err) {
            // if the generator throws (it could happen in the `keypress`
            // event), we need to restart it.
            stream[ESCAPE_DECODER] = emitKeys(stream);
            stream[ESCAPE_DECODER].next();
            throw err;
          } finally {
            if (iface) {
              iface.isCompletionEnabled = true;
            }
          }
        }
      }
    } else {
      // Nobody's watching anyway
      stream.removeListener('data', onData);
      stream.on('newListener', onNewListener);
    }
  }

  function onNewListener(event) {
    if (event == 'keypress') {
      stream.on('data', onData);
      stream.removeListener('newListener', onNewListener);
    }
  }

  if (stream.listenerCount('keypress') > 0) {
    stream.on('data', onData);
  } else {
    stream.on('newListener', onNewListener);
  }
}
exports.emitKeypressEvents = emitKeypressEvents;


/**
 * moves the cursor to the x and y coordinate on the given stream
 */

function cursorTo(stream, x, y) {
  if (stream === null || stream === undefined)
    return;

  if (typeof x !== 'number' && typeof y !== 'number')
    return;

  if (typeof x !== 'number')
    throw new Error('Can\'t set cursor row without also setting it\'s column');

  if (typeof y !== 'number') {
    stream.write('\x1b[' + (x + 1) + 'G');
  } else {
    stream.write('\x1b[' + (y + 1) + ';' + (x + 1) + 'H');
  }
}
exports.cursorTo = cursorTo;


/**
 * moves the cursor relative to its current location
 */

function moveCursor(stream, dx, dy) {
  if (stream === null || stream === undefined)
    return;

  if (dx < 0) {
    stream.write('\x1b[' + (-dx) + 'D');
  } else if (dx > 0) {
    stream.write('\x1b[' + dx + 'C');
  }

  if (dy < 0) {
    stream.write('\x1b[' + (-dy) + 'A');
  } else if (dy > 0) {
    stream.write('\x1b[' + dy + 'B');
  }
}
exports.moveCursor = moveCursor;


/**
 * clears the current line the cursor is on:
 *   -1 for left of the cursor
 *   +1 for right of the cursor
 *    0 for the entire line
 */

function clearLine(stream, dir) {
  if (stream === null || stream === undefined)
    return;

  if (dir < 0) {
    // to the beginning
    stream.write('\x1b[1K');
  } else if (dir > 0) {
    // to the end
    stream.write('\x1b[0K');
  } else {
    // entire line
    stream.write('\x1b[2K');
  }
}
exports.clearLine = clearLine;


/**
 * clears the screen from the current position of the cursor down
 */

function clearScreenDown(stream) {
  if (stream === null || stream === undefined)
    return;

  stream.write('\x1b[0J');
}
exports.clearScreenDown = clearScreenDown;


/**
 * Returns the Unicode code point for the character at the
 * given index in the given string. Similar to String.charCodeAt(),
 * but this function handles surrogates (code point >= 0x10000).
 */

function codePointAt(str, index) {
  var code = str.charCodeAt(index);
  var low;
  if (0xd800 <= code && code <= 0xdbff) { // High surrogate
    low = str.charCodeAt(index + 1);
    if (!isNaN(low)) {
      code = 0x10000 + (code - 0xd800) * 0x400 + (low - 0xdc00);
    }
  }
  return code;
}
exports.codePointAt = internalUtil.deprecate(codePointAt,
    'readline.codePointAt is deprecated. ' +
    'Use String.prototype.codePointAt instead.');


exports.getStringWidth = internalUtil.deprecate(getStringWidth,
    'getStringWidth is deprecated and will be removed.');


exports.isFullWidthCodePoint = internalUtil.deprecate(isFullWidthCodePoint,
    'isFullWidthCodePoint is deprecated and will be removed.');


exports.stripVTControlCharacters = internalUtil.deprecate(
    stripVTControlCharacters,
    'stripVTControlCharacters is deprecated and will be removed.');
