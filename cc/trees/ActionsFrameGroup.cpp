
#include "cc/trees/ActionsFrameGroup.h"

#include "cc/playback/LayerChangeAction.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, actionsFrameCounter, ("ccActionsFrame"));
#endif

ActionsFrame::ActionsFrame()
{
}

ActionsFrame* ActionsFrame::createWithBeginId(int64 beginId, bool isComefromMainframe)
{
    ActionsFrame* out = new ActionsFrame();
    out->m_beginId = beginId;
    out->m_endId = -1;
    out->m_allAreFull = false;
    out->m_hadRunCount = 0;
    out->m_isComefromMainframe = isComefromMainframe;
#ifndef NDEBUG
    actionsFrameCounter.increment();
#endif

    return out;
}

ActionsFrame* ActionsFrame::createWithBeginEndId(int64 beginId, int64 endId, bool isComefromMainframe)
{
    ActionsFrame* out = new ActionsFrame();
    out->m_beginId = beginId;
    out->m_endId = endId;
    out->m_allAreFull = false;
    out->m_hadRunCount = 0;
    out->m_isComefromMainframe = isComefromMainframe;
#ifndef NDEBUG
    actionsFrameCounter.increment();
#endif
    return out;
}

ActionsFrame::~ActionsFrame()
{
    ASSERT(0 == m_actions.size());
#ifndef NDEBUG
    actionsFrameCounter.decrement();
#endif
}

bool ActionsFrame::isEmpty() const
{
    return m_actions.size() == 0;
}

bool ActionsFrame::areAllfull() const 
{
    return m_allAreFull;
}

bool ActionsFrame::isContainBlendActions() const
{
    for (size_t i = 0; i < m_actions.size(); ++i) {
        LayerChangeAction* action = m_actions[i];
        if (LayerChangeAction::LayerChangeBlend == action->type()) {
            return true;
        }
    }
    return false;
}

void ActionsFrame::checkFull()
{
    if (-1 != m_endId && m_actions.size() == (size_t)(m_endId - m_beginId) + 1)
        m_allAreFull = true;
}

void ActionsFrame::appendLayerChangeAction(LayerChangeAction* action)
{
    ASSERT(action->actionId() >= beginId());
    if (-1 != m_endId) { // 如果不为-1,表示是从光栅线程发过来的
        ASSERT(action->actionId() <= endId());
    }

    bool find = false;
    for (int i = (int)(m_actions.size() - 1); i >= 0; --i) {
        LayerChangeAction* actionIt = m_actions[i];
        ASSERT(actionIt->actionId() != action->actionId());
        if (actionIt->actionId() < action->actionId()) {
            m_actions.insert(i + 1, action); // TODO i
            find = true;
            break;
        }
    }

    if (!find)
        m_actions.insert(0, action);

    checkFull();
}

bool ActionsFrame::applyActions(ActionsFrameGroup* group, LayerTreeHost* host)
{
    ASSERT(-1 != m_endId);

    for (size_t i = 0; i < m_actions.size(); ++i) {
        LayerChangeAction* action = m_actions[i];

        ASSERT(-1 != action->actionId());
        if (action->actionId() != group->curActionId()) {
            continue;
        }

//         String outString = String::format("LayerTreeHost::applyActions type: %d, action: %d m_curActionId:%d\n", (int)(action->type()), (int)action->actionId(), group->curActionId());
//         OutputDebugStringW(outString.charactersWithNullTermination().data());

        group->incCurActionId();
        action->run(host);
        ++m_hadRunCount;
    }

    if (m_hadRunCount != m_actions.size())
        return false;

    for (size_t i = 0; i < m_actions.size(); ++i)
        delete m_actions[i];
    m_actions.clear();
    return true;
}

void ActionsFrame::setEndId(int64 endId)
{
    ASSERT(endId > m_endId);
    m_endId = endId;
    checkFull();
}

//////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, actionsFrameGroupCounter, ("ccActionsFrameGroup"));
#endif

ActionsFrameGroup::ActionsFrameGroup(LayerTreeHost* host)
{
    m_curFrame = nullptr;
    m_newestActionId = 0;
    m_curActionId = 1;
    m_actionsMutex = new WTF::Mutex();
    m_host = host;

#ifndef NDEBUG
    actionsFrameGroupCounter.increment();
#endif
}

ActionsFrameGroup::~ActionsFrameGroup()
{
    delete m_curFrame;
    m_curFrame = nullptr;

    delete m_actionsMutex;
    m_actionsMutex = nullptr;

    ASSERT(0 == m_actions.size());
    ASSERT(0 == m_frames.size());

#ifndef NDEBUG
    actionsFrameGroupCounter.decrement();
#endif
}

size_t ActionsFrameGroup::getFramesSize() const
{
    return m_frames.size();
}

void ActionsFrameGroup::beginRecordActions(bool isComefromMainframe)
{
    WTF::MutexLocker locker(*m_actionsMutex);
    ASSERT(!m_curFrame);

    m_curFrame = ActionsFrame::createWithBeginId(m_newestActionId + 1, isComefromMainframe);
    m_frames.append(m_curFrame);
}

void ActionsFrameGroup::endRecordActions()
{
    WTF::MutexLocker locker(*m_actionsMutex);
    ASSERT(m_curFrame);

    if (m_curFrame->beginId() == m_newestActionId + 1) {
        ASSERT(0 != m_frames.size() && m_curFrame == m_frames.last() && m_curFrame->isEmpty());
        m_frames.removeLast();
        delete m_curFrame;
        m_curFrame = nullptr;
        return;
    }

    m_curFrame->setEndId(m_newestActionId);
    m_curFrame = nullptr;
}

int64 ActionsFrameGroup::genActionId()
{
    ASSERT(WTF::isMainThread());
    WTF::MutexLocker locker(*m_actionsMutex);

    if (!m_curFrame) { // 如果不属于任何一个，说明是一些异步回调调用(如网络回调)进来的，另起一帧
        m_curFrame = ActionsFrame::createWithBeginEndId(m_newestActionId + 1, m_newestActionId + 1, false);
        m_frames.append(m_curFrame);
        m_curFrame = nullptr;
    }

    atomicIncrement(&m_newestActionId);

    return m_newestActionId;
}

void ActionsFrameGroup::saveLayerChangeAction(LayerChangeAction* action)
{
    m_actionsMutex->lock();
    m_actions.append(action);
    m_actionsMutex->unlock();
}

void ActionsFrameGroup::appendActionToFrame(LayerChangeAction* action)
{
    WTF::MutexLocker locker(*m_actionsMutex);
    if (m_frames.size() == 0) {
        ASSERT(false);
        return;
    }

    ActionsFrame* frame = m_frames[m_frames.size() - 1];
   
    if (-1 != frame->endId() && action->actionId() > frame->endId()) {
        ASSERT(!m_curFrame);
        return;
    }

    bool appendOk = false;
    for (int i = (int)(m_frames.size() - 1); i >= 0; --i) {
        frame = m_frames[i];
        if (action->actionId() < frame->beginId())
            continue;
        
        appendOk = true;
        frame->appendLayerChangeAction(action);
        break;
    }
    ASSERT(frame);
    RELEASE_ASSERT(appendOk);
}

bool ActionsFrameGroup::applyActions(bool needCheck)
{
    m_actionsMutex->lock();
    WTF::Vector<LayerChangeAction*> actions = m_actions;
    m_actions.clear();
    m_actionsMutex->unlock();

    for (size_t i = 0; i < actions.size(); ++i) {
        appendActionToFrame(actions[i]);
    }

    bool okOnce = false;
    while (true) {
        m_actionsMutex->lock();
        if (0 == m_frames.size()) {
            okOnce = true;
            m_actionsMutex->unlock();
            break;
        }

        ActionsFrame* frame = m_frames[0];
        if (!frame->areAllfull()) {
            ASSERT(!needCheck);
        }
        if (!needCheck && !canApplyActions()) {
            m_actionsMutex->unlock();
            return okOnce;
        }

        m_frames.remove(0);

        m_actionsMutex->unlock();

        bool ok = frame->applyActions(this, m_host);
        ASSERT(ok);
        delete frame;

        okOnce = true;
    }

    return okOnce;
}

bool ActionsFrameGroup::canApplyActions() const
{
    for (size_t i = 0; i < m_frames.size(); ++i) {
        ActionsFrame* frame = m_frames[i];
        bool isComefromMainframe = frame->isComefromMainframe();
        if (frame->areAllfull()) {
            if (isComefromMainframe)
                return true;
            else
                continue;
        } else
            return false;
    }
    return false;
}

bool ActionsFrameGroup::containComefromMainframeLocked() const
{
    WTF::MutexLocker locker(*m_actionsMutex);

    for (size_t i = 0; i < m_frames.size(); ++i) {
        ActionsFrame* frame = m_frames[i];
        bool isComefromMainframe = frame->isComefromMainframe();
        if (isComefromMainframe)
            return true;
    }
    return false;
}

int64 ActionsFrameGroup::curActionId() const
{
    int64 curActionId;
    WTF::MutexLocker locker(*m_actionsMutex);
    curActionId =  m_curActionId;

    return curActionId;
}

void ActionsFrameGroup::incCurActionId()
{
    WTF::MutexLocker locker(*m_actionsMutex);
    ++m_curActionId;
}

} // cc