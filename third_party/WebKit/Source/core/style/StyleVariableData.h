#ifndef StyleVariableData_h
#define StyleVariableData_h

#include "core/css/CSSVariableData.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/RefCounted.h"
#include "wtf/text/AtomicStringHash.h"

namespace blink {

class StyleVariableData : public RefCounted<StyleVariableData> {
public:
    static PassRefPtr<StyleVariableData> create() { return adoptRef(new StyleVariableData()); }
    PassRefPtr<StyleVariableData> copy() const { return adoptRef(new StyleVariableData(*this)); }

    bool operator==(const StyleVariableData& other) const { return other.m_data == m_data; }
    bool operator!=(const StyleVariableData& other) const { return !(*this == other); }

    void setVariable(const AtomicString& name, PassRefPtr<CSSVariableData> value) { m_data.set(name, value); }
    CSSVariableData* getVariable(const AtomicString& name) const { return m_data.get(name); }
    void removeVariable(const AtomicString& name) { return m_data.remove(name); }
private:
    StyleVariableData() = default;
    StyleVariableData(const StyleVariableData& other) : RefCounted<StyleVariableData>(), m_data(other.m_data) { }

    friend class CSSVariableResolver;

    HashMap<AtomicString, RefPtr<CSSVariableData>> m_data;
};

} // namespace blink

#endif // StyleVariableData_h