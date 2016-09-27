/*
 * Copyright (C) 2011 Brian Grinstead All rights reserved.
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
 * @extends {WebInspector.VBox}
 */
WebInspector.Spectrum = function()
{
    /**
     * @param {!Element} parentElement
     */
    function appendSwitcherIcon(parentElement)
    {
        var icon = parentElement.createSVGChild("svg");
        icon.setAttribute("height", 16);
        icon.setAttribute("width", 16);
        var path = icon.createSVGChild("path");
        path.setAttribute("d", "M5,6 L11,6 L8,2 Z M5,10 L11,10 L8,14 Z");
        return icon;
    }

    WebInspector.VBox.call(this, true);
    this.registerRequiredCSS("elements/spectrum.css");
    this.contentElement.tabIndex = 0;

    this._colorElement = this.contentElement.createChild("div", "spectrum-color");
    this._colorDragElement = this._colorElement.createChild("div", "spectrum-sat fill").createChild("div", "spectrum-val fill").createChild("div", "spectrum-dragger");

    var toolbar = new WebInspector.Toolbar(this.contentElement);
    toolbar.element.classList.add("spectrum-eye-dropper");
    this._colorPickerButton = new WebInspector.ToolbarButton(WebInspector.UIString("Toggle color picker"), "eyedropper-toolbar-item");
    this._colorPickerButton.setToggled(true);
    this._colorPickerButton.addEventListener("click", this._toggleColorPicker.bind(this, undefined));
    toolbar.appendToolbarItem(this._colorPickerButton);

    var swatchElement = this.contentElement.createChild("span", "swatch");
    this._swatchInnerElement = swatchElement.createChild("span", "swatch-inner");

    this._hueElement = this.contentElement.createChild("div", "spectrum-hue");
    this._hueSlider = this._hueElement.createChild("div", "spectrum-slider");
    this._alphaElement = this.contentElement.createChild("div", "spectrum-alpha");
    this._alphaElementBackground = this._alphaElement.createChild("div", "spectrum-alpha-background");
    this._alphaSlider = this._alphaElement.createChild("div", "spectrum-slider");

    var displaySwitcher = this.contentElement.createChild("div", "spectrum-display-switcher");
    appendSwitcherIcon(displaySwitcher);
    displaySwitcher.addEventListener("click", this._formatViewSwitch.bind(this));

    // RGBA/HSLA display.
    this._displayContainer = this.contentElement.createChild("div", "spectrum-text source-code");
    this._textValues = [];
    for (var i = 0; i < 4; ++i) {
        var inputValue = this._displayContainer.createChild("input", "spectrum-text-value");
        inputValue.maxLength = 4;
        this._textValues.push(inputValue);
        inputValue.addEventListener("keydown", this._inputChanged.bind(this), false);
        inputValue.addEventListener("input", this._inputChanged.bind(this), false);
        inputValue.addEventListener("mousewheel", this._inputChanged.bind(this), false);
    }

    this._textLabels = this._displayContainer.createChild("div", "spectrum-text-label");

    // HEX display.
    this._hexContainer = this.contentElement.createChild("div", "spectrum-text spectrum-text-hex source-code");
    this._hexValue = this._hexContainer.createChild("input", "spectrum-text-value");
    this._hexValue.maxLength = 7;
    this._hexValue.addEventListener("keydown", this._inputChanged.bind(this), false);
    this._hexValue.addEventListener("mousewheel", this._inputChanged.bind(this), false);

    var label = this._hexContainer.createChild("div", "spectrum-text-label");
    label.textContent = "HEX";

    WebInspector.installDragHandle(this._hueElement, dragStart.bind(this, positionHue.bind(this)), positionHue.bind(this), null, "default");
    WebInspector.installDragHandle(this._alphaElement, dragStart.bind(this, positionAlpha.bind(this)), positionAlpha.bind(this), null, "default");
    WebInspector.installDragHandle(this._colorElement, dragStart.bind(this, positionColor.bind(this)), positionColor.bind(this), null, "default");

    /**
     * @param {function(!Event)} callback
     * @param {!Event} event
     * @return {boolean}
     * @this {WebInspector.Spectrum}
     */
    function dragStart(callback, event)
    {
        this._hueAlphaLeft = this._hueElement.totalOffsetLeft();
        this._colorOffset = this._colorElement.totalOffset();
        callback(event);
        return true;
    }

    /**
     * @param {!Event} event
     * @this {WebInspector.Spectrum}
     */
    function positionHue(event)
    {
        var hsva = this._hsv.slice();
        hsva[0] = Number.constrain(1 - (event.x - this._hueAlphaLeft) / this._hueAlphaWidth, 0, 1);
        this._innerSetColor(hsva,  "", undefined, WebInspector.Spectrum._ChangeSource.Other);
    }

    /**
     * @param {!Event} event
     * @this {WebInspector.Spectrum}
     */
    function positionAlpha(event)
    {
        var newAlpha = Math.round((event.x - this._hueAlphaLeft) / this._hueAlphaWidth * 100) / 100;
        var hsva = this._hsv.slice();
        hsva[3] = Number.constrain(newAlpha, 0, 1);
        var colorFormat = undefined;
        if (this._color().hasAlpha() && (this._colorFormat === WebInspector.Color.Format.ShortHEX || this._colorFormat === WebInspector.Color.Format.HEX || this._colorFormat === WebInspector.Color.Format.Nickname))
            colorFormat = WebInspector.Color.Format.RGB;
        this._innerSetColor(hsva, "", colorFormat, WebInspector.Spectrum._ChangeSource.Other);
    }

    /**
     * @param {!Event} event
     * @this {WebInspector.Spectrum}
     */
    function positionColor(event)
    {
        var hsva = this._hsv.slice();
        hsva[1] = Number.constrain((event.x - this._colorOffset.left) / this.dragWidth, 0, 1);
        hsva[2] = Number.constrain(1 - (event.y - this._colorOffset.top) / this.dragHeight, 0, 1);
        this._innerSetColor(hsva,  "", undefined, WebInspector.Spectrum._ChangeSource.Other);
    }
}

WebInspector.Spectrum._ChangeSource = {
    Input: "Input",
    Model: "Model",
    Other: "Other"
}

WebInspector.Spectrum.Events = {
    ColorChanged: "ColorChanged"
};

WebInspector.Spectrum.prototype = {
    /**
     * @param {!WebInspector.Color} color
     * @param {string} colorFormat
     */
    setColor: function(color, colorFormat)
    {
        this._originalFormat = colorFormat;
        this._innerSetColor(color.hsva(), "", colorFormat, WebInspector.Spectrum._ChangeSource.Model);
    },

    /**
     * @param {!Array<number>|undefined} hsva
     * @param {string|undefined} colorString
     * @param {string|undefined} colorFormat
     * @param {string} changeSource
     */
    _innerSetColor: function(hsva, colorString, colorFormat, changeSource)
    {
        if (hsva !== undefined)
            this._hsv = hsva;
        if (colorString !== undefined)
            this._colorString = colorString;
        if (colorFormat !== undefined) {
            console.assert(colorFormat !== WebInspector.Color.Format.Original, "Spectrum's color format cannot be Original");
            if (colorFormat === WebInspector.Color.Format.RGBA)
                colorFormat = WebInspector.Color.Format.RGB;
            else if (colorFormat === WebInspector.Color.Format.HSLA)
                colorFormat = WebInspector.Color.Format.HSL;
            this._colorFormat = colorFormat;
        }

        this._updateHelperLocations();
        this._updateUI();

        if (changeSource !== WebInspector.Spectrum._ChangeSource.Input)
            this._updateInput();
        if (changeSource !== WebInspector.Spectrum._ChangeSource.Model)
            this.dispatchEventToListeners(WebInspector.Spectrum.Events.ColorChanged, this.colorString());
    },

    /**
     * @return {!WebInspector.Color}
     */
    _color: function()
    {
        return WebInspector.Color.fromHSVA(this._hsv);
    },

    /**
     * @return {string}
     */
    colorString: function()
    {
        if (this._colorString)
            return this._colorString;
        var cf = WebInspector.Color.Format;
        var color = this._color();
        var colorString = color.asString(this._colorFormat);
        if (colorString)
            return colorString;

        if (this._colorFormat === cf.Nickname || this._colorFormat === cf.ShortHEX) {
            colorString = color.asString(cf.HEX);
            if (colorString)
                return colorString;
        }

        console.assert(color.hasAlpha());
        return this._colorFormat === cf.HSL ? /** @type {string} */(color.asString(cf.HSLA)) : /** @type {string} */(color.asString(cf.RGBA));
    },

    _updateHelperLocations: function()
    {
        var h = this._hsv[0];
        var s = this._hsv[1];
        var v = this._hsv[2];
        var alpha = this._hsv[3];

        // Where to show the little circle that displays your current selected color.
        var dragX = s * this.dragWidth;
        var dragY = this.dragHeight - (v * this.dragHeight);

        dragX = Math.max(-this._colorDragElementHeight,
                        Math.min(this.dragWidth - this._colorDragElementHeight, dragX - this._colorDragElementHeight));
        dragY = Math.max(-this._colorDragElementHeight,
                        Math.min(this.dragHeight - this._colorDragElementHeight, dragY - this._colorDragElementHeight));

        this._colorDragElement.positionAt(dragX, dragY);

        // Where to show the bar that displays your current selected hue.
        var hueSlideX = (1 - h) * this._hueAlphaWidth - this.slideHelperWidth;
        this._hueSlider.style.left = hueSlideX + "px";
        var alphaSlideX = alpha * this._hueAlphaWidth - this.slideHelperWidth;
        this._alphaSlider.style.left = alphaSlideX + "px";
    },

    _updateInput: function()
    {
        var cf = WebInspector.Color.Format;
        if (this._colorFormat === cf.HEX || this._colorFormat === cf.ShortHEX || this._colorFormat === cf.Nickname) {
            this._hexContainer.hidden = false;
            this._displayContainer.hidden = true;
            if (this._colorFormat === cf.ShortHEX && this._color().canBeShortHex())
                this._hexValue.value = this._color().asString(cf.ShortHEX);
            else
                this._hexValue.value = this._color().asString(cf.HEX);
        } else {
            // RGBA, HSLA display.
            this._hexContainer.hidden = true;
            this._displayContainer.hidden = false;
            var isRgb = this._colorFormat === cf.RGB;
            this._textLabels.textContent = isRgb ? "RGBA" : "HSLA";
            var colorValues = isRgb ? this._color().canonicalRGBA() : this._color().canonicalHSLA();
            for (var i = 0; i < 3; ++i) {
                this._textValues[i].value = colorValues[i];
                if (!isRgb && (i === 1 || i === 2))
                    this._textValues[i].value += "%";
            }
            this._textValues[3].value= Math.round(colorValues[3] * 100) / 100;
        }
    },

    _updateUI: function()
    {
        var h = WebInspector.Color.fromHSVA([this._hsv[0], 1, 1, 1]);
        this._colorElement.style.backgroundColor = /** @type {string} */ (h.asString(WebInspector.Color.Format.RGB));
        this._swatchInnerElement.style.backgroundColor = /** @type {string} */ (this._color().asString(WebInspector.Color.Format.RGBA));
        // Show border if the swatch is white.
        this._swatchInnerElement.classList.toggle("swatch-inner-white", this._color().hsla()[2] > 0.9);
        this._colorDragElement.style.backgroundColor = /** @type {string} */ (this._color().asString(WebInspector.Color.Format.RGBA));
        var noAlpha = WebInspector.Color.fromHSVA(this._hsv.slice(0,3).concat(1));
        this._alphaElementBackground.style.backgroundImage = String.sprintf("linear-gradient(to right, rgba(0,0,0,0), %s)", noAlpha.asString(WebInspector.Color.Format.RGB));
    },

    _formatViewSwitch: function()
    {
        var cf = WebInspector.Color.Format;
        var format = cf.RGB;
        if (this._colorFormat === cf.RGB)
            format = cf.HSL;
        else if (this._colorFormat === cf.HSL && !this._color().hasAlpha())
            format = this._originalFormat === cf.ShortHEX ? cf.ShortHEX : cf.HEX;
        this._innerSetColor(undefined, "", format, WebInspector.Spectrum._ChangeSource.Other);
    },

    /**
     * @param {!Event} event
     */
    _inputChanged: function(event)
    {
        /**
         * @param {!Element} element
         * @return {string}
         */
        function elementValue(element)
        {
            return element.value;
        }

        var inputElement = /** @type {!Element} */(event.currentTarget);
        var arrowKeyOrMouseWheelEvent = (event.keyIdentifier === "Up" || event.keyIdentifier === "Down" || event.type === "mousewheel");
        var pageKeyPressed = (event.keyIdentifier === "PageUp" || event.keyIdentifier === "PageDown");
        if (arrowKeyOrMouseWheelEvent || pageKeyPressed) {
            var newValue = WebInspector.createReplacementString(inputElement.value, event);
            if (newValue) {
                inputElement.value = newValue;
                inputElement.selectionStart = 0;
                inputElement.selectionEnd = newValue.length;
            }
            event.consume(true);
        }

        const cf = WebInspector.Color.Format;
        var colorString;
        if (this._colorFormat === cf.HEX || this._colorFormat === cf.ShortHEX) {
            colorString = this._hexValue.value;
        } else {
            var format = this._colorFormat === cf.RGB ? "rgba" : "hsla";
            var values = this._textValues.map(elementValue).join(",");
            colorString = String.sprintf("%s(%s)", format, values);
        }

        var color = WebInspector.Color.parse(colorString);
        if (!color)
            return;
        var hsv = color.hsva();
        if (this._colorFormat === cf.HEX || this._colorFormat === cf.ShortHEX)
            this._colorFormat = color.canBeShortHex() ? cf.ShortHEX : cf.HEX;
        this._innerSetColor(hsv, colorString, undefined, WebInspector.Spectrum._ChangeSource.Input);
    },

    wasShown: function()
    {
        this._hueAlphaWidth = this._hueElement.offsetWidth;
        this.slideHelperWidth = this._hueSlider.offsetWidth / 2;
        this.dragWidth = this._colorElement.offsetWidth;
        this.dragHeight = this._colorElement.offsetHeight;
        this._colorDragElementHeight = this._colorDragElement.offsetHeight / 2;
        this._innerSetColor(undefined, undefined, undefined, WebInspector.Spectrum._ChangeSource.Model);
        this._toggleColorPicker(true);
        WebInspector.targetManager.addModelListener(WebInspector.ResourceTreeModel, WebInspector.ResourceTreeModel.EventTypes.ColorPicked, this._colorPicked, this);
    },

    willHide: function()
    {
        this._toggleColorPicker(false);
        WebInspector.targetManager.removeModelListener(WebInspector.ResourceTreeModel, WebInspector.ResourceTreeModel.EventTypes.ColorPicked, this._colorPicked, this);
    },

    /**
     * @param {boolean=} enabled
     * @param {!WebInspector.Event=} event
     */
    _toggleColorPicker: function(enabled, event)
    {
        if (enabled === undefined)
            enabled = !this._colorPickerButton.toggled();
        this._colorPickerButton.setToggled(enabled);
        for (var target of WebInspector.targetManager.targets())
            target.pageAgent().setColorPickerEnabled(enabled);
    },

    /**
     * @param {!WebInspector.Event} event
     */
    _colorPicked: function(event)
    {
        var rgbColor = /** @type {!DOMAgent.RGBA} */ (event.data);
        var rgba = [rgbColor.r, rgbColor.g, rgbColor.b, (rgbColor.a / 2.55 | 0) / 100];
        var color = WebInspector.Color.fromRGBA(rgba);
        this._innerSetColor(color.hsva(), "", undefined, WebInspector.Spectrum._ChangeSource.Other);
        InspectorFrontendHost.bringToFront();
    },


    __proto__: WebInspector.VBox.prototype
}
