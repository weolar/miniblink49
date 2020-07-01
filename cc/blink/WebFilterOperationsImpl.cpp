
#include "cc/blink/WebFilterOperationsImpl.h"

namespace cc_blink {

WebFilterOperationsImpl::WebFilterOperationsImpl()
{

}

WebFilterOperationsImpl::WebFilterOperationsImpl(const blink::WebFilterOperations& other)
{
    const WebFilterOperationsImpl* otherPtr = (const WebFilterOperationsImpl*)&other;
    m_operations = otherPtr->m_operations;
}

WebFilterOperationsImpl::WebFilterOperationsImpl(const WebFilterOperationsImpl& other)
{
    m_operations = other.m_operations;
}

WebFilterOperationsImpl::~WebFilterOperationsImpl()
{
//     for (size_t i = 0; i < m_operations.size(); ++i) {
//         delete m_operations[i];
//     }
}

void WebFilterOperationsImpl::appendGrayscaleFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::GRAYSCALE).leakRef());
}

void WebFilterOperationsImpl::appendSepiaFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::SEPIA).leakRef());
}

void WebFilterOperationsImpl::appendSaturateFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::SATURATE).leakRef());
}

void WebFilterOperationsImpl::appendHueRotateFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::HUE_ROTATE).leakRef());
}

void WebFilterOperationsImpl::appendInvertFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::INVERT).leakRef());
}

void WebFilterOperationsImpl::appendBrightnessFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::BRIGHTNESS).leakRef());
}

void WebFilterOperationsImpl::appendContrastFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::CONTRAST).leakRef());
}

void WebFilterOperationsImpl::appendOpacityFilter(float amount)
{
    m_operations.append(blink::BasicColorMatrixFilterOperation::create(amount, blink::FilterOperation::OPACITY).leakRef());
}

void WebFilterOperationsImpl::appendBlurFilter(float amount)
{
    m_operations.append(blink::BlurFilterOperation::create(blink::Length(amount, blink::LengthType::Fixed)).leakRef());
}

void WebFilterOperationsImpl::appendDropShadowFilter(blink::WebPoint offset, float stdDeviation, blink::WebColor color)
{
    m_operations.append(blink::DropShadowFilterOperation::create(offset, (int)stdDeviation, color).leakRef());
}

void WebFilterOperationsImpl::appendColorMatrixFilter(SkScalar matrix[20])
{
    //m_operations.append(blink::BasicColorMatrixFilterOperation::create());
}

void WebFilterOperationsImpl::appendZoomFilter(float amount, int inset)
{
    //m_operations.append(blink::FilterOperation::GRAYSCALE);
}

void WebFilterOperationsImpl::appendSaturatingBrightnessFilter(float amount)
{
    //m_operations.append(blink::FilterOperation::GRAYSCALE);
}

// This grabs a ref on the passed-in filter.
void WebFilterOperationsImpl::appendReferenceFilter(SkImageFilter*)
{
    //m_operations.append(blink::FilterOperation::GRAYSCALE);
}

void WebFilterOperationsImpl::clear()
{
    m_operations.clear();
}

bool WebFilterOperationsImpl::isEmpty() const
{
    return m_operations.isEmpty();
}

const blink::FilterOperation& WebFilterOperationsImpl::at(size_t i) const
{
    return *(m_operations[i]);
}

size_t WebFilterOperationsImpl::getSize() const
{
    return m_operations.size();
}

} // cc_blink
