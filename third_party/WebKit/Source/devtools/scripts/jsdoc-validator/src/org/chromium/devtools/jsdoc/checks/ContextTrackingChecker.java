package org.chromium.devtools.jsdoc.checks;

import com.google.javascript.rhino.Node;

import org.chromium.devtools.jsdoc.ValidatorContext;

abstract class ContextTrackingChecker {
    private ContextTrackingState state;

    abstract void enterNode(Node node);

    abstract void leaveNode(Node node);

    void setState(ContextTrackingState state) {
        this.state = state;
    }

    protected ContextTrackingState getState() {
        return state;
    }

    protected ValidatorContext getContext() {
        return state.getContext();
    }

    protected void reportErrorAtNodeStart(Node node, String errorText) {
        getContext().reportErrorInNode(node, 0, errorText);
    }

    protected void reportErrorAtOffset(int offset, String errorText) {
        getContext().reportErrorAtOffset(offset, errorText);
    }

}
