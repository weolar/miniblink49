const hasCrypto = (process.versions.openssl != '0');

const http = require('http');
const https = hasCrypto ? require('https') : null;

const URL = require('url').URL;

function ClientRequest(options) {
	this.req = null;
	if ("string" == typeof options) {
		const urlObj = new URL('https://example.org');
		if (hasCrypto && "https:" == urlObj.protocol)
			this.req = https.request(options);
	} else if (hasCrypto && "https:" == options.protocol) {
		this.req = https.request(options);
	}
	if (!this.req)
		this.req = http.request(options);
}

ClientRequest.prototype.on = function(evt, callback) {
	this.req.on(evt, callback);
}

ClientRequest.prototype.chunkedEncoding = false;

ClientRequest.prototype.setHeader = function(name, value) {
	this.req.setHeader(name, value);
}

ClientRequest.prototype.getHeader = function(name) {
	return this.req.getHeader(name);
}

ClientRequest.prototype.removeHeader = function(name) {
	this.req.removeHeader(name);
}

ClientRequest.prototype.write = function(chunk, encoding, callback) {
	this.req.write(chunk, encoding, callback);
}

ClientRequest.prototype.end = function(chunk, encoding, callback) {
	this.req.end(chunk, encoding, callback);
}

ClientRequest.prototype.abort = function() {
	this.req.abort();
}

ClientRequest.prototype.followRedirect = function() {
	;
}

function Net() {
}

Net.request = function(options) {
	return new ClientRequest(options);
}

exports.net = Net;

