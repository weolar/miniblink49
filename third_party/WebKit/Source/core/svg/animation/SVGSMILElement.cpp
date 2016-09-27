/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/svg/animation/SVGSMILElement.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptEventListener.h"
#include "core/XLinkNames.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/events/EventListener.h"
#include "core/events/EventSender.h"
#include "core/frame/UseCounter.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGURIReference.h"
#include "core/svg/animation/SMILTimeContainer.h"
#include "platform/FloatConversion.h"
#include "wtf/MathExtras.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Vector.h"

namespace blink {

class RepeatEvent final : public Event {
public:
    static PassRefPtrWillBeRawPtr<RepeatEvent> create(const AtomicString& type, int repeat)
    {
        return adoptRefWillBeNoop(new RepeatEvent(type, false, false, repeat));
    }

    ~RepeatEvent() override {}

    int repeat() const { return m_repeat; }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        Event::trace(visitor);
    }

protected:
    RepeatEvent(const AtomicString& type, bool canBubble, bool cancelable, int repeat = -1)
        : Event(type, canBubble, cancelable)
        , m_repeat(repeat)
    {
    }

private:
    int m_repeat;
};

inline RepeatEvent* toRepeatEvent(Event* event)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!event || event->type() == "repeatn");
    return static_cast<RepeatEvent*>(event);
}

static SMILEventSender& smilEndEventSender()
{
    DEFINE_STATIC_LOCAL(SMILEventSender, sender, (EventTypeNames::endEvent));
    return sender;
}

static SMILEventSender& smilBeginEventSender()
{
    DEFINE_STATIC_LOCAL(SMILEventSender, sender, (EventTypeNames::beginEvent));
    return sender;
}

static SMILEventSender& smilRepeatEventSender()
{
    DEFINE_STATIC_LOCAL(SMILEventSender, sender, (EventTypeNames::repeatEvent));
    return sender;
}

static SMILEventSender& smilRepeatNEventSender()
{
    DEFINE_STATIC_LOCAL(SMILEventSender, sender, (AtomicString("repeatn", AtomicString::ConstructFromLiteral)));
    return sender;
}

// This is used for duration type time values that can't be negative.
static const double invalidCachedTime = -1.;

class ConditionEventListener final : public EventListener {
public:
    static PassRefPtr<ConditionEventListener> create(SVGSMILElement* animation, SVGSMILElement::Condition* condition)
    {
        return adoptRef(new ConditionEventListener(animation, condition));
    }

    static const ConditionEventListener* cast(const EventListener* listener)
    {
        return listener->type() == ConditionEventListenerType
            ? static_cast<const ConditionEventListener*>(listener)
            : nullptr;
    }

    bool operator==(const EventListener& other) override;

    void disconnectAnimation()
    {
        m_animation = nullptr;
    }

private:
    ConditionEventListener(SVGSMILElement* animation, SVGSMILElement::Condition* condition)
        : EventListener(ConditionEventListenerType)
        , m_animation(animation)
        , m_condition(condition)
    {
    }

    void handleEvent(ExecutionContext*, Event*) override;

    SVGSMILElement* m_animation;
    SVGSMILElement::Condition* m_condition;
};

bool ConditionEventListener::operator==(const EventListener& listener)
{
    if (const ConditionEventListener* conditionEventListener = ConditionEventListener::cast(&listener))
        return m_animation == conditionEventListener->m_animation && m_condition == conditionEventListener->m_condition;
    return false;
}

void ConditionEventListener::handleEvent(ExecutionContext*, Event* event)
{
    if (!m_animation)
        return;
    m_animation->handleConditionEvent(event, m_condition);
}

void SVGSMILElement::Condition::setEventListener(PassRefPtr<ConditionEventListener> eventListener)
{
    m_eventListener = eventListener;
}

SVGSMILElement::Condition::Condition(Type type, BeginOrEnd beginOrEnd, const String& baseID, const String& name, SMILTime offset, int repeat)
    : m_type(type)
    , m_beginOrEnd(beginOrEnd)
    , m_baseID(baseID)
    , m_name(name)
    , m_offset(offset)
    , m_repeat(repeat)
{
}

SVGSMILElement::SVGSMILElement(const QualifiedName& tagName, Document& doc)
    : SVGElement(tagName, doc)
    , SVGTests(this)
    , m_attributeName(anyQName())
    , m_targetElement(nullptr)
    , m_syncBaseConditionsConnected(false)
    , m_hasEndEventConditions(false)
    , m_isWaitingForFirstInterval(true)
    , m_isScheduled(false)
    , m_interval(SMILInterval(SMILTime::unresolved(), SMILTime::unresolved()))
    , m_previousIntervalBegin(SMILTime::unresolved())
    , m_activeState(Inactive)
    , m_lastPercent(0)
    , m_lastRepeat(0)
    , m_nextProgressTime(0)
    , m_documentOrderIndex(0)
    , m_cachedDur(invalidCachedTime)
    , m_cachedRepeatDur(invalidCachedTime)
    , m_cachedRepeatCount(invalidCachedTime)
    , m_cachedMin(invalidCachedTime)
    , m_cachedMax(invalidCachedTime)
{
    resolveFirstInterval();
}

SVGSMILElement::~SVGSMILElement()
{
#if !ENABLE(OILPAN)
    clearResourceAndEventBaseReferences();
#endif
    smilEndEventSender().cancelEvent(this);
    smilBeginEventSender().cancelEvent(this);
    smilRepeatEventSender().cancelEvent(this);
    smilRepeatNEventSender().cancelEvent(this);
#if !ENABLE(OILPAN)
    clearConditions();

    unscheduleIfScheduled();
#endif
}

void SVGSMILElement::clearResourceAndEventBaseReferences()
{
    removeAllOutgoingReferences();
}

void SVGSMILElement::clearConditions()
{
    disconnectSyncBaseConditions();
    disconnectEventBaseConditions();
    m_conditions.clear();
}

void SVGSMILElement::buildPendingResource()
{
    clearResourceAndEventBaseReferences();

    if (!inDocument()) {
        // Reset the target element if we are no longer in the document.
        setTargetElement(nullptr);
        return;
    }

    AtomicString id;
    AtomicString href = getAttribute(XLinkNames::hrefAttr);
    Element* target;
    if (href.isEmpty())
        target = parentNode() && parentNode()->isElementNode() ? toElement(parentNode()) : nullptr;
    else
        target = SVGURIReference::targetElementFromIRIString(href, treeScope(), &id);
    SVGElement* svgTarget = target && target->isSVGElement() ? toSVGElement(target) : nullptr;

    if (svgTarget && !svgTarget->inDocument())
        svgTarget = nullptr;

    if (svgTarget != targetElement())
        setTargetElement(svgTarget);

    if (!svgTarget) {
        // Do not register as pending if we are already pending this resource.
        if (document().accessSVGExtensions().isElementPendingResource(this, id))
            return;

        if (!id.isEmpty()) {
            document().accessSVGExtensions().addPendingResource(id, this);
            ASSERT(hasPendingResources());
        }
    } else {
        // Register us with the target in the dependencies map. Any change of hrefElement
        // that leads to relayout/repainting now informs us, so we can react to it.
        addReferenceTo(svgTarget);
    }
    connectEventBaseConditions();
}

static inline QualifiedName constructQualifiedName(const SVGElement* svgElement, const AtomicString& attributeName)
{
    ASSERT(svgElement);
    if (attributeName.isEmpty())
        return anyQName();
    if (!attributeName.contains(':'))
        return QualifiedName(nullAtom, attributeName, nullAtom);

    AtomicString prefix;
    AtomicString localName;
    if (!Document::parseQualifiedName(attributeName, prefix, localName, IGNORE_EXCEPTION))
        return anyQName();

    const AtomicString& namespaceURI = svgElement->lookupNamespaceURI(prefix);
    if (namespaceURI.isEmpty())
        return anyQName();

    return QualifiedName(nullAtom, localName, namespaceURI);
}

static inline void clearTimesWithDynamicOrigins(Vector<SMILTimeWithOrigin>& timeList)
{
    for (int i = timeList.size() - 1; i >= 0; --i) {
        if (timeList[i].originIsScript())
            timeList.remove(i);
    }
}

void SVGSMILElement::reset()
{
    clearAnimatedType();

    m_activeState = Inactive;
    m_isWaitingForFirstInterval = true;
    m_interval.begin = SMILTime::unresolved();
    m_interval.end = SMILTime::unresolved();
    m_previousIntervalBegin = SMILTime::unresolved();
    m_lastPercent = 0;
    m_lastRepeat = 0;
    m_nextProgressTime = 0;
    resolveFirstInterval();
}

Node::InsertionNotificationRequest SVGSMILElement::insertedInto(ContainerNode* rootParent)
{
    SVGElement::insertedInto(rootParent);

    if (!rootParent->inDocument())
        return InsertionDone;

    UseCounter::countDeprecation(document(), UseCounter::SVGSMILElementInDocument);

    setAttributeName(constructQualifiedName(this, fastGetAttribute(SVGNames::attributeNameAttr)));
    SVGSVGElement* owner = ownerSVGElement();
    if (!owner)
        return InsertionDone;

    m_timeContainer = owner->timeContainer();
    ASSERT(m_timeContainer);
    m_timeContainer->setDocumentOrderIndexesDirty();

    // "If no attribute is present, the default begin value (an offset-value of 0) must be evaluated."
    if (!fastHasAttribute(SVGNames::beginAttr))
        m_beginTimes.append(SMILTimeWithOrigin());

    if (m_isWaitingForFirstInterval)
        resolveFirstInterval();

    if (m_timeContainer)
        m_timeContainer->notifyIntervalsChanged();

    buildPendingResource();

    return InsertionDone;
}

void SVGSMILElement::removedFrom(ContainerNode* rootParent)
{
    if (rootParent->inDocument()) {
        clearResourceAndEventBaseReferences();
        clearConditions();
        setTargetElement(nullptr);
        setAttributeName(anyQName());
        animationAttributeChanged();
        m_timeContainer = nullptr;
    }

    SVGElement::removedFrom(rootParent);
}

bool SVGSMILElement::hasValidAttributeName()
{
    return attributeName() != anyQName();
}

SMILTime SVGSMILElement::parseOffsetValue(const String& data)
{
    bool ok;
    double result = 0;
    String parse = data.stripWhiteSpace();
    if (parse.endsWith('h'))
        result = parse.left(parse.length() - 1).toDouble(&ok) * 60 * 60;
    else if (parse.endsWith("min"))
        result = parse.left(parse.length() - 3).toDouble(&ok) * 60;
    else if (parse.endsWith("ms"))
        result = parse.left(parse.length() - 2).toDouble(&ok) / 1000;
    else if (parse.endsWith('s'))
        result = parse.left(parse.length() - 1).toDouble(&ok);
    else
        result = parse.toDouble(&ok);
    if (!ok || !SMILTime(result).isFinite())
        return SMILTime::unresolved();
    return result;
}

SMILTime SVGSMILElement::parseClockValue(const String& data)
{
    if (data.isNull())
        return SMILTime::unresolved();

    String parse = data.stripWhiteSpace();

    DEFINE_STATIC_LOCAL(const AtomicString, indefiniteValue, ("indefinite", AtomicString::ConstructFromLiteral));
    if (parse == indefiniteValue)
        return SMILTime::indefinite();

    double result = 0;
    bool ok;
    size_t doublePointOne = parse.find(':');
    size_t doublePointTwo = parse.find(':', doublePointOne + 1);
    if (doublePointOne == 2 && doublePointTwo == 5 && parse.length() >= 8) {
        result += parse.substring(0, 2).toUIntStrict(&ok) * 60 * 60;
        if (!ok)
            return SMILTime::unresolved();
        result += parse.substring(3, 2).toUIntStrict(&ok) * 60;
        if (!ok)
            return SMILTime::unresolved();
        result += parse.substring(6).toDouble(&ok);
    } else if (doublePointOne == 2 && doublePointTwo == kNotFound && parse.length() >= 5) {
        result += parse.substring(0, 2).toUIntStrict(&ok) * 60;
        if (!ok)
            return SMILTime::unresolved();
        result += parse.substring(3).toDouble(&ok);
    } else
        return parseOffsetValue(parse);

    if (!ok || !SMILTime(result).isFinite())
        return SMILTime::unresolved();
    return result;
}

static void sortTimeList(Vector<SMILTimeWithOrigin>& timeList)
{
    std::sort(timeList.begin(), timeList.end());
}

bool SVGSMILElement::parseCondition(const String& value, BeginOrEnd beginOrEnd)
{
    String parseString = value.stripWhiteSpace();

    double sign = 1.;
    bool ok;
    size_t pos = parseString.find('+');
    if (pos == kNotFound) {
        pos = parseString.find('-');
        if (pos != kNotFound)
            sign = -1.;
    }
    String conditionString;
    SMILTime offset = 0;
    if (pos == kNotFound)
        conditionString = parseString;
    else {
        conditionString = parseString.left(pos).stripWhiteSpace();
        String offsetString = parseString.substring(pos + 1).stripWhiteSpace();
        offset = parseOffsetValue(offsetString);
        if (offset.isUnresolved())
            return false;
        offset = offset * sign;
    }
    if (conditionString.isEmpty())
        return false;
    pos = conditionString.find('.');

    String baseID;
    String nameString;
    if (pos == kNotFound)
        nameString = conditionString;
    else {
        baseID = conditionString.left(pos);
        nameString = conditionString.substring(pos + 1);
    }
    if (nameString.isEmpty())
        return false;

    Condition::Type type;
    int repeat = -1;
    if (nameString.startsWith("repeat(") && nameString.endsWith(')')) {
        repeat = nameString.substring(7, nameString.length() - 8).toUIntStrict(&ok);
        if (!ok)
            return false;
        nameString = "repeatn";
        type = Condition::EventBase;
    } else if (nameString == "begin" || nameString == "end") {
        if (baseID.isEmpty())
            return false;
        type = Condition::Syncbase;
    } else if (nameString.startsWith("accesskey(")) {
        // FIXME: accesskey() support.
        type = Condition::AccessKey;
    } else
        type = Condition::EventBase;

    m_conditions.append(Condition::create(type, beginOrEnd, baseID, nameString, offset, repeat));

    if (type == Condition::EventBase && beginOrEnd == End)
        m_hasEndEventConditions = true;

    return true;
}

void SVGSMILElement::parseBeginOrEnd(const String& parseString, BeginOrEnd beginOrEnd)
{
    Vector<SMILTimeWithOrigin>& timeList = beginOrEnd == Begin ? m_beginTimes : m_endTimes;
    if (beginOrEnd == End)
        m_hasEndEventConditions = false;
    HashSet<SMILTime> existing;
    for (unsigned n = 0; n < timeList.size(); ++n) {
        if (!timeList[n].time().isUnresolved())
            existing.add(timeList[n].time().value());
    }
    Vector<String> splitString;
    parseString.split(';', splitString);
    for (unsigned n = 0; n < splitString.size(); ++n) {
        SMILTime value = parseClockValue(splitString[n]);
        if (value.isUnresolved())
            parseCondition(splitString[n], beginOrEnd);
        else if (!existing.contains(value.value()))
            timeList.append(SMILTimeWithOrigin(value, SMILTimeWithOrigin::ParserOrigin));
    }
    sortTimeList(timeList);
}

void SVGSMILElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == SVGNames::beginAttr) {
        if (!m_conditions.isEmpty()) {
            clearConditions();
            parseBeginOrEnd(fastGetAttribute(SVGNames::endAttr), End);
        }
        parseBeginOrEnd(value.string(), Begin);
        if (inDocument())
            connectSyncBaseConditions();
    } else if (name == SVGNames::endAttr) {
        if (!m_conditions.isEmpty()) {
            clearConditions();
            parseBeginOrEnd(fastGetAttribute(SVGNames::beginAttr), Begin);
        }
        parseBeginOrEnd(value.string(), End);
        if (inDocument())
            connectSyncBaseConditions();
    } else if (name == SVGNames::onbeginAttr) {
        setAttributeEventListener(EventTypeNames::beginEvent, createAttributeEventListener(this, name, value, eventParameterName()));
    } else if (name == SVGNames::onendAttr) {
        setAttributeEventListener(EventTypeNames::endEvent, createAttributeEventListener(this, name, value, eventParameterName()));
    } else if (name == SVGNames::onrepeatAttr) {
        setAttributeEventListener(EventTypeNames::repeatEvent, createAttributeEventListener(this, name, value, eventParameterName()));
    } else {
        SVGElement::parseAttribute(name, value);
    }
}

void SVGSMILElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::durAttr)
        m_cachedDur = invalidCachedTime;
    else if (attrName == SVGNames::repeatDurAttr)
        m_cachedRepeatDur = invalidCachedTime;
    else if (attrName == SVGNames::repeatCountAttr)
        m_cachedRepeatCount = invalidCachedTime;
    else if (attrName == SVGNames::minAttr)
        m_cachedMin = invalidCachedTime;
    else if (attrName == SVGNames::maxAttr)
        m_cachedMax = invalidCachedTime;
    else if (attrName == SVGNames::attributeNameAttr)
        setAttributeName(constructQualifiedName(this, fastGetAttribute(SVGNames::attributeNameAttr)));
    else if (attrName.matches(XLinkNames::hrefAttr)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        buildPendingResource();
        if (m_targetElement)
            clearAnimatedType();
    } else if (attrName == SVGNames::beginAttr || attrName == SVGNames::endAttr) {
        if (inDocument()) {
            connectEventBaseConditions();
            if (attrName == SVGNames::beginAttr)
                beginListChanged(elapsed());
            else if (attrName == SVGNames::endAttr)
                endListChanged(elapsed());
        }
    } else {
        SVGElement::svgAttributeChanged(attrName);
        return;
    }

    animationAttributeChanged();
}

inline SVGElement* SVGSMILElement::eventBaseFor(const Condition& condition)
{
    Element* eventBase = condition.baseID().isEmpty() ? targetElement() : treeScope().getElementById(AtomicString(condition.baseID()));
    if (eventBase && eventBase->isSVGElement())
        return toSVGElement(eventBase);
    return nullptr;
}

void SVGSMILElement::connectSyncBaseConditions()
{
    if (m_syncBaseConditionsConnected)
        disconnectSyncBaseConditions();
    m_syncBaseConditionsConnected = true;
    for (unsigned n = 0; n < m_conditions.size(); ++n) {
        Condition* condition = m_conditions[n].get();
        if (condition->type() == Condition::Syncbase) {
            ASSERT(!condition->baseID().isEmpty());
            Element* element = treeScope().getElementById(AtomicString(condition->baseID()));
            if (!element || !isSVGSMILElement(*element)) {
                condition->setSyncBase(0);
                continue;
            }
            SVGSMILElement* svgSMILElement = toSVGSMILElement(element);
            condition->setSyncBase(svgSMILElement);
            svgSMILElement->addSyncBaseDependent(this);
        }
    }
}

void SVGSMILElement::disconnectSyncBaseConditions()
{
    if (!m_syncBaseConditionsConnected)
        return;
    m_syncBaseConditionsConnected = false;
    for (unsigned n = 0; n < m_conditions.size(); ++n) {
        Condition* condition = m_conditions[n].get();
        if (condition->type() == Condition::Syncbase) {
            if (condition->syncBase())
                condition->syncBase()->removeSyncBaseDependent(this);
            condition->setSyncBase(0);
        }
    }
}

void SVGSMILElement::connectEventBaseConditions()
{
    disconnectEventBaseConditions();
    for (unsigned n = 0; n < m_conditions.size(); ++n) {
        Condition* condition = m_conditions[n].get();
        if (condition->type() == Condition::EventBase) {
            ASSERT(!condition->syncBase());
            SVGElement* eventBase = eventBaseFor(*condition);
            if (!eventBase) {
                if (!condition->baseID().isEmpty() && !document().accessSVGExtensions().isElementPendingResource(this, AtomicString(condition->baseID())))
                    document().accessSVGExtensions().addPendingResource(AtomicString(condition->baseID()), this);
                continue;
            }
            ASSERT(!condition->eventListener());
            condition->setEventListener(ConditionEventListener::create(this, condition));
            eventBase->addEventListener(AtomicString(condition->name()), condition->eventListener(), false);
            addReferenceTo(eventBase);
        }
    }
}

void SVGSMILElement::disconnectEventBaseConditions()
{
    for (unsigned n = 0; n < m_conditions.size(); ++n) {
        Condition* condition = m_conditions[n].get();
        if (condition->type() == Condition::EventBase) {
            ASSERT(!condition->syncBase());
            if (!condition->eventListener())
                continue;
            // Note: It's a memory optimization to try to remove our condition
            // event listener, but it's not guaranteed to work, since we have
            // no guarantee that eventBaseFor() will be able to find our condition's
            // original eventBase. So, we also have to disconnect ourselves from
            // our condition event listener, in case it later fires.
            SVGElement* eventBase = eventBaseFor(*condition);
            if (eventBase)
                eventBase->removeEventListener(AtomicString(condition->name()), condition->eventListener(), false);
            condition->eventListener()->disconnectAnimation();
            condition->setEventListener(nullptr);
        }
    }
}

void SVGSMILElement::setAttributeName(const QualifiedName& attributeName)
{
    unscheduleIfScheduled();
    if (m_targetElement)
        clearAnimatedType();
    m_attributeName = attributeName;
    schedule();
}

void SVGSMILElement::setTargetElement(SVGElement* target)
{
    unscheduleIfScheduled();

    if (m_targetElement) {
        // Clear values that may depend on the previous target.
        clearAnimatedType();
        disconnectSyncBaseConditions();
    }

    // If the animation state is not Inactive, always reset to a clear state before leaving the old target element.
    if (m_activeState != Inactive)
        endedActiveInterval();

    m_targetElement = target;
    schedule();
}

SMILTime SVGSMILElement::elapsed() const
{
    return m_timeContainer ? m_timeContainer->elapsed() : 0;
}

bool SVGSMILElement::isFrozen() const
{
    return m_activeState == Frozen;
}

SVGSMILElement::Restart SVGSMILElement::restart() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, never, ("never", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, whenNotActive, ("whenNotActive", AtomicString::ConstructFromLiteral));
    const AtomicString& value = fastGetAttribute(SVGNames::restartAttr);
    if (value == never)
        return RestartNever;
    if (value == whenNotActive)
        return RestartWhenNotActive;
    return RestartAlways;
}

SVGSMILElement::FillMode SVGSMILElement::fill() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, freeze, ("freeze", AtomicString::ConstructFromLiteral));
    const AtomicString& value = fastGetAttribute(SVGNames::fillAttr);
    return value == freeze ? FillFreeze : FillRemove;
}

SMILTime SVGSMILElement::dur() const
{
    if (m_cachedDur != invalidCachedTime)
        return m_cachedDur;
    const AtomicString& value = fastGetAttribute(SVGNames::durAttr);
    SMILTime clockValue = parseClockValue(value);
    return m_cachedDur = clockValue <= 0 ? SMILTime::unresolved() : clockValue;
}

SMILTime SVGSMILElement::repeatDur() const
{
    if (m_cachedRepeatDur != invalidCachedTime)
        return m_cachedRepeatDur;
    const AtomicString& value = fastGetAttribute(SVGNames::repeatDurAttr);
    SMILTime clockValue = parseClockValue(value);
    m_cachedRepeatDur = clockValue <= 0 ? SMILTime::unresolved() : clockValue;
    return m_cachedRepeatDur;
}

// So a count is not really a time but let just all pretend we did not notice.
SMILTime SVGSMILElement::repeatCount() const
{
    if (m_cachedRepeatCount != invalidCachedTime)
        return m_cachedRepeatCount;
    SMILTime computedRepeatCount = SMILTime::unresolved();
    const AtomicString& value = fastGetAttribute(SVGNames::repeatCountAttr);
    if (!value.isNull()) {
        DEFINE_STATIC_LOCAL(const AtomicString, indefiniteValue, ("indefinite", AtomicString::ConstructFromLiteral));
        if (value == indefiniteValue) {
            computedRepeatCount = SMILTime::indefinite();
        } else {
            bool ok;
            double result = value.toDouble(&ok);
            if (ok && result > 0)
                computedRepeatCount = result;
        }
    }
    m_cachedRepeatCount = computedRepeatCount;
    return m_cachedRepeatCount;
}

SMILTime SVGSMILElement::maxValue() const
{
    if (m_cachedMax != invalidCachedTime)
        return m_cachedMax;
    const AtomicString& value = fastGetAttribute(SVGNames::maxAttr);
    SMILTime result = parseClockValue(value);
    return m_cachedMax = (result.isUnresolved() || result <= 0) ? SMILTime::indefinite() : result;
}

SMILTime SVGSMILElement::minValue() const
{
    if (m_cachedMin != invalidCachedTime)
        return m_cachedMin;
    const AtomicString& value = fastGetAttribute(SVGNames::minAttr);
    SMILTime result = parseClockValue(value);
    return m_cachedMin = (result.isUnresolved() || result < 0) ? 0 : result;
}

SMILTime SVGSMILElement::simpleDuration() const
{
    return std::min(dur(), SMILTime::indefinite());
}

void SVGSMILElement::addBeginTime(SMILTime eventTime, SMILTime beginTime, SMILTimeWithOrigin::Origin origin)
{
    m_beginTimes.append(SMILTimeWithOrigin(beginTime, origin));
    sortTimeList(m_beginTimes);
    beginListChanged(eventTime);
}

void SVGSMILElement::addEndTime(SMILTime eventTime, SMILTime endTime, SMILTimeWithOrigin::Origin origin)
{
    m_endTimes.append(SMILTimeWithOrigin(endTime, origin));
    sortTimeList(m_endTimes);
    endListChanged(eventTime);
}

inline bool compareTimes(const SMILTimeWithOrigin& left, const SMILTimeWithOrigin& right)
{
    return left.time() < right.time();
}

SMILTime SVGSMILElement::findInstanceTime(BeginOrEnd beginOrEnd, SMILTime minimumTime, bool equalsMinimumOK) const
{
    const Vector<SMILTimeWithOrigin>& list = beginOrEnd == Begin ? m_beginTimes : m_endTimes;
    int sizeOfList = list.size();

    if (!sizeOfList)
        return beginOrEnd == Begin ? SMILTime::unresolved() : SMILTime::indefinite();

    const SMILTimeWithOrigin dummyTimeWithOrigin(minimumTime, SMILTimeWithOrigin::ParserOrigin);
    const SMILTimeWithOrigin* result = std::lower_bound(list.begin(), list.end(), dummyTimeWithOrigin, compareTimes);
    int indexOfResult = result - list.begin();
    if (indexOfResult == sizeOfList)
        return SMILTime::unresolved();
    const SMILTime& currentTime = list[indexOfResult].time();

    // The special value "indefinite" does not yield an instance time in the begin list.
    if (currentTime.isIndefinite() && beginOrEnd == Begin)
        return SMILTime::unresolved();

    if (currentTime > minimumTime)
        return currentTime;

    ASSERT(currentTime == minimumTime);
    if (equalsMinimumOK)
        return currentTime;

    // If the equals is not accepted, return the next bigger item in the list.
    SMILTime nextTime = currentTime;
    while (indexOfResult < sizeOfList - 1) {
        nextTime = list[indexOfResult + 1].time();
        if (nextTime > minimumTime)
            return nextTime;
        ++indexOfResult;
    }

    return beginOrEnd == Begin ? SMILTime::unresolved() : SMILTime::indefinite();
}

SMILTime SVGSMILElement::repeatingDuration() const
{
    // Computing the active duration
    // http://www.w3.org/TR/SMIL2/smil-timing.html#Timing-ComputingActiveDur
    SMILTime repeatCount = this->repeatCount();
    SMILTime repeatDur = this->repeatDur();
    SMILTime simpleDuration = this->simpleDuration();
    if (!simpleDuration || (repeatDur.isUnresolved() && repeatCount.isUnresolved()))
        return simpleDuration;
    repeatDur = std::min(repeatDur, SMILTime::indefinite());
    SMILTime repeatCountDuration = simpleDuration * repeatCount;
    if (!repeatCountDuration.isUnresolved())
        return std::min(repeatDur, repeatCountDuration);
    return repeatDur;
}

SMILTime SVGSMILElement::resolveActiveEnd(SMILTime resolvedBegin, SMILTime resolvedEnd) const
{
    // Computing the active duration
    // http://www.w3.org/TR/SMIL2/smil-timing.html#Timing-ComputingActiveDur
    SMILTime preliminaryActiveDuration;
    if (!resolvedEnd.isUnresolved() && dur().isUnresolved() && repeatDur().isUnresolved() && repeatCount().isUnresolved())
        preliminaryActiveDuration = resolvedEnd - resolvedBegin;
    else if (!resolvedEnd.isFinite())
        preliminaryActiveDuration = repeatingDuration();
    else
        preliminaryActiveDuration = std::min(repeatingDuration(), resolvedEnd - resolvedBegin);

    SMILTime minValue = this->minValue();
    SMILTime maxValue = this->maxValue();
    if (minValue > maxValue) {
        // Ignore both.
        // http://www.w3.org/TR/2001/REC-smil-animation-20010904/#MinMax
        minValue = 0;
        maxValue = SMILTime::indefinite();
    }
    return resolvedBegin + std::min(maxValue, std::max(minValue, preliminaryActiveDuration));
}

SMILInterval SVGSMILElement::resolveInterval(ResolveInterval resolveIntervalType) const
{
    bool first = resolveIntervalType == FirstInterval;
    // See the pseudocode in http://www.w3.org/TR/SMIL3/smil-timing.html#q90.
    SMILTime beginAfter = first ? -std::numeric_limits<double>::infinity() : m_interval.end;
    SMILTime lastIntervalTempEnd = std::numeric_limits<double>::infinity();
    while (true) {
        bool equalsMinimumOK = !first || m_interval.end > m_interval.begin;
        SMILTime tempBegin = findInstanceTime(Begin, beginAfter, equalsMinimumOK);
        if (tempBegin.isUnresolved())
            break;
        SMILTime tempEnd;
        if (m_endTimes.isEmpty())
            tempEnd = resolveActiveEnd(tempBegin, SMILTime::indefinite());
        else {
            tempEnd = findInstanceTime(End, tempBegin, true);
            if ((first && tempBegin == tempEnd && tempEnd == lastIntervalTempEnd) || (!first && tempEnd == m_interval.end))
                tempEnd = findInstanceTime(End, tempBegin, false);
            if (tempEnd.isUnresolved()) {
                if (!m_endTimes.isEmpty() && !m_hasEndEventConditions)
                    break;
            }
            tempEnd = resolveActiveEnd(tempBegin, tempEnd);
        }
        if (!first || (tempEnd > 0 || (!tempBegin.value() && !tempEnd.value())))
            return SMILInterval(tempBegin, tempEnd);

        beginAfter = tempEnd;
        lastIntervalTempEnd = tempEnd;
    }
    return SMILInterval(SMILTime::unresolved(), SMILTime::unresolved());
}

void SVGSMILElement::resolveFirstInterval()
{
    SMILInterval firstInterval = resolveInterval(FirstInterval);
    ASSERT(!firstInterval.begin.isIndefinite());

    if (!firstInterval.begin.isUnresolved() && firstInterval != m_interval) {
        m_interval = firstInterval;
        notifyDependentsIntervalChanged();
        m_nextProgressTime = m_nextProgressTime.isUnresolved() ? m_interval.begin : std::min(m_nextProgressTime, m_interval.begin);

        if (m_timeContainer)
            m_timeContainer->notifyIntervalsChanged();
    }
}

bool SVGSMILElement::resolveNextInterval()
{
    SMILInterval nextInterval = resolveInterval(NextInterval);
    ASSERT(!nextInterval.begin.isIndefinite());

    if (!nextInterval.begin.isUnresolved() && nextInterval.begin != m_interval.begin) {
        m_interval = nextInterval;
        notifyDependentsIntervalChanged();
        m_nextProgressTime = m_nextProgressTime.isUnresolved() ? m_interval.begin : std::min(m_nextProgressTime, m_interval.begin);
        return true;
    }

    return false;
}

SMILTime SVGSMILElement::nextProgressTime() const
{
    return m_nextProgressTime;
}

void SVGSMILElement::beginListChanged(SMILTime eventTime)
{
    if (m_isWaitingForFirstInterval) {
        resolveFirstInterval();
    } else if (this->restart() != RestartNever) {
        SMILTime newBegin = findInstanceTime(Begin, eventTime, true);
        if (newBegin.isFinite() && (m_interval.end <= eventTime || newBegin < m_interval.begin)) {
            // Begin time changed, re-resolve the interval.
            SMILTime oldBegin = m_interval.begin;
            m_interval.end = eventTime;
            m_interval = resolveInterval(NextInterval);
            ASSERT(!m_interval.begin.isUnresolved());
            if (m_interval.begin != oldBegin) {
                if (m_activeState == Active && m_interval.begin > eventTime) {
                    m_activeState = determineActiveState(eventTime);
                    if (m_activeState != Active)
                        endedActiveInterval();
                }
                notifyDependentsIntervalChanged();
            }
        }
    }
    m_nextProgressTime = elapsed();

    if (m_timeContainer)
        m_timeContainer->notifyIntervalsChanged();
}

void SVGSMILElement::endListChanged(SMILTime)
{
    SMILTime elapsed = this->elapsed();
    if (m_isWaitingForFirstInterval) {
        resolveFirstInterval();
    } else if (elapsed < m_interval.end && m_interval.begin.isFinite()) {
        SMILTime newEnd = findInstanceTime(End, m_interval.begin, false);
        if (newEnd < m_interval.end) {
            newEnd = resolveActiveEnd(m_interval.begin, newEnd);
            if (newEnd != m_interval.end) {
                m_interval.end = newEnd;
                notifyDependentsIntervalChanged();
            }
        }
    }
    m_nextProgressTime = elapsed;

    if (m_timeContainer)
        m_timeContainer->notifyIntervalsChanged();
}

SVGSMILElement::RestartedInterval SVGSMILElement::maybeRestartInterval(SMILTime elapsed)
{
    ASSERT(!m_isWaitingForFirstInterval);
    ASSERT(elapsed >= m_interval.begin);

    Restart restart = this->restart();
    if (restart == RestartNever)
        return DidNotRestartInterval;

    if (elapsed < m_interval.end) {
        if (restart != RestartAlways)
            return DidNotRestartInterval;
        SMILTime nextBegin = findInstanceTime(Begin, m_interval.begin, false);
        if (nextBegin < m_interval.end) {
            m_interval.end = nextBegin;
            notifyDependentsIntervalChanged();
        }
    }

    if (elapsed >= m_interval.end) {
        if (resolveNextInterval() && elapsed >= m_interval.begin)
            return DidRestartInterval;
    }
    return DidNotRestartInterval;
}

void SVGSMILElement::seekToIntervalCorrespondingToTime(SMILTime elapsed)
{
    ASSERT(!m_isWaitingForFirstInterval);
    ASSERT(elapsed >= m_interval.begin);

    // Manually seek from interval to interval, just as if the animation would run regulary.
    while (true) {
        // Figure out the next value in the begin time list after the current interval begin.
        SMILTime nextBegin = findInstanceTime(Begin, m_interval.begin, false);

        // If the 'nextBegin' time is unresolved (eg. just one defined interval), we're done seeking.
        if (nextBegin.isUnresolved())
            return;

        // If the 'nextBegin' time is larger than or equal to the current interval end time, we're done seeking.
        // If the 'elapsed' time is smaller than the next begin interval time, we're done seeking.
        if (nextBegin < m_interval.end && elapsed >= nextBegin) {
            // End current interval, and start a new interval from the 'nextBegin' time.
            m_interval.end = nextBegin;
            if (!resolveNextInterval())
                break;
            continue;
        }

        // If the desired 'elapsed' time is past the current interval, advance to the next.
        if (elapsed >= m_interval.end) {
            if (!resolveNextInterval())
                break;
            continue;
        }

        return;
    }
}

float SVGSMILElement::calculateAnimationPercentAndRepeat(SMILTime elapsed, unsigned& repeat) const
{
    SMILTime simpleDuration = this->simpleDuration();
    repeat = 0;
    if (simpleDuration.isIndefinite()) {
        repeat = 0;
        return 0.f;
    }
    if (!simpleDuration) {
        repeat = 0;
        return 1.f;
    }
    ASSERT(m_interval.begin.isFinite());
    ASSERT(simpleDuration.isFinite());
    SMILTime activeTime = elapsed - m_interval.begin;
    SMILTime repeatingDuration = this->repeatingDuration();
    if (elapsed >= m_interval.end || activeTime > repeatingDuration) {
        repeat = static_cast<unsigned>(repeatingDuration.value() / simpleDuration.value());
        if (!fmod(repeatingDuration.value(), simpleDuration.value()))
            repeat--;

        double percent = (m_interval.end.value() - m_interval.begin.value()) / simpleDuration.value();
        percent = percent - floor(percent);
        if (percent < std::numeric_limits<float>::epsilon() || 1 - percent < std::numeric_limits<float>::epsilon())
            return 1.0f;
        return narrowPrecisionToFloat(percent);
    }
    repeat = static_cast<unsigned>(activeTime.value() / simpleDuration.value());
    SMILTime simpleTime = fmod(activeTime.value(), simpleDuration.value());
    return narrowPrecisionToFloat(simpleTime.value() / simpleDuration.value());
}

SMILTime SVGSMILElement::calculateNextProgressTime(SMILTime elapsed) const
{
    if (m_activeState == Active) {
        // If duration is indefinite the value does not actually change over time. Same is true for <set>.
        SMILTime simpleDuration = this->simpleDuration();
        if (simpleDuration.isIndefinite() || isSVGSetElement(*this)) {
            SMILTime repeatingDurationEnd = m_interval.begin + repeatingDuration();
            // We are supposed to do freeze semantics when repeating ends, even if the element is still active.
            // Take care that we get a timer callback at that point.
            if (elapsed < repeatingDurationEnd && repeatingDurationEnd < m_interval.end && repeatingDurationEnd.isFinite())
                return repeatingDurationEnd;
            return m_interval.end;
        }
        return elapsed + 0.025;
    }
    return m_interval.begin >= elapsed ? m_interval.begin : SMILTime::unresolved();
}

SVGSMILElement::ActiveState SVGSMILElement::determineActiveState(SMILTime elapsed) const
{
    if (elapsed >= m_interval.begin && elapsed < m_interval.end)
        return Active;

    return fill() == FillFreeze ? Frozen : Inactive;
}

bool SVGSMILElement::isContributing(SMILTime elapsed) const
{
    // Animation does not contribute during the active time if it is past its repeating duration and has fill=remove.
    return (m_activeState == Active && (fill() == FillFreeze || elapsed <= m_interval.begin + repeatingDuration())) || m_activeState == Frozen;
}

bool SVGSMILElement::progress(SMILTime elapsed, SVGSMILElement* resultElement, bool seekToTime)
{
    ASSERT(resultElement);
    ASSERT(m_timeContainer);
    ASSERT(m_isWaitingForFirstInterval || m_interval.begin.isFinite());

    if (!m_syncBaseConditionsConnected)
        connectSyncBaseConditions();

    if (!m_interval.begin.isFinite()) {
        ASSERT(m_activeState == Inactive);
        m_nextProgressTime = SMILTime::unresolved();
        return false;
    }

    if (elapsed < m_interval.begin) {
        ASSERT(m_activeState != Active);
        bool isFrozen = (m_activeState == Frozen);
        if (isFrozen) {
            if (this == resultElement)
                resetAnimatedType();
            updateAnimation(m_lastPercent, m_lastRepeat, resultElement);
        }
        m_nextProgressTime = m_interval.begin;
        // If the animation is frozen, it's still contributing.
        return isFrozen;
    }

    m_previousIntervalBegin = m_interval.begin;

    if (m_isWaitingForFirstInterval) {
        m_isWaitingForFirstInterval = false;
        resolveFirstInterval();
    }

    // This call may obtain a new interval -- never call calculateAnimationPercentAndRepeat() before!
    if (seekToTime) {
        seekToIntervalCorrespondingToTime(elapsed);
        if (elapsed < m_interval.begin) {
            // elapsed is not within an interval.
            m_nextProgressTime = m_interval.begin;
            return false;
        }
    }

    unsigned repeat = 0;
    float percent = calculateAnimationPercentAndRepeat(elapsed, repeat);
    RestartedInterval restartedInterval = maybeRestartInterval(elapsed);

    ActiveState oldActiveState = m_activeState;
    m_activeState = determineActiveState(elapsed);
    bool animationIsContributing = isContributing(elapsed);

    // Only reset the animated type to the base value once for the lowest priority animation that animates and contributes to a particular element/attribute pair.
    if (this == resultElement && animationIsContributing)
        resetAnimatedType();

    if (animationIsContributing) {
        if (oldActiveState == Inactive || restartedInterval == DidRestartInterval) {
            smilBeginEventSender().dispatchEventSoon(this);
            startedActiveInterval();
        }

        if (repeat && repeat != m_lastRepeat)
            dispatchRepeatEvents(repeat);

        updateAnimation(percent, repeat, resultElement);
        m_lastPercent = percent;
        m_lastRepeat = repeat;
    }

    if ((oldActiveState == Active && m_activeState != Active) || restartedInterval == DidRestartInterval) {
        smilEndEventSender().dispatchEventSoon(this);
        endedActiveInterval();
        if (!animationIsContributing && this == resultElement)
            clearAnimatedType();
    }

    // Triggering all the pending events if the animation timeline is changed.
    if (seekToTime) {
        if (m_activeState == Inactive)
            smilBeginEventSender().dispatchEventSoon(this);

        if (repeat) {
            for (unsigned repeatEventCount = 1; repeatEventCount < repeat; repeatEventCount++)
                dispatchRepeatEvents(repeatEventCount);
            if (m_activeState == Inactive)
                dispatchRepeatEvents(repeat);
        }

        if (m_activeState == Inactive || m_activeState == Frozen)
            smilEndEventSender().dispatchEventSoon(this);
    }

    m_nextProgressTime = calculateNextProgressTime(elapsed);
    return animationIsContributing;
}

void SVGSMILElement::notifyDependentsIntervalChanged()
{
    ASSERT(m_interval.begin.isFinite());
    // |loopBreaker| is used to avoid infinite recursions which may be caused from:
    // |notifyDependentsIntervalChanged| -> |createInstanceTimesFromSyncbase| -> |add{Begin,End}Time| -> |{begin,end}TimeChanged| -> |notifyDependentsIntervalChanged|
    // |loopBreaker| is defined as a Persistent<HeapHashSet<Member<SVGSMILElement>>>. This won't cause leaks because it is guaranteed to be empty after the root |notifyDependentsIntervalChanged| has exited.
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WillBeHeapHashSet<RawPtrWillBeMember<SVGSMILElement>>>, loopBreaker, (adoptPtrWillBeNoop(new WillBeHeapHashSet<RawPtrWillBeMember<SVGSMILElement>>())));
    if (!loopBreaker->add(this).isNewEntry)
        return;

    for (SVGSMILElement* element : m_syncBaseDependents)
        element->createInstanceTimesFromSyncbase(this);

    loopBreaker->remove(this);
}

void SVGSMILElement::createInstanceTimesFromSyncbase(SVGSMILElement* syncBase)
{
    // FIXME: To be really correct, this should handle updating exising interval by changing
    // the associated times instead of creating new ones.
    for (unsigned n = 0; n < m_conditions.size(); ++n) {
        Condition* condition = m_conditions[n].get();
        if (condition->type() == Condition::Syncbase && condition->syncBase() == syncBase) {
            ASSERT(condition->name() == "begin" || condition->name() == "end");
            // No nested time containers in SVG, no need for crazy time space conversions. Phew!
            SMILTime time = 0;
            if (condition->name() == "begin")
                time = syncBase->m_interval.begin + condition->offset();
            else
                time = syncBase->m_interval.end + condition->offset();
            if (!time.isFinite())
                continue;
            SMILTime elapsed = this->elapsed();
            if (elapsed.isUnresolved())
                continue;
            if (condition->beginOrEnd() == Begin)
                addBeginTime(elapsed, time);
            else
                addEndTime(elapsed, time);
        }
    }
}

void SVGSMILElement::addSyncBaseDependent(SVGSMILElement* animation)
{
    m_syncBaseDependents.add(animation);
    if (m_interval.begin.isFinite())
        animation->createInstanceTimesFromSyncbase(this);
}

void SVGSMILElement::removeSyncBaseDependent(SVGSMILElement* animation)
{
    m_syncBaseDependents.remove(animation);
}

void SVGSMILElement::handleConditionEvent(Event* event, Condition* condition)
{
    if (event->type() == "repeatn" && toRepeatEvent(event)->repeat() != condition->repeat())
        return;

    SMILTime elapsed = this->elapsed();
    if (elapsed.isUnresolved())
        return;
    if (condition->beginOrEnd() == Begin)
        addBeginTime(elapsed, elapsed + condition->offset());
    else
        addEndTime(elapsed, elapsed + condition->offset());
}

void SVGSMILElement::beginByLinkActivation()
{
    SMILTime elapsed = this->elapsed();
    if (elapsed.isUnresolved())
        return;
    addBeginTime(elapsed, elapsed);
}

void SVGSMILElement::endedActiveInterval()
{
    clearTimesWithDynamicOrigins(m_beginTimes);
    clearTimesWithDynamicOrigins(m_endTimes);
}

void SVGSMILElement::dispatchRepeatEvents(unsigned count)
{
    m_repeatEventCountList.append(count);
    smilRepeatEventSender().dispatchEventSoon(this);
    smilRepeatNEventSender().dispatchEventSoon(this);
}

void SVGSMILElement::dispatchPendingEvent(SMILEventSender* eventSender)
{
    ASSERT(eventSender == &smilEndEventSender() || eventSender == &smilBeginEventSender() || eventSender == &smilRepeatEventSender() || eventSender == &smilRepeatNEventSender());
    const AtomicString& eventType = eventSender->eventType();
    if (eventType == "repeatn") {
        unsigned repeatEventCount = m_repeatEventCountList.first();
        m_repeatEventCountList.remove(0);
        dispatchEvent(RepeatEvent::create(eventType, repeatEventCount));
    } else {
        dispatchEvent(Event::create(eventType));
    }
}

void SVGSMILElement::schedule()
{
    ASSERT(!m_isScheduled);

    if (!m_timeContainer || !m_targetElement || !hasValidAttributeName() || !hasValidAttributeType() || !m_targetElement->inActiveDocument())
        return;

    m_timeContainer->schedule(this, m_targetElement, m_attributeName);
    m_isScheduled = true;
}

void SVGSMILElement::unscheduleIfScheduled()
{
    if (!m_isScheduled)
        return;

    ASSERT(m_timeContainer);
    ASSERT(m_targetElement);
    m_timeContainer->unschedule(this, m_targetElement, m_attributeName);
    m_isScheduled = false;
}

SVGSMILElement::Condition::~Condition()
{
}

DEFINE_TRACE(SVGSMILElement::Condition)
{
    visitor->trace(m_syncBase);
}

DEFINE_TRACE(SVGSMILElement)
{
#if ENABLE(OILPAN)
    visitor->trace(m_targetElement);
    visitor->trace(m_timeContainer);
    visitor->trace(m_conditions);
    visitor->trace(m_syncBaseDependents);
#endif
    SVGElement::trace(visitor);
    SVGTests::trace(visitor);
}

} // namespace blink
