
#ifndef net_FlattenHTTPBodyElement_h
#define net_FlattenHTTPBodyElement_h

#include "third_party/WebKit/public/platform/WebHTTPBody.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include <vector>

namespace net {

struct FlattenHTTPBodyElement {
    blink::WebHTTPBody::Element::Type type;

    Vector<char> data;
    std::wstring filePath;
    long long fileStart;
    long long fileLength; // -1 means to the end of the file.
};

class FlattenHTTPBodyElementStream {
public:
    FlattenHTTPBodyElementStream(const WTF::Vector<FlattenHTTPBodyElement*>& elements)
    {
        m_elements = elements;
        m_file = nullptr;
        m_formDataElementIndex = 0;
        m_formDataElementDataOffset = 0;
    }

    ~FlattenHTTPBodyElementStream()
    {
        for (size_t i = 0; i < m_elements.size(); ++i) {
            delete m_elements[i];
            if (m_file)
                fclose(m_file);
        }
    }

    void reset()
    {
        m_formDataElementIndex = 0;
        if (m_file)
            fclose(m_file);
        m_file = nullptr;
    }
    
    size_t read(void* ptr, size_t blockSize, size_t numberOfBlocks)
    {
        // Check for overflow.
        if (!numberOfBlocks || blockSize > std::numeric_limits<size_t>::max() / numberOfBlocks) {
            //DebugBreak();
            return 0;
        }

        if (m_formDataElementIndex >= m_elements.size()) {
            //DebugBreak();
            return 0;
        }

        const FlattenHTTPBodyElement& element = *(m_elements[m_formDataElementIndex]);

        size_t toSend = blockSize * numberOfBlocks;
        size_t sent;
        
        if (blink::WebHTTPBody::Element::Type::TypeFile == element.type ||
            blink::WebHTTPBody::Element::Type::TypeFileSystemURL == element.type) {
            std::vector<char> filePath;
            WTF::WCharToMByte(element.filePath.c_str(), element.filePath.size(), &filePath, CP_ACP);
            if (!m_file)
                m_file = _wfopen(element.filePath.c_str(), L"rb");

            if (!m_file) {
                OutputDebugStringW(L"FlattenHTTPBodyElementStream._wfopen Fail:");
                OutputDebugStringW(element.filePath.c_str());
                OutputDebugStringW(L"\n");
                // FIXME: show a user error?
                return 0;
            }

            sent = fread(ptr, blockSize, numberOfBlocks, m_file);
            if (!blockSize && ferror(m_file)) {
                OutputDebugStringW(L"FlattenHTTPBodyElementStream.ferror Fail:");
                OutputDebugStringW(element.filePath.c_str());
                OutputDebugStringW(L"\n");
                // FIXME: show a user error?
                return 0;
            }
            if (feof(m_file)) {
                fclose(m_file);
                m_file = nullptr;
                m_formDataElementIndex++;
            }
        } else {
            size_t elementSize = element.data.size() - m_formDataElementDataOffset;
            sent = elementSize > toSend ? toSend : elementSize;

            const char* realData = element.data.data() + m_formDataElementDataOffset;
            memcpy(ptr, realData, sent);

            if (elementSize > sent)
                m_formDataElementDataOffset += sent;
            else {
                m_formDataElementDataOffset = 0;
                m_formDataElementIndex++;
            }
        }

        return sent;
    }

    bool hasMoreElements() const
    {
        return m_formDataElementIndex < m_elements.size();
    }

private:
    FILE* m_file;
    size_t m_formDataElementIndex;
    size_t m_formDataElementDataOffset;
    WTF::Vector<FlattenHTTPBodyElement*> m_elements;
};

}

#endif // net_FlattenHTTPBodyElement_h