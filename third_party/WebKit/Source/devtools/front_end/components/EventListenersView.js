// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @constructor
 * @param {!Element} element
 */
WebInspector.EventListenersView = function(element)
{
    this._element = element;
    this._treeOutline = new TreeOutlineInShadow("event-listener-tree");
    this._treeOutline.registerRequiredCSS("components/objectValue.css");
    this._treeOutline.registerRequiredCSS("components/eventListenersView.css");
    this._treeOutline.setComparator(WebInspector.EventListenersTreeElement.comparator);
    this._treeOutline.element.classList.add("monospace");
    this._element.appendChild(this._treeOutline.element)
    this._emptyHolder = createElementWithClass("div", "info");
    this._emptyHolder.textContent = WebInspector.UIString("No Event Listeners");
    this._linkifier = new WebInspector.Linkifier();
    /** @type {!Map<string, !WebInspector.EventListenersTreeElement>} */
    this._treeItemMap = new Map();
}

WebInspector.EventListenersView.prototype = {
    /**
     * @param {!Array<!WebInspector.RemoteObject>} objects
     * @return {!Promise<undefined>}
     */
    addObjects: function(objects)
    {
        var promises = [];
        for (var i = 0; i < objects.length; ++i)
            promises.push(objects[i].eventListeners());
        return Promise.all(promises).then(listenersCallback.bind(this));
        /**
         * @param {!Array<?Array<!WebInspector.EventListener>>} listeners
         * @this {WebInspector.EventListenersView}
         */
        function listenersCallback(listeners)
        {
            this.reset();
            for (var i = 0; i < listeners.length; ++i)
                this._addObjectEventListeners(objects[i], listeners[i]);
            this.addEmptyHolderIfNeeded();
            this._eventListenersArrivedForTest();
        }
    },

    /**
     * @param {!WebInspector.RemoteObject} object
     * @param {?Array<!WebInspector.EventListener>} eventListeners
     */
    _addObjectEventListeners: function(object, eventListeners)
    {
        if (!eventListeners)
            return;
        for (var eventListener of eventListeners) {
            var treeItem = this._getOrCreateTreeElementForType(eventListener.type());
            treeItem.addObjectEventListener(eventListener, object);
        }
    },

    /**
     * @param {string} type
     * @return {!WebInspector.EventListenersTreeElement}
     */
    _getOrCreateTreeElementForType: function(type)
    {
        var treeItem = this._treeItemMap.get(type);
        if (!treeItem) {
            treeItem = new WebInspector.EventListenersTreeElement(type, this._linkifier);
            this._treeItemMap.set(type, treeItem);
            this._treeOutline.appendChild(treeItem);
            this._emptyHolder.remove();
        }
        return treeItem;
    },

    addEmptyHolderIfNeeded: function()
    {
        if (!this._treeOutline.firstChild() && !this._emptyHolder.parentNode)
           this._element.appendChild(this._emptyHolder);
    },

    reset: function()
    {
        this._treeItemMap = new Map();
        this._treeOutline.removeChildren();
        this._linkifier.reset();
    },

    _eventListenersArrivedForTest: function()
    {
    }
}

/**
 * @constructor
 * @extends {TreeElement}
 * @param {string} type
 * @param {!WebInspector.Linkifier} linkifier
 */
WebInspector.EventListenersTreeElement = function(type, linkifier)
{
    TreeElement.call(this, type);
    this.toggleOnClick = true;
    this.selectable = false;
    this._linkifier = linkifier;
}

/**
 * @param {!TreeElement} element1
 * @param {!TreeElement} element2
 * @return {number}
 */
WebInspector.EventListenersTreeElement.comparator = function(element1, element2) {
    if (element1.title === element2.title)
        return 0;
    return element1.title > element2.title ? 1 : -1;
}

WebInspector.EventListenersTreeElement.prototype = {
    /**
     * @param {!WebInspector.EventListener} eventListener
     * @param {!WebInspector.RemoteObject} object
     */
    addObjectEventListener: function(eventListener, object)
    {
        var treeElement = new WebInspector.ObjectEventListenerBar(eventListener, object, this._linkifier);
        this.appendChild(/** @type {!TreeElement} */ (treeElement));
    },

    __proto__: TreeElement.prototype
}

/**
 * @constructor
 * @extends {TreeElement}
 * @param {!WebInspector.EventListener} eventListener
 * @param {!WebInspector.RemoteObject} object
 * @param {!WebInspector.Linkifier} linkifier
 */
WebInspector.ObjectEventListenerBar = function(eventListener, object, linkifier)
{
    TreeElement.call(this, "", true);
    this._eventListener = eventListener;
    this.editable = false;
    this.selectable = false;
    this._setTitle(object, linkifier);
}

WebInspector.ObjectEventListenerBar.prototype = {
    onpopulate: function()
    {
        var properties = [];
        var eventListener = this._eventListener;
        var runtimeModel = eventListener.target().runtimeModel;
        properties.push(runtimeModel.createRemotePropertyFromPrimitiveValue("useCapture", eventListener.useCapture()));
        if (typeof eventListener.handler() !== "undefined")
            properties.push(new WebInspector.RemoteObjectProperty("handler", eventListener.handler()));
        WebInspector.ObjectPropertyTreeElement.populateWithProperties(this, properties, [], true, null);
    },

    /**
     * @param {!WebInspector.RemoteObject} object
     * @param {!WebInspector.Linkifier} linkifier
     */
    _setTitle: function(object, linkifier)
    {
        var title = this.listItemElement.createChild("span");
        var subtitle = this.listItemElement.createChild("span", "event-listener-tree-subtitle");
        subtitle.appendChild(linkifier.linkifyRawLocation(this._eventListener.location(), this._eventListener.sourceName()));
        title.appendChild(WebInspector.ObjectPropertiesSection.createValueElement(object, false));
    },

    __proto__: TreeElement.prototype
}