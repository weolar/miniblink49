// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


/**
 * @constructor
 * @extends {WebInspector.SDKModel}
 * @param {!WebInspector.Target} target
 */
WebInspector.AccessibilityModel = function(target)
{
    WebInspector.SDKModel.call(this, WebInspector.AccessibilityModel, target);
    this._agent = target.accessibilityAgent();
};

WebInspector.AccessibilityModel.prototype = {
    /**
     * @param {!DOMAgent.NodeId} nodeId
     * @param {function(?AccessibilityAgent.AXNode)} userCallback
     */
    getAXNode: function(nodeId, userCallback)
    {
        var wrappedCallback = InspectorBackend.wrapClientCallback(userCallback, "AccessibilityAgent.getAXNode(): ");
        this._agent.getAXNode(nodeId, wrappedCallback);
    },

    __proto__: WebInspector.SDKModel.prototype
}

WebInspector.AccessibilityModel._symbol = Symbol("AccessibilityModel");
/**
 * @param {!WebInspector.Target} target
 * @return {!WebInspector.AccessibilityModel}
 */
WebInspector.AccessibilityModel.fromTarget = function(target)
{
    if (!target[WebInspector.AccessibilityModel._symbol])
        target[WebInspector.AccessibilityModel._symbol] = new WebInspector.AccessibilityModel(target);

    return target[WebInspector.AccessibilityModel._symbol];
}