package org.chromium.devtools.jsdoc;

import com.google.javascript.rhino.Node;

/**
 * A base class for all JSDoc validation checks.
 */
public abstract class ValidationCheck implements DoDidNodeVisitor {

    private ValidatorContext context;

    protected String getNodeText(Node node) {
        return context.getNodeText(node);
    }

    protected void setContext(ValidatorContext context) {
        if (this.context != null) {
            throw new RuntimeException("ValidatorContext already set");
        }
        this.context = context;
    }
}
