#include "config.h"
#include "core/xml/parser/XMLDocumentParser.h"


#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8Document.h"
#include "core/HTMLNames.h"
#include "core/XMLNSNames.h"
#include "core/dom/CDATASection.h"
#include "core/dom/Comment.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/ScriptLoader.h"
#include "core/dom/TransformSource.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fetch/RawResource.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ScriptResource.h"
#include "core/frame/ConsoleTypes.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/parser/HTMLEntityParser.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/ImageLoader.h"
#include "core/svg/graphics/SVGImage.h"
#include "core/xml/DocumentXSLT.h"
#include "core/xml/parser/SharedBufferReader.h"
#include "core/xml/parser/XMLDocumentParserScope.h"
#include "core/xml/parser/XMLParserInput.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/SharedBuffer.h"
#include "platform/TraceEvent.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/StringExtras.h"
#include "wtf/TemporaryChange.h"
#include "wtf/Threading.h"
#include "wtf/Vector.h"
#include "wtf/text/UTF8.h"

namespace blink {

bool XMLDocumentParser::supportsXMLVersion(const String& version)
{
    return version == "1.0";
}

XMLDocumentParser::XMLDocumentParser(Document& document, FrameView* frameView)
    : ScriptableDocumentParser(document)
    , m_hasView(frameView)
    , m_context(nullptr)
    , m_currentNode(&document)
    , m_isCurrentlyParsing8BitChunk(false)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(&document)
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition::belowRangePosition())
    , m_parsingFragment(false)
{
    notImplemented();
}

XMLDocumentParser::XMLDocumentParser(DocumentFragment* fragment, Element* parentElement, ParserContentPolicy parserContentPolicy)
    : ScriptableDocumentParser(fragment->document(), parserContentPolicy)
    , m_hasView(false)
    , m_context(nullptr)
    , m_currentNode(fragment)
    , m_isCurrentlyParsing8BitChunk(false)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(&fragment->document())
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition::belowRangePosition())
    , m_parsingFragment(true)
{
    notImplemented();
}

XMLParserContext::~XMLParserContext()
{

}

void XMLDocumentParser::trace(Visitor *){ notImplemented(); }
void XMLDocumentParser::trace(InlinedGlobalMarkingVisitor) { notImplemented(); }

XMLDocumentParser::~XMLDocumentParser() {}

TextPosition XMLDocumentParser::textPosition() const { notImplemented(); return TextPosition(); }
void XMLDocumentParser::insert(SegmentedString const &) { notImplemented(); }
void XMLDocumentParser::append(class WTF::String const &) { notImplemented(); }
void XMLDocumentParser::finish(void) { notImplemented(); }
bool XMLDocumentParser::isWaitingForScripts(void) const { notImplemented(); return false; }
void XMLDocumentParser::stopParsing(void) { notImplemented(); }
void XMLDocumentParser::detach(void) { notImplemented(); }
OrdinalNumber XMLDocumentParser::lineNumber(void) const { notImplemented(); return OrdinalNumber(); }
void XMLDocumentParser::notifyFinished(class blink::Resource *) { notImplemented(); }

bool XMLDocumentParser::parseDocumentFragment(const String& chunk, DocumentFragment* fragment, Element* contextElement, ParserContentPolicy parserContentPolicy)
{
    notImplemented();
    return false;
}

HashMap<String, String> parseAttributes(const String& string, bool& attrsOK)
{
    notImplemented();
    return HashMap < String, String >();
}

} // namespace blink