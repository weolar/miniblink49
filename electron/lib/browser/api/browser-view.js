
const binding = process.binding('atom_browser_browserview');
const BrowserView = binding.BrowserView;

Object.defineProperty(BrowserView.prototype, "webContents", {
    get: function () {
        var webContents =  this._getWebContents();
        webContents._init();
        return webContents;
    },
    configurable : true
});

BrowserView.prototype.m_bounds = {
};
BrowserView.prototype.getBounds = function() {
    return this.m_bounds;
};

BrowserView.prototype.setBackgroundColor = function(bounds) {
}

BrowserView.prototype.setAutoResize = function(options) {
}

BrowserView.prototype.setBounds = function(bounds) {
    this.m_bounds = bounds;
    var x = 0;
    var y = 0;
    var w = 1;
    var h = 1;
    if ("x" in bounds)
        x = bounds.x;
    if ("y" in bounds)
        y = bounds.y;
    if ("width" in bounds)
        w = bounds.width;
    if ("height" in bounds)
        h = bounds.height;
    this._setBounds(x, y, w, h);
}


module.exports = BrowserView;