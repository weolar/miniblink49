// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


/** @interface */
function InspectorFrontendHostAPI()
{
}

/** @typedef
{{
    type: string,
    id: (number|undefined),
    label: (string|undefined),
    enabled: (boolean|undefined),
    checked: (boolean|undefined),
    subItems: (!Array.<!InspectorFrontendHostAPI.ContextMenuDescriptor>|undefined)
}} */
InspectorFrontendHostAPI.ContextMenuDescriptor;

/** @typedef
{{
    statusCode: number,
    headers: (!Object.<string, string>|undefined)
}} */
InspectorFrontendHostAPI.LoadNetworkResourceResult;

InspectorFrontendHostAPI.Events = {
    AddExtensions: "addExtensions",
    AppendedToURL: "appendedToURL",
    CanceledSaveURL: "canceledSaveURL",
    ContextMenuCleared: "contextMenuCleared",
    ContextMenuItemSelected: "contextMenuItemSelected",
    DeviceCountUpdated: "deviceCountUpdated",
    DevicesUpdated: "devicesUpdated",
    DispatchMessage: "dispatchMessage",
    DispatchMessageChunk: "dispatchMessageChunk",
    EnterInspectElementMode: "enterInspectElementMode",
    FileSystemsLoaded: "fileSystemsLoaded",
    FileSystemRemoved: "fileSystemRemoved",
    FileSystemAdded: "fileSystemAdded",
    IndexingTotalWorkCalculated: "indexingTotalWorkCalculated",
    IndexingWorked: "indexingWorked",
    IndexingDone: "indexingDone",
    KeyEventUnhandled: "keyEventUnhandled",
    RevealSourceLine: "revealSourceLine",
    SavedURL: "savedURL",
    SearchCompleted: "searchCompleted",
    SetInspectedTabId: "setInspectedTabId",
    SetUseSoftMenu: "setUseSoftMenu",
    ShowConsole: "showConsole"
}

InspectorFrontendHostAPI.EventDescriptors = [
    [InspectorFrontendHostAPI.Events.AddExtensions, ["extensions"]],
    [InspectorFrontendHostAPI.Events.AppendedToURL, ["url"]],
    [InspectorFrontendHostAPI.Events.CanceledSaveURL, ["url"]],
    [InspectorFrontendHostAPI.Events.ContextMenuCleared, []],
    [InspectorFrontendHostAPI.Events.ContextMenuItemSelected, ["id"]],
    [InspectorFrontendHostAPI.Events.DeviceCountUpdated, ["count"]],
    [InspectorFrontendHostAPI.Events.DevicesUpdated, ["devices"]],
    [InspectorFrontendHostAPI.Events.DispatchMessage, ["messageObject"]],
    [InspectorFrontendHostAPI.Events.DispatchMessageChunk, ["messageChunk", "messageSize"]],
    [InspectorFrontendHostAPI.Events.EnterInspectElementMode, []],
    [InspectorFrontendHostAPI.Events.FileSystemsLoaded, ["fileSystems"]],
    [InspectorFrontendHostAPI.Events.FileSystemRemoved, ["fileSystemPath"]],
    [InspectorFrontendHostAPI.Events.FileSystemAdded, ["errorMessage", "fileSystem"]],
    [InspectorFrontendHostAPI.Events.IndexingTotalWorkCalculated, ["requestId", "fileSystemPath", "totalWork"]],
    [InspectorFrontendHostAPI.Events.IndexingWorked, ["requestId", "fileSystemPath", "worked"]],
    [InspectorFrontendHostAPI.Events.IndexingDone, ["requestId", "fileSystemPath"]],
    [InspectorFrontendHostAPI.Events.KeyEventUnhandled, ["event"]],
    [InspectorFrontendHostAPI.Events.RevealSourceLine, ["url", "lineNumber", "columnNumber"]],
    [InspectorFrontendHostAPI.Events.SavedURL, ["url"]],
    [InspectorFrontendHostAPI.Events.SearchCompleted, ["requestId", "fileSystemPath", "files"]],
    [InspectorFrontendHostAPI.Events.SetInspectedTabId, ["tabId"]],
    [InspectorFrontendHostAPI.Events.SetUseSoftMenu, ["useSoftMenu"]],
    [InspectorFrontendHostAPI.Events.ShowConsole, []]
];

InspectorFrontendHostAPI.prototype = {
    addFileSystem: function() { },

    /**
     * @param {string} url
     * @param {string} content
     */
    append: function(url, content) { },

    loadCompleted: function() { },

    /**
     * @param {number} requestId
     * @param {string} fileSystemPath
     */
    indexPath: function(requestId, fileSystemPath) { },

    /**
     * @return {string}
     */
    getSelectionBackgroundColor: function() { },

    /**
     * @return {string}
     */
    getSelectionForegroundColor: function() { },

    /**
     * Requests inspected page to be placed atop of the inspector frontend with specified bounds.
     * @param {{x: number, y: number, width: number, height: number}} bounds
     */
    setInspectedPageBounds: function(bounds) { },

    /**
     * @param {string} shortcuts
     */
    setWhitelistedShortcuts: function(shortcuts) { },

    inspectElementCompleted: function() { },

    /**
     * @param {string} url
     */
    openInNewTab: function(url) { },

    /**
     * @param {string} fileSystemPath
     */
    removeFileSystem: function(fileSystemPath) { },

    requestFileSystems: function() { },

    /**
     * @param {string} url
     * @param {string} content
     * @param {boolean} forceSaveAs
     */
    save: function(url, content, forceSaveAs) { },

    /**
     * @param {number} requestId
     * @param {string} fileSystemPath
     * @param {string} query
     */
    searchInPath: function(requestId, fileSystemPath, query) { },

    /**
     * @param {number} requestId
     */
    stopIndexing: function(requestId) { },

    bringToFront: function() { },

    closeWindow: function() { },

    copyText: function(text) { },

    /**
     * @param {string} url
     */
    inspectedURLChanged: function(url) { },

    /**
     * @param {string} fileSystemId
     * @param {string} registeredName
     * @return {?DOMFileSystem}
     */
    isolatedFileSystem: function(fileSystemId, registeredName) { },

    /**
     * @param {string} url
     * @param {string} headers
     * @param {number} streamId
     * @param {function(!InspectorFrontendHostAPI.LoadNetworkResourceResult)} callback
     */
    loadNetworkResource: function(url, headers, streamId, callback) { },

    /**
     * @param {function(!Object<string, string>)} callback
     */
    getPreferences: function(callback) { },

    /**
     * @param {string} name
     * @param {string} value
     */
    setPreference: function(name, value) { },

    /**
     * @param {string} name
     */
    removePreference: function(name) { },

    clearPreferences: function() { },

    /**
     * @param {!FileSystem} fileSystem
     */
    upgradeDraggedFileSystemPermissions: function(fileSystem) { },

    /**
     * @return {string}
     */
    platform: function() { },

    /**
     * @param {string} actionName
     * @param {number} actionCode
     * @param {number} bucketSize
     */
    recordEnumeratedHistogram: function(actionName, actionCode, bucketSize) { },

    /**
     * @param {string} message
     */
    sendMessageToBackend: function(message) { },

    /**
     * @param {boolean} enabled
     */
    setDevicesUpdatesEnabled: function(enabled) { },

    /**
     * @param {string} origin
     * @param {string} script
     */
    setInjectedScriptForOrigin: function(origin, script) { },

    /**
     * @param {boolean} isDocked
     * @param {function()} callback
     */
    setIsDocked: function(isDocked, callback) { },

    /* qb_begin: aero effect */
    /**
     ** @param {string} dockSide
     * **/
    setDockSide: function (dockSide) { },
    /* qb_end */

    /* qb_begin: aero effect */
    /**
      * @param {bool} emulationEnabled
      */
    setEmulationEnabled: function (emulationEnabled) { },
    /* qb_end */

    /**
     * @return {number}
     */
    zoomFactor: function() { },

    zoomIn: function() { },

    zoomOut: function() { },

    resetZoom: function() { },

    /**
     * @param {number} x
     * @param {number} y
     * @param {!Array.<!InspectorFrontendHostAPI.ContextMenuDescriptor>} items
     * @param {!Document} document
     */
    showContextMenuAtPoint: function(x, y, items, document) { },

    /**
     * @return {boolean}
     */
    isUnderTest: function() { },

    /**
     * @return {boolean}
     */
    isHostedMode: function() { }
}
