// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/linux/text_edit_command_auralinux.h"

#include "base/logging.h"

namespace ui {

std::string TextEditCommandAuraLinux::GetCommandString() const
{
    std::string base_name;
    switch (command_id_) {
    case COPY:
        base_name = "Copy";
        break;
    case CUT:
        base_name = "Cut";
        break;
    case DELETE_BACKWARD:
        base_name = "DeleteBackward";
        break;
    case DELETE_FORWARD:
        base_name = "DeleteForward";
        break;
    case DELETE_TO_BEGINING_OF_LINE:
        base_name = "DeleteToBeginningOfLine";
        break;
    case DELETE_TO_BEGINING_OF_PARAGRAPH:
        base_name = "DeleteToBeginningOfParagraph";
        break;
    case DELETE_TO_END_OF_LINE:
        base_name = "DeleteToEndOfLine";
        break;
    case DELETE_TO_END_OF_PARAGRAPH:
        base_name = "DeleteToEndOfParagraph";
        break;
    case DELETE_WORD_BACKWARD:
        base_name = "DeleteWordBackward";
        break;
    case DELETE_WORD_FORWARD:
        base_name = "DeleteWordForward";
        break;
    case INSERT_TEXT:
        base_name = "InsertText";
        break;
    case MOVE_BACKWARD:
        base_name = "MoveBackward";
        break;
    case MOVE_DOWN:
        base_name = "MoveDown";
        break;
    case MOVE_FORWARD:
        base_name = "MoveForward";
        break;
    case MOVE_LEFT:
        base_name = "MoveLeft";
        break;
    case MOVE_PAGE_DOWN:
        base_name = "MovePageDown";
        break;
    case MOVE_PAGE_UP:
        base_name = "MovePageUp";
        break;
    case MOVE_RIGHT:
        base_name = "MoveRight";
        break;
    case MOVE_TO_BEGINING_OF_DOCUMENT:
        base_name = "MoveToBeginningOfDocument";
        break;
    case MOVE_TO_BEGINING_OF_LINE:
        base_name = "MoveToBeginningOfLine";
        break;
    case MOVE_TO_BEGINING_OF_PARAGRAPH:
        base_name = "MoveToBeginningOfParagraph";
        break;
    case MOVE_TO_END_OF_DOCUMENT:
        base_name = "MoveToEndOfDocument";
        break;
    case MOVE_TO_END_OF_LINE:
        base_name = "MoveToEndOfLine";
        break;
    case MOVE_TO_END_OF_PARAGRAPH:
        base_name = "MoveToEndOfParagraph";
        break;
    case MOVE_UP:
        base_name = "MoveUp";
        break;
    case MOVE_WORD_BACKWARD:
        base_name = "MoveWordBackward";
        break;
    case MOVE_WORD_FORWARD:
        base_name = "MoveWordForward";
        break;
    case MOVE_WORD_LEFT:
        base_name = "MoveWordLeft";
        break;
    case MOVE_WORD_RIGHT:
        base_name = "MoveWordRight";
        break;
    case PASTE:
        base_name = "Paste";
        break;
    case SELECT_ALL:
        base_name = "SelectAll";
        break;
    case SET_MARK:
        base_name = "SetMark";
        break;
    case UNSELECT:
        base_name = "Unselect";
        break;
    case INVALID_COMMAND:
        NOTREACHED();
        return std::string();
    }

    if (extend_selection())
        base_name += "AndModifySelection";

    return base_name;
}

} // namespace ui
