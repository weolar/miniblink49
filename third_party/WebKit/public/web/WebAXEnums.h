/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef WebAXEnums_h
#define WebAXEnums_h

namespace blink {

// Accessibility events sent from Blink to the embedder.
// These values must match blink::AXObjectCache::AXNotification values.
// Enforced in AssertMatchingEnums.cpp.
enum WebAXEvent {
    WebAXEventActiveDescendantChanged,
    WebAXEventAlert,
    WebAXEventAriaAttributeChanged,
    WebAXEventAutocorrectionOccured,
    WebAXEventBlur,
    WebAXEventCheckedStateChanged,
    WebAXEventChildrenChanged,
    WebAXEventFocus,
    WebAXEventHide,
    WebAXEventInvalidStatusChanged,
    WebAXEventLayoutComplete,
    WebAXEventLiveRegionChanged,
    WebAXEventLoadComplete,
    WebAXEventLocationChanged,
    WebAXEventMenuListItemSelected,
    WebAXEventMenuListItemUnselected,
    WebAXEventMenuListValueChanged,
    WebAXEventRowCollapsed,
    WebAXEventRowCountChanged,
    WebAXEventRowExpanded,
    WebAXEventScrollPositionChanged,
    WebAXEventScrolledToAnchor,
    WebAXEventSelectedChildrenChanged,
    WebAXEventSelectedTextChanged,
    WebAXEventShow,
    WebAXEventTextChanged,
    WebAXEventTextInserted,
    WebAXEventTextRemoved,
    WebAXEventValueChanged
};

// Accessibility roles.
// These values must match blink::AccessibilityRole values.
// Enforced in AssertMatchingEnums.cpp.
enum WebAXRole {
    WebAXRoleUnknown = 0,
    WebAXRoleAlertDialog,
    WebAXRoleAlert,
    WebAXRoleAnnotation,
    WebAXRoleApplication,
    WebAXRoleArticle,
    WebAXRoleBanner,
    WebAXRoleBlockquote,
    WebAXRoleBusyIndicator,
    WebAXRoleButton,
    WebAXRoleCanvas,
    WebAXRoleCaption,
    WebAXRoleCell,
    WebAXRoleCheckBox,
    WebAXRoleColorWell,
    WebAXRoleColumnHeader,
    WebAXRoleColumn,
    WebAXRoleComboBox,
    WebAXRoleComplementary,
    WebAXRoleContentInfo,
    WebAXRoleDate,
    WebAXRoleDateTime,
    WebAXRoleDefinition,
    WebAXRoleDescriptionListDetail,
    WebAXRoleDescriptionList,
    WebAXRoleDescriptionListTerm,
    WebAXRoleDetails,
    WebAXRoleDialog,
    WebAXRoleDirectory,
    WebAXRoleDisclosureTriangle,
    WebAXRoleDiv,
    WebAXRoleDocument,
    WebAXRoleEmbeddedObject,
    WebAXRoleFigcaption,
    WebAXRoleFigure,
    WebAXRoleFooter,
    WebAXRoleForm,
    WebAXRoleGrid,
    WebAXRoleGroup,
    WebAXRoleHeading,
    WebAXRoleIframePresentational,
    WebAXRoleIframe,
    WebAXRoleIgnored,
    WebAXRoleImageMapLink,
    WebAXRoleImageMap,
    WebAXRoleImage,
    WebAXRoleInlineTextBox,
    WebAXRoleInputTime,
    WebAXRoleLabel,
    WebAXRoleLegend,
    WebAXRoleLink,
    WebAXRoleListBoxOption,
    WebAXRoleListBox,
    WebAXRoleListItem,
    WebAXRoleListMarker,
    WebAXRoleList,
    WebAXRoleLog,
    WebAXRoleMain,
    WebAXRoleMark,
    WebAXRoleMarquee,
    WebAXRoleMath,
    WebAXRoleMenuBar,
    WebAXRoleMenuButton,
    WebAXRoleMenuItem,
    WebAXRoleMenuItemCheckBox,
    WebAXRoleMenuItemRadio,
    WebAXRoleMenuListOption,
    WebAXRoleMenuListPopup,
    WebAXRoleMenu,
    WebAXRoleMeter,
    WebAXRoleNavigation,
    WebAXRoleNone,
    WebAXRoleNote,
    WebAXRoleOutline,
    WebAXRoleParagraph,
    WebAXRolePopUpButton,
    WebAXRolePre,
    WebAXRolePresentational,
    WebAXRoleProgressIndicator,
    WebAXRoleRadioButton,
    WebAXRoleRadioGroup,
    WebAXRoleRegion,
    WebAXRoleRootWebArea,
    WebAXRoleRowHeader,
    WebAXRoleRow,
    WebAXRoleRuby,
    WebAXRoleRuler,
    WebAXRoleSVGRoot,
    WebAXRoleScrollArea,
    WebAXRoleScrollBar,
    WebAXRoleSeamlessWebArea,
    WebAXRoleSearch,
    WebAXRoleSearchBox,
    WebAXRoleSlider,
    WebAXRoleSliderThumb,
    WebAXRoleSpinButtonPart,
    WebAXRoleSpinButton,
    WebAXRoleSplitter,
    WebAXRoleStaticText,
    WebAXRoleStatus,
    WebAXRoleSwitch,
    WebAXRoleTabGroup,
    WebAXRoleTabList,
    WebAXRoleTabPanel,
    WebAXRoleTab,
    WebAXRoleTableHeaderContainer,
    WebAXRoleTable,
    WebAXRoleTextField,
    WebAXRoleTime,
    WebAXRoleTimer,
    WebAXRoleToggleButton,
    WebAXRoleToolbar,
    WebAXRoleTreeGrid,
    WebAXRoleTreeItem,
    WebAXRoleTree,
    WebAXRoleUserInterfaceTooltip,
    WebAXRoleWebArea,
    WebAXRoleLineBreak,
    WebAXRoleWindow,
};

// Accessibility states, used as a bitmask.
enum WebAXState {
    WebAXStateBusy,
    WebAXStateChecked,
    WebAXStateEnabled,
    WebAXStateExpanded,
    WebAXStateFocusable,
    WebAXStateFocused,
    WebAXStateHaspopup,
    WebAXStateHovered,
    WebAXStateIndeterminate,
    WebAXStateInvisible,
    WebAXStateLinked,
    WebAXStateMultiline,
    WebAXStateMultiselectable,
    WebAXStateOffscreen,
    WebAXStatePressed,
    WebAXStateProtected,
    WebAXStateReadonly,
    WebAXStateRequired,
    WebAXStateSelectable,
    WebAXStateSelected,
    WebAXStateVertical,
    WebAXStateVisited,
};

enum WebAXTextDirection {
    WebAXTextDirectionLR,
    WebAXTextDirectionRL,
    WebAXTextDirectionTB,
    WebAXTextDirectionBT
};

// Sort direction, only used for roles = WebAXRoleRowHeader and
// WebAXRoleColumnHeader.
enum WebAXSortDirection {
    WebAXSortDirectionUndefined = 0,
    WebAXSortDirectionNone,
    WebAXSortDirectionAscending,
    WebAXSortDirectionDescending,
    WebAXSortDirectionOther
};

// Expanded State.
// These values must match blink::AccessibilityExpanded values.
// Enforced in AssertMatchingEnums.cpp.
enum WebAXExpanded {
    WebAXExpandedUndefined = 0,
    WebAXExpandedCollapsed,
    WebAXExpandedExpanded
};

// These values must match blink::AccessibilityOrientation values.
// Enforced in AssertMatchingEnums.cpp.
enum WebAXOrientation {
    WebAXOrientationUndefined = 0,
    WebAXOrientationVertical,
    WebAXOrientationHorizontal,
};

// Only used by HTML form controls and any other element that has
// an aria-invalid attribute specified.
enum WebAXInvalidState {
    WebAXInvalidStateUndefined = 0,
    WebAXInvalidStateFalse,
    WebAXInvalidStateTrue,
    WebAXInvalidStateSpelling,
    WebAXInvalidStateGrammar,
    WebAXInvalidStateOther
};

// Used for exposing text attributes.
enum WebAXTextStyle {
    WebAXTextStyleNone = 0,
    WebAXTextStyleBold = 1 << 0,
    WebAXTextStyleItalic = 1 << 1,
    WebAXTextStyleUnderline = 1 << 2,
    WebAXTextStyleLineThrough = 1 << 3
};

// The source of the accessible name of an element. This is needed
// because on some platforms this determines how the accessible name
// is exposed.
enum WebAXNameFrom {
    WebAXNameFromAttribute = 0,
    WebAXNameFromContents,
    WebAXNameFromPlaceholder,
    WebAXNameFromRelatedElement,
};

// The source of the accessible description of an element. This is needed
// because on some platforms this determines how the accessible description
// is exposed.
enum WebAXDescriptionFrom {
    WebAXDescriptionFromPlaceholder,
    WebAXDescriptionFromRelatedElement
};

} // namespace blink

#endif
