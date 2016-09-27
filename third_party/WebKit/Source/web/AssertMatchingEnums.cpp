/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Use this file to assert that various public API enum values continue
// matching blink defined enum values.

#include "config.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/DocumentMarker.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/IconURL.h"
#include "core/editing/SelectionType.h"
#include "core/editing/TextAffinity.h"
#include "core/fileapi/FileError.h"
#include "core/frame/Frame.h"
#include "core/frame/Settings.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/layout/compositing/CompositedSelectionBound.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/FrameLoaderTypes.h"
#include "core/loader/NavigationPolicy.h"
#include "core/loader/appcache/ApplicationCacheHost.h"
#include "core/page/PageVisibilityState.h"
#include "core/style/ComputedStyleConstants.h"
#include "modules/accessibility/AXObject.h"
#include "modules/geolocation/GeolocationError.h"
#include "modules/geolocation/GeolocationPosition.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBKeyPath.h"
#include "modules/indexeddb/IDBMetadata.h"
#include "modules/indexeddb/IndexedDB.h"
#include "modules/navigatorcontentutils/NavigatorContentUtilsClient.h"
#include "modules/quota/DeprecatedStorageQuota.h"
#include "modules/speech/SpeechRecognitionError.h"
#include "platform/Cursor.h"
#include "platform/FileMetadata.h"
#include "platform/FileSystemType.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/FontSmoothingMode.h"
#include "platform/graphics/filters/FilterOperation.h"
#include "platform/graphics/media/MediaPlayer.h"
#include "platform/mediastream/MediaStreamSource.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/ResourceLoadPriority.h"
#include "platform/network/ResourceResponse.h"
#include "platform/scroll/ScrollTypes.h"
#include "platform/text/TextChecking.h"
#include "platform/text/TextDecoration.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "public/platform/WebApplicationCacheHost.h"
#include "public/platform/WebClipboard.h"
#include "public/platform/WebCursorInfo.h"
#include "public/platform/WebFileError.h"
#include "public/platform/WebFileInfo.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebHistoryScrollRestorationType.h"
#include "public/platform/WebMediaPlayer.h"
#include "public/platform/WebMediaPlayerClient.h"
#include "public/platform/WebMediaSource.h"
#include "public/platform/WebMediaStreamSource.h"
#include "public/platform/WebPageVisibilityState.h"
#include "public/platform/WebReferrerPolicy.h"
#include "public/platform/WebScrollbar.h"
#include "public/platform/WebScrollbarBehavior.h"
#include "public/platform/WebSelectionBound.h"
#include "public/platform/WebStorageQuotaError.h"
#include "public/platform/WebStorageQuotaType.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/modules/indexeddb/WebIDBCursor.h"
#include "public/platform/modules/indexeddb/WebIDBDatabase.h"
#include "public/platform/modules/indexeddb/WebIDBDatabaseException.h"
#include "public/platform/modules/indexeddb/WebIDBFactory.h"
#include "public/platform/modules/indexeddb/WebIDBKey.h"
#include "public/platform/modules/indexeddb/WebIDBKeyPath.h"
#include "public/platform/modules/indexeddb/WebIDBMetadata.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"
#include "public/web/WebAXEnums.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebConsoleMessage.h"
#include "public/web/WebContentSecurityPolicy.h"
#include "public/web/WebFontDescription.h"
#include "public/web/WebFormElement.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebFrameLoadType.h"
#include "public/web/WebGeolocationError.h"
#include "public/web/WebGeolocationPosition.h"
#include "public/web/WebHistoryCommitType.h"
#include "public/web/WebHistoryItem.h"
#include "public/web/WebIconURL.h"
#include "public/web/WebInputElement.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebNavigationPolicy.h"
#include "public/web/WebNavigatorContentUtilsClient.h"
#include "public/web/WebRemoteFrameClient.h"
#include "public/web/WebSandboxFlags.h"
#include "public/web/WebSecurityPolicy.h"
#include "public/web/WebSelection.h"
#include "public/web/WebSerializedScriptValueVersion.h"
#include "public/web/WebSettings.h"
#include "public/web/WebSpeechRecognizerClient.h"
#include "public/web/WebTextAffinity.h"
#include "public/web/WebTextCheckingResult.h"
#include "public/web/WebTextCheckingType.h"
#include "public/web/WebTextDecorationType.h"
#include "public/web/WebTouchAction.h"
#include "public/web/WebView.h"
#include "wtf/Assertions.h"
#include "wtf/text/StringImpl.h"

namespace blink {

#define STATIC_ASSERT_MATCHING_ENUM(public_name, core_name) \
    static_assert(int(public_name) == int(core_name), "mismatching enum values")

#define STATIC_ASSERT_MATCHING_UINT64(public_name, core_name) \
    static_assert(public_name == core_name, "mismatching enum values")

STATIC_ASSERT_MATCHING_ENUM(WebAXEventActiveDescendantChanged, AXObjectCache::AXActiveDescendantChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventAlert, AXObjectCache::AXAlert);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventAriaAttributeChanged, AXObjectCache::AXAriaAttributeChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventAutocorrectionOccured, AXObjectCache::AXAutocorrectionOccured);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventBlur, AXObjectCache::AXBlur);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventCheckedStateChanged, AXObjectCache::AXCheckedStateChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventChildrenChanged, AXObjectCache::AXChildrenChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventFocus, AXObjectCache::AXFocusedUIElementChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventHide, AXObjectCache::AXHide);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventInvalidStatusChanged, AXObjectCache::AXInvalidStatusChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventLayoutComplete, AXObjectCache::AXLayoutComplete);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventLiveRegionChanged, AXObjectCache::AXLiveRegionChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventLoadComplete, AXObjectCache::AXLoadComplete);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventLocationChanged, AXObjectCache::AXLocationChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventMenuListItemSelected, AXObjectCache::AXMenuListItemSelected);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventMenuListItemUnselected, AXObjectCache::AXMenuListItemUnselected);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventMenuListValueChanged, AXObjectCache::AXMenuListValueChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventRowCollapsed, AXObjectCache::AXRowCollapsed);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventRowCountChanged, AXObjectCache::AXRowCountChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventRowExpanded, AXObjectCache::AXRowExpanded);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventScrollPositionChanged, AXObjectCache::AXScrollPositionChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventScrolledToAnchor, AXObjectCache::AXScrolledToAnchor);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventSelectedChildrenChanged, AXObjectCache::AXSelectedChildrenChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventSelectedTextChanged, AXObjectCache::AXSelectedTextChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventShow, AXObjectCache::AXShow);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventTextChanged, AXObjectCache::AXTextChanged);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventTextInserted, AXObjectCache::AXTextInserted);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventTextRemoved, AXObjectCache::AXTextRemoved);
STATIC_ASSERT_MATCHING_ENUM(WebAXEventValueChanged, AXObjectCache::AXValueChanged);

STATIC_ASSERT_MATCHING_ENUM(WebAXRoleAlertDialog, AlertDialogRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleAlert, AlertRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleAnnotation, AnnotationRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleApplication, ApplicationRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleArticle, ArticleRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleBanner, BannerRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleBlockquote, BlockquoteRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleBusyIndicator, BusyIndicatorRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleButton, ButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleCanvas, CanvasRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleCaption, CaptionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleCell, CellRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleCheckBox, CheckBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleColorWell, ColorWellRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleColumnHeader, ColumnHeaderRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleColumn, ColumnRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleComboBox, ComboBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleComplementary, ComplementaryRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleContentInfo, ContentInfoRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDate, DateRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDateTime, DateTimeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDefinition, DefinitionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDescriptionListDetail, DescriptionListDetailRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDescriptionList, DescriptionListRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDescriptionListTerm, DescriptionListTermRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDetails, DetailsRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDialog, DialogRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDirectory, DirectoryRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDisclosureTriangle, DisclosureTriangleRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDiv, DivRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleDocument, DocumentRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleEmbeddedObject, EmbeddedObjectRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleFigcaption, FigcaptionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleFigure, FigureRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleFooter, FooterRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleForm, FormRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleGrid, GridRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleGroup, GroupRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleHeading, HeadingRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleIframe, IframeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleIframePresentational, IframePresentationalRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleIgnored, IgnoredRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleImageMapLink, ImageMapLinkRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleImageMap, ImageMapRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleImage, ImageRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleInlineTextBox, InlineTextBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleInputTime, InputTimeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleLabel, LabelRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleLegend, LegendRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleLink, LinkRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleListBoxOption, ListBoxOptionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleListBox, ListBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleListItem, ListItemRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleListMarker, ListMarkerRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleList, ListRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleLog, LogRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMain, MainRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMark, MarkRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMarquee, MarqueeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMath, MathRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuBar, MenuBarRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuButton, MenuButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuItem, MenuItemRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuItemCheckBox, MenuItemCheckBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuItemRadio, MenuItemRadioRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuListOption, MenuListOptionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenuListPopup, MenuListPopupRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMenu, MenuRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleMeter, MeterRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleNavigation, NavigationRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleNone, NoneRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleNote, NoteRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleOutline, OutlineRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleParagraph, ParagraphRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRolePopUpButton, PopUpButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRolePre, PreRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRolePresentational, PresentationalRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleProgressIndicator, ProgressIndicatorRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRadioButton, RadioButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRadioGroup, RadioGroupRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRegion, RegionRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRootWebArea, RootWebAreaRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleLineBreak, LineBreakRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRowHeader, RowHeaderRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRow, RowRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRuby, RubyRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleRuler, RulerRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSVGRoot, SVGRootRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleScrollArea, ScrollAreaRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleScrollBar, ScrollBarRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSeamlessWebArea, SeamlessWebAreaRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSearch, SearchRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSearchBox, SearchBoxRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSlider, SliderRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSliderThumb, SliderThumbRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSpinButtonPart, SpinButtonPartRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSpinButton, SpinButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSplitter, SplitterRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleStaticText, StaticTextRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleStatus, StatusRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleSwitch, SwitchRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTabGroup, TabGroupRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTabList, TabListRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTabPanel, TabPanelRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTab, TabRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTableHeaderContainer, TableHeaderContainerRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTable, TableRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTextField, TextFieldRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTime, TimeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTimer, TimerRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleToggleButton, ToggleButtonRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleToolbar, ToolbarRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTreeGrid, TreeGridRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTreeItem, TreeItemRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleTree, TreeRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleUnknown, UnknownRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleUserInterfaceTooltip, UserInterfaceTooltipRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleWebArea, WebAreaRole);
STATIC_ASSERT_MATCHING_ENUM(WebAXRoleWindow, WindowRole);

STATIC_ASSERT_MATCHING_ENUM(WebAXStateBusy, AXBusyState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateChecked, AXCheckedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateEnabled, AXEnabledState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateExpanded, AXExpandedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateFocusable, AXFocusableState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateFocused, AXFocusedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateHaspopup, AXHaspopupState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateHovered, AXHoveredState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateIndeterminate, AXIndeterminateState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateInvisible, AXInvisibleState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateLinked, AXLinkedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateMultiline, AXMultilineState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateMultiselectable, AXMultiselectableState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateOffscreen, AXOffscreenState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStatePressed, AXPressedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateProtected, AXProtectedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateReadonly, AXReadonlyState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateRequired, AXRequiredState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateSelectable, AXSelectableState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateSelected, AXSelectedState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateVertical, AXVerticalState);
STATIC_ASSERT_MATCHING_ENUM(WebAXStateVisited, AXVisitedState);

STATIC_ASSERT_MATCHING_ENUM(WebAXTextDirectionLR, AccessibilityTextDirectionLTR);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextDirectionRL, AccessibilityTextDirectionRTL);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextDirectionTB, AccessibilityTextDirectionTTB);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextDirectionBT, AccessibilityTextDirectionBTT);

STATIC_ASSERT_MATCHING_ENUM(WebAXSortDirectionUndefined, SortDirectionUndefined);
STATIC_ASSERT_MATCHING_ENUM(WebAXSortDirectionNone, SortDirectionNone);
STATIC_ASSERT_MATCHING_ENUM(WebAXSortDirectionAscending, SortDirectionAscending);
STATIC_ASSERT_MATCHING_ENUM(WebAXSortDirectionDescending, SortDirectionDescending);
STATIC_ASSERT_MATCHING_ENUM(WebAXSortDirectionOther, SortDirectionOther);

STATIC_ASSERT_MATCHING_ENUM(WebAXExpandedUndefined, ExpandedUndefined);
STATIC_ASSERT_MATCHING_ENUM(WebAXExpandedCollapsed, ExpandedCollapsed);
STATIC_ASSERT_MATCHING_ENUM(WebAXExpandedExpanded, ExpandedExpanded);

STATIC_ASSERT_MATCHING_ENUM(WebAXOrientationUndefined, AccessibilityOrientationUndefined);
STATIC_ASSERT_MATCHING_ENUM(WebAXOrientationVertical, AccessibilityOrientationVertical);
STATIC_ASSERT_MATCHING_ENUM(WebAXOrientationHorizontal, AccessibilityOrientationHorizontal);

STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateUndefined, InvalidStateUndefined);
STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateFalse, InvalidStateFalse);
STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateTrue, InvalidStateTrue);
STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateSpelling, InvalidStateSpelling);
STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateGrammar, InvalidStateGrammar);
STATIC_ASSERT_MATCHING_ENUM(WebAXInvalidStateOther, InvalidStateOther);

STATIC_ASSERT_MATCHING_ENUM(WebAXTextStyleNone, TextStyleNone);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextStyleBold, TextStyleBold);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextStyleItalic, TextStyleItalic);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextStyleUnderline, TextStyleUnderline);
STATIC_ASSERT_MATCHING_ENUM(WebAXTextStyleLineThrough, TextStyleLineThrough);

STATIC_ASSERT_MATCHING_ENUM(WebAXNameFromAttribute, AXNameFromAttribute);
STATIC_ASSERT_MATCHING_ENUM(WebAXNameFromContents, AXNameFromContents);
STATIC_ASSERT_MATCHING_ENUM(WebAXNameFromPlaceholder, AXNameFromPlaceholder);
STATIC_ASSERT_MATCHING_ENUM(WebAXNameFromRelatedElement, AXNameFromRelatedElement);

STATIC_ASSERT_MATCHING_ENUM(WebAXDescriptionFromPlaceholder, AXDescriptionFromPlaceholder);
STATIC_ASSERT_MATCHING_ENUM(WebAXDescriptionFromRelatedElement, AXDescriptionFromRelatedElement);

STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::Uncached, ApplicationCacheHost::UNCACHED);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::Idle, ApplicationCacheHost::IDLE);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::Checking, ApplicationCacheHost::CHECKING);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::Downloading, ApplicationCacheHost::DOWNLOADING);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::UpdateReady, ApplicationCacheHost::UPDATEREADY);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::Obsolete, ApplicationCacheHost::OBSOLETE);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::CheckingEvent, ApplicationCacheHost::CHECKING_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::ErrorEvent, ApplicationCacheHost::ERROR_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::NoUpdateEvent, ApplicationCacheHost::NOUPDATE_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::DownloadingEvent, ApplicationCacheHost::DOWNLOADING_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::ProgressEvent, ApplicationCacheHost::PROGRESS_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::UpdateReadyEvent, ApplicationCacheHost::UPDATEREADY_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::CachedEvent, ApplicationCacheHost::CACHED_EVENT);
STATIC_ASSERT_MATCHING_ENUM(WebApplicationCacheHost::ObsoleteEvent, ApplicationCacheHost::OBSOLETE_EVENT);

STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypePointer, Cursor::Pointer);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeCross, Cursor::Cross);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeHand, Cursor::Hand);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeIBeam, Cursor::IBeam);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeWait, Cursor::Wait);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeHelp, Cursor::Help);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeEastResize, Cursor::EastResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthResize, Cursor::NorthResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthEastResize, Cursor::NorthEastResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthWestResize, Cursor::NorthWestResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthResize, Cursor::SouthResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthEastResize, Cursor::SouthEastResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthWestResize, Cursor::SouthWestResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeWestResize, Cursor::WestResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthSouthResize, Cursor::NorthSouthResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeEastWestResize, Cursor::EastWestResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthEastSouthWestResize, Cursor::NorthEastSouthWestResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthWestSouthEastResize, Cursor::NorthWestSouthEastResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeColumnResize, Cursor::ColumnResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeRowResize, Cursor::RowResize);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeMiddlePanning, Cursor::MiddlePanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeEastPanning, Cursor::EastPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthPanning, Cursor::NorthPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthEastPanning, Cursor::NorthEastPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNorthWestPanning, Cursor::NorthWestPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthPanning, Cursor::SouthPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthEastPanning, Cursor::SouthEastPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeSouthWestPanning, Cursor::SouthWestPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeWestPanning, Cursor::WestPanning);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeMove, Cursor::Move);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeVerticalText, Cursor::VerticalText);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeCell, Cursor::Cell);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeContextMenu, Cursor::ContextMenu);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeAlias, Cursor::Alias);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeProgress, Cursor::Progress);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNoDrop, Cursor::NoDrop);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeCopy, Cursor::Copy);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNone, Cursor::None);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeNotAllowed, Cursor::NotAllowed);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeZoomIn, Cursor::ZoomIn);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeZoomOut, Cursor::ZoomOut);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeGrab, Cursor::Grab);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeGrabbing, Cursor::Grabbing);
STATIC_ASSERT_MATCHING_ENUM(WebCursorInfo::TypeCustom, Cursor::Custom);

STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilyNone, FontDescription::NoFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilyStandard, FontDescription::StandardFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilySerif, FontDescription::SerifFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilySansSerif, FontDescription::SansSerifFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilyMonospace, FontDescription::MonospaceFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilyCursive, FontDescription::CursiveFamily);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::GenericFamilyFantasy, FontDescription::FantasyFamily);

STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::SmoothingAuto, AutoSmoothing);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::SmoothingNone, NoSmoothing);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::SmoothingGrayscale, Antialiased);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::SmoothingSubpixel, SubpixelAntialiased);

STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight100, FontWeight100);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight200, FontWeight200);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight300, FontWeight300);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight400, FontWeight400);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight500, FontWeight500);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight600, FontWeight600);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight700, FontWeight700);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight800, FontWeight800);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::Weight900, FontWeight900);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::WeightNormal, FontWeightNormal);
STATIC_ASSERT_MATCHING_ENUM(WebFontDescription::WeightBold, FontWeightBold);

STATIC_ASSERT_MATCHING_ENUM(WebIconURL::TypeInvalid, InvalidIcon);
STATIC_ASSERT_MATCHING_ENUM(WebIconURL::TypeFavicon, Favicon);
STATIC_ASSERT_MATCHING_ENUM(WebIconURL::TypeTouch, TouchIcon);
STATIC_ASSERT_MATCHING_ENUM(WebIconURL::TypeTouchPrecomposed, TouchPrecomposedIcon);

STATIC_ASSERT_MATCHING_ENUM(WebNode::ElementNode, Node::ELEMENT_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::AttributeNode, Node::ATTRIBUTE_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::TextNode, Node::TEXT_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::CDataSectionNode, Node::CDATA_SECTION_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::ProcessingInstructionsNode, Node::PROCESSING_INSTRUCTION_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::CommentNode, Node::COMMENT_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::DocumentNode, Node::DOCUMENT_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::DocumentTypeNode, Node::DOCUMENT_TYPE_NODE);
STATIC_ASSERT_MATCHING_ENUM(WebNode::DocumentFragmentNode, Node::DOCUMENT_FRAGMENT_NODE);

STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::ReadyStateHaveNothing, HTMLMediaElement::HAVE_NOTHING);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::ReadyStateHaveMetadata, HTMLMediaElement::HAVE_METADATA);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::ReadyStateHaveCurrentData, HTMLMediaElement::HAVE_CURRENT_DATA);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::ReadyStateHaveFutureData, HTMLMediaElement::HAVE_FUTURE_DATA);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::ReadyStateHaveEnoughData, HTMLMediaElement::HAVE_ENOUGH_DATA);

STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::PreloadNone, MediaPlayer::None);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::PreloadMetaData, MediaPlayer::MetaData);
STATIC_ASSERT_MATCHING_ENUM(WebMediaPlayer::PreloadAuto, MediaPlayer::Auto);

STATIC_ASSERT_MATCHING_ENUM(WebMouseEvent::ButtonNone, NoButton);
STATIC_ASSERT_MATCHING_ENUM(WebMouseEvent::ButtonLeft, LeftButton);
STATIC_ASSERT_MATCHING_ENUM(WebMouseEvent::ButtonMiddle, MiddleButton);
STATIC_ASSERT_MATCHING_ENUM(WebMouseEvent::ButtonRight, RightButton);

STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::Horizontal, HorizontalScrollbar);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::Vertical, VerticalScrollbar);

STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollByLine, ScrollByLine);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollByPage, ScrollByPage);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollByDocument, ScrollByDocument);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollByPixel, ScrollByPixel);

STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::RegularScrollbar, RegularScrollbar);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::SmallScrollbar, SmallScrollbar);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::NoPart, NoPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::BackButtonStartPart, BackButtonStartPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ForwardButtonStartPart, ForwardButtonStartPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::BackTrackPart, BackTrackPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ThumbPart, ThumbPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ForwardTrackPart, ForwardTrackPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::BackButtonEndPart, BackButtonEndPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ForwardButtonEndPart, ForwardButtonEndPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollbarBGPart, ScrollbarBGPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::TrackBGPart, TrackBGPart);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::AllParts, AllParts);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollbarOverlayStyleDefault, ScrollbarOverlayStyleDefault);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollbarOverlayStyleDark, ScrollbarOverlayStyleDark);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbar::ScrollbarOverlayStyleLight, ScrollbarOverlayStyleLight);

STATIC_ASSERT_MATCHING_ENUM(WebScrollbarBehavior::ButtonNone, NoButton);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbarBehavior::ButtonLeft, LeftButton);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbarBehavior::ButtonMiddle, MiddleButton);
STATIC_ASSERT_MATCHING_ENUM(WebScrollbarBehavior::ButtonRight, RightButton);

STATIC_ASSERT_MATCHING_ENUM(WebSettings::EditingBehaviorMac, EditingMacBehavior);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::EditingBehaviorWin, EditingWindowsBehavior);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::EditingBehaviorUnix, EditingUnixBehavior);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::EditingBehaviorAndroid, EditingAndroidBehavior);

STATIC_ASSERT_MATCHING_ENUM(WebTextAffinityUpstream, UPSTREAM);
STATIC_ASSERT_MATCHING_ENUM(WebTextAffinityDownstream, DOWNSTREAM);

STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionUnknownError, UnknownError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionConstraintError, ConstraintError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionDataError, DataError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionVersionError, VersionError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionAbortError, AbortError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionQuotaError, QuotaExceededError);
STATIC_ASSERT_MATCHING_ENUM(WebIDBDatabaseExceptionTimeoutError, TimeoutError);

STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeInvalid, IDBKey::InvalidType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeArray, IDBKey::ArrayType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeBinary, IDBKey::BinaryType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeString, IDBKey::StringType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeDate, IDBKey::DateType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyTypeNumber, IDBKey::NumberType);

STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyPathTypeNull, IDBKeyPath::NullType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyPathTypeString, IDBKeyPath::StringType);
STATIC_ASSERT_MATCHING_ENUM(WebIDBKeyPathTypeArray, IDBKeyPath::ArrayType);

STATIC_ASSERT_MATCHING_ENUM(WebIDBMetadata::NoIntVersion, IDBDatabaseMetadata::NoIntVersion);

STATIC_ASSERT_MATCHING_ENUM(WebFileSystem::TypeTemporary, FileSystemTypeTemporary);
STATIC_ASSERT_MATCHING_ENUM(WebFileSystem::TypePersistent, FileSystemTypePersistent);
STATIC_ASSERT_MATCHING_ENUM(WebFileSystem::TypeExternal, FileSystemTypeExternal);
STATIC_ASSERT_MATCHING_ENUM(WebFileSystem::TypeIsolated, FileSystemTypeIsolated);
STATIC_ASSERT_MATCHING_ENUM(WebFileInfo::TypeUnknown, FileMetadata::TypeUnknown);
STATIC_ASSERT_MATCHING_ENUM(WebFileInfo::TypeFile, FileMetadata::TypeFile);
STATIC_ASSERT_MATCHING_ENUM(WebFileInfo::TypeDirectory, FileMetadata::TypeDirectory);

STATIC_ASSERT_MATCHING_ENUM(WebFileErrorNotFound, FileError::NOT_FOUND_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorSecurity, FileError::SECURITY_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorAbort, FileError::ABORT_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorNotReadable, FileError::NOT_READABLE_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorEncoding, FileError::ENCODING_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorNoModificationAllowed, FileError::NO_MODIFICATION_ALLOWED_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorInvalidState, FileError::INVALID_STATE_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorSyntax, FileError::SYNTAX_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorInvalidModification, FileError::INVALID_MODIFICATION_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorQuotaExceeded, FileError::QUOTA_EXCEEDED_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorTypeMismatch, FileError::TYPE_MISMATCH_ERR);
STATIC_ASSERT_MATCHING_ENUM(WebFileErrorPathExists, FileError::PATH_EXISTS_ERR);

STATIC_ASSERT_MATCHING_ENUM(WebGeolocationError::ErrorPermissionDenied, GeolocationError::PermissionDenied);
STATIC_ASSERT_MATCHING_ENUM(WebGeolocationError::ErrorPositionUnavailable, GeolocationError::PositionUnavailable);

STATIC_ASSERT_MATCHING_ENUM(WebTextCheckingTypeSpelling, TextCheckingTypeSpelling);
STATIC_ASSERT_MATCHING_ENUM(WebTextCheckingTypeGrammar, TextCheckingTypeGrammar);

// TODO(rouslan): Remove these comparisons between text-checking and text-decoration enum values after removing the
// deprecated constructor WebTextCheckingResult(WebTextCheckingType).
STATIC_ASSERT_MATCHING_ENUM(WebTextCheckingTypeSpelling, TextDecorationTypeSpelling);
STATIC_ASSERT_MATCHING_ENUM(WebTextCheckingTypeGrammar, TextDecorationTypeGrammar);

STATIC_ASSERT_MATCHING_ENUM(WebTextDecorationTypeSpelling, TextDecorationTypeSpelling);
STATIC_ASSERT_MATCHING_ENUM(WebTextDecorationTypeGrammar, TextDecorationTypeGrammar);
STATIC_ASSERT_MATCHING_ENUM(WebTextDecorationTypeInvisibleSpellcheck, TextDecorationTypeInvisibleSpellcheck);

STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaErrorNotSupported, NotSupportedError);
STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaErrorInvalidModification, InvalidModificationError);
STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaErrorInvalidAccess, InvalidAccessError);
STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaErrorAbort, AbortError);

STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaTypeTemporary, DeprecatedStorageQuota::Temporary);
STATIC_ASSERT_MATCHING_ENUM(WebStorageQuotaTypePersistent, DeprecatedStorageQuota::Persistent);

STATIC_ASSERT_MATCHING_ENUM(WebPageVisibilityStateVisible, PageVisibilityStateVisible);
STATIC_ASSERT_MATCHING_ENUM(WebPageVisibilityStateHidden, PageVisibilityStateHidden);
STATIC_ASSERT_MATCHING_ENUM(WebPageVisibilityStatePrerender, PageVisibilityStatePrerender);

STATIC_ASSERT_MATCHING_ENUM(WebMediaStreamSource::TypeAudio, MediaStreamSource::TypeAudio);
STATIC_ASSERT_MATCHING_ENUM(WebMediaStreamSource::TypeVideo, MediaStreamSource::TypeVideo);
STATIC_ASSERT_MATCHING_ENUM(WebMediaStreamSource::ReadyStateLive, MediaStreamSource::ReadyStateLive);
STATIC_ASSERT_MATCHING_ENUM(WebMediaStreamSource::ReadyStateMuted, MediaStreamSource::ReadyStateMuted);
STATIC_ASSERT_MATCHING_ENUM(WebMediaStreamSource::ReadyStateEnded, MediaStreamSource::ReadyStateEnded);

STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::OtherError, SpeechRecognitionError::ErrorCodeOther);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::NoSpeechError, SpeechRecognitionError::ErrorCodeNoSpeech);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::AbortedError, SpeechRecognitionError::ErrorCodeAborted);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::AudioCaptureError, SpeechRecognitionError::ErrorCodeAudioCapture);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::NetworkError, SpeechRecognitionError::ErrorCodeNetwork);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::NotAllowedError, SpeechRecognitionError::ErrorCodeNotAllowed);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::ServiceNotAllowedError, SpeechRecognitionError::ErrorCodeServiceNotAllowed);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::BadGrammarError, SpeechRecognitionError::ErrorCodeBadGrammar);
STATIC_ASSERT_MATCHING_ENUM(WebSpeechRecognizerClient::LanguageNotSupportedError, SpeechRecognitionError::ErrorCodeLanguageNotSupported);

STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyAlways, ReferrerPolicyAlways);
STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyDefault, ReferrerPolicyDefault);
STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyNoReferrerWhenDowngrade, ReferrerPolicyNoReferrerWhenDowngrade);
STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyNever, ReferrerPolicyNever);
STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyOrigin, ReferrerPolicyOrigin);
STATIC_ASSERT_MATCHING_ENUM(WebReferrerPolicyOriginWhenCrossOrigin, ReferrerPolicyOriginWhenCrossOrigin);

STATIC_ASSERT_MATCHING_ENUM(WebContentSecurityPolicyTypeReport, ContentSecurityPolicyHeaderTypeReport);
STATIC_ASSERT_MATCHING_ENUM(WebContentSecurityPolicyTypeEnforce, ContentSecurityPolicyHeaderTypeEnforce);

STATIC_ASSERT_MATCHING_ENUM(WebURLResponse::Unknown, ResourceResponse::Unknown);
STATIC_ASSERT_MATCHING_ENUM(WebURLResponse::HTTP_0_9, ResourceResponse::HTTP_0_9);
STATIC_ASSERT_MATCHING_ENUM(WebURLResponse::HTTP_1_0, ResourceResponse::HTTP_1_0);
STATIC_ASSERT_MATCHING_ENUM(WebURLResponse::HTTP_1_1, ResourceResponse::HTTP_1_1);

STATIC_ASSERT_MATCHING_ENUM(WebFormElement::AutocompleteResultSuccess, HTMLFormElement::AutocompleteResultSuccess);
STATIC_ASSERT_MATCHING_ENUM(WebFormElement::AutocompleteResultErrorDisabled, HTMLFormElement::AutocompleteResultErrorDisabled);
STATIC_ASSERT_MATCHING_ENUM(WebFormElement::AutocompleteResultErrorCancel, HTMLFormElement::AutocompleteResultErrorCancel);
STATIC_ASSERT_MATCHING_ENUM(WebFormElement::AutocompleteResultErrorInvalid, HTMLFormElement::AutocompleteResultErrorInvalid);

STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityUnresolved, ResourceLoadPriorityUnresolved);
STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityVeryLow, ResourceLoadPriorityVeryLow);
STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityLow, ResourceLoadPriorityLow);
STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityMedium, ResourceLoadPriorityMedium);
STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityHigh, ResourceLoadPriorityHigh);
STATIC_ASSERT_MATCHING_ENUM(WebURLRequest::PriorityVeryHigh, ResourceLoadPriorityVeryHigh);

STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyIgnore, NavigationPolicyIgnore);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyDownload, NavigationPolicyDownload);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyCurrentTab, NavigationPolicyCurrentTab);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyNewBackgroundTab, NavigationPolicyNewBackgroundTab);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyNewForegroundTab, NavigationPolicyNewForegroundTab);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyNewWindow, NavigationPolicyNewWindow);
STATIC_ASSERT_MATCHING_ENUM(WebNavigationPolicyNewPopup, NavigationPolicyNewPopup);

STATIC_ASSERT_MATCHING_ENUM(WebStandardCommit, StandardCommit);
STATIC_ASSERT_MATCHING_ENUM(WebBackForwardCommit, BackForwardCommit);
STATIC_ASSERT_MATCHING_ENUM(WebInitialCommitInChildFrame, InitialCommitInChildFrame);
STATIC_ASSERT_MATCHING_ENUM(WebHistoryInertCommit, HistoryInertCommit);

STATIC_ASSERT_MATCHING_ENUM(WebHistorySameDocumentLoad, HistorySameDocumentLoad);
STATIC_ASSERT_MATCHING_ENUM(WebHistoryDifferentDocumentLoad, HistoryDifferentDocumentLoad);

STATIC_ASSERT_MATCHING_ENUM(WebHistoryScrollRestorationManual, ScrollRestorationManual);
STATIC_ASSERT_MATCHING_ENUM(WebHistoryScrollRestorationAuto, ScrollRestorationAuto);

STATIC_ASSERT_MATCHING_ENUM(WebConsoleMessage::LevelDebug, DebugMessageLevel);
STATIC_ASSERT_MATCHING_ENUM(WebConsoleMessage::LevelLog, LogMessageLevel);
STATIC_ASSERT_MATCHING_ENUM(WebConsoleMessage::LevelWarning, WarningMessageLevel);
STATIC_ASSERT_MATCHING_ENUM(WebConsoleMessage::LevelError, ErrorMessageLevel);
STATIC_ASSERT_MATCHING_ENUM(WebConsoleMessage::LevelInfo, InfoMessageLevel);

STATIC_ASSERT_MATCHING_ENUM(WebCustomHandlersNew, NavigatorContentUtilsClient::CustomHandlersNew);
STATIC_ASSERT_MATCHING_ENUM(WebCustomHandlersRegistered, NavigatorContentUtilsClient::CustomHandlersRegistered);
STATIC_ASSERT_MATCHING_ENUM(WebCustomHandlersDeclined, NavigatorContentUtilsClient::CustomHandlersDeclined);

STATIC_ASSERT_MATCHING_ENUM(WebTouchActionNone, TouchActionNone);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionAuto, TouchActionAuto);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanLeft, TouchActionPanLeft);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanRight, TouchActionPanRight);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanX, TouchActionPanX);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanUp, TouchActionPanUp);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanDown, TouchActionPanDown);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPanY, TouchActionPanY);
STATIC_ASSERT_MATCHING_ENUM(WebTouchActionPinchZoom, TouchActionPinchZoom);

STATIC_ASSERT_MATCHING_ENUM(WebSelection::NoSelection, NoSelection);
STATIC_ASSERT_MATCHING_ENUM(WebSelection::CaretSelection, CaretSelection);
STATIC_ASSERT_MATCHING_ENUM(WebSelection::RangeSelection, RangeSelection);

STATIC_ASSERT_MATCHING_ENUM(WebSettings::ImageAnimationPolicyAllowed, ImageAnimationPolicyAllowed);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::ImageAnimationPolicyAnimateOnce, ImageAnimationPolicyAnimateOnce);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::ImageAnimationPolicyNoAnimation, ImageAnimationPolicyNoAnimation);

STATIC_ASSERT_MATCHING_ENUM(WebSettings::V8CacheOptionsDefault, V8CacheOptionsDefault);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::V8CacheOptionsNone, V8CacheOptionsNone);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::V8CacheOptionsParse, V8CacheOptionsParse);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::V8CacheOptionsCode, V8CacheOptionsCode);

STATIC_ASSERT_MATCHING_ENUM(WebSettings::PointerTypeNone, PointerTypeNone);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::PointerTypeCoarse, PointerTypeCoarse);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::PointerTypeFine, PointerTypeFine);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::HoverTypeNone, HoverTypeNone);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::HoverTypeOnDemand, HoverTypeOnDemand);
STATIC_ASSERT_MATCHING_ENUM(WebSettings::HoverTypeHover, HoverTypeHover);

STATIC_ASSERT_MATCHING_ENUM(WebSecurityPolicy::PolicyAreaNone, SchemeRegistry::PolicyAreaNone);
STATIC_ASSERT_MATCHING_ENUM(WebSecurityPolicy::PolicyAreaImage, SchemeRegistry::PolicyAreaImage);
STATIC_ASSERT_MATCHING_ENUM(WebSecurityPolicy::PolicyAreaStyle, SchemeRegistry::PolicyAreaStyle);
STATIC_ASSERT_MATCHING_ENUM(WebSecurityPolicy::PolicyAreaAll, SchemeRegistry::PolicyAreaAll);

STATIC_ASSERT_MATCHING_UINT64(kSerializedScriptValueVersion, SerializedScriptValue::wireFormatVersion);

STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::None, SandboxNone);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Navigation, SandboxNavigation);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Plugins, SandboxPlugins);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Origin, SandboxOrigin);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Forms, SandboxForms);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Scripts, SandboxScripts);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::TopNavigation, SandboxTopNavigation);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Popups, SandboxPopups);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::AutomaticFeatures, SandboxAutomaticFeatures);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::PointerLock, SandboxPointerLock);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::DocumentDomain, SandboxDocumentDomain);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::OrientationLock, SandboxOrientationLock);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::PropagatesToAuxiliaryBrowsingContexts, SandboxPropagatesToAuxiliaryBrowsingContexts);
STATIC_ASSERT_MATCHING_ENUM(WebSandboxFlags::Modals, SandboxModals);

STATIC_ASSERT_MATCHING_ENUM(FrameLoaderClient::BeforeUnloadHandler, WebFrameClient::BeforeUnloadHandler);
STATIC_ASSERT_MATCHING_ENUM(FrameLoaderClient::UnloadHandler, WebFrameClient::UnloadHandler);

STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::Standard, FrameLoadTypeStandard);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::BackForward, FrameLoadTypeBackForward);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::Reload, FrameLoadTypeReload);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::Same, FrameLoadTypeSame);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::RedirectWithLockedBackForwardList, FrameLoadTypeRedirectWithLockedBackForwardList);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::InitialInChildFrame, FrameLoadTypeInitialInChildFrame);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::InitialHistoryLoad, FrameLoadTypeInitialHistoryLoad);
STATIC_ASSERT_MATCHING_ENUM(WebFrameLoadType::ReloadFromOrigin, FrameLoadTypeReloadFromOrigin);

STATIC_ASSERT_MATCHING_ENUM(FrameDetachType::Remove, WebFrameClient::DetachType::Remove);
STATIC_ASSERT_MATCHING_ENUM(FrameDetachType::Swap, WebFrameClient::DetachType::Swap);
STATIC_ASSERT_MATCHING_ENUM(FrameDetachType::Remove, WebRemoteFrameClient::DetachType::Remove);
STATIC_ASSERT_MATCHING_ENUM(FrameDetachType::Swap, WebRemoteFrameClient::DetachType::Swap);
} // namespace blink
