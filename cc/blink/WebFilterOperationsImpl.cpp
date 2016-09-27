
#include "cc/blink/WebFilterOperationsImpl.h"

namespace cc_blink {

    WebFilterOperationsImpl::WebFilterOperationsImpl()
    {

    }
    WebFilterOperationsImpl::~WebFilterOperationsImpl()
    {

    }

    void WebFilterOperationsImpl::appendGrayscaleFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::GRAYSCALE);
    }

    void WebFilterOperationsImpl::appendSepiaFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::SEPIA);
    }

    void WebFilterOperationsImpl::appendSaturateFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::SATURATE);
    }

    void WebFilterOperationsImpl::appendHueRotateFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::HUE_ROTATE);
    }

    void WebFilterOperationsImpl::appendInvertFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::INVERT);
    }

    void WebFilterOperationsImpl::appendBrightnessFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::BRIGHTNESS);
    }

    void WebFilterOperationsImpl::appendContrastFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::CONTRAST);
    }

    void WebFilterOperationsImpl::appendOpacityFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::OPACITY);
    }

    void WebFilterOperationsImpl::appendBlurFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::BLUR);
    }

    void WebFilterOperationsImpl::appendDropShadowFilter(blink::WebPoint offset, float stdDeviation, blink::WebColor)
    {
        m_operationTypes.append(blink::FilterOperation::DROP_SHADOW);
    }

    void WebFilterOperationsImpl::appendColorMatrixFilter(SkScalar matrix[20])
    {
        m_operationTypes.append(blink::FilterOperation::GRAYSCALE);
    }

    void WebFilterOperationsImpl::appendZoomFilter(float amount, int inset)
    {
        m_operationTypes.append(blink::FilterOperation::GRAYSCALE);
    }

    void WebFilterOperationsImpl::appendSaturatingBrightnessFilter(float amount)
    {
        m_operationTypes.append(blink::FilterOperation::GRAYSCALE);
    }

    // This grabs a ref on the passed-in filter.
    void WebFilterOperationsImpl::appendReferenceFilter(SkImageFilter*)
    {
        m_operationTypes.append(blink::FilterOperation::GRAYSCALE);
    }

    void WebFilterOperationsImpl::clear()
    {
        m_operationTypes.clear();
    }

    bool WebFilterOperationsImpl::isEmpty() const
    {
        return m_operationTypes.isEmpty();
    }

} // cc_blink
