#include "config.h"
#include "core/dom/custom/CustomElementUpgradeCandidateMap.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "bindings/core/v8/CustomElementConstructorBuilder.h"
#include "core/dom/custom/CustomElementMicrotaskRunQueue.h"
#include "core/dom/custom/CustomElementSyncMicrotaskQueue.h"
#include "core/dom/custom/CustomElementAsyncImportMicrotaskQueue.h"

#include "core/dom/Element.h"

namespace blink {

bool CustomElement::isValidName(const AtomicString& name, NameSet validNames)
{
    return false;
}

CustomElementUpgradeCandidateMap::~CustomElementUpgradeCandidateMap()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();
}

void CustomElementRegistrationContext::registerElement(Document*, CustomElementConstructorBuilder*, const AtomicString& type, CustomElement::NameSet validNames, ExceptionState&)
{
	notImplemented();
}

void CustomElementRegistrationContext::setIsAttributeAndTypeExtension(Element*, const AtomicString& type)
{
	notImplemented();
}

void CustomElementRegistrationContext::setTypeExtension(Element*, const AtomicString& type)
{
    notImplemented();
}

PassRefPtrWillBeRawPtr<Element> CustomElementRegistrationContext::createCustomTagElement(Document&, const QualifiedName&)
{
    notImplemented();
    return nullptr;
}

CustomElementRegistrationContext::CustomElementRegistrationContext()
    : m_candidates(nullptr)
{
}

CustomElementConstructorBuilder::CustomElementConstructorBuilder(ScriptState*, const ElementRegistrationOptions& options)
    : m_options(options)
{
	notImplemented();
}

ScriptValue CustomElementConstructorBuilder::bindingsReturnValue() const
{
	notImplemented();
    return ScriptValue();
}

CustomElementMicrotaskRunQueue::CustomElementMicrotaskRunQueue()
    : m_weakFactory(this)
    , m_syncQueue(nullptr)
    , m_asyncQueue(nullptr)
    , m_dispatchIsPending(false)
{
	notImplemented();
}

CustomElementMicrotaskRunQueue::~CustomElementMicrotaskRunQueue()
{
	notImplemented();
}

void CustomElementUpgradeCandidateMap::trace(Visitor *)
{
    notImplemented();
}

void CustomElementUpgradeCandidateMap::trace(InlinedGlobalMarkingVisitor)
{
    notImplemented();
}

void CustomElementUpgradeCandidateMap::elementWasDestroyed(Element *)
{
    notImplemented();
}

} // namespace blink