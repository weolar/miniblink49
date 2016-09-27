/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2009 Joseph Pecoraro
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
 * @param {!WebInspector.StylesSidebarPane} stylesSidebarPane
 * @param {!WebInspector.SharedSidebarModel} sharedModel
 * @extends {WebInspector.ThrottledWidget}
 */
WebInspector.ComputedStyleWidget = function(stylesSidebarPane, sharedModel)
{
    WebInspector.ThrottledWidget.call(this);
    this.element.classList.add("computed-style-sidebar-pane");

    this.registerRequiredCSS("elements/computedStyleSidebarPane.css");
    this._alwaysShowComputedProperties = { "display": true, "height": true, "width": true };

    this._sharedModel = sharedModel;
    this._sharedModel.addEventListener(WebInspector.SharedSidebarModel.Events.ComputedStyleChanged, this.update, this);

    this._showInheritedComputedStylePropertiesSetting = WebInspector.settings.createSetting("showInheritedComputedStyleProperties", false);
    this._showInheritedComputedStylePropertiesSetting.addChangeListener(this._showInheritedComputedStyleChanged.bind(this));

    var hbox = this.element.createChild("div", "hbox styles-sidebar-pane-toolbar");
    var filterContainerElement = hbox.createChild("div", "styles-sidebar-pane-filter-box");
    var filterInput = WebInspector.StylesSidebarPane.createPropertyFilterElement(WebInspector.UIString("Filter"), hbox, filterCallback.bind(this));
    filterContainerElement.appendChild(filterInput);

    var toolbar = new WebInspector.Toolbar(hbox);
    toolbar.element.classList.add("styles-pane-toolbar");
    toolbar.appendToolbarItem(new WebInspector.ToolbarCheckbox(WebInspector.UIString("Show inherited"),
                                                               WebInspector.UIString("Show inherited properties"),
                                                               this._showInheritedComputedStylePropertiesSetting));

    this._propertiesContainer = this.element.createChild("div", "monospace");
    this._propertiesContainer.classList.add("computed-properties");
    this._onTracePropertyBound = this._onTraceProperty.bind(this);

    this._stylesSidebarPane = stylesSidebarPane;

    /**
     * @param {?RegExp} regex
     * @this {WebInspector.ComputedStyleWidget}
     */
    function filterCallback(regex)
    {
        this._filterRegex = regex;
        this._updateFilter(regex);
    }
}

/**
 * @param {!WebInspector.StylesSidebarPane} stylesSidebarPane
 * @param {!WebInspector.SharedSidebarModel} sharedModel
 * @return {!WebInspector.ElementsSidebarViewWrapperPane}
 */
WebInspector.ComputedStyleWidget.createSidebarWrapper = function(stylesSidebarPane, sharedModel)
{
    var widget = new WebInspector.ComputedStyleWidget(stylesSidebarPane, sharedModel);
    return new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Computed Style"), widget)
}

WebInspector.ComputedStyleWidget._propertySymbol = Symbol("property");

WebInspector.ComputedStyleWidget.prototype = {
    /**
     * @param {!Event} event
     */
    _onTraceProperty: function(event)
    {
        var item = event.target.enclosingNodeOrSelfWithClass("computed-style-property");
        var property = item && item[WebInspector.ComputedStyleWidget._propertySymbol];
        if (!property)
            return;
        this._stylesSidebarPane.tracePropertyName(property.name);
    },

    _showInheritedComputedStyleChanged: function()
    {
        this.update();
    },

    /**
     * @override
     * @param {!WebInspector.Throttler.FinishCallback} finishedCallback
     */
    doUpdate: function(finishedCallback)
    {
        var promises = [
            this._sharedModel.fetchComputedStyle(),
            this._stylesSidebarPane.fetchMatchedCascade()
        ];
        Promise.all(promises)
            .spread(this._innerRebuildUpdate.bind(this))
            .then(finishedCallback);
    },

    /**
     * @param {string} text
     * @return {!Node}
     */
    _processColor: function(text)
    {
        var color = WebInspector.Color.parse(text);
        if (!color)
            return createTextNode(text);
        var swatch = WebInspector.ColorSwatch.create();
        swatch.setColorText(text);
        return swatch;
    },

    /**
     * @param {?WebInspector.SharedSidebarModel.ComputedStyle} nodeStyle
       @param {?{matched: !WebInspector.SectionCascade, pseudo: !Map.<number, !WebInspector.SectionCascade>}} cascades
     */
    _innerRebuildUpdate: function(nodeStyle, cascades)
    {
        this._propertiesContainer.removeChildren();
        if (!nodeStyle || !cascades)
            return;

        var uniqueProperties = nodeStyle.computedStyle.allProperties.slice();
        uniqueProperties.sort(propertySorter);

        var showInherited = this._showInheritedComputedStylePropertiesSetting.get();
        for (var i = 0; i < uniqueProperties.length; ++i) {
            var property = uniqueProperties[i];
            var inherited = this._isPropertyInherited(cascades.matched, property.name);
            if (!showInherited && inherited && !(property.name in this._alwaysShowComputedProperties))
                continue;
            var canonicalName = WebInspector.CSSMetadata.canonicalPropertyName(property.name);
            if (property.name !== canonicalName && property.value === nodeStyle.computedStyle.getPropertyValue(canonicalName))
                continue;
            var item = this._propertiesContainer.createChild("div", "computed-style-property");
            item[WebInspector.ComputedStyleWidget._propertySymbol] = property;
            item.classList.toggle("computed-style-property-inherited", inherited);
            var renderer = new WebInspector.StylesSidebarPropertyRenderer(null, nodeStyle.node, property.name, property.value);
            renderer.setColorHandler(this._processColor.bind(this));

            if (!inherited) {
                var traceButton = item.createChild("div", "computed-style-trace-button");
                traceButton.createChild("div", "glyph");
                traceButton.addEventListener("click", this._onTracePropertyBound, false);
            }
            item.appendChild(renderer.renderName());
            item.appendChild(createTextNode(": "));
            item.appendChild(renderer.renderValue());
            item.appendChild(createTextNode(";"));
            this._propertiesContainer.appendChild(item);
        }

        this._updateFilter(this._filterRegex);

        /**
         * @param {!WebInspector.CSSProperty} a
         * @param {!WebInspector.CSSProperty} b
         */
        function propertySorter(a, b)
        {
            var canonicalName = WebInspector.CSSMetadata.canonicalPropertyName;
            return canonicalName(a.name).compareTo(canonicalName(b.name));
        }
    },

    /**
     * @param {!WebInspector.SectionCascade} matchedCascade
     * @param {string} propertyName
     */
    _isPropertyInherited: function(matchedCascade, propertyName)
    {
        var canonicalName = WebInspector.CSSMetadata.canonicalPropertyName(propertyName);
        return !matchedCascade.allUsedProperties().has(canonicalName);
    },

    /**
     * @param {?RegExp} regex
     */
    _updateFilter: function(regex)
    {
        for (var i = 0; i < this._propertiesContainer.children.length; ++i) {
            var item = this._propertiesContainer.children[i];
            var property = item[WebInspector.ComputedStyleWidget._propertySymbol];
            var matched = !regex || regex.test(property.name) || regex.test(property.value);
            item.classList.toggle("hidden", !matched);
        }
    },

    __proto__: WebInspector.ThrottledWidget.prototype
}
