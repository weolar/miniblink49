
class InputText {
    // Prevent copying
    InputText(const InputText &);
public:
    InputText(UErrorCode &status);
    ~InputText();

    void setText(const char *in, int32_t len);
//     void setDeclaredEncoding(const char *encoding, int32_t len);
    UBool isSet() const;
    void MungeInput(UBool fStripTags);

    // The text to be checked.  Markup will have been
    //   removed if appropriate.
    uint8_t    *fInputBytes;
    int32_t     fInputLen;          // Length of the byte data in fInputBytes.
    // byte frequency statistics for the input text.
    //   Value is percent, not absolute.
    //   Value is rounded up, so zero really means zero occurences. 
    int16_t  *fByteStats;
    UBool     fC1Bytes;          // True if any bytes in the range 0x80 - 0x9F are in the input;false by default
    char     *fDeclaredEncoding;

    const uint8_t           *fRawInput;     // Original, untouched input bytes.
    //  If user gave us a byte array, this is it.
    //  If user gave us a stream, it's read to a 
    //   buffer here.
    int32_t                  fRawLength;    // Length of data in fRawInput array.
};

class CharsetRecognizer;

class CharsetMatch {
private:
    InputText               *textIn;
    int32_t                 confidence;
    const char              *fCharsetName;
    const char              *fLang;

public:
    CharsetMatch();

    /**
    * fully set the state of this CharsetMatch.
    * Called by the CharsetRecognizers to record match results.
    * Default (NULL) parameters for names will be filled by calling the
    *   corresponding getters on the recognizer.
    */
    void set(InputText               *input,
        const CharsetRecognizer *cr,
        int32_t                  conf,
        const char              *csName = NULL,
        const char              *lang = NULL);

    /**
    * Return the name of the charset for this Match
    */
    const char *getName() const;

    const char *getLanguage()const;

    int32_t getConfidence()const;

    int32_t getUChars(UChar *buf, int32_t cap, UErrorCode *status) const;
};

class CharsetRecognizer {
public:

    virtual const char *getName() const = 0;

    virtual const char *getLanguage() const { return ""; }

    virtual UBool match(InputText *textIn, CharsetMatch *results) const = 0;

    virtual ~CharsetRecognizer() {}
};

class CharsetRecog_UTF8 : public CharsetRecognizer {

public:

    virtual ~CharsetRecog_UTF8();

    const char *getName() const;

    /* (non-Javadoc)
    * @see com.ibm.icu.text.CharsetRecognizer#match(com.ibm.icu.text.CharsetDetector)
    */
    UBool match(InputText *input, CharsetMatch *results) const;
};

class CharsetDetector {
private:
    InputText *textIn;
    CharsetMatch **resultArray;
    int32_t resultCount;
    UBool fStripTags;   // If true, setText() will strip tags from input text.
    UBool fFreshTextSet;
    static void setRecognizers(UErrorCode &status);

    UBool *fEnabledRecognizers;  // If not null, active set of charset recognizers had
    // been changed from the default. The array index is
    // corresponding to fCSRecognizers. See setDetectableCharset().

public:
    CharsetDetector(UErrorCode &status);

    ~CharsetDetector();

    void setText(const char *in, int32_t len);

    const CharsetMatch * const *detectAll(int32_t &maxMatchesFound, UErrorCode &status);

    const CharsetMatch *detect(UErrorCode& status);
};

//////////////////////////////////////////////////////////////////////////
struct CSRecognizerInfo {
    CSRecognizerInfo(CharsetRecognizer *recognizer, UBool isDefaultEnabled)
        : recognizer(recognizer), isDefaultEnabled(isDefaultEnabled) {};

    ~CSRecognizerInfo() { delete recognizer; };

    CharsetRecognizer *recognizer;
    UBool isDefaultEnabled;
};