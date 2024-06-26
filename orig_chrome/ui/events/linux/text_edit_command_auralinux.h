// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_X_TEXT_EDIT_COMMAND_X11_H_
#define UI_EVENTS_X_TEXT_EDIT_COMMAND_X11_H_

#include <string>

#include "ui/events/events_export.h"

namespace ui {

// Represents a command that performs a specific operation on text.
// Copy and assignment are explicitly allowed; these objects live in vectors.
class EVENTS_EXPORT TextEditCommandAuraLinux {
public:
    enum CommandId {
        COPY,
        CUT,
        DELETE_BACKWARD,
        DELETE_FORWARD,
        DELETE_TO_BEGINING_OF_LINE,
        DELETE_TO_BEGINING_OF_PARAGRAPH,
        DELETE_TO_END_OF_LINE,
        DELETE_TO_END_OF_PARAGRAPH,
        DELETE_WORD_BACKWARD,
        DELETE_WORD_FORWARD,
        INSERT_TEXT,
        MOVE_BACKWARD,
        MOVE_DOWN,
        MOVE_FORWARD,
        MOVE_LEFT,
        MOVE_PAGE_DOWN,
        MOVE_PAGE_UP,
        MOVE_RIGHT,
        MOVE_TO_BEGINING_OF_DOCUMENT,
        MOVE_TO_BEGINING_OF_LINE,
        MOVE_TO_BEGINING_OF_PARAGRAPH,
        MOVE_TO_END_OF_DOCUMENT,
        MOVE_TO_END_OF_LINE,
        MOVE_TO_END_OF_PARAGRAPH,
        MOVE_UP,
        MOVE_WORD_BACKWARD,
        MOVE_WORD_FORWARD,
        MOVE_WORD_LEFT,
        MOVE_WORD_RIGHT,
        PASTE,
        SELECT_ALL,
        SET_MARK,
        UNSELECT,
        INVALID_COMMAND
    };

    TextEditCommandAuraLinux(CommandId command_id,
        const std::string& argument,
        bool extend_selection)
        : command_id_(command_id)
        , argument_(argument)
        , extend_selection_(extend_selection)
    {
    }

    CommandId command_id() const { return command_id_; }
    const std::string& argument() const { return argument_; }
    bool extend_selection() const { return extend_selection_; }

    // We communicate these commands back to blink with a string representation.
    // This will combine the base command name with "AndModifySelection" if we
    // have |extend_selection_| set.
    std::string GetCommandString() const;

private:
    CommandId command_id_;

    std::string argument_;

    // In addition to executing the command, modify the selection.
    bool extend_selection_;
};

} // namespace ui

#endif // UI_EVENTS_X_TEXT_EDIT_COMMAND_X11_H_
