/*
 * Copyright (C) 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/xml/XPathFunctions.h"

#include "core/XMLNames.h"
#include "core/dom/Attr.h"
#include "core/dom/Element.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/TreeScope.h"
#include "core/xml/XPathUtil.h"
#include "core/xml/XPathValue.h"
#include "wtf/MathExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {
namespace XPath {

static inline bool isWhitespace(UChar c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}


#define DEFINE_FUNCTION_CREATOR(Class) static Function* create##Class() { return new Class; }

class Interval {
public:
    static const int Inf = -1;

    Interval();
    Interval(int value);
    Interval(int min, int max);

    bool contains(int value) const;

private:
    int m_min;
    int m_max;
};

struct FunctionRec {
    typedef Function *(*FactoryFn)();
    FactoryFn factoryFn;
    Interval args;
};

static HashMap<String, FunctionRec>* functionMap;

class FunLast final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
public:
    FunLast() { setIsContextSizeSensitive(true); }
};

class FunPosition final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
public:
    FunPosition() { setIsContextPositionSensitive(true); }
};

class FunCount final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
};

class FunId final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NodeSetValue; }
};

class FunLocalName final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
public:
    FunLocalName() { setIsContextNodeSensitive(true); } // local-name() with no arguments uses context node.
};

class FunNamespaceURI final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
public:
    FunNamespaceURI() { setIsContextNodeSensitive(true); } // namespace-uri() with no arguments uses context node.
};

class FunName final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
public:
    FunName() { setIsContextNodeSensitive(true); } // name() with no arguments uses context node.
};

class FunString final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
public:
    FunString() { setIsContextNodeSensitive(true); } // string() with no arguments uses context node.
};

class FunConcat final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
};

class FunStartsWith final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunContains final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunSubstringBefore final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
};

class FunSubstringAfter final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
};

class FunSubstring final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
};

class FunStringLength final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
public:
    FunStringLength() { setIsContextNodeSensitive(true); } // string-length() with no arguments uses context node.
};

class FunNormalizeSpace final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
public:
    FunNormalizeSpace() { setIsContextNodeSensitive(true); } // normalize-space() with no arguments uses context node.
};

class FunTranslate final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::StringValue; }
};

class FunBoolean final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunNot final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunTrue final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunFalse final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
};

class FunLang final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::BooleanValue; }
public:
    FunLang() { setIsContextNodeSensitive(true); } // lang() always works on context node.
};

class FunNumber final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
public:
    FunNumber() { setIsContextNodeSensitive(true); } // number() with no arguments uses context node.
};

class FunSum final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
};

class FunFloor final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
};

class FunCeiling final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
};

class FunRound final : public Function {
    Value evaluate(EvaluationContext&) const override;
    Value::Type resultType() const override { return Value::NumberValue; }
public:
    static double round(double);
};

DEFINE_FUNCTION_CREATOR(FunLast)
DEFINE_FUNCTION_CREATOR(FunPosition)
DEFINE_FUNCTION_CREATOR(FunCount)
DEFINE_FUNCTION_CREATOR(FunId)
DEFINE_FUNCTION_CREATOR(FunLocalName)
DEFINE_FUNCTION_CREATOR(FunNamespaceURI)
DEFINE_FUNCTION_CREATOR(FunName)

DEFINE_FUNCTION_CREATOR(FunString)
DEFINE_FUNCTION_CREATOR(FunConcat)
DEFINE_FUNCTION_CREATOR(FunStartsWith)
DEFINE_FUNCTION_CREATOR(FunContains)
DEFINE_FUNCTION_CREATOR(FunSubstringBefore)
DEFINE_FUNCTION_CREATOR(FunSubstringAfter)
DEFINE_FUNCTION_CREATOR(FunSubstring)
DEFINE_FUNCTION_CREATOR(FunStringLength)
DEFINE_FUNCTION_CREATOR(FunNormalizeSpace)
DEFINE_FUNCTION_CREATOR(FunTranslate)

DEFINE_FUNCTION_CREATOR(FunBoolean)
DEFINE_FUNCTION_CREATOR(FunNot)
DEFINE_FUNCTION_CREATOR(FunTrue)
DEFINE_FUNCTION_CREATOR(FunFalse)
DEFINE_FUNCTION_CREATOR(FunLang)

DEFINE_FUNCTION_CREATOR(FunNumber)
DEFINE_FUNCTION_CREATOR(FunSum)
DEFINE_FUNCTION_CREATOR(FunFloor)
DEFINE_FUNCTION_CREATOR(FunCeiling)
DEFINE_FUNCTION_CREATOR(FunRound)

#undef DEFINE_FUNCTION_CREATOR

inline Interval::Interval()
    : m_min(Inf), m_max(Inf)
{
}

inline Interval::Interval(int value)
    : m_min(value), m_max(value)
{
}

inline Interval::Interval(int min, int max)
    : m_min(min), m_max(max)
{
}

inline bool Interval::contains(int value) const
{
    if (m_min == Inf && m_max == Inf)
        return true;

    if (m_min == Inf)
        return value <= m_max;

    if (m_max == Inf)
        return value >= m_min;

    return value >= m_min && value <= m_max;
}

void Function::setArguments(HeapVector<Member<Expression>>& args)
{
    ASSERT(!subExprCount());

    // Some functions use context node as implicit argument, so when explicit arguments are added, they may no longer be context node sensitive.
    if (m_name != "lang" && !args.isEmpty())
        setIsContextNodeSensitive(false);

    for (Expression* arg : args)
        addSubExpression(arg);
}

Value FunLast::evaluate(EvaluationContext& context) const
{
    return context.size;
}

Value FunPosition::evaluate(EvaluationContext& context) const
{
    return context.position;
}

Value FunId::evaluate(EvaluationContext& context) const
{
    Value a = arg(0)->evaluate(context);
    StringBuilder idList; // A whitespace-separated list of IDs

    if (a.isNodeSet()) {
        const NodeSet& nodes = a.toNodeSet(&context);
        for (size_t i = 0; i < nodes.size(); ++i) {
            String str = stringValue(nodes[i]);
            idList.append(str);
            idList.append(' ');
        }
    } else {
        String str = a.toString();
        idList.append(str);
    }

    TreeScope& contextScope = context.node->treeScope();
    NodeSet* result(NodeSet::create());
    WillBeHeapHashSet<RawPtrWillBeMember<Node>> resultSet;

    unsigned startPos = 0;
    unsigned length = idList.length();
    while (true) {
        while (startPos < length && isWhitespace(idList[startPos]))
            ++startPos;

        if (startPos == length)
            break;

        size_t endPos = startPos;
        while (endPos < length && !isWhitespace(idList[endPos]))
            ++endPos;

        // If there are several nodes with the same id, id() should return the first one.
        // In WebKit, getElementById behaves so, too, although its behavior in this case is formally undefined.
        Node* node = contextScope.getElementById(AtomicString(idList.substring(startPos, endPos - startPos)));
        if (node && resultSet.add(node).isNewEntry)
            result->append(node);

        startPos = endPos;
    }

    result->markSorted(false);

    return Value(result, Value::adopt);
}

static inline String expandedNameLocalPart(Node* node)
{
    // The local part of an XPath expanded-name matches DOM local name for most node types, except for namespace nodes and processing instruction nodes.
    // But note that Blink does not support namespace nodes.
    if (node->nodeType() == Node::PROCESSING_INSTRUCTION_NODE)
        return toProcessingInstruction(node)->target();
    return node->localName().string();
}

static inline String expandedName(Node* node)
{
    AtomicString prefix;

    switch (node->nodeType()) {
    case Node::ELEMENT_NODE:
        prefix = toElement(node)->prefix();
        break;
    case Node::ATTRIBUTE_NODE:
        prefix = toAttr(node)->prefix();
        break;
    default:
        break;
    }

    return prefix.isEmpty() ? expandedNameLocalPart(node) : prefix + ":" + expandedNameLocalPart(node);
}

Value FunLocalName::evaluate(EvaluationContext& context) const
{
    if (argCount() > 0) {
        Value a = arg(0)->evaluate(context);
        if (!a.isNodeSet())
            return "";

        Node* node = a.toNodeSet(&context).firstNode();
        return node ? expandedNameLocalPart(node) : "";
    }

    return expandedNameLocalPart(context.node.get());
}

Value FunNamespaceURI::evaluate(EvaluationContext& context) const
{
    if (argCount() > 0) {
        Value a = arg(0)->evaluate(context);
        if (!a.isNodeSet())
            return "";

        Node* node = a.toNodeSet(&context).firstNode();
        return node ? node->namespaceURI().string() : "";
    }

    return context.node->namespaceURI().string();
}

Value FunName::evaluate(EvaluationContext& context) const
{
    if (argCount() > 0) {
        Value a = arg(0)->evaluate(context);
        if (!a.isNodeSet())
            return "";

        Node* node = a.toNodeSet(&context).firstNode();
        return node ? expandedName(node) : "";
    }

    return expandedName(context.node.get());
}

Value FunCount::evaluate(EvaluationContext& context) const
{
    Value a = arg(0)->evaluate(context);

    return double(a.toNodeSet(&context).size());
}

Value FunString::evaluate(EvaluationContext& context) const
{
    if (!argCount())
        return Value(context.node.get()).toString();
    return arg(0)->evaluate(context).toString();
}

Value FunConcat::evaluate(EvaluationContext& context) const
{
    StringBuilder result;
    result.reserveCapacity(1024);

    unsigned count = argCount();
    for (unsigned i = 0; i < count; ++i) {
        String str(arg(i)->evaluate(context).toString());
        result.append(str);
    }

    return result.toString();
}

Value FunStartsWith::evaluate(EvaluationContext& context) const
{
    String s1 = arg(0)->evaluate(context).toString();
    String s2 = arg(1)->evaluate(context).toString();

    if (s2.isEmpty())
        return true;

    return s1.startsWith(s2);
}

Value FunContains::evaluate(EvaluationContext& context) const
{
    String s1 = arg(0)->evaluate(context).toString();
    String s2 = arg(1)->evaluate(context).toString();

    if (s2.isEmpty())
        return true;

    return s1.contains(s2) != 0;
}

Value FunSubstringBefore::evaluate(EvaluationContext& context) const
{
    String s1 = arg(0)->evaluate(context).toString();
    String s2 = arg(1)->evaluate(context).toString();

    if (s2.isEmpty())
        return "";

    size_t i = s1.find(s2);

    if (i == kNotFound)
        return "";

    return s1.left(i);
}

Value FunSubstringAfter::evaluate(EvaluationContext& context) const
{
    String s1 = arg(0)->evaluate(context).toString();
    String s2 = arg(1)->evaluate(context).toString();

    size_t i = s1.find(s2);
    if (i == kNotFound)
        return "";

    return s1.substring(i + s2.length());
}

Value FunSubstring::evaluate(EvaluationContext& context) const
{
    String s = arg(0)->evaluate(context).toString();
    double doublePos = arg(1)->evaluate(context).toNumber();
    if (std::isnan(doublePos))
        return "";
    long pos = static_cast<long>(FunRound::round(doublePos));
    bool haveLength = argCount() == 3;
    long len = -1;
    if (haveLength) {
        double doubleLen = arg(2)->evaluate(context).toNumber();
        if (std::isnan(doubleLen))
            return "";
        len = static_cast<long>(FunRound::round(doubleLen));
    }

    if (pos > long(s.length()))
        return "";

    if (pos < 1) {
        if (haveLength) {
            len -= 1 - pos;
            if (len < 1)
                return "";
        }
        pos = 1;
    }

    return s.substring(pos - 1, len);
}

Value FunStringLength::evaluate(EvaluationContext& context) const
{
    if (!argCount())
        return Value(context.node.get()).toString().length();
    return arg(0)->evaluate(context).toString().length();
}

Value FunNormalizeSpace::evaluate(EvaluationContext& context) const
{
    if (!argCount()) {
        String s = Value(context.node.get()).toString();
        return s.simplifyWhiteSpace();
    }

    String s = arg(0)->evaluate(context).toString();
    return s.simplifyWhiteSpace();
}

Value FunTranslate::evaluate(EvaluationContext& context) const
{
    String s1 = arg(0)->evaluate(context).toString();
    String s2 = arg(1)->evaluate(context).toString();
    String s3 = arg(2)->evaluate(context).toString();
    StringBuilder result;

    for (unsigned i1 = 0; i1 < s1.length(); ++i1) {
        UChar ch = s1[i1];
        size_t i2 = s2.find(ch);

        if (i2 == kNotFound)
            result.append(ch);
        else if (i2 < s3.length())
            result.append(s3[i2]);
    }

    return result.toString();
}

Value FunBoolean::evaluate(EvaluationContext& context) const
{
    return arg(0)->evaluate(context).toBoolean();
}

Value FunNot::evaluate(EvaluationContext& context) const
{
    return !arg(0)->evaluate(context).toBoolean();
}

Value FunTrue::evaluate(EvaluationContext&) const
{
    return true;
}

Value FunLang::evaluate(EvaluationContext& context) const
{
    String lang = arg(0)->evaluate(context).toString();

    const Attribute* languageAttribute = nullptr;
    Node* node = context.node.get();
    while (node) {
        if (node->isElementNode()) {
            Element* element = toElement(node);
            languageAttribute = element->attributes().find(XMLNames::langAttr);
        }
        if (languageAttribute)
            break;
        node = node->parentNode();
    }

    if (!languageAttribute)
        return false;

    String langValue = languageAttribute->value();
    while (true) {
        if (equalIgnoringCase(langValue, lang))
            return true;

        // Remove suffixes one by one.
        size_t index = langValue.reverseFind('-');
        if (index == kNotFound)
            break;
        langValue = langValue.left(index);
    }

    return false;
}

Value FunFalse::evaluate(EvaluationContext&) const
{
    return false;
}

Value FunNumber::evaluate(EvaluationContext& context) const
{
    if (!argCount())
        return Value(context.node.get()).toNumber();
    return arg(0)->evaluate(context).toNumber();
}

Value FunSum::evaluate(EvaluationContext& context) const
{
    Value a = arg(0)->evaluate(context);
    if (!a.isNodeSet())
        return 0.0;

    double sum = 0.0;
    const NodeSet& nodes = a.toNodeSet(&context);
    // To be really compliant, we should sort the node-set, as floating point addition is not associative.
    // However, this is unlikely to ever become a practical issue, and sorting is slow.

    for (unsigned i = 0; i < nodes.size(); i++)
        sum += Value(stringValue(nodes[i])).toNumber();

    return sum;
}

Value FunFloor::evaluate(EvaluationContext& context) const
{
    return floor(arg(0)->evaluate(context).toNumber());
}

Value FunCeiling::evaluate(EvaluationContext& context) const
{
    return ceil(arg(0)->evaluate(context).toNumber());
}

double FunRound::round(double val)
{
    if (!std::isnan(val) && !std::isinf(val)) {
        if (std::signbit(val) && val >= -0.5)
            val *= 0; // negative zero
        else
            val = floor(val + 0.5);
    }
    return val;
}

Value FunRound::evaluate(EvaluationContext& context) const
{
    return round(arg(0)->evaluate(context).toNumber());
}

struct FunctionMapping {
    const char* name;
    FunctionRec function;
};

static void createFunctionMap()
{
    static const FunctionMapping functions[] = {
        { "boolean", { &createFunBoolean, 1 } },
        { "ceiling", { &createFunCeiling, 1 } },
        { "concat", { &createFunConcat, Interval(2, Interval::Inf) } },
        { "contains", { &createFunContains, 2 } },
        { "count", { &createFunCount, 1 } },
        { "false", { &createFunFalse, 0 } },
        { "floor", { &createFunFloor, 1 } },
        { "id", { &createFunId, 1 } },
        { "lang", { &createFunLang, 1 } },
        { "last", { &createFunLast, 0 } },
        { "local-name", { &createFunLocalName, Interval(0, 1) } },
        { "name", { &createFunName, Interval(0, 1) } },
        { "namespace-uri", { &createFunNamespaceURI, Interval(0, 1) } },
        { "normalize-space", { &createFunNormalizeSpace, Interval(0, 1) } },
        { "not", { &createFunNot, 1 } },
        { "number", { &createFunNumber, Interval(0, 1) } },
        { "position", { &createFunPosition, 0 } },
        { "round", { &createFunRound, 1 } },
        { "starts-with", { &createFunStartsWith, 2 } },
        { "string", { &createFunString, Interval(0, 1) } },
        { "string-length", { &createFunStringLength, Interval(0, 1) } },
        { "substring", { &createFunSubstring, Interval(2, 3) } },
        { "substring-after", { &createFunSubstringAfter, 2 } },
        { "substring-before", { &createFunSubstringBefore, 2 } },
        { "sum", { &createFunSum, 1 } },
        { "translate", { &createFunTranslate, 3 } },
        { "true", { &createFunTrue, 0 } },
    };

    functionMap = new HashMap<String, FunctionRec>;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(functions); ++i)
        functionMap->set(functions[i].name, functions[i].function);
}


Function* createFunction(const String& name)
{
    HeapVector<Member<Expression>> args;
    return createFunction(name, args);
}

Function* createFunction(const String& name, HeapVector<Member<Expression>>& args)
{
    if (!functionMap)
        createFunctionMap();

    HashMap<String, FunctionRec>::iterator functionMapIter = functionMap->find(name);
    FunctionRec* functionRec = nullptr;

    if (functionMapIter == functionMap->end() || !(functionRec = &functionMapIter->value)->args.contains(args.size()))
        return nullptr;

    Function* function = functionRec->factoryFn();
    function->setArguments(args);
    function->setName(name);
    return function;
}

}
}
