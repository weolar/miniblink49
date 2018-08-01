
#include "content/browser/CheckReEnter.h"
#include "content/browser/WebPageImpl.h"

namespace content {

int CheckReEnter::s_kEnterCount = 0;

CheckReEnter::CheckReEnter(WebPageImpl* webPageImpl)
{
    m_webPageImpl = webPageImpl;
    if (m_webPageImpl)
        ++m_webPageImpl->m_enterCount;
    incrementEnterCount();
}

CheckReEnter::~CheckReEnter()
{
    if (m_webPageImpl)
        --m_webPageImpl->m_enterCount;
    decrementEnterCount();

    if (m_webPageImpl && pageDestroying == m_webPageImpl->m_state)
        m_webPageImpl->doClose();
}

void CheckReEnter::incrementEnterCount()
{
    ++s_kEnterCount;
}

void CheckReEnter::decrementEnterCount()
{
    --s_kEnterCount;
}

int CheckReEnter::getEnterCount()
{
    return s_kEnterCount;
}

} // content