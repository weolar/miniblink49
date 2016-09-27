// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebContentSettingsClient_h
#define WebContentSettingsClient_h

#include "public/platform/WebContentSettingCallbacks.h"

namespace blink {

class WebSecurityOrigin;
class WebString;
class WebURL;

class WebContentSettingsClient {
public:
    // Controls whether access to Web Databases is allowed for this frame.
    virtual bool allowDatabase(const WebString& name, const WebString& displayName, unsigned long estimatedSize) { return true; }

    // Controls whether access to File System is allowed for this frame.
    virtual bool requestFileSystemAccessSync() { return true; }

    // Controls whether access to File System is allowed for this frame.
    virtual void requestFileSystemAccessAsync(const WebContentSettingCallbacks& callbacks) { WebContentSettingCallbacks permissionCallbacks(callbacks); permissionCallbacks.doAllow(); }

    // Controls whether images are allowed for this frame.
    virtual bool allowImage(bool enabledPerSettings, const WebURL& imageURL) { return enabledPerSettings; }

    // Controls whether access to Indexed DB are allowed for this frame.
    virtual bool allowIndexedDB(const WebString& name, const WebSecurityOrigin&) { return true; }

    // Controls whether HTML5 media elements (<audio>, <video>) are allowed for this frame.
    virtual bool allowMedia(const WebURL& videoURL) { return true; }

    // Controls whether plugins are allowed for this frame.
    virtual bool allowPlugins(bool enabledPerSettings) { return enabledPerSettings; }

    // Controls whether scripts are allowed to execute for this frame.
    virtual bool allowScript(bool enabledPerSettings) { return enabledPerSettings; }

    // Controls whether scripts loaded from the given URL are allowed to execute for this frame.
    virtual bool allowScriptFromSource(bool enabledPerSettings, const WebURL& scriptURL) { return enabledPerSettings; }

    // Controls whether insecrure content is allowed to display for this frame.
    virtual bool allowDisplayingInsecureContent(bool enabledPerSettings, const WebSecurityOrigin&, const WebURL&) { return enabledPerSettings; }

    // Controls whether insecrure scripts are allowed to execute for this frame.
    virtual bool allowRunningInsecureContent(bool enabledPerSettings, const WebSecurityOrigin&, const WebURL&) { return enabledPerSettings; }

    // Controls whether the given script extension should run in a new script
    // context in this frame. If extensionGroup is 0, the script context is the
    // frame's main context. Otherwise, it is a context created by
    // WebLocalFrame::executeScriptInIsolatedWorld with that same extensionGroup
    // value.
    virtual bool allowScriptExtension(const WebString& extensionName, int extensionGroup) { return true; }

    virtual bool allowScriptExtension(const WebString& extensionName, int extensionGroup, int worldId)
    {
        return allowScriptExtension(extensionName, extensionGroup);
    }

    // Controls whether HTML5 Web Storage is allowed for this frame.
    // If local is true, then this is for local storage, otherwise it's for session storage.
    virtual bool allowStorage(bool local) { return true; }

    // Controls whether access to read the clipboard is allowed for this frame.
    virtual bool allowReadFromClipboard(bool defaultValue) { return defaultValue; }

    // Controls whether access to write the clipboard is allowed for this frame.
    virtual bool allowWriteToClipboard(bool defaultValue) { return defaultValue; }

    // Controls whether enabling Web Components API for this frame.
    virtual bool allowWebComponents(bool defaultValue) { return defaultValue; }

    // Controls whether to enable MutationEvents for this frame.
    // The common use case of this method is actually to selectively disable MutationEvents,
    // but it's been named for consistency with the rest of the interface.
    virtual bool allowMutationEvents(bool defaultValue) { return defaultValue; }

    // Notifies the client that the frame would have instantiated a plugin if plugins were enabled.
    virtual void didNotAllowPlugins() { }

    // Notifies the client that the frame would have executed script if script were enabled.
    virtual void didNotAllowScript() { }

    virtual ~WebContentSettingsClient() { }
};

} // namespace blink

#endif // WebContentSettingsClient_h
