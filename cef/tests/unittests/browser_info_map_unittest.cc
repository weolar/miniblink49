// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <list>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "libcef_dll/wrapper/cef_browser_info_map.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

struct MyObject {
  MyObject(int val = 0) : member(val) {}
  int member;
};

int g_destruct_ct = 0;

struct MyObjectTraits {
  static void Destruct(MyObject info) {
    g_destruct_ct++;
  }
};

typedef CefBrowserInfoMap<int, MyObject, MyObjectTraits> MyObjectMap;

class MyVisitor : public MyObjectMap::Visitor {
 public:
  MyVisitor(bool remove = false,
            int remove_browser_id = 0,
            InfoIdType remove_info_id = 0)
      : remove_(remove),
        remove_browser_id_(remove_browser_id),
        remove_info_id_(remove_info_id) {}

  bool OnNextInfo(int browser_id,
                  InfoIdType info_id,
                  InfoObjectType info,
                  bool* remove) override {
    Info info_rec;
    info_rec.browser_id = browser_id;
    info_rec.info_id = info_id;
    info_rec.info = info;
    info_list_.push_back(info_rec);

    // Based on test configuration remove no objects, all objects, or only the
    // specified object.
    *remove = remove_ || (browser_id == remove_browser_id_ &&
                          info_id == remove_info_id_);
    return true;
  }

  // Returns true if the specified info was passed to OnNextInfo. Removes the
  // record if found.
  bool Exists(int browser_id,
              InfoIdType info_id,
              InfoObjectType info) {
    InfoList::iterator it = info_list_.begin();
    for (; it != info_list_.end(); ++it) {
      const Info& found_info = *it;
      if (browser_id == found_info.browser_id &&
          info_id == found_info.info_id &&
          info.member == found_info.info.member) {
        info_list_.erase(it);
        return true;
      }
    }
    return false;
  }

  size_t info_size() const { return info_list_.size(); }

 private:
  bool remove_;
  int remove_browser_id_;
  InfoIdType remove_info_id_;

  struct Info {
    int browser_id;
    InfoIdType info_id;
    InfoObjectType info;
  };

  // Track calls to OnNextInfo.
  typedef std::list<Info> InfoList;
  InfoList info_list_;
};

}  // namespace

TEST(BrowserInfoMapTest, AddSingleBrowser) {
  MyObjectMap map;
  const int kBrowserId = 1;

  g_destruct_ct = 0;

  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId));

  MyObject obj1(1);
  map.Add(kBrowserId, 1, obj1);
  EXPECT_EQ(1U, map.size());
  EXPECT_EQ(1U, map.size(kBrowserId));

  MyObject obj2(2);
  map.Add(kBrowserId, 2, obj2);
  EXPECT_EQ(2U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId));

  MyObject obj3(3);
  map.Add(kBrowserId, 3, obj3);
  EXPECT_EQ(3U, map.size());
  EXPECT_EQ(3U, map.size(kBrowserId));

  EXPECT_EQ(0, g_destruct_ct);

  map.clear(kBrowserId);
  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId));

  EXPECT_EQ(3, g_destruct_ct);
}

TEST(BrowserInfoMapTest, AddMultipleBrowsers) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId1));
  EXPECT_EQ(0U, map.size(kBrowserId2));

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);
  EXPECT_EQ(1U, map.size());
  EXPECT_EQ(1U, map.size(kBrowserId1));
  EXPECT_EQ(0U, map.size(kBrowserId2));

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);
  EXPECT_EQ(2U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(0U, map.size(kBrowserId2));

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);
  EXPECT_EQ(3U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(1U, map.size(kBrowserId2));

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);
  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  EXPECT_EQ(0, g_destruct_ct);

  map.clear(kBrowserId1);
  EXPECT_EQ(2U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  EXPECT_EQ(2, g_destruct_ct);

  map.clear(kBrowserId2);
  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId1));
  EXPECT_EQ(0U, map.size(kBrowserId2));

  EXPECT_EQ(4, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindSingleBrowser) {
  MyObjectMap map;
  const int kBrowserId = 1;

  g_destruct_ct = 0;

  // obj1 not added yet.
  MyObject nf1 = map.Find(kBrowserId, 1, NULL);
  EXPECT_EQ(0, nf1.member);

  MyObject obj1(1);
  map.Add(kBrowserId, 1, obj1);

  // obj1 should exist.
  MyObject f1 = map.Find(kBrowserId, 1, NULL);
  EXPECT_EQ(obj1.member, f1.member);

  // obj2 not added yet.
  MyObject nf2 = map.Find(kBrowserId, 2, NULL);
  EXPECT_EQ(0, nf2.member);

  MyObject obj2(2);
  map.Add(kBrowserId, 2, obj2);

  // obj2 should exist.
  MyObject f2 = map.Find(kBrowserId, 2, NULL);
  EXPECT_EQ(obj2.member, f2.member);

  // find obj1 again.
  MyObject f1b = map.Find(kBrowserId, 1, NULL);
  EXPECT_EQ(obj1.member, f1b.member);

  // find obj2 again.
  MyObject f2b = map.Find(kBrowserId, 2, NULL);
  EXPECT_EQ(obj2.member, f2b.member);

  // doesn't exist.
  MyObject nf3 = map.Find(kBrowserId, 3, NULL);
  EXPECT_EQ(0, nf3.member);
  MyObject nf4 = map.Find(10, 1, NULL);
  EXPECT_EQ(0, nf4.member);
  MyObject nf5 = map.Find(10, 3, NULL);
  EXPECT_EQ(0, nf5.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  EXPECT_EQ(2, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindMultipleBrowsers) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  // obj1 not added yet.
  MyObject nf1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(0, nf1.member);

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  // obj1 should exist.
  MyObject f1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(obj1.member, f1.member);

  // obj2 not added yet.
  MyObject nf2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(0, nf2.member);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  // obj2 should exist.
  MyObject f2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(obj2.member, f2.member);

  // obj3 not added yet.
  MyObject nf3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(0, nf3.member);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  // obj3 should exist.
  MyObject f3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(obj3.member, f3.member);

  // obj4 not added yet.
  MyObject nf4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(0, nf4.member);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  // obj4 should exist.
  MyObject f4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(obj4.member, f4.member);

  // obj1-3 should exist.
  MyObject f1b = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(obj1.member, f1b.member);
  MyObject f2b = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(obj2.member, f2b.member);
  MyObject f3b = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(obj3.member, f3b.member);

  // wrong browser
  MyObject nf5 = map.Find(kBrowserId1, 4, NULL);
  EXPECT_EQ(0, nf5.member);
  MyObject nf6 = map.Find(kBrowserId2, 1, NULL);
  EXPECT_EQ(0, nf6.member);

  // deosn't exist
  MyObject nf7 = map.Find(kBrowserId2, 5, NULL);
  EXPECT_EQ(0, nf7.member);
  MyObject nf8 = map.Find(8, 1, NULL);
  EXPECT_EQ(0, nf8.member);
  MyObject nf9 = map.Find(8, 10, NULL);
  EXPECT_EQ(0, nf9.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  EXPECT_EQ(4, g_destruct_ct);
}

TEST(BrowserInfoMapTest, Find) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should only visit the single object
  MyVisitor visitor;
  map.Find(kBrowserId2, 4, &visitor);
  EXPECT_EQ(1U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  EXPECT_EQ(4, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAndRemove) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should only visit and remove the single object.
  MyVisitor visitor(true);
  map.Find(kBrowserId1, 2, &visitor);
  EXPECT_EQ(1U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 2, obj2));
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(3U, map.size());
  EXPECT_EQ(1U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // visited object shouldn't exist
  MyObject nf2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(0, nf2.member);

  // other objects should exist
  MyObject nf1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(obj1.member, nf1.member);
  MyObject nf3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(obj3.member, nf3.member);
  MyObject nf4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(obj4.member, nf4.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  // should destruct the remaining 3 objects
  EXPECT_EQ(3, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAll) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should visit all objects
  MyVisitor visitor;
  map.FindAll(&visitor);
  EXPECT_EQ(4U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 1, obj1));
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 2, obj2));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 3, obj3));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  EXPECT_EQ(4, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAllByBrowser) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should only visit browser2 objects
  MyVisitor visitor;
  map.FindAll(kBrowserId2, &visitor);
  EXPECT_EQ(2U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 3, obj3));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  EXPECT_EQ(4, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAllAndRemoveAll) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should visit all objects
  MyVisitor visitor(true);
  map.FindAll(&visitor);
  EXPECT_EQ(4U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 1, obj1));
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 2, obj2));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 3, obj3));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId1));
  EXPECT_EQ(0U, map.size(kBrowserId2));

  EXPECT_EQ(0, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAllAndRemoveOne) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should visit all objects and remove one
  MyVisitor visitor(false, kBrowserId2, 3);
  map.FindAll(&visitor);
  EXPECT_EQ(4U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 1, obj1));
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 2, obj2));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 3, obj3));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(3U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(1U, map.size(kBrowserId2));

  // removed object shouldn't exist
  MyObject nf3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(0, nf3.member);

  // other objects should exist
  MyObject f1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(obj1.member, f1.member);
  MyObject f2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(obj2.member, f2.member);
  MyObject f4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(obj4.member, f4.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  // should destruct the remaining 3 objects
  EXPECT_EQ(3, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAllAndRemoveAllByBrowser) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should only visit browser1 objects
  MyVisitor visitor(true);
  map.FindAll(kBrowserId1, &visitor);
  EXPECT_EQ(2U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 1, obj1));
  EXPECT_TRUE(visitor.Exists(kBrowserId1, 2, obj2));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(2U, map.size());
  EXPECT_EQ(0U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // browser1 objects shouldn't exist
  MyObject nf1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(0, nf1.member);
  MyObject nf2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(0, nf2.member);

  // browser 2 objects should exist
  MyObject f3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(obj3.member, f3.member);
  MyObject f4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(obj4.member, f4.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  // should destruct the remaining 2 objects
  EXPECT_EQ(2, g_destruct_ct);
}

TEST(BrowserInfoMapTest, FindAllAndRemoveOneByBrowser) {
  MyObjectMap map;
  const int kBrowserId1 = 1;
  const int kBrowserId2 = 2;

  g_destruct_ct = 0;

  MyObject obj1(1);
  map.Add(kBrowserId1, 1, obj1);

  MyObject obj2(2);
  map.Add(kBrowserId1, 2, obj2);

  MyObject obj3(3);
  map.Add(kBrowserId2, 3, obj3);

  MyObject obj4(4);
  map.Add(kBrowserId2, 4, obj4);

  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(2U, map.size(kBrowserId2));

  // should visit browser2 objects and remove one
  MyVisitor visitor(false, kBrowserId2, 4);
  map.FindAll(kBrowserId2, &visitor);
  EXPECT_EQ(2U, visitor.info_size());
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 3, obj3));
  EXPECT_TRUE(visitor.Exists(kBrowserId2, 4, obj4));
  // should be no unexpected visits
  EXPECT_EQ(0U, visitor.info_size());

  EXPECT_EQ(3U, map.size());
  EXPECT_EQ(2U, map.size(kBrowserId1));
  EXPECT_EQ(1U, map.size(kBrowserId2));

  // removed object shouldn't exist
  MyObject nf4 = map.Find(kBrowserId2, 4, NULL);
  EXPECT_EQ(0, nf4.member);

  // other objects should exist
  MyObject f1 = map.Find(kBrowserId1, 1, NULL);
  EXPECT_EQ(obj1.member, f1.member);
  MyObject f2 = map.Find(kBrowserId1, 2, NULL);
  EXPECT_EQ(obj2.member, f2.member);
  MyObject f3 = map.Find(kBrowserId2, 3, NULL);
  EXPECT_EQ(obj3.member, f3.member);

  EXPECT_EQ(0, g_destruct_ct);
  map.clear();
  // should destruct the remaining 3 objects
  EXPECT_EQ(3, g_destruct_ct);
}


namespace {

class MyHeapObject {
 public:
  MyHeapObject(int* destroy_ct) : destroy_ct_(destroy_ct) {}
  ~MyHeapObject() {
    (*destroy_ct_)++;
  }

 private:
  int* destroy_ct_;
};

}  // namespace

TEST(BrowserInfoMapTest, DefaultTraits) {
  CefBrowserInfoMap<int, MyHeapObject*> map;

  int destroy_ct = 0;
  map.Add(1, 1, new MyHeapObject(&destroy_ct));
  map.Add(1, 2, new MyHeapObject(&destroy_ct));
  map.Add(2, 1, new MyHeapObject(&destroy_ct));
  map.Add(2, 2, new MyHeapObject(&destroy_ct));
  map.Add(3, 1, new MyHeapObject(&destroy_ct));
  map.Add(3, 2, new MyHeapObject(&destroy_ct));

  EXPECT_EQ(6U, map.size());

  map.clear(1);
  EXPECT_EQ(4U, map.size());
  EXPECT_EQ(2, destroy_ct);

  map.clear();
  EXPECT_EQ(0U, map.size());
  EXPECT_EQ(6, destroy_ct);
}
