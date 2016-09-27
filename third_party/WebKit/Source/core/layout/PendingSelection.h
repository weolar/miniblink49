/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef PendingSelection_h
#define PendingSelection_h

#include "core/dom/Position.h"
#include "core/editing/TextAffinity.h"
#include "core/editing/VisibleSelection.h"

namespace blink {

class Document;
class FrameSelection;

class PendingSelection final : public NoBaseWillBeGarbageCollected<PendingSelection> {
public:
    static PassOwnPtrWillBeRawPtr<PendingSelection> create()
    {
        return adoptPtrWillBeNoop(new PendingSelection);
    }

    bool hasPendingSelection() const { return m_hasPendingSelection; }
    void setSelection(const FrameSelection&);
    void clear();

    bool isInDocument(const Document&) const;
    VisibleSelection calcVisibleSelection() const;

    DECLARE_TRACE();

private:
    PendingSelection();

    template <typename Strategy>
    bool isInDocumentAlgorithm(const Document&) const;

    template <typename Strategy>
    VisibleSelection calcVisibleSelectionAlgorithm() const;

    VisibleSelection m_selection;
    bool m_hasPendingSelection : 1;
    bool m_shouldShowBlockCursor : 1;
};

} // namespace blink

#endif
