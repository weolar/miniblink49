
#include "config.h"
#include <unicode/ucnv.h>
#include <unicode/utypes.h>
#include <wtf/FastMalloc.h>
#include "platform/text/IcuCharsetDetector.h"

#define BUFFER_SIZE 8192
#define ARRAY_SIZE(array) (sizeof array / sizeof array[0])
#define NEW_ARRAY(type,count) (type *) WTF::fastMalloc((count) * sizeof(type))
#define DELETE_ARRAY(array) WTF::fastFree((void *) (array))

InputText::InputText(UErrorCode &status)
    : fInputBytes(NEW_ARRAY(uint8_t, BUFFER_SIZE)), // The text to be checked.  Markup will have been
    //   removed if appropriate.
    fByteStats(NEW_ARRAY(int16_t, 256)),       // byte frequency statistics for the input text.
    //   Value is percent, not absolute.
    fDeclaredEncoding(0),
    fRawInput(0),
    fRawLength(0)
{
    if (fInputBytes == NULL || fByteStats == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

InputText::~InputText()
{
    DELETE_ARRAY(fDeclaredEncoding);
    DELETE_ARRAY(fByteStats);
    DELETE_ARRAY(fInputBytes);
}

void InputText::setText(const char *in, int32_t len)
{
    fInputLen = 0;
    fC1Bytes = FALSE;
    fRawInput = (const uint8_t *)in;
    fRawLength = len == -1 ? (int32_t)strlen(in) : len;
}

UBool InputText::isSet() const
{
    return fRawInput != NULL;
}

void InputText::MungeInput(UBool fStripTags) {
    int     srci = 0;
    int     dsti = 0;
    uint8_t b;
    bool    inMarkup = FALSE;
    int32_t openTags = 0;
    int32_t badTags = 0;

    //
    //  html / xml markup stripping.
    //     quick and dirty, not 100% accurate, but hopefully good enough, statistically.
    //     discard everything within < brackets >
    //     Count how many total '<' and illegal (nested) '<' occur, so we can make some
    //     guess as to whether the input was actually marked up at all.
    // TODO: Think about how this interacts with EBCDIC charsets that are detected.
    if (fStripTags) {
        for (srci = 0; srci < fRawLength && dsti < BUFFER_SIZE; srci += 1) {
            b = fRawInput[srci];

            if (b == (uint8_t)0x3C) { /* Check for the ASCII '<' */
                if (inMarkup) {
                    badTags += 1;
                }

                inMarkup = TRUE;
                openTags += 1;
            }

            if (!inMarkup) {
                fInputBytes[dsti++] = b;
            }

            if (b == (uint8_t)0x3E) { /* Check for the ASCII '>' */
                inMarkup = FALSE;
            }
        }

        fInputLen = dsti;
    }

    //
    //  If it looks like this input wasn't marked up, or if it looks like it's
    //    essentially nothing but markup abandon the markup stripping.
    //    Detection will have to work on the unstripped input.
    //
    if (openTags < 5 || openTags / 5 < badTags ||
        (fInputLen < 100 && fRawLength>600))
    {
        int32_t limit = fRawLength;

        if (limit > BUFFER_SIZE) {
            limit = BUFFER_SIZE;
        }

        for (srci = 0; srci < limit; srci++) {
            fInputBytes[srci] = fRawInput[srci];
        }

        fInputLen = srci;
    }

    //
    // Tally up the byte occurence statistics.
    // These are available for use by the various detectors.
    //

    memset(fByteStats, 0, (sizeof fByteStats[0]) * 256);

    for (srci = 0; srci < fInputLen; srci += 1) {
        fByteStats[fInputBytes[srci]] += 1;
    }

    for (int32_t i = 0x80; i <= 0x9F; i += 1) {
        if (fByteStats[i] != 0) {
            fC1Bytes = TRUE;
            break;
        }
    }
}

CharsetRecog_UTF8::~CharsetRecog_UTF8()
{
    // nothing to do
}

const char *CharsetRecog_UTF8::getName() const
{
    return "UTF-8";
}

UBool CharsetRecog_UTF8::match(InputText* input, CharsetMatch *results) const {
    bool hasBOM = FALSE;
    int32_t numValid = 0;
    int32_t numInvalid = 0;
    const uint8_t *inputBytes = input->fRawInput;
    int32_t i;
    int32_t trailBytes = 0;
    int32_t confidence;

    if (input->fRawLength >= 3 &&
        inputBytes[0] == 0xEF && inputBytes[1] == 0xBB && inputBytes[2] == 0xBF) {
        hasBOM = TRUE;
    }

    // Scan for multi-byte sequences
    for (i = 0; i < input->fRawLength; i += 1) {
        int32_t b = inputBytes[i];

        if ((b & 0x80) == 0) {
            continue;   // ASCII
        }

        // Hi bit on char found.  Figure out how long the sequence should be
        if ((b & 0x0E0) == 0x0C0) {
            trailBytes = 1;
        }
        else if ((b & 0x0F0) == 0x0E0) {
            trailBytes = 2;
        }
        else if ((b & 0x0F8) == 0xF0) {
            trailBytes = 3;
        }
        else {
            numInvalid += 1;
            continue;
        }

        // Verify that we've got the right number of trail bytes in the sequence
        for (;;) {
            i += 1;

            if (i >= input->fRawLength) {
                break;
            }

            b = inputBytes[i];

            if ((b & 0xC0) != 0x080) {
                numInvalid += 1;
                break;
            }

            if (--trailBytes == 0) {
                numValid += 1;
                break;
            }
        }

    }

    // Cook up some sort of confidence score, based on presence of a BOM
    //    and the existence of valid and/or invalid multi-byte sequences.
    confidence = 0;
    if (hasBOM && numInvalid == 0) {
        confidence = 100;
    }
    else if (hasBOM && numValid > numInvalid * 10) {
        confidence = 80;
    }
    else if (numValid > 3 && numInvalid == 0) {
        confidence = 100;
    }
    else if (numValid > 0 && numInvalid == 0) {
        confidence = 80;
    }
    else if (numValid == 0 && numInvalid == 0) {
        // Plain ASCII. Confidence must be > 10, it's more likely than UTF-16, which
        //              accepts ASCII with confidence = 10.
        confidence = 15;
    }
    else if (numValid > numInvalid * 10) {
        // Probably corruput utf-8 data.  Valid sequences aren't likely by chance.
        confidence = 25;
    }

    results->set(input, this, confidence);
    return (confidence > 0);
}
//////////////////////////////////////////////////////////////////////////

CharsetMatch::CharsetMatch()
    : textIn(NULL), confidence(0), fCharsetName(NULL), fLang(NULL)
{
    // nothing else to do.
}

void CharsetMatch::set(InputText *input, const CharsetRecognizer *cr, int32_t conf,
    const char *csName, const char *lang)
{
    textIn = input;
    confidence = conf;
    fCharsetName = csName;
    fLang = lang;
    if (cr != NULL) {
        if (fCharsetName == NULL) {
            fCharsetName = cr->getName();
        }
        if (fLang == NULL) {
            fLang = cr->getLanguage();
        }
    }
}

const char* CharsetMatch::getName()const
{
    return fCharsetName;
}

const char* CharsetMatch::getLanguage()const
{
    return fLang;
}

int32_t CharsetMatch::getConfidence()const
{
    return confidence;
}

static CSRecognizerInfo **fCSRecognizers = NULL;
static int32_t fCSRecognizers_size = 0;

CharsetDetector::CharsetDetector(UErrorCode &status)
    : textIn(new InputText(status)), resultArray(NULL),
    resultCount(0), fStripTags(FALSE), fFreshTextSet(FALSE),
    fEnabledRecognizers(NULL)
{
    setRecognizers(status);

    resultArray = (CharsetMatch **)WTF::fastMalloc(sizeof(CharsetMatch *)*fCSRecognizers_size);

    if (resultArray == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return;
    }

    for (int32_t i = 0; i < fCSRecognizers_size; i += 1) {
        resultArray[i] = new CharsetMatch();

        if (resultArray[i] == NULL) {
            status = U_MEMORY_ALLOCATION_ERROR;
            break;
        }
    }
}

CharsetDetector::~CharsetDetector()
{
    delete textIn;

    for (int32_t i = 0; i < fCSRecognizers_size; i += 1) {
        delete resultArray[i];
    }

    WTF::fastFree(resultArray);

    if (fEnabledRecognizers) {
        WTF::fastFree(fEnabledRecognizers);
    }
}

void CharsetDetector::setText(const char *in, int32_t len)
{
    textIn->setText(in, len);
    fFreshTextSet = TRUE;
}

const CharsetMatch *CharsetDetector::detect(UErrorCode &status)
{
    int32_t maxMatchesFound = 0;

    detectAll(maxMatchesFound, status);

    if (maxMatchesFound > 0) {
        return resultArray[0];
    }
    else {
        return NULL;
    }
}

void CharsetDetector::setRecognizers(UErrorCode &status)
{
    static bool init = false;
    if (init)
        return;
    init = true;

    CSRecognizerInfo *tempArray[] = {
        new CSRecognizerInfo(new CharsetRecog_UTF8(), TRUE),
    };
    int32_t rCount = ARRAY_SIZE(tempArray);

    fCSRecognizers = NEW_ARRAY(CSRecognizerInfo *, rCount);

    if (fCSRecognizers == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    else {
        fCSRecognizers_size = rCount;
        for (int32_t r = 0; r < rCount; r += 1) {
            fCSRecognizers[r] = tempArray[r];
            if (fCSRecognizers[r] == NULL) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
}

const CharsetMatch * const *CharsetDetector::detectAll(int32_t &maxMatchesFound, UErrorCode &status)
{
    if (!textIn->isSet()) {
        status = U_MISSING_RESOURCE_ERROR;// TODO:  Need to set proper status code for input text not set

        return NULL;
    }
    else if (fFreshTextSet) {
        CharsetRecognizer *csr;
        int32_t            i;

        textIn->MungeInput(fStripTags);

        // Iterate over all possible charsets, remember all that
        // give a match quality > 0.
        resultCount = 0;
        for (i = 0; i < fCSRecognizers_size; i += 1) {
            csr = fCSRecognizers[i]->recognizer;
            if (csr->match(textIn, resultArray[resultCount])) {
                resultCount++;
            }
        }

//         if (resultCount > 1) {
//             uprv_sortArray(resultArray, resultCount, sizeof resultArray[0], charsetMatchComparator, NULL, TRUE, &status);
//         }
        fFreshTextSet = FALSE;
    }

    maxMatchesFound = resultCount;

    return resultArray;
}