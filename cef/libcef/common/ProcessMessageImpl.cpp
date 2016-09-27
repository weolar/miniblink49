
#include "include/capi/cef_process_message_capi.h"
#include "libcef/common/CommonBase.h"
#include "wtf/text/WTFString.h"

namespace cef {
class ProcessMessageImpl : public CefCppBase < cef_process_message_t, ProcessMessageImpl > {
public:
    ProcessMessageImpl(const WTF::String& name);
    void willDeleted() {}

    static int CEF_CALLBACK is_valid(struct _cef_process_message_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK is_read_only(struct _cef_process_message_t* self) { DebugBreak(); return 0; }

    static cef_process_message_t* CEF_CALLBACK copy(cef_process_message_t* self) { DebugBreak(); return 0; }

    static cef_string_userfree_t CEF_CALLBACK get_name(cef_process_message_t* self) { DebugBreak(); return 0; }

    static cef_list_value_t* CEF_CALLBACK get_argument_list(cef_process_message_t* self) { return 0; }

    String m_name;
};

ProcessMessageImpl::ProcessMessageImpl(const String& name)
{
    m_name = name;

    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.is_valid = is_valid;
    m_baseClass.is_read_only = is_read_only;
    m_baseClass.copy = copy;
    m_baseClass.get_name = get_name;
    m_baseClass.get_argument_list = get_argument_list;
    CefCppBase<cef_process_message_t, ProcessMessageImpl>::init(this);
}

} // cef

CEF_EXPORT cef_process_message_t* cef_process_message_create(const cef_string_t* name)
{
    cef::ProcessMessageImpl* processMessageImpl = new cef::ProcessMessageImpl(String(name->str, name->length));
    processMessageImpl->ref();

    return processMessageImpl->cast();
}