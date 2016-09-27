package org.chromium.devtools.jsdoc.checks;

import com.google.javascript.rhino.JSTypeExpression;
import com.google.javascript.rhino.Node;
import com.google.javascript.rhino.Token;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

public final class ProtoFollowsExtendsChecker extends ContextTrackingChecker {

    private static final String PROTO_PROPERTY_NAME = "__proto__";
    private static final Set<String> IGNORED_SUPER_TYPES = new HashSet<>();
    static {
        IGNORED_SUPER_TYPES.add("WebInspector.Object");
    }

    private final Set<TypeRecord> typesWithAssignedProto = new HashSet<>();
    private final Set<FunctionRecord> functionsMissingSuperCall = new HashSet<>();

    @Override
    protected void enterNode(Node node) {
        switch (node.getType()) {
        case Token.ASSIGN:
        case Token.VAR:
            handleAssignment(node);
            break;
        case Token.STRING_KEY:
            handleColonNode(node);
            break;
        case Token.FUNCTION:
            enterFunction();
            break;
        case Token.CALL:
            handleCall(node);
            break;
        default:
            break;
        }
    }

    private void handleCall(Node callNode) {
        FunctionRecord contextFunction = getState().getCurrentFunctionRecord();
        if (contextFunction == null || !contextFunction.isConstructor()
                || !functionsMissingSuperCall.contains(contextFunction)) {
            return;
        }
        String typeName = validSuperConstructorName(callNode);
        if (typeName == null) {
            return;
        }
        TypeRecord typeRecord = getState().typeRecordsByTypeName.get(contextFunction.name);
        if (typeRecord == null) {
            return;
        }
        JSTypeExpression extendedType = typeRecord.getExtendedType();
        // FIXME: Strip template parameters from the extendedType.
        if (extendedType == null ||
                !typeName.equals(AstUtil.getAnnotationTypeString(extendedType))) {
            return;
        }
        functionsMissingSuperCall.remove(contextFunction);
    }

    private String validSuperConstructorName(Node callNode) {
        String callTarget = getContext().getNodeText(callNode.getFirstChild());
        int lastDotIndex = callTarget.lastIndexOf('.');
        if (lastDotIndex == -1) {
            return null;
        }
        String methodName = callTarget.substring(lastDotIndex + 1);
        if (!"call".equals(methodName) && !"apply".equals(methodName)) {
            return null;
        }
        List<Node> arguments = AstUtil.getArguments(callNode);
        if (arguments.isEmpty() || !"this".equals(getContext().getNodeText(arguments.get(0)))) {
            return null;
        }
        return callTarget.substring(0, lastDotIndex);
    }

    @Override
    protected void leaveNode(Node node) {
        if (node.getType() == Token.SCRIPT) {
            checkFinished();
            return;
        }
        if (node.getType() == Token.FUNCTION) {
            leaveFunction();
            return;
        }
    }

    private void enterFunction() {
        FunctionRecord function = getState().getCurrentFunctionRecord();
        JSTypeExpression extendedType = getExtendedTypeToCheck(function);
        if (extendedType == null) {
            return;
        }
        if (!IGNORED_SUPER_TYPES.contains(AstUtil.getAnnotationTypeString(extendedType))) {
            functionsMissingSuperCall.add(function);
        }
    }

    private void leaveFunction() {
        FunctionRecord function = getState().getCurrentFunctionRecord();
        if (!functionsMissingSuperCall.contains(function)) {
            return;
        }
        JSTypeExpression extendedType = getExtendedTypeToCheck(function);
        if (extendedType == null) {
            return;
        }
        String annotationTypeString = AstUtil.getAnnotationTypeString(extendedType);
        if (annotationTypeString.startsWith("HTML")) {
            return;
        }

        reportErrorAtNodeStart(AstUtil.getFunctionNameNode(function.functionNode),
                String.format("Type %s extends %s but does not properly invoke its constructor",
                        function.name, annotationTypeString));
    }

    private JSTypeExpression getExtendedTypeToCheck(FunctionRecord function) {
        if (!function.isConstructor() || function.name == null) {
            return null;
        }
        TypeRecord type = getState().typeRecordsByTypeName.get(function.name);
        if (type == null || type.isInterface()) {
            return null;
        }
        return type.getExtendedType();
    }

    private void checkFinished() {
        for (TypeRecord record : getState().getTypeRecordsByTypeName().values()) {
            if (record.isInterface() || typesWithAssignedProto.contains(record)) {
                continue;
            }
            JSTypeExpression extendedType = record.getExtendedType();
            if (extendedType != null) {
                Node rootNode = extendedType.getRoot();
                if (rootNode.getType() == Token.BANG && rootNode.getFirstChild() != null) {
                    rootNode = rootNode.getFirstChild();
                }
                getContext().reportErrorAtOffset(
                        rootNode.getSourceOffset(),
                        String.format("No __proto__ assigned for type %s having @extends",
                                record.typeName));
            }
        }
    }

    private void handleColonNode(Node node) {
        ContextTrackingState state = getState();
        TypeRecord type = state.getCurrentTypeRecord();
        if (type == null) {
            return;
        }
        String propertyName = node.getString();
        if (!PROTO_PROPERTY_NAME.equals(propertyName)) {
            return;
        }
        TypeRecord currentType = state.getCurrentTypeRecord();
        if (currentType == null) {
            // FIXME: __proto__: Foo.prototype not in an object literal for Bar.prototype.
            return;
        }
        typesWithAssignedProto.add(currentType);
        Node rightNode = node.getFirstChild();
        String value = state.getNodeText(rightNode);
        boolean isNullPrototype = "null".equals(value);
        if (!isNullPrototype && !AstUtil.isPrototypeName(value)) {
            reportErrorAtNodeStart(
                    rightNode, "__proto__ value is not a prototype");
            return;
        }
        String superType = isNullPrototype ? "null" : AstUtil.getTypeNameFromPrototype(value);
        if (type.isInterface()) {
            reportErrorAtNodeStart(node, String.format(
                    "__proto__ defined for interface %s", type.typeName));
            return;
        } else {
            if (!isNullPrototype && type.getExtendedType() == null) {
                reportErrorAtNodeStart(rightNode, String.format(
                        "No @extends annotation for %s extending %s", type.typeName, superType));
                return;
            }
        }

        if (isNullPrototype) {
            return;
        }

        // FIXME: Should we check that there is only one @extend-ed type
        // for the non-interface |type|? Closure is supposed to do this anyway...
        JSTypeExpression extendedType = type.getExtendedType();
        String extendedTypeName = AstUtil.getAnnotationTypeString(extendedType);
        if (!superType.equals(extendedTypeName)) {
            reportErrorAtNodeStart(rightNode, String.format(
                    "Supertype does not match %s declared in @extends for %s (line %d)",
                    extendedTypeName, type.typeName,
                    state.getContext().getPosition(
                            extendedType.getRoot().getSourceOffset()).line));
        }
    }

    private void handleAssignment(Node assignment) {
        String assignedTypeName =
                getState().getNodeText(AstUtil.getAssignedTypeNameNode(assignment));
        if (assignedTypeName == null) {
            return;
        }
        if (!AstUtil.isPrototypeName(assignedTypeName)) {
            return;
        }
        Node prototypeValueNode = assignment.getLastChild();

        if (prototypeValueNode.getType() == Token.OBJECTLIT) {
            return;
        }

        // Foo.prototype = notObjectLiteral
        ContextTrackingState state = getState();
        TypeRecord type = state.getCurrentTypeRecord();
        if (type == null) {
            // Assigning a prototype for unknown type. Leave it to the closure compiler.
            return;
        }
        if (type.getExtendedType() != null) {
            reportErrorAtNodeStart(prototypeValueNode, String.format(
                    "@extends found for type %s but its prototype is not an object "
                    + "containing __proto__", AstUtil.getTypeNameFromPrototype(assignedTypeName)));
        }
    }
}
