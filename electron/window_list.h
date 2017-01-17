// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_WINDOW_LIST_H_
#define ATOM_BROWSER_WINDOW_LIST_H_

#include <vector>

namespace atom {
    class Window;
class WindowList {
 public:
  typedef std::vector<Window*> WindowVector;
  typedef WindowVector::iterator iterator;
  typedef WindowVector::const_iterator const_iterator;

  // Windows are added to the list before they have constructed windows,
  // so the |window()| member function may return NULL.
  const_iterator begin() const { return windows_.begin(); }
  const_iterator end() const { return windows_.end(); }

  iterator begin() { return windows_.begin(); }
  iterator end() { return windows_.end(); }

  bool empty() const { return windows_.empty(); }
  size_t size() const { return windows_.size(); }

  Window* get(size_t index) const { return windows_[index]; }

  static WindowList* GetInstance();

  // Adds or removes |window| from the list it is associated with.
  static void AddWindow(Window* window);
  static void RemoveWindow(Window* window);

  // Called by window when a close is cancelled by beforeunload handler.
  static void WindowCloseCancelled(Window* window);


  // Closes all windows.
  static void CloseAllWindows();

 private:
  WindowList();
  ~WindowList();

  // A vector of the windows in this list, in the order they were added.
  WindowVector windows_;


  static WindowList* instance_;

};

}  // namespace atom

#endif  // ATOM_BROWSER_WINDOW_LIST_H_
