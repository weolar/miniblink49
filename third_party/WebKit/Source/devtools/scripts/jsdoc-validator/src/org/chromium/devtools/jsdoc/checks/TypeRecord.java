package org.chromium.devtools.jsdoc.checks;

import com.google.common.base.Preconditions;
import com.google.javascript.rhino.JSDocInfo;
import com.google.javascript.rhino.JSTypeExpression;

public class TypeRecord {
    public final String typeName;
    public final JSDocInfo info;

    public TypeRecord(String typeName, JSDocInfo info) {
        Preconditions.checkNotNull(info);
        this.typeName = typeName;
        this.info = info;
    }

    public boolean isInterface() {
        return this.info.isInterface();
    }

    public JSTypeExpression getExtendedType() {
        return this.info.getBaseType();
    }
}
