
#include "config.h"
#include "CustomElementProcessingStack.h"

namespace blink {

	size_t CustomElementProcessingStack::s_elementQueueStart = 0;

	// The base of the stack has a null sentinel value.
	size_t CustomElementProcessingStack::s_elementQueueEnd = kNumSentinels;

// Dispatches callbacks when popping the processing stack.
void CustomElementProcessingStack::processElementQueueAndPop()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();
}

void CustomElementProcessingStack::processElementQueueAndPop(size_t start, size_t end)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();
}

} // namespace blink