// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorTypeBuilderHelper_h
#define InspectorTypeBuilderHelper_h

#include "core/InspectorTypeBuilder.h"
#include "modules/ModulesExport.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

using TypeBuilder::Accessibility::AXGlobalStates;
using TypeBuilder::Accessibility::AXLiveRegionAttributes;
using TypeBuilder::Accessibility::AXProperty;
using TypeBuilder::Accessibility::AXValueType;
using TypeBuilder::Accessibility::AXRelationshipAttributes;
using TypeBuilder::Accessibility::AXValue;
using TypeBuilder::Accessibility::AXWidgetAttributes;
using TypeBuilder::Accessibility::AXWidgetStates;

PassRefPtr<AXProperty> createProperty(String name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(AXGlobalStates::Enum name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(AXLiveRegionAttributes::Enum name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(AXRelationshipAttributes::Enum name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(AXWidgetAttributes::Enum name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(AXWidgetStates::Enum name, PassRefPtr<AXValue>);
PassRefPtr<AXProperty> createProperty(IgnoredReason);

PassRefPtr<AXValue> createValue(String value, AXValueType::Enum = AXValueType::String);
PassRefPtr<AXValue> createValue(int value, AXValueType::Enum = AXValueType::Integer);
PassRefPtr<AXValue> createValue(float value, AXValueType::Enum = AXValueType::Number);
PassRefPtr<AXValue> createBooleanValue(bool value, AXValueType::Enum = AXValueType::Boolean);
PassRefPtr<AXValue> createRelatedNodeValue(const AXObject*);
PassRefPtr<AXValue> createRelatedNodeListValue(AXObject::AccessibilityChildrenVector);

} // namespace blink

#endif // InspectorAccessibilityAgent_h
