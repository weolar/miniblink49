// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @constructor
 * @extends {WebInspector.Widget}
 */
WebInspector.ThrottledWidget = function()
{
    WebInspector.Widget.call(this);
    this._updateThrottler = new WebInspector.Throttler(100);
    this._updateWhenVisible = false;
}

WebInspector.ThrottledWidget.prototype = {
    /**
     * @param {!WebInspector.Throttler.FinishCallback} finishedCallback
     * @protected
     */
    doUpdate: function(finishedCallback)
    {
        finishedCallback();
    },

    update: function()
    {
        this._updateWhenVisible = !this.isShowing();
        if (this._updateWhenVisible)
            return;
        this._updateThrottler.schedule(innerUpdate.bind(this));

        /**
         * @param {!WebInspector.Throttler.FinishCallback} finishedCallback
         * @this {WebInspector.ThrottledWidget}
         */
        function innerUpdate(finishedCallback)
        {
            if (this.isShowing()) {
                this.doUpdate(finishedCallback);
            } else {
                this._updateWhenVisible = true;
                finishedCallback();
            }
        }
    },

    /**
     * @override
     */
    wasShown: function()
    {
        WebInspector.Widget.prototype.wasShown.call(this);
        if (this._updateWhenVisible)
            this.update();
    },

    __proto__: WebInspector.Widget.prototype
}
