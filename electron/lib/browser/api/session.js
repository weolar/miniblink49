const EventEmitter = require('events').EventEmitter;
const sessionBinding = process.binding('atom_browser_session');
Session = sessionBinding.Session;

Object.setPrototypeOf(Session.prototype, EventEmitter.prototype); 

//--
const webRequestBinding = process.binding('atom_browser_webrequest');
Session.defaultSession = Session.fromPartition("");
//--
const downloaditemBinding = process.binding('atom_browser_downloaditem');
DownloadItem = downloaditemBinding.DownloadItem;
Object.setPrototypeOf(DownloadItem.prototype, EventEmitter.prototype); 
//--

exports.session = Session;