package org.chromium.devtools.compiler;

import com.google.javascript.jscomp.CodingConventions;

public class DevToolsCodingConvention extends CodingConventions.Proxy {

    private static final long serialVersionUID = 1L;

    public DevToolsCodingConvention() {
        super(CodingConventions.getDefault());
    }

    @Override
    public boolean isPrivate(String name) {
        return name.length() > 1 && name.charAt(0) == '_' && name.charAt(1) != '_';
    }

    @Override
    public boolean isValidEnumKey(String key) {
        return !key.isEmpty() && Character.isUpperCase(key.charAt(0));
    }
}
