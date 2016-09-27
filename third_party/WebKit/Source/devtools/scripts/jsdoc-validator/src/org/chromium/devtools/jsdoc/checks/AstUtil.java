package org.chromium.devtools.jsdoc.checks;

import com.google.common.base.Preconditions;
import com.google.javascript.rhino.JSTypeExpression;
import com.google.javascript.rhino.Node;
import com.google.javascript.rhino.Token;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class AstUtil {

    private static final String PROTOTYPE_SUFFIX = ".prototype";

    static Node parentOfType(Node node, int tokenType) {
        Node parent = node.getParent();
        return (parent == null || parent.getType() != tokenType) ? null : parent;
    }

    /**
     * Based on NodeUtil#getFunctionNameNode(Node).
     */
    static Node getFunctionNameNode(Node node) {
        Preconditions.checkState(node.isFunction());

        Node parent = node.getParent();
        if (parent != null) {
            switch (parent.getType()) {
            case Token.NAME:
                // var name = function() ...
                // var name2 = function name1() ...
                return parent;
            // FIXME: Enable the setter and getter checks.
            // case Token.SETTER_DEF:
            // case Token.GETTER_DEF:
            case Token.STRING_KEY:
                return parent;
            case Token.NUMBER:
                return parent;
            case Token.ASSIGN:
                int nameType = parent.getFirstChild().getType();
                // We only consider these types of name nodes as acceptable.
                return nameType == Token.NAME || nameType == Token.GETPROP
                    ? parent.getFirstChild()
                    : null;
            case Token.VAR:
                return parent.getFirstChild();
            default:
                Node funNameNode = node.getFirstChild();
                // Don't return the name node for anonymous functions
                return funNameNode.getString().isEmpty() ? null : funNameNode;
            }
        }

        return null;
    }

    static String getTypeNameFromPrototype(String value) {
        return value.substring(0, value.length() - PROTOTYPE_SUFFIX.length());
    }

    static boolean isPrototypeName(String typeName) {
        return typeName.endsWith(PROTOTYPE_SUFFIX);
    }

    static Node getAssignedTypeNameNode(Node assignment) {
        Preconditions.checkState(assignment.isAssign() || assignment.isVar());
        Node typeNameNode = assignment.getFirstChild();
        if (typeNameNode.getType() != Token.GETPROP && typeNameNode.getType() != Token.NAME) {
            return null;
        }
        return typeNameNode;
    }

    static List<Node> getArguments(Node functionCall) {
        int childCount = functionCall.getChildCount();
        if (childCount == 1) {
            return Collections.emptyList();
        }
        List<Node> arguments = new ArrayList<>(childCount - 1);
        for (int i = 1; i < childCount; ++i) {
            arguments.add(functionCall.getChildAtIndex(i));
        }
        return arguments;
    }

    static String getAnnotationTypeString(JSTypeExpression expression) {
        return expression.getRoot().getFirstChild().getString();
    }

    private AstUtil() {}
}
