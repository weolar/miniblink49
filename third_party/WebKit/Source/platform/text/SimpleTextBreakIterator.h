
#ifndef SimpleTextBreakIterator_h
#define SimpleTextBreakIterator_h

#include "wtf/text/qt4/QChar.h"

namespace blink {

#if 0

TextBreakIterator::TextBreakIterator(){}
TextBreakIterator::~TextBreakIterator(){}

class SimpleTextBreakIterator : public TextBreakIterator {
public:
    SimpleTextBreakIterator();
    ~SimpleTextBreakIterator();

    void setText(const LChar* string, int length);
    void setText(const UChar* string, int length);
    void scan();

    virtual int32_t first(void) OVERRIDE;
    virtual int32_t last(void) OVERRIDE;
    virtual int32_t previous(void) OVERRIDE;
    virtual int32_t next() OVERRIDE;
    virtual bool isBoundary(int32_t offset) OVERRIDE;
    virtual int32_t following(int32_t offset) OVERRIDE;
    virtual int32_t preceding(int32_t offset) OVERRIDE;
    virtual int32_t current(void) const OVERRIDE;

private:
    struct BreakRun {
        BreakRun(int start, int end)
        {
            this->start = start;
            this->end = end;
        }
        int start;
        int end;
    };
    Vector<BreakRun> m_breakRuns;
    const UChar* m_string;
    int m_length;
    int m_pos;
};

SimpleTextBreakIterator::SimpleTextBreakIterator()
{
    m_string = nullptr;
    m_length = 0;
}

void SimpleTextBreakIterator::setText(const LChar* string, int length)
{
    m_string = nullptr;
    m_length = 0;
}

void SimpleTextBreakIterator::setText(const UChar* string, int length)
{
    m_string = string;
    m_length = length;
    scan();
    first();
}

bool isWordSpace(UChar c)
{
    const static UChar kWordSpace[] = L" ,.;\"{}~!@#$%^&*()_+|'<>/\\";
    const int length = sizeof(kWordSpace) / sizeof(UChar);
    for (int i = 0; i < length; ++i) {
        if (kWordSpace[i] == c)
            return true;
    }

    return false;
}

void SimpleTextBreakIterator::scan()
{
    return;

    m_breakRuns.clear();

    int posA = 0;
    int posB = 0;
    for (posA = 0; posA < m_length; ++posA) {
        if (!isWordSpace(m_string[posA]))
            continue;

        posB = posA;

        for (; posB < m_length; ++posB) {
            if (!isWordSpace(m_string[posB]))
                continue;
            m_breakRuns.append(BreakRun(posA, posB));
            posA = posB;
            ++posB;
            break;
        }
    }
}

int32_t SimpleTextBreakIterator::first()
{
    return (int32_t)-1;

    if (0 == m_breakRuns.size())
        return (int32_t)-1;

    m_pos = 0;
    return m_breakRuns[m_pos].start;
}

int32_t SimpleTextBreakIterator::last(void)
{
    return (int32_t)-1;

    if (0 == m_breakRuns.size())
        return (int32_t)-1;

    m_pos = m_breakRuns.size() - 1;
    return m_breakRuns[m_pos].start;
}

int32_t SimpleTextBreakIterator::previous(void)
{
    return (int32_t)-1;

    if (0 == m_breakRuns.size())
        return (int32_t)-1;

    if (0 == m_pos)
        return (int32_t)-1;

    --m_pos;
    return m_breakRuns[m_pos].start;
}

int32_t SimpleTextBreakIterator::next()
{
    return (int32_t)-1;

    if (0 == m_breakRuns.size())
        return (int32_t)-1;

    if (m_breakRuns.size() - 1 == m_pos)
        return (int32_t)-1;

    ++m_pos;
    return m_breakRuns[m_pos].start;
}

bool SimpleTextBreakIterator::isBoundary(int32_t offset)
{
    return true;
}
int32_t SimpleTextBreakIterator::following(int32_t offset)
{
    return (int32_t)-1;
}

int32_t SimpleTextBreakIterator::preceding(int32_t offset)
{
    return (int32_t)-1;
}

int32_t SimpleTextBreakIterator::current(void) const
{
    return (int32_t)-1;
}
#endif // if 0

//////////////////////////////////////////////////////////////////////////
void TextBreakIterator::reset(const UChar* str, int len)
{
    m_string = str;
    m_length = len;
    m_currentPos = 0;
}

int32_t TextBreakIterator::last()
{
    m_currentPos = m_length;
    return m_currentPos;
}

int32_t TextBreakIterator::following(int32_t position)
{
    m_currentPos = position;
    return next();
}

int32_t TextBreakIterator::preceding(int32_t position)
{
    m_currentPos = position;
    return previous();
}

bool TextBreakIterator::isBoundary(int32_t offset)
{
    UChar c = m_string[m_currentPos + offset];
    if (WTF::Unicode::isSpace(c))
        return true;

    if (c > 255)
        return true;

    return false;
}

int32_t TextBreakIterator::current(void) const
{
    return m_currentPos;
}

//////////////////////////////////////////////////////////////////////////

static bool isWordBreak(UChar c)
{
    return WTF::Unicode::isSpace(c) || L'\n' == c;
}

struct WordBreakIterator : TextBreakIterator {
    WordBreakIterator()
    {
    }

    ~WordBreakIterator()
    {
    }

    virtual int first() OVERRIDE
    {
        m_currentPos = 0;
        return m_currentPos;
    }

    virtual int next() OVERRIDE
    {
        ++m_currentPos;
        if (m_currentPos >= m_length) {
            m_currentPos = -1;
            return m_currentPos;
        }
        
        int haveSpace = 0;
        while (m_currentPos < m_length) {
            bool isBreak = isWordBreak(m_string[m_currentPos]);
            ++m_currentPos;
            if (isBreak) {
                haveSpace = 1;
                break;
            }
        }
        return m_currentPos;
    }

//     int nextError2()
//     {
//         if (m_currentPos == m_length) {
//             m_currentPos = -1;
//             return m_currentPos;
//         }
//         bool haveSpace = false;
//         while (m_currentPos < m_length) {
//             if (haveSpace && !isWordBreak(m_string[m_currentPos]))
//                 break;
//             if (isWordBreak(m_string[m_currentPos]))
//                 haveSpace = true;
//             ++m_currentPos;
//         }
//         return m_currentPos;
//     }

    virtual int previous() OVERRIDE
    {
        --m_currentPos;
        if (m_currentPos <= 0) {
            m_currentPos = -1;
            return m_currentPos;
        }

        int haveSpace = 0;
        while (m_currentPos > 0) {
            bool isBreak = isWordBreak(m_string[m_currentPos]);
            --m_currentPos;
            if (isBreak) {
                haveSpace = 1;
                break;
            }
            
        }
        return m_currentPos + haveSpace;
    }

//     int previousError2()
//     {
//         if (!m_currentPos) {
//             m_currentPos = -1;
//             return m_currentPos;
//         }
// 
//         bool haveSpace = false;
//         while (m_currentPos > 0) {
//             if (haveSpace && !isWordBreak(m_string[m_currentPos]))
//                 break;
//             if (isWordBreak(m_string[m_currentPos]))
//                 haveSpace = true;
//             --m_currentPos;
//         }
//         return m_currentPos;
//     }
};

struct CharBreakIterator : TextBreakIterator {
    CharBreakIterator()
    {
    }

    ~CharBreakIterator()
    {
    }

    virtual int first() OVERRIDE
    {
        m_currentPos = 0;
        return m_currentPos;
    }

    virtual int next() OVERRIDE
    {
        if (m_currentPos >= m_length)
            return m_length - 1;
        ++m_currentPos;
        return m_currentPos;
    }

    virtual int previous() OVERRIDE
    {
        if (m_currentPos <= 0)
            return -1;
        if (m_currentPos > m_length)
            m_currentPos = m_length - 1;
        --m_currentPos;
        return m_currentPos;
    }
};

struct LineBreakIterator : TextBreakIterator {
    virtual int first() OVERRIDE
    {
        m_currentPos = 0;
        return m_currentPos;
    }

    virtual int next() OVERRIDE
    {
        if (m_currentPos >= m_length) {
            return -1;
        }

        ++m_currentPos;
        while (m_currentPos < m_length && !WTF::Unicode::isLineStop(m_string[m_currentPos]))
            ++m_currentPos;
        return m_currentPos;
    }

    virtual int previous() OVERRIDE
    {
        if (m_currentPos <= 0)
            return -1;
        if (m_currentPos > m_length)
            m_currentPos = m_length;
        --m_currentPos;
        while (m_currentPos > 0 && !WTF::Unicode::isLineStop(m_string[m_currentPos]))
            --m_currentPos;
        return m_currentPos;
    }
};

struct SentenceBreakIterator : TextBreakIterator {
    SentenceBreakIterator()
    {
    }

    ~SentenceBreakIterator()
    {
    }

    virtual int first() OVERRIDE
    {
        m_currentPos = 0;
        return m_currentPos;
    }

    virtual int next() OVERRIDE
    {
        m_currentPos++;
        if (m_currentPos >= m_length) {
            m_currentPos = -1;
            return m_currentPos;
        }       

        bool haveSpace = false;
        while (m_currentPos < m_length) {
            if (haveSpace && !WTF::Unicode::isSentenceStop(m_string[m_currentPos]))
                break;
            if (WTF::Unicode::isSentenceStop(m_string[m_currentPos]))
                haveSpace = true;
            ++m_currentPos;
        }
        return m_currentPos;
    }

    virtual int previous() OVERRIDE
    {
        --m_currentPos;
        if (m_currentPos <= 0) {
            m_currentPos = -1;
            return m_currentPos;
        }

        bool haveSpace = false;
        while (m_currentPos > 0) {
            if (haveSpace && !WTF::Unicode::isSentenceStop(m_string[m_currentPos]))
                break;
            if (WTF::Unicode::isSentenceStop(m_string[m_currentPos]))
                haveSpace = true;
            --m_currentPos;
        }
        return m_currentPos;
    }
};

} // blink 

#endif SimpleTextBreakIterator_h