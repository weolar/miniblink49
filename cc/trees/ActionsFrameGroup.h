
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
	ActionsFrame(int64 beginId);
    ActionsFrame(int64 beginId, int64 endId);
	~ActionsFrame();

	bool isEmpty() const;

	int64 beginId() const { return m_beginId; }
	int64 endId() const { return m_endId; }

	void appendLayerChangeAction(LayerChangeAction* action);

    bool areAllfull() const;

private:
	bool applyActions(ActionsFrameGroup* group, LayerTreeHost* host);
	
	void setEndId(int64 endId);
    void checkFull();

	friend class ActionsFrameGroup;

	WTF::Vector<LayerChangeAction*> m_actions;
	int64 m_beginId;
	int64 m_endId;
	bool m_allAreFull;
	int m_hadRunCount;
};

class ActionsFrameGroup {
public:
	ActionsFrameGroup(LayerTreeHost* host);
	~ActionsFrameGroup();

	void beginRecordActions();
	void endRecordActions();

	int64 genActionId();
	void saveLayerChangeAction(LayerChangeAction* action);
	void appendActionToFrame(LayerChangeAction* action);
	bool applyActions(bool needCheck);
	int64 curActionId() const;
	void incCurActionId();

private:
	WTF::Vector<ActionsFrame*> m_frames;

	ActionsFrame* m_curFrame;
	int64 m_curActionId;
	int64 m_newestActionId;

	WTF::Mutex* m_actionsMutex;
	WTF::Vector<LayerChangeAction*> m_actions;

	LayerTreeHost* m_host;
};

} // namespace cc

#endif // cc_trees_ActionsFrameGroup_h