/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @constructor
 * @extends {WebInspector.ThrottledWidget}
 */
WebInspector.PropertiesWidget = function()
{
    WebInspector.ThrottledWidget.call(this);

    WebInspector.targetManager.addModelListener(WebInspector.DOMModel, WebInspector.DOMModel.Events.AttrModified, this._onNodeChange, this);
    WebInspector.targetManager.addModelListener(WebInspector.DOMModel, WebInspector.DOMModel.Events.AttrRemoved, this._onNodeChange, this);
    WebInspector.targetManager.addModelListener(WebInspector.DOMModel, WebInspector.DOMModel.Events.CharacterDataModified, this._onNodeChange, this);
    WebInspector.targetManager.addModelListener(WebInspector.DOMModel, WebInspector.DOMModel.Events.ChildNodeCountUpdated, this._onNodeChange, this);
    WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode, this._setNode, this);
}

/**
 * @return {!WebInspector.ElementsSidebarViewWrapperPane}
 */
WebInspector.PropertiesWidget.createSidebarWrapper = function()
{
    return new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Properties"), new WebInspector.PropertiesWidget());
}

WebInspector.PropertiesWidget._objectGroupName = "properties-sidebar-pane";

WebInspector.PropertiesWidget.prototype = {
    /**
     * @param {!WebInspector.Event} event
     */
    _setNode: function(event)
    {
        this._node = /** @type {?WebInspector.DOMNode} */(event.data);
        this.update();
    },

    /**
     * @override
     * @param {!WebInspector.Throttler.FinishCallback} finishCallback
     * @protected
     */
    doUpdate: function(finishCallback)
    {
        if (this._lastRequestedNode) {
            this._lastRequestedNode.target().runtimeAgent().releaseObjectGroup(WebInspector.PropertiesWidget._objectGroupName);
            delete this._lastRequestedNode;
        }

        if (!this._node) {
            this.element.removeChildren();
            this.sections = [];
            finishCallback();
            return;
        }

        this._lastRequestedNode = this._node;
        this._node.resolveToObject(WebInspector.PropertiesWidget._objectGroupName, nodeResolved.bind(this));

        /**
         * @param {?WebInspector.RemoteObject} object
         * @this {WebInspector.PropertiesWidget}
         */
        function nodeResolved(object)
        {
            if (!object) {
                finishCallback();
                return;
            }

            /**
             * @suppressReceiverCheck
             * @this {*}
             */
            function protoList()
            {
                var proto = this;
                var result = { __proto__: null };
                var counter = 1;
                while (proto) {
                    result[counter++] = proto;
                    proto = proto.__proto__;
                }
                return result;
            }
            object.callFunction(protoList, undefined, nodePrototypesReady.bind(this));
            object.release();
        }

        /**
         * @param {?WebInspector.RemoteObject} object
         * @param {boolean=} wasThrown
         * @this {WebInspector.PropertiesWidget}
         */
        function nodePrototypesReady(object, wasThrown)
        {
            if (!object || wasThrown) {
                finishCallback();
                return;
            }
            object.getOwnProperties(fillSection.bind(this));
            object.release();
        }

        /**
         * @param {?Array.<!WebInspector.RemoteObjectProperty>} prototypes
         * @this {WebInspector.PropertiesWidget}
         */
        function fillSection(prototypes)
        {
            if (!prototypes) {
                finishCallback();
                return;
            }

            var expanded = [];
            var sections = this.sections || [];
            for (var i = 0; i < sections.length; ++i)
                expanded.push(sections[i].expanded);

            this.element.removeChildren();
            this.sections = [];

            // Get array of prototype user-friendly names.
            for (var i = 0; i < prototypes.length; ++i) {
                if (!parseInt(prototypes[i].name, 10))
                    continue;
                var prototype = prototypes[i].value;
                var title = prototype.description;
                title = title.replace(/Prototype$/, "");
                var section = new WebInspector.ObjectPropertiesSection(prototype, title);
                section.element.classList.add("properties-widget-section");
                this.sections.push(section);
                this.element.appendChild(section.element);
                if (expanded[this.sections.length - 1])
                    section.expand();
            }

            finishCallback();
        }
    },

    /**
     * @param {!WebInspector.Event} event
     */
    _onNodeChange: function(event)
    {
        if (!this._node)
            return;
        var data = event.data;
        var node = /** @type {!WebInspector.DOMNode} */ (data instanceof WebInspector.DOMNode ? data : data.node);
        if (this._node !== node)
            return;
        this.update();
    },

    __proto__: WebInspector.ThrottledWidget.prototype
}
