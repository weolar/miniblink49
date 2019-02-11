#include "base/macros.h"

namespace content {

#define MAKE_UCHAR_TO_WEBSTRING(s) \
    blink::WebString(s, sizeof(s)/sizeof(WebUChar))

blink::WebString queryLocalizedStringFromResources(blink::WebLocalizedString::Name name)
{
    blink::WebString out;
    switch (name) {
    case blink::WebLocalizedString::BlockedPluginText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"阻塞插件");
        break;
    case blink::WebLocalizedString::FileButtonChooseFileLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"选择文件");
        break;
    case blink::WebLocalizedString::FileButtonChooseMultipleFilesLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"选择多个文件");
        break;
    case blink::WebLocalizedString::FileButtonNoFileSelectedLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"没有文件被选中");
        break;
    case blink::WebLocalizedString::InputElementAltText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"AltText");
        break;
    case blink::WebLocalizedString::MissingPluginText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"缺少插件");
        break;
    case blink::WebLocalizedString::MultipleFileUploadText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"上传文件");
        break;
    case blink::WebLocalizedString::OtherColorLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"其他颜色");
        break;
    case blink::WebLocalizedString::OtherDateLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"其他日期");
        break;
    case blink::WebLocalizedString::OtherMonthLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"其他月份");
        break;
    case blink::WebLocalizedString::ResetButtonDefaultLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"重置");
        break;
    case blink::WebLocalizedString::SearchableIndexIntroduction:
        out = MAKE_UCHAR_TO_WEBSTRING(L"SearchableIndexIntroduction");
        break;
    case blink::WebLocalizedString::SearchMenuClearRecentSearchesText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"SearchMenuClearRecentSearchesText");
        break;
    case blink::WebLocalizedString::SelectMenuListText:
        out = MAKE_UCHAR_TO_WEBSTRING(L"选择菜单");
        break;
    case blink::WebLocalizedString::SubmitButtonDefaultLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"提交");
        break;
    case blink::WebLocalizedString::ThisMonthButtonLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"本月");
        break;
    case blink::WebLocalizedString::ThisWeekButtonLabel:
        out = MAKE_UCHAR_TO_WEBSTRING(L"本周");
        break;
    }

    return out;
}

#undef MAKE_UCHAR_TO_WEBSTRING

} // namespace content