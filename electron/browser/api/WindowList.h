// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_WINDOW_LIST_H_
#define ATOM_BROWSER_WINDOW_LIST_H_

#include "browser/api/WindowInterface.h"
#include <vector>

namespace atom {

class WindowList {
public:
    typedef std::vector<WindowInterface*> WindowVector;
    typedef WindowVector::iterator iterator;
    typedef WindowVector::const_iterator const_iterator;

    // Windows are added to the list before they have constructed windows,
    // so the |window()| member function may return NULL.
    const_iterator begin() const { return m_windows.begin(); }
    const_iterator end() const { return m_windows.end(); }

    iterator begin() { return m_windows.begin(); }
    iterator end() { return m_windows.end(); }

    bool empty() const { return m_windows.empty(); }
    size_t size() const { return m_windows.size(); }

    WindowInterface* get(size_t index) const { return m_windows[index]; }

    WindowInterface* find(int id) const;

    static WindowList* getInstance();

    // Adds or removes |window| from the list it is associated with.
    static void addWindow(WindowInterface* window);
    static void removeWindow(WindowInterface* window);

    // Called by window when a close is cancelled by beforeunload handler.
    static void WindowCloseCancelled(WindowInterface* window);

    // Closes all windows.
    static void closeAllWindows();

private:
    WindowList();
    ~WindowList();

    // A vector of the windows in this list, in the order they were added.
    WindowVector m_windows;

    static WindowList* m_instance;
};

} // namespace atom

#endif // ATOM_BROWSER_WINDOW_LIST_H_
