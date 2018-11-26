
#ifndef cc_trees_ActionsFrameGroup_h
#define cc_trees_ActionsFrameGroup_h

#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

namespace cc {

class ActionsFrameGroup;
class LayerChangeAction;
class LayerTreeHost;

class ActionsFrame {
public:
    static ActionsFrame* createWithBeginId(int64 beginId, bool isComefromMainframe);
    static ActionsFrame* createWithBeginEndId(int64 beginId, int64 endId, bool isComefromMainframe);
    ~ActionsFrame();

    bool isEmpty() const;

    int64 beginId() const { return m_beginId; }
    int64 endId() const { return m_endId; }

    void appendLayerChangeAction(LayerChangeAction* action);

    bool areAllfull() const;
    bool isContainBlendActions() const;

    bool isComefromMainframe() const { return m_isComefromMainframe; }

private:
    ActionsFrame();

    bool applyActions(ActionsFrameGroup* group, LayerTreeHost* host);
    
    void setEndId(int64 endId);
    void checkFull();

    friend class ActionsFrameGroup;

    WTF::Vector<LayerChangeAction*> m_actions;
    int64 m_beginId;
    int64 m_endId;
    bool m_allAreFull;
    int m_hadRunCount;
    bool m_isComefromMainframe;
};

class ActionsFrameGroup {
public:
    ActionsFrameGroup(LayerTreeHost* host);
    ~ActionsFrameGroup();

    void beginRecordActions(bool isComefromMainframe);
    void endRecordActions();

    int64 genActionId();
    void saveLayerChangeAction(LayerChangeAction* action);
    void appendActionToFrame(LayerChangeAction* action);
    bool applyActions(bool needCheck);
    int64 curActionId() const;
    void incCurActionId();

    size_t getFramesSize() const;

    bool containComefromMainframeLocked() const;

private:
    bool canApplyActions() const;

    WTF::Vector<ActionsFrame*> m_frames;

    ActionsFrame* m_curFrame;
    int64 m_curActionId;
    int64 m_newestActionId;

    mutable WTF::Mutex* m_actionsMutex;
    WTF::Vector<LayerChangeAction*> m_actions;

    LayerTreeHost* m_host;
};

} // namespace cc

#endif // cc_trees_ActionsFrameGroup_h