/*
 * Copyright 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
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
#include "core/xml/XPathPredicate.h"

#include "core/xml/XPathFunctions.h"
#include "core/xml/XPathUtil.h"
#include "wtf/MathExtras.h"
#include <math.h>

namespace blink {

namespace XPath {

Number::Number(double value)
    : m_value(value)
{
}

DEFINE_TRACE(Number)
{
    visitor->trace(m_value);
    Expression::trace(visitor);
}

Value Number::evaluate(EvaluationContext&) const
{
    return m_value;
}

StringExpression::StringExpression(const String& value)
    : m_value(value)
{
}

DEFINE_TRACE(StringExpression)
{
    visitor->trace(m_value);
    Expression::trace(visitor);
}

Value StringExpression::evaluate(EvaluationContext&) const
{
    return m_value;
}

Value Negative::evaluate(EvaluationContext& context) const
{
    Value p(subExpr(0)->evaluate(context));
    return -p.toNumber();
}

NumericOp::NumericOp(Opcode opcode, Expression* lhs, Expression* rhs)
    : m_opcode(opcode)
{
    addSubExpression(lhs);
    addSubExpression(rhs);
}

Value NumericOp::evaluate(EvaluationContext& context) const
{
    Value lhs(subExpr(0)->evaluate(context));
    Value rhs(subExpr(1)->evaluate(context));

    double leftVal = lhs.toNumber();
    double rightVal = rhs.toNumber();

    switch (m_opcode) {
    case OP_Add:
        return leftVal + rightVal;
    case OP_Sub:
        return leftVal - rightVal;
    case OP_Mul:
        return leftVal * rightVal;
    case OP_Div:
        return leftVal / rightVal;
    case OP_Mod:
        return fmod(leftVal, rightVal);
    }
    ASSERT_NOT_REACHED();
    return 0.0;
}

EqTestOp::EqTestOp(Opcode opcode, Expression* lhs, Expression* rhs)
    : m_opcode(opcode)
{
    addSubExpression(lhs);
    addSubExpression(rhs);
}

bool EqTestOp::compare(EvaluationContext& context, const Value& lhs, const Value& rhs) const
{
    if (lhs.isNodeSet()) {
        const NodeSet& lhsSet = lhs.toNodeSet(&context);
        if (rhs.isNodeSet()) {
            // If both objects to be compared are node-sets, then the comparison
            // will be true if and only if there is a node in the first node-set
            // and a node in the second node-set such that the result of
            // performing the comparison on the string-values of the two nodes
            // is true.
            const NodeSet& rhsSet = rhs.toNodeSet(&context);
            for (unsigned lindex = 0; lindex < lhsSet.size(); ++lindex) {
                for (unsigned rindex = 0; rindex < rhsSet.size(); ++rindex) {
                    if (compare(context, stringValue(lhsSet[lindex]), stringValue(rhsSet[rindex])))
                        return true;
                }
            }
            return false;
        }
        if (rhs.isNumber()) {
            // If one object to be compared is a node-set and the other is a
            // number, then the comparison will be true if and only if there is
            // a node in the node-set such that the result of performing the
            // comparison on the number to be compared and on the result of
            // converting the string-value of that node to a number using the
            // number function is true.
            for (unsigned lindex = 0; lindex < lhsSet.size(); ++lindex) {
                if (compare(context, Value(stringValue(lhsSet[lindex])).toNumber(), rhs))
                    return true;
            }
            return false;
        }
        if (rhs.isString()) {
            // If one object to be compared is a node-set and the other is a
            // string, then the comparison will be true if and only if there is
            // a node in the node-set such that the result of performing the
            // comparison on the string-value of the node and the other string
            // is true.
            for (unsigned lindex = 0; lindex < lhsSet.size(); ++lindex) {
                if (compare(context, stringValue(lhsSet[lindex]), rhs))
                    return true;
            }
            return false;
        }
        if (rhs.isBoolean()) {
            // If one object to be compared is a node-set and the other is a
            // boolean, then the comparison will be true if and only if the
            // result of performing the comparison on the boolean and on the
            // result of converting the node-set to a boolean using the boolean
            // function is true.
            return compare(context, lhs.toBoolean(), rhs);
        }
        ASSERT(0);
    }
    if (rhs.isNodeSet()) {
        const NodeSet& rhsSet = rhs.toNodeSet(&context);
        if (lhs.isNumber()) {
            for (unsigned rindex = 0; rindex < rhsSet.size(); ++rindex) {
                if (compare(context, lhs, Value(stringValue(rhsSet[rindex])).toNumber()))
                    return true;
            }
            return false;
        }
        if (lhs.isString()) {
            for (unsigned rindex = 0; rindex < rhsSet.size(); ++rindex) {
                if (compare(context, lhs, stringValue(rhsSet[rindex])))
                    return true;
            }
            return false;
        }
        if (lhs.isBoolean())
            return compare(context, lhs, rhs.toBoolean());
        ASSERT(0);
    }

    // Neither side is a NodeSet.
    switch (m_opcode) {
    case OpcodeEqual:
    case OpcodeNotEqual:
        bool equal;
        if (lhs.isBoolean() || rhs.isBoolean())
            equal = lhs.toBoolean() == rhs.toBoolean();
        else if (lhs.isNumber() || rhs.isNumber())
            equal = lhs.toNumber() == rhs.toNumber();
        else
            equal = lhs.toString() == rhs.toString();

        if (m_opcode == OpcodeEqual)
            return equal;
        return !equal;
    case OpcodeGreaterThan:
        return lhs.toNumber() > rhs.toNumber();
    case OpcodeGreaterOrEqual:
        return lhs.toNumber() >= rhs.toNumber();
    case OpcodeLessThan:
        return lhs.toNumber() < rhs.toNumber();
    case OpcodeLessOrEqual:
        return lhs.toNumber() <= rhs.toNumber();
    }
    ASSERT(0);
    return false;
}

Value EqTestOp::evaluate(EvaluationContext& context) const
{
    Value lhs(subExpr(0)->evaluate(context));
    Value rhs(subExpr(1)->evaluate(context));

    return compare(context, lhs, rhs);
}

LogicalOp::LogicalOp(Opcode opcode, Expression* lhs, Expression* rhs)
    : m_opcode(opcode)
{
    addSubExpression(lhs);
    addSubExpression(rhs);
}

bool LogicalOp::shortCircuitOn() const
{
    return m_opcode != OP_And;
}

Value LogicalOp::evaluate(EvaluationContext& context) const
{
    Value lhs(subExpr(0)->evaluate(context));

    // This is not only an optimization, http://www.w3.org/TR/xpath
    // dictates that we must do short-circuit evaluation
    bool lhsBool = lhs.toBoolean();
    if (lhsBool == shortCircuitOn())
        return lhsBool;

    return subExpr(1)->evaluate(context).toBoolean();
}

Value Union::evaluate(EvaluationContext& context) const
{
    Value lhsResult = subExpr(0)->evaluate(context);
    Value rhs = subExpr(1)->evaluate(context);

    NodeSet& resultSet = lhsResult.modifiableNodeSet(context);
    const NodeSet& rhsNodes = rhs.toNodeSet(&context);

    WillBeHeapHashSet<RawPtrWillBeMember<Node>> nodes;
    for (size_t i = 0; i < resultSet.size(); ++i)
        nodes.add(resultSet[i]);

    for (size_t i = 0; i < rhsNodes.size(); ++i) {
        Node* node = rhsNodes[i];
        if (nodes.add(node).isNewEntry)
            resultSet.append(node);
    }

    // It is also possible to use merge sort to avoid making the result
    // unsorted; but this would waste the time in cases when order is not
    // important.
    resultSet.markSorted(false);
    return lhsResult;
}

Predicate::Predicate(Expression* expr)
    : m_expr(expr)
{
}

DEFINE_TRACE(Predicate)
{
    visitor->trace(m_expr);
}

bool Predicate::evaluate(EvaluationContext& context) const
{
    ASSERT(m_expr);

    Value result(m_expr->evaluate(context));

    // foo[3] means foo[position()=3]
    if (result.isNumber())
        return EqTestOp(EqTestOp::OpcodeEqual, createFunction("position"), new Number(result.toNumber())).evaluate(context).toBoolean();

    return result.toBoolean();
}

} // namespace XPath

} // namespace blink
