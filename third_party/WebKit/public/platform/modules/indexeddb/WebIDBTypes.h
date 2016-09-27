/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebIDBTypes_h
#define WebIDBTypes_h

namespace blink {

enum WebIDBKeyType {
    WebIDBKeyTypeInvalid = 0,
    WebIDBKeyTypeArray,
    WebIDBKeyTypeBinary,
    WebIDBKeyTypeString,
    WebIDBKeyTypeDate,
    WebIDBKeyTypeNumber,
    WebIDBKeyTypeNull,
    WebIDBKeyTypeMin,
};

enum WebIDBKeyPathType {
    WebIDBKeyPathTypeNull = 0,
    WebIDBKeyPathTypeString,
    WebIDBKeyPathTypeArray,
};

enum WebIDBDataLoss {
    WebIDBDataLossNone = 0,
    WebIDBDataLossTotal,
};

enum WebIDBCursorDirection {
    WebIDBCursorDirectionNext = 0,
    WebIDBCursorDirectionNextNoDuplicate = 1,
    WebIDBCursorDirectionPrev = 2,
    WebIDBCursorDirectionPrevNoDuplicate = 3,
    WebIDBCursorDirectionLast = WebIDBCursorDirectionPrevNoDuplicate
};

enum WebIDBTaskType {
    WebIDBTaskTypeNormal = 0,
    WebIDBTaskTypePreemptive,
    WebIDBTaskTypeLast = WebIDBTaskTypePreemptive
};

enum WebIDBPutMode {
    WebIDBPutModeAddOrUpdate,
    WebIDBPutModeAddOnly,
    WebIDBPutModeCursorUpdate,
    WebIDBPutModeLast = WebIDBPutModeCursorUpdate
};

enum WebIDBTransactionMode {
    WebIDBTransactionModeReadOnly = 0,
    WebIDBTransactionModeReadWrite,
    WebIDBTransactionModeVersionChange,
    WebIDBTransactionModeLast = WebIDBTransactionModeVersionChange
};

} // namespace blink

#endif // WebIDBTypes_h
