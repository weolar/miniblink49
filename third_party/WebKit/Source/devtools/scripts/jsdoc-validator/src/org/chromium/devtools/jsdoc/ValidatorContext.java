package org.chromium.devtools.jsdoc;

import com.google.javascript.jscomp.SourceFile;
import com.google.javascript.rhino.Node;

import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;
import java.util.SortedSet;
import java.util.TreeSet;

public class ValidatorContext {

    private static final Comparator<MessageRecord> MESSAGE_RECORD_COMPARATOR =
            new Comparator<MessageRecord>() {
                @Override
                public int compare(MessageRecord left, MessageRecord right) {
                    return left.position - right.position;
                }
            };

    public final String scriptFileName;
    public final SourceFile sourceFile;
    private final SortedSet<MessageRecord> validationResult =
            new TreeSet<>(MESSAGE_RECORD_COMPARATOR);


    public ValidatorContext(String text, String scriptFileName) {
        this.scriptFileName = scriptFileName;
        this.sourceFile = SourceFile.builder().buildFromCode(scriptFileName, text);
    }

    public SortedSet<MessageRecord> getValidationResult() {
        return Collections.unmodifiableSortedSet(validationResult);
    }

    public String getNodeText(Node node) {
        if (node == null) {
            return null;
        }
        try {
            return sourceFile.getCode().substring(
                    node.getSourceOffset(), node.getSourceOffset() + node.getLength());
        } catch (IOException e) {
            return null;
        }
    }

    public SourcePosition getPosition(int offset) {
        return new SourcePosition(
                sourceFile.getLineOfOffset(offset), sourceFile.getColumnOfOffset(offset));
    }

    public void reportErrorInNode(Node node, int offsetInNodeText, String errorText) {
        int errorAbsoluteOffset = node.getSourceOffset() + offsetInNodeText;
        reportErrorAtOffset(errorAbsoluteOffset, errorText);
    }

    public void reportErrorAtOffset(int offset, String errorText) {
        SourcePosition position = getPosition(offset);
        StringBuilder positionMarker = new StringBuilder(position.column + 1);
        for (int i = position.column; i > 0; --i) {
            positionMarker.append(' ');
        }
        positionMarker.append('^');
        String message = String.format("%s:%d: ERROR - %s%n%s%n%s%n",
                scriptFileName,
                position.line,
                errorText,
                sourceFile.getLine(position.line),
                positionMarker.toString());
        validationResult.add(new MessageRecord(offset, message));
    }

    public static class MessageRecord {
        public final int position;
        public final String text;

        public MessageRecord(int position, String text) {
            this.position = position;
            this.text = text;
        }
    }

    public static class SourcePosition {
        public final int line;
        public final int column;

        public SourcePosition(int line, int column) {
            this.line = line;
            this.column = column;
        }
    }
}
