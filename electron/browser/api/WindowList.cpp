// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "WindowList.h"

#include <algorithm>

namespace atom {

// static
WindowList* WindowList::instance_ = nullptr;

// static
WindowList* WindowList::GetInstance() {
  if (!instance_)
    instance_ = new WindowList;
  return instance_;
}

// static
void WindowList::AddWindow(Window* window) {
    if (window) {
        // Push |window| on the appropriate list instance.
        WindowVector& windows = GetInstance()->windows_;
        windows.push_back(window);
    }
}

// static
void WindowList::RemoveWindow(Window* window) {
  WindowVector& windows = GetInstance()->windows_;
  windows.erase(std::remove(windows.begin(), windows.end(), window),
                windows.end());
}

// static
void WindowList::WindowCloseCancelled(Window* window) {

}

// static
void WindowList::CloseAllWindows() {
  WindowVector windows = GetInstance()->windows_;
  //for (const auto& window : windows)
  //  if (!window->IsClosed())
  //    window->Close();
}

WindowList::WindowList() {
}

WindowList::~WindowList() {
}

}  // namespace atom
