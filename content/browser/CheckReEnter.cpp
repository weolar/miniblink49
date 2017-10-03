
#include "content/browser/CheckReEnter.h"
#include "content/browser/WebPageImpl.h"

namespace content {

int CheckReEnter::s_kEnterContent = 0;

CheckReEnter::CheckReEnter(WebPageImpl* webPageImpl)
{
    m_webPageImpl = webPageImpl;
    ++m_webPageImpl->m_enterCount;
    ++s_kEnterContent;
}

CheckReEnter::~CheckReEnter()
{
    --m_webPageImpl->m_enterCount;
    --s_kEnterContent;

    if (WebPageImpl::pageDestroying == m_webPageImpl->m_state)
        m_webPageImpl->doClose();
}

} // content