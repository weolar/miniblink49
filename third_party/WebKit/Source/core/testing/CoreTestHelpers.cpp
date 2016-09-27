// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/testing/CoreTestHelpers.h"

#include "core/dom/Document.h"
#include "core/dom/Range.h"
#include "core/dom/Text.h"
#include "core/html/HTMLElement.h"
#include "wtf/text/StringBuilder.h"
#include <ios>
#include <ostream> // NOLINT

namespace blink {

namespace {

// Copied from "dom/Node.cpp".
void appendAttributeDesc(const Node& node, StringBuilder& stringBuilder, const QualifiedName& name, const char* attrDesc)
{
    if (!node.isElementNode())
        return;

    String attr = toElement(node).getAttribute(name);
    if (attr.isEmpty())
        return;

    stringBuilder.append(attrDesc);
    stringBuilder.appendLiteral("=\"");
    stringBuilder.append(attr);
    stringBuilder.appendLiteral("\"");
}

template <typename PositionType>
std::ostream& printPosition(std::ostream& ostream, const PositionType& position)
{
    if (position.isNull())
        return ostream << "null";
    ostream << position.anchorNode() << "@";
    if (position.anchorType() == PositionAnchorType::OffsetInAnchor)
        return ostream << position.offsetInContainerNode();
    return ostream << position.anchorType();
}

} // namespace

std::ostream& operator<<(std::ostream& ostream, PositionAnchorType anchorType)
{
    switch (anchorType) {
    case PositionAnchorType::AfterAnchor:
        return ostream << "afterAnchor";
    case PositionAnchorType::AfterChildren:
        return ostream << "afterChildren";
    case PositionAnchorType::BeforeAnchor:
        return ostream << "beforeAnchor";
    case PositionAnchorType::BeforeChildren:
        return ostream << "beforeChildren";
    case PositionAnchorType::OffsetInAnchor:
        return ostream << "offsetInAnchor";
    }
    ASSERT_NOT_REACHED();
    return ostream << "anchorType=" << static_cast<int>(anchorType);
}

// |std::ostream| version of |Node::showNode|
std::ostream& operator<<(std::ostream& ostream, const Node& node)
{
    ostream << node.nodeName().utf8().data();
    if (node.isTextNode())
        return ostream << " " << node.nodeValue();
    StringBuilder attrs;
    appendAttributeDesc(node, attrs, HTMLNames::idAttr, " ID");
    appendAttributeDesc(node, attrs, HTMLNames::classAttr, " CLASS");
    appendAttributeDesc(node, attrs, HTMLNames::styleAttr, " STYLE");
    return ostream << attrs.toString().utf8().data();
}

std::ostream& operator<<(std::ostream& ostream, const Node* node)
{
    if (!node)
        return ostream << "null";
    return ostream << *node;
}

std::ostream& operator<<(std::ostream& ostream, const Position& position)
{
    return printPosition(ostream, position);
}

std::ostream& operator<<(std::ostream& ostream, const PositionInComposedTree& position)
{
    return printPosition(ostream, position);
}

} // namespace blink
