package org.chromium.devtools.jsdoc.checks;

import com.google.common.base.Joiner;
import com.google.javascript.jscomp.NodeUtil;
import com.google.javascript.rhino.JSDocInfo;
import com.google.javascript.rhino.Node;
import com.google.javascript.rhino.Token;

import java.util.HashSet;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class MethodAnnotationChecker extends ContextTrackingChecker {

    private static final Pattern PARAM_PATTERN =
            Pattern.compile("^[^@\n]*@param\\s+(\\{.+\\}\\s+)?([^\\s]+).*$", Pattern.MULTILINE);

    private static final Pattern INVALID_RETURN_PATTERN =
            Pattern.compile("^[^@\n]*(@)return(?:s.*|\\s+[^{]*)$", Pattern.MULTILINE);

    private final Set<FunctionRecord> valueReturningFunctions = new HashSet<>();
    private final Set<FunctionRecord> throwingFunctions = new HashSet<>();

    @Override
    public void enterNode(Node node) {
        switch (node.getType()) {
        case Token.FUNCTION:
            handleFunction(node);
            break;
        case Token.RETURN:
            handleReturn(node);
            break;
        case Token.THROW:
            handleThrow();
            break;
        default:
            break;
        }
    }

    private void handleFunction(Node functionNode) {
        FunctionRecord function = getState().getCurrentFunctionRecord();
        if (function == null || function.parameterNames.size() == 0) {
            return;
        }
        String[] nonAnnotatedParams = getNonAnnotatedParamData(function);
        if (nonAnnotatedParams.length > 0
            && function.parameterNames.size() != nonAnnotatedParams.length) {
            reportErrorAtOffset(function.info.getOriginalCommentPosition(),
                    String.format(
                            "No @param JSDoc tag found for parameters: [%s]",
                            Joiner.on(',').join(nonAnnotatedParams)));
        }
    }

    private String[] getNonAnnotatedParamData(FunctionRecord function) {
        if (function.info == null) {
            return new String[0];
        }
        Set<String> formalParamNames = new HashSet<>();
        for (int i = 0; i < function.parameterNames.size(); ++i) {
            String paramName = function.parameterNames.get(i);
            if (!formalParamNames.add(paramName)) {
                reportErrorAtNodeStart(function.functionNode,
                        String.format("Duplicate function argument name: %s", paramName));
            }
        }
        Matcher m = PARAM_PATTERN.matcher(function.info.getOriginalCommentString());
        while (m.find()) {
            String paramType = m.group(1);
            if (paramType == null) {
                reportErrorAtOffset(function.info.getOriginalCommentPosition() + m.start(2),
                        String.format(
                                "Invalid @param annotation found -"
                                + " should be \"@param {<type>} paramName\""));
            } else {
                formalParamNames.remove(m.group(2));
            }
        }
        return formalParamNames.toArray(new String[formalParamNames.size()]);
    }

    private void handleReturn(Node node) {
        if (node.getFirstChild() == null || AstUtil.parentOfType(node, Token.ASSIGN) != null) {
            return;
        }

        FunctionRecord record = getState().getCurrentFunctionRecord();
        if (record == null) {
            return;
        }
        Node nameNode = getFunctionNameNode(record.functionNode);
        if (nameNode == null) {
            return;
        }
        valueReturningFunctions.add(record);
    }

    private void handleThrow() {
        FunctionRecord record = getState().getCurrentFunctionRecord();
        if (record == null) {
            return;
        }
        Node nameNode = getFunctionNameNode(record.functionNode);
        if (nameNode == null) {
            return;
        }
        throwingFunctions.add(record);
    }

    @Override
    public void leaveNode(Node node) {
        if (node.getType() != Token.FUNCTION) {
            return;
        }

        FunctionRecord record = getState().getCurrentFunctionRecord();
        if (record != null) {
            checkFunctionAnnotation(record);
        }
    }

    @SuppressWarnings("unused")
    private void checkFunctionAnnotation(FunctionRecord function) {
        String functionName = getFunctionName(function.functionNode);
        if (functionName == null) {
            return;
        }
        String[] parts = functionName.split("\\.");
        functionName = parts[parts.length - 1];
        boolean isApiFunction = !functionName.startsWith("_")
                && (function.isTopLevelFunction()
                        || (function.enclosingType != null
                                && isPlainTopLevelFunction(function.enclosingFunctionRecord)));

        boolean isReturningFunction = valueReturningFunctions.contains(function);
        boolean isInterfaceFunction =
                function.enclosingType != null && function.enclosingType.isInterface();
        int invalidAnnotationIndex =
                invalidReturnAnnotationIndex(function.info);
        if (invalidAnnotationIndex != -1) {
            String suggestedResolution = (isReturningFunction || isInterfaceFunction)
                    ? "should be \"@return {<type>}\""
                    : "please remove, as function does not return value";
            getContext().reportErrorAtOffset(
                    function.info.getOriginalCommentPosition() + invalidAnnotationIndex,
                    String.format(
                            "invalid return type annotation found - %s", suggestedResolution));
            return;
        }
        Node functionNameNode = getFunctionNameNode(function.functionNode);
        if (functionNameNode == null) {
            return;
        }

        if (isReturningFunction) {
            if (!function.isConstructor() && !function.hasReturnAnnotation() && isApiFunction) {
                reportErrorAtNodeStart(
                        functionNameNode,
                        "@return annotation is required for API functions that return value");
            }
        } else {
            // A @return-function that does not actually return anything and
            // is intended to be overridden in subclasses must throw.
            if (function.hasReturnAnnotation()
                    && !isInterfaceFunction
                    && !throwingFunctions.contains(function)) {
                reportErrorAtNodeStart(functionNameNode,
                        "@return annotation found, yet function does not return value");
            }
        }
    }

    private static boolean isPlainTopLevelFunction(FunctionRecord record) {
        return record != null && record.isTopLevelFunction()
                && (record.enclosingType == null && !record.isConstructor());
    }

    private String getFunctionName(Node functionNode) {
        Node nameNode = getFunctionNameNode(functionNode);
        return nameNode == null ? null : getState().getNodeText(nameNode);
    }

    private static int invalidReturnAnnotationIndex(JSDocInfo info) {
        if (info == null) {
            return -1;
        }
        Matcher m = INVALID_RETURN_PATTERN.matcher(info.getOriginalCommentString());
        return m.find() ? m.start(1) : -1;
    }

    private static Node getFunctionNameNode(Node functionNode) {
        // FIXME: Do not require annotation for assignment-RHS functions.
        return AstUtil.getFunctionNameNode(functionNode);
    }
}
