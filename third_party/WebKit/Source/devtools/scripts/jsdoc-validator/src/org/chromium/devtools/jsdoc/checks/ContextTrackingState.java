package org.chromium.devtools.jsdoc.checks;

import com.google.javascript.rhino.Node;

import org.chromium.devtools.jsdoc.ValidatorContext;

import java.util.Deque;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

public class ContextTrackingState {
    private final ValidatorContext context;

    ContextTrackingState(ValidatorContext context) {
        this.context = context;
    }

    final Map<String, TypeRecord> typeRecordsByTypeName = new HashMap<>();
    final Deque<TypeRecord> typeRecords = new LinkedList<>();
    final Deque<FunctionRecord> functionRecords = new LinkedList<>();

    TypeRecord getCurrentTypeRecord() {
        return typeRecords.peekLast();
    }

    FunctionRecord getCurrentFunctionRecord() {
        return functionRecords.peekLast();
    }

    ValidatorContext getContext() {
        return context;
    }

    Map<String, TypeRecord> getTypeRecordsByTypeName() {
        return typeRecordsByTypeName;
    }

    String getNodeText(Node node) {
        return getContext().getNodeText(node);
    }

    void pushTypeRecord(TypeRecord record) {
        typeRecords.addLast(record);
    }

    void popTypeRecord() {
        typeRecords.removeLast();
    }

    void pushFunctionRecord(FunctionRecord record) {
        functionRecords.addLast(record);
    }

    void popFunctionRecord() {
        functionRecords.removeLast();
    }
}
