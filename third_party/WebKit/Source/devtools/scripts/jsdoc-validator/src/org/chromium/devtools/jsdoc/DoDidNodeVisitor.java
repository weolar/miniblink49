package org.chromium.devtools.jsdoc;

import com.google.javascript.rhino.Node;

public interface DoDidNodeVisitor {
    /**
     * Pre-visit node. Visitable children (if any) of {@code node} will be visited afterwards.
     * @param node
     */
    void doVisit(Node node);

    /**
     * Post-visit node. The method is invoked once all visitable children (if any) of {@code node}
     * have been visited.
     * @param node
     */
    void didVisit(Node node);
}
