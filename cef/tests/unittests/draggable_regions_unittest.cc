// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"

namespace {

const char kTestURLWithRegions[] = "http://test.com/regions";
const char kTestHTMLWithRegions[] =
    "<html>"
    "  <body>"
    "    <div style=\"position: absolute; top: 50px; left: 50px; width: 200px; "
    "height: 200px; background-color: red; -webkit-app-region: drag;\">"
    "      <div style=\"position: absolute; top: 50%; left: 50%; "
    "transform: translate(-50%, -50%); width: 50px; height: 50px; "
    "background-color: blue; -webkit-app-region: no-drag;\">"
    "      </div>"
    "    </div>"
    "  </body>"
    "</html>";

const char kTestURLWithoutRegions[] = "http://test.com/no-regions";
const char kTestHTMLWithoutRegions[] =
    "<html><body>Hello World!</body></html>";

const char kTestURLWithChangingRegions[] = "http://test.com/changing-regions";
const char kTestHTMLWithChangingRegions[] =
    "<html>"
    "  <body>"
    "    <div id=\"layer\" style=\"position: absolute; top: 50px; left: 50px; "
    "width: 200px; height: 200px; background-color: red; "
    "-webkit-app-region: drag;\">"
    "      <div style=\"position: absolute; top: 50%; left: 50%; "
    "transform: translate(-50%, -50%); width: 50px; height: 50px; "
    "background-color: blue; -webkit-app-region: no-drag;\">"
    "      </div>"
    "    </div>"
    "    <script>"
    "      window.setTimeout(function() {"
    "        var layer = document.getElementById('layer');"
    "        layer.style.top = '0px';"
    "        layer.style.left = '0px';"
    "      }, 500);"
    "    </script>"
    "  </body>"
    "</html>";

class DraggableRegionsTestHandler : public TestHandler {
 public:
  DraggableRegionsTestHandler()
    : step_(kStepWithRegions) {}

  void RunTest() override {
    // Add HTML documents with and without draggable regions.
    AddResource(kTestURLWithRegions, kTestHTMLWithRegions, "text/html");
    AddResource(kTestURLWithoutRegions, kTestHTMLWithoutRegions, "text/html");
    AddResource(kTestURLWithChangingRegions, kTestHTMLWithChangingRegions,
                "text/html");

    // Create the browser
    CreateBrowser(kTestURLWithRegions);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnDraggableRegionsChanged(
      CefRefPtr<CefBrowser> browser,
      const std::vector<CefDraggableRegion>& regions) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_UI));
    EXPECT_TRUE(browser->IsSame(GetBrowser()));

    did_call_on_draggable_regions_changed_.yes();

    switch (step_) {
      case kStepWithRegions:
      case kStepWithChangingRegions1:
        EXPECT_EQ(2U, regions.size());
        EXPECT_EQ(50, regions[0].bounds.x);
        EXPECT_EQ(50, regions[0].bounds.y);
        EXPECT_EQ(200, regions[0].bounds.width);
        EXPECT_EQ(200, regions[0].bounds.height);
        EXPECT_EQ(1, regions[0].draggable);
        EXPECT_EQ(125, regions[1].bounds.x);
        EXPECT_EQ(125, regions[1].bounds.y);
        EXPECT_EQ(50, regions[1].bounds.width);
        EXPECT_EQ(50, regions[1].bounds.height);
        EXPECT_EQ(0, regions[1].draggable);
        break;
      case kStepWithChangingRegions2:
        EXPECT_EQ(2U, regions.size());
        EXPECT_EQ(0, regions[0].bounds.x);
        EXPECT_EQ(0, regions[0].bounds.y);
        EXPECT_EQ(200, regions[0].bounds.width);
        EXPECT_EQ(200, regions[0].bounds.height);
        EXPECT_EQ(1, regions[0].draggable);
        EXPECT_EQ(75, regions[1].bounds.x);
        EXPECT_EQ(75, regions[1].bounds.y);
        EXPECT_EQ(50, regions[1].bounds.width);
        EXPECT_EQ(50, regions[1].bounds.height);
        EXPECT_EQ(0, regions[1].draggable);
        break;
      case kStepWithoutRegions:
        // Should not be reached.
        EXPECT_TRUE(false);
        break;
    }

    NextTest(browser);
  }

  void DestroyTest() override {
    EXPECT_EQ(false, did_call_on_draggable_regions_changed_);

    TestHandler::DestroyTest();
  }

 private:
  void NextTest(CefRefPtr<CefBrowser> browser) {
    CefRefPtr<CefFrame> frame(browser->GetMainFrame());

    did_call_on_draggable_regions_changed_.reset();

    switch (step_) {
      case kStepWithRegions:
        step_ = kStepWithChangingRegions1;
        frame->LoadURL(kTestURLWithChangingRegions);
        break;
      case kStepWithChangingRegions1:
        step_ = kStepWithChangingRegions2;
        break;
      case kStepWithChangingRegions2:
        step_ = kStepWithoutRegions;
        frame->LoadURL(kTestURLWithoutRegions);
        // Needed because this test doesn't call OnDraggableRegionsChanged.
        CefPostDelayedTask(TID_UI,
            base::Bind(&DraggableRegionsTestHandler::DestroyTest, this), 500);
        break;
      case kStepWithoutRegions: {
        // Should not be reached.
        EXPECT_TRUE(false);
        break;
      }
    }
  }

  enum Step {
    kStepWithRegions,
    kStepWithChangingRegions1,
    kStepWithChangingRegions2,
    kStepWithoutRegions,
  } step_;

  TrackCallback did_call_on_draggable_regions_changed_;

  IMPLEMENT_REFCOUNTING(DraggableRegionsTestHandler);
};

}  // namespace

// Verify that draggable regions work.
TEST(DraggableRegionsTest, DraggableRegions) {
  CefRefPtr<DraggableRegionsTestHandler> handler =
      new DraggableRegionsTestHandler();
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
