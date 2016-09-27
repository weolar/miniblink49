// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

'use strict';

privateScriptController.installClass('PluginPlaceholderElement', function(PluginPlaceholderElementPrototype) {
    // FIXME: Load this from a .css file.
    var styleSource =
        '#plugin-placeholder {' +
        '    all: initial;' +
        '    width: 100%;' +
        '    height: 100%;' +
        '    overflow: hidden;' +
        '    display: flex;' +
        '    align-items: center;' +
        '    background: gray;' +
        '    font: 12px -webkit-control;' +
        '}' +
        '#plugin-placeholder-content {' +
        '    text-align: center;' +
        '    margin: auto;' +
        '}';

    PluginPlaceholderElementPrototype.createdCallback = function() {
        this.id = 'plugin-placeholder';

        var styleElement = document.createElement('style');
        styleElement.textContent = styleSource;

        var contentElement = document.createElement('div');
        contentElement.id = 'plugin-placeholder-content';

        var messageElement = document.createElement('div');
        messageElement.id = 'plugin-placeholder-message';

        // FIXME: UI polish, l10n, etc. for the close button.
        var closeButton = document.createElement('button');
        closeButton.id = 'plugin-placeholder-close-button';
        closeButton.textContent = 'Close';
        closeButton.style.display = 'none';
        closeButton.addEventListener('click', function() {
            // FIXME: Record UMA Plugin_Hide_Click.
            this.hide();
        }.bind(this));

        contentElement.appendChild(messageElement);
        contentElement.appendChild(closeButton);
        this.appendChild(styleElement);
        this.appendChild(contentElement);

        this.messageElement = messageElement;
        this.closeButton = closeButton;
    };

    PluginPlaceholderElementPrototype.hide = function() {
        var host = (this.parentNode instanceof ShadowRoot) ? this.parentNode.host : this;
        host.style.display = 'none';

        // If we have a width and height, search for a parent (often <div>) with the
        // same dimensions. If we find such a parent, hide that as well.
        // This makes much more uncovered page content usable (including clickable)
        // as opposed to merely visible.
        // TODO(cevans) -- it's a foul heuristic but we're going to tolerate it for
        // now for these reasons:
        // 1) Makes the user experience better.
        // 2) Foulness is encapsulated within this single function.
        // 3) Confidence in no false positives.
        // 4) Seems to have a good / low false negative rate at this time.
        //
        // This heuristic was copied from plugins::PluginPlaceholder::HidePlugin
        // (src/components/plugins/renderer/plugin_placeholder.cc) and should be
        // kept in sync with it until it goes away.
        if (host.hasAttribute('width') && host.hasAttribute('height')) {
            var presentationAttributeInPixels = function(attr) {
                var match = host.getAttribute(attr).match(/^\s*(\d+)\s*(px)?\s*$/);
                if (match)
                    return match[1] + 'px';
            };
            var width = presentationAttributeInPixels('width');
            var height = presentationAttributeInPixels('height');
            if (!width || !height)
                return;

            var element = host;
            while (element instanceof Element) {
                if (element.style.width == width && element.style.height == height)
                    element.style.display = 'none';
                element = element.parentNode;
            }
        }
    };

    Object.defineProperty(PluginPlaceholderElementPrototype, 'message', {
        get: function() { return this.messageElement.textContent; },
        set: function(message) { this.messageElement.textContent = message; },
    });

    Object.defineProperty(PluginPlaceholderElementPrototype, 'closeable', {
        get: function() { return this.closeButton.style.display != 'none'; },
        set: function(closeable) { this.closeButton.style.display = closeable ? '' : 'none'; },
    });
});
