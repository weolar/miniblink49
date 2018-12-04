// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "WindowList.h"

#include <algorithm>

namespace atom {

// static
WindowList* WindowList::m_instance = nullptr;

// static
WindowList* WindowList::getInstance() {
    if (!m_instance)
        m_instance = new WindowList;
    return m_instance;
}

// static
void WindowList::addWindow(WindowInterface* window) {
    if (!window)
        return;
    
    // Push |window| on the appropriate list instance.
    WindowVector& windows = getInstance()->m_windows;
    windows.push_back(window);
}

// static
void WindowList::removeWindow(WindowInterface* window) {
    WindowVector& windows = getInstance()->m_windows;
    windows.erase(std::remove(windows.begin(), windows.end(), window), windows.end());
}

WindowInterface* WindowList::find(int id) const {
    for (WindowVector::const_iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
        if ((*it)->getId() == id)
            return *it;
    }
    return nullptr;
}

// static
void WindowList::WindowCloseCancelled(WindowInterface* window) {

}

// static
void WindowList::closeAllWindows() {
    WindowVector windows = getInstance()->m_windows;
    for (WindowInterface* window : windows)
        if (!window->isClosed())
            window->close();
}

WindowList::WindowList() {
}

WindowList::~WindowList() {
}

}  // namespace atom
