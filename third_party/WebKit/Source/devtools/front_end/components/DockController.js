/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @constructor
 * @extends {WebInspector.Object}
 * @param {boolean} canDock
 */
WebInspector.DockController = function(canDock)
{
    this._canDock = canDock;

    this._closeButton = new WebInspector.ToolbarButton(WebInspector.UIString("Close"), "delete-toolbar-item");
    this._closeButton.addEventListener("click", InspectorFrontendHost.closeWindow.bind(InspectorFrontendHost));

    if (!canDock) {
        this._dockSide = WebInspector.DockController.State.Undocked;
        this._updateUI();
        return;
    }

    this._currentDockStateSetting = WebInspector.settings.createSetting("currentDockState", "");
    this._lastDockStateSetting = WebInspector.settings.createSetting("lastDockState", "");

    /** @type {!WebInspector.ToolbarStatesSettingButton|undefined} */
    this._dockToggleButton;
}

WebInspector.DockController.State = {
    DockedToBottom: "bottom",
    DockedToRight: "right",
    Undocked: "undocked"
}

// Use BeforeDockSideChanged to do something before all the UI bits are updated,
// DockSideChanged to update UI, and AfterDockSideChanged to perform actions
// after frontend is docked/undocked in the browser.
WebInspector.DockController.Events = {
    BeforeDockSideChanged: "BeforeDockSideChanged",
    DockSideChanged: "DockSideChanged",
    AfterDockSideChanged: "AfterDockSideChanged"
}

WebInspector.DockController.prototype = {
    initialize: function()
    {
        if (!this._canDock)
            return;

        this._states = [WebInspector.DockController.State.DockedToRight, WebInspector.DockController.State.DockedToBottom, WebInspector.DockController.State.Undocked];
        this._titles = [WebInspector.UIString("Dock to right"), WebInspector.UIString("Dock to bottom"), WebInspector.UIString("Undock into separate window")];
        var initialState = this._currentDockStateSetting.get();
        initialState = this._states.indexOf(initialState) >= 0 ? initialState : this._states[0];
        this._dockSideChanged(initialState);
    },

    /**
     * @return {string}
     */
    dockSide: function()
    {
        return this._dockSide;
    },

    /**
     * @return {boolean}
     */
    canDock: function()
    {
        return this._canDock;
    },

    /**
     * @return {boolean}
     */
    isVertical: function()
    {
        return this._dockSide === WebInspector.DockController.State.DockedToRight;
    },

    /**
     * @param {string} dockSide
     */
    _dockSideChanged: function(dockSide)
    {
        if (this._dockSide === dockSide)
            return;

        if (this._dockToggleButton)
            this._dockToggleButton.setEnabled(false);
        var eventData = { from: this._dockSide, to: dockSide };
        this.dispatchEventToListeners(WebInspector.DockController.Events.BeforeDockSideChanged, eventData);
        console.timeStamp("DockController.setIsDocked");
        InspectorFrontendHost.setIsDocked(dockSide !== WebInspector.DockController.State.Undocked, this._setIsDockedResponse.bind(this, eventData));
        this._dockSide = dockSide;
        /* qb_begin: aero effect */
        InspectorFrontendHost.setDockSide(dockSide);
        /* qb_end */
        this._updateUI();
        this.dispatchEventToListeners(WebInspector.DockController.Events.DockSideChanged, eventData);
    },

    /**
     * @param {{from: string, to: string}} eventData
     */
    _setIsDockedResponse: function(eventData)
    {
        this.dispatchEventToListeners(WebInspector.DockController.Events.AfterDockSideChanged, eventData);
        if (this._dockToggleButton)
            this._dockToggleButton.setEnabled(true);
    },

    /**
     * @suppressGlobalPropertiesCheck
     */
    _updateUI: function()
    {
        var body = document.body;  // Only for main window.
        switch (this._dockSide) {
        case WebInspector.DockController.State.DockedToBottom:
            body.classList.remove("undocked");
            body.classList.remove("dock-to-right");
            body.classList.add("dock-to-bottom");
            break;
        case WebInspector.DockController.State.DockedToRight:
            body.classList.remove("undocked");
            body.classList.add("dock-to-right");
            body.classList.remove("dock-to-bottom");
            break;
        case WebInspector.DockController.State.Undocked:
            body.classList.add("undocked");
            body.classList.remove("dock-to-right");
            body.classList.remove("dock-to-bottom");
            break;
        }
        this._closeButton.setVisible(this._dockSide !== WebInspector.DockController.State.Undocked);
    },

    __proto__: WebInspector.Object.prototype
}

/**
 * @constructor
 * @implements {WebInspector.ToolbarItem.Provider}
 */
WebInspector.DockController.ButtonProvider = function()
{
}

WebInspector.DockController.ButtonProvider.prototype = {
    /**
     * @override
     * @return {?WebInspector.ToolbarItem}
     */
    item: function()
    {
        if (!WebInspector.dockController.canDock())
            return null;

        if (!WebInspector.dockController._dockToggleButton) {
            WebInspector.dockController._dockToggleButton = new WebInspector.ToolbarStatesSettingButton(
                    "dock-toolbar-item",
                    WebInspector.dockController._states,
                    WebInspector.dockController._titles,
                    WebInspector.dockController.dockSide(),
                    WebInspector.dockController._currentDockStateSetting,
                    WebInspector.dockController._lastDockStateSetting,
                    WebInspector.dockController._dockSideChanged.bind(WebInspector.dockController));
        }
        return WebInspector.dockController._dockToggleButton;
    }
}

/**
 * @constructor
 * @implements {WebInspector.ActionDelegate}
 */
WebInspector.DockController.ToggleDockActionDelegate = function()
{
}

WebInspector.DockController.ToggleDockActionDelegate.prototype = {
    /**
     * @override
     * @param {!WebInspector.Context} context
     * @param {string} actionId
     */
    handleAction: function(context, actionId)
    {
        var toggleButton = new WebInspector.DockController.ButtonProvider().item();
        if (!toggleButton || !toggleButton.enabled())
            return;
        /** @type {!WebInspector.ToolbarStatesSettingButton} */ (toggleButton).toggle();
    }
}

/**
 * @constructor
 * @implements {WebInspector.ToolbarItem.Provider}
 */
WebInspector.DockController.CloseButtonProvider = function()
{
}

WebInspector.DockController.CloseButtonProvider.prototype = {
    /**
     * @override
     * @return {?WebInspector.ToolbarItem}
     */
    item: function()
    {
        return WebInspector.dockController._closeButton;
    }
}

/**
 * @type {!WebInspector.DockController}
 */
WebInspector.dockController;
