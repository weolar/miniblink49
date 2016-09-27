package org.chromium.devtools.jsdoc.checks;

import com.google.javascript.jscomp.NodeUtil;
import com.google.javascript.rhino.JSDocInfo;
import com.google.javascript.rhino.Node;
import com.google.javascript.rhino.Token;

import org.chromium.devtools.jsdoc.ValidationCheck;
import org.chromium.devtools.jsdoc.ValidatorContext;

import java.util.ArrayList;
import java.util.List;

public class ContextTrackingValidationCheck extends ValidationCheck {

    private ContextTrackingState state;
    private final List<ContextTrackingChecker> clients = new ArrayList<>(5);

    @Override
    protected void setContext(ValidatorContext context) {
        super.setContext(context);
        state = new ContextTrackingState(context);
        registerClient(new ProtoFollowsExtendsChecker());
        registerClient(new MethodAnnotationChecker());
        registerClient(new FunctionReceiverChecker());
        registerClient(new DisallowedGlobalPropertiesChecker());
    }

    @Override
    public void doVisit(Node node) {
        switch (node.getType()) {
        case Token.ASSIGN:
        case Token.VAR:
            enterAssignOrVarNode(node);
            break;
        case Token.FUNCTION:
            enterFunctionNode(node);
            break;
        default:
            break;
        }

        enterNode(node);
    }

    @Override
    public void didVisit(Node node) {
        leaveNode(node);

        switch (node.getType()) {
        case Token.ASSIGN:
            leaveAssignNode(node);
            break;
        case Token.FUNCTION:
            leaveFunctionNode(node);
            break;
        default:
            break;
        }
    }

    public void registerClient(ContextTrackingChecker client) {
        this.clients.add(client);
        client.setState(state);
    }

    private void enterNode(Node node) {
        for (ContextTrackingChecker client : clients) {
            client.enterNode(node);
        }
    }

    private void leaveNode(Node node) {
        for (ContextTrackingChecker client : clients) {
            client.leaveNode(node);
        }
    }

    private void enterFunctionNode(Node node) {
        TypeRecord parentType = state.getCurrentFunctionRecord() == null
                ? state.getCurrentTypeRecord()
                : null;
        Node nameNode = AstUtil.getFunctionNameNode(node);
        String functionName = nameNode == null ? null : state.getNodeText(nameNode);
        FunctionRecord functionRecord = new FunctionRecord(
                node,
                functionName,
                getFunctionParameterNames(node),
                parentType,
                state.getCurrentFunctionRecord());
        state.pushFunctionRecord(functionRecord);
        rememberTypeRecordIfNeeded(functionName, functionRecord.info);
    }

    @SuppressWarnings("unused")
    private void leaveFunctionNode(Node node) {
        state.functionRecords.removeLast();
    }

    private void enterAssignOrVarNode(Node node) {
        String assignedTypeName = getAssignedTypeName(node);
        if (assignedTypeName == null) {
            return;
        }
        if (AstUtil.isPrototypeName(assignedTypeName)) {
            // MyType.prototype = ...
            String typeName = AstUtil.getTypeNameFromPrototype(assignedTypeName);
            TypeRecord typeRecord = state.typeRecordsByTypeName.get(typeName);
            // We should push anything here to maintain a valid current type record.
            state.pushTypeRecord(typeRecord);
            state.pushFunctionRecord(null);
            return;
        }
    }

    private void leaveAssignNode(Node assignment) {
        String assignedTypeName = getAssignedTypeName(assignment);
        if (assignedTypeName == null) {
            return;
        }
        if (AstUtil.isPrototypeName(assignedTypeName)) {
            // Remove the current type record when leaving prototype object.
            state.typeRecords.removeLast();
            state.functionRecords.removeLast();
            return;
        }
    }

    private String getAssignedTypeName(Node assignment) {
        Node node = AstUtil.getAssignedTypeNameNode(assignment);
        return getNodeText(node);
    }

    private List<String> getFunctionParameterNames(Node functionNode) {
        List<String> parameterNames = new ArrayList<String>();
        Node parametersNode = NodeUtil.getFunctionParameters(functionNode);
        for (int i = 0, childCount = parametersNode.getChildCount(); i < childCount; ++i) {
            Node paramNode = parametersNode.getChildAtIndex(i);
            String paramName = state.getContext().getNodeText(paramNode);
            parameterNames.add(paramName);
        }
        return parameterNames;
    }

    private boolean rememberTypeRecordIfNeeded(String typeName, JSDocInfo info) {
        if (info == null) {
            return false;
        }
        if (typeName == null) {
            return info.isConstructor() || info.isInterface();
        }
        if (!info.isConstructor() && !info.isInterface()) {
            return false;
        }
        TypeRecord record = new TypeRecord(typeName, info);
        state.typeRecordsByTypeName.put(typeName, record);
        return true;
    }
}
