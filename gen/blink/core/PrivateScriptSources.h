#ifndef PrivateScriptSources_h
#define PrivateScriptSources_h
struct PrivateScriptSources {
    const char* scriptClassName;
    const char* className;
    const char* resourceFile;
};

struct PrivateScriptSources kPrivateScriptSources[] = {
    { "PrivateScriptRunner", "PrivateScriptRunner", "PrivateScriptRunner.js" },
    { "HTMLMarqueeElement", "HTMLMarqueeElement", "HTMLMarqueeElement.js" },
    { "PluginPlaceholderElement", "PluginPlaceholderElement", "PluginPlaceholderElement.js" },
    { "DocumentXMLTreeViewer", "Document", "DocumentXMLTreeViewer.js" },
};
#endif // PrivateScriptSources_h
