package org.chromium.devtools.jsdoc;

import com.google.javascript.jscomp.NodeTraversal;
import com.google.javascript.rhino.Node;

public abstract class DoDidVisitorAdapter implements DoDidNodeVisitor, NodeTraversal.Callback {

    @Override
    public boolean shouldTraverse(NodeTraversal nodeTraversal, Node n, Node parent) {
        doVisit(n);
        return true;
    }

    @Override
    public void visit(NodeTraversal t, Node n, Node parent) {
        didVisit(n);
    }
}
