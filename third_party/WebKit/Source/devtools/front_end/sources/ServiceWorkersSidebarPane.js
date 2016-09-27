// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @constructor
 * @extends {WebInspector.SidebarPane}
 * @implements {WebInspector.TargetManager.Observer}
 */
WebInspector.ServiceWorkersSidebarPane = function()
{
    WebInspector.SidebarPane.call(this, WebInspector.UIString("\u2699 Service Workers"));
    this.registerRequiredCSS("sources/serviceWorkersSidebar.css");
    this._bodyElement = this.element.createChild("div", "vbox");
    this.setVisible(false);

    /** @type {?WebInspector.ServiceWorkerManager} */
    this._manager = null;
    WebInspector.targetManager.observeTargets(this, WebInspector.Target.Type.Page);
    this._placeholderElement = createElementWithClass("div", "info");
    this._placeholderElement.textContent = WebInspector.UIString("No service workers control this page");
}

WebInspector.ServiceWorkersSidebarPane.prototype = {
    /**
     * @override
     * @param {!WebInspector.Target} target
     */
    targetAdded: function(target)
    {
        if (this._manager)
            return;
        this._manager = target.serviceWorkerManager;
        this._updateVisibility();
        target.serviceWorkerManager.addEventListener(WebInspector.ServiceWorkerManager.Events.WorkersUpdated, this._update, this);
    },

    /**
     * @override
     * @param {!WebInspector.Target} target
     */
    targetRemoved: function(target)
    {
        target.serviceWorkerManager.removeEventListener(WebInspector.ServiceWorkerManager.Events.WorkersUpdated, this._update, this);
        this._updateVisibility();
    },

    _update: function()
    {
        this._updateVisibility();
        this._bodyElement.removeChildren();

        if (!this.isShowing() || !this._manager)
            return;

        if (!this._manager.hasWorkers()) {
            this._bodyElement.appendChild(this._placeholderElement);
            return;
        }

        for (var worker of this._manager.workers()) {
            var workerElement = this._bodyElement.createChild("div", "service-worker");
            var leftBox = workerElement.createChild("div", "vbox flex-auto");
            leftBox.appendChild(WebInspector.linkifyURLAsNode(worker.url(), worker.name()));
            var scopeElement = leftBox.createChild("span", "service-worker-scope");
            scopeElement.textContent = worker.scope();
            scopeElement.title = worker.scope();
            workerElement.appendChild(createTextButton(WebInspector.UIString("Unregister"), worker.stop.bind(worker)));
        }
    },

    _updateVisibility: function()
    {
        this._wasVisibleAtLeastOnce = this._wasVisibleAtLeastOnce || !!this._manager && this._manager.hasWorkers();
        this.setVisible(this._wasVisibleAtLeastOnce);
    },

    wasShown: function()
    {
        this._update();
    },

    __proto__: WebInspector.SidebarPane.prototype
}
