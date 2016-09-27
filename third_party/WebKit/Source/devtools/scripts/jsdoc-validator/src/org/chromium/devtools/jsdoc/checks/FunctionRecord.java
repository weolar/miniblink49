package org.chromium.devtools.jsdoc.checks;

import com.google.javascript.jscomp.NodeUtil;
import com.google.javascript.rhino.JSDocInfo;
import com.google.javascript.rhino.Node;

import java.util.ArrayList;
import java.util.List;

public class FunctionRecord {
    final Node functionNode;
    final JSDocInfo info;
    final String name;
    final List<String> parameterNames;
    final TypeRecord enclosingType;
    final FunctionRecord enclosingFunctionRecord;

    public FunctionRecord(Node functionNode, String name,
            List<String> parameterNames, TypeRecord parentType,
            FunctionRecord enclosingFunctionRecord) {
        this.functionNode = functionNode;
        this.info = NodeUtil.getBestJSDocInfo(functionNode);
        this.name = name;
        this.parameterNames = parameterNames;
        this.enclosingType = parentType;
        this.enclosingFunctionRecord = enclosingFunctionRecord;
    }

    public FunctionRecord() {
        this.functionNode = null;
        this.info = null;
        this.name = "";
        this.parameterNames = new ArrayList<>();
        this.enclosingType = null;
        this.enclosingFunctionRecord = null;
    }

    public boolean isConstructor() {
        return info != null && info.isConstructor();
    }

    public boolean isTopLevelFunction() {
        return enclosingFunctionRecord == null;
    }

    public boolean hasReturnAnnotation() {
        return info != null && info.getReturnType() != null;
    }

    public boolean hasThisAnnotation() {
        return info != null && info.getThisType() != null;
    }

    public boolean suppressesReceiverCheck() {
        return info != null && info.getOriginalCommentString().contains("@suppressReceiverCheck");
    }

    public boolean suppressesGlobalPropertiesCheck() {
        return info != null
            && info.getOriginalCommentString().contains("@suppressGlobalPropertiesCheck");
    }

    @Override
    public String toString() {
        return (info == null ? "" : info.getOriginalCommentString() + "\n") +
                (name == null ? "<anonymous>" : name) + "() @" +
                functionNode.getLineno();
    }
}
