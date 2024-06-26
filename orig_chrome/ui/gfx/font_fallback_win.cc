// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/font_fallback_win.h"

#include <usp10.h>

#include <map>

#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/profiler/scoped_tracker.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/registry.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_fallback.h"

namespace gfx {

namespace {

    // Queries the registry to get a mapping from font filenames to font names.
    void QueryFontsFromRegistry(std::map<std::string, std::string>* map)
    {
        const wchar_t* kFonts = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

        base::win::RegistryValueIterator it(HKEY_LOCAL_MACHINE, kFonts);
        for (; it.Valid(); ++it) {
            const std::string filename = base::ToLowerASCII(base::WideToUTF8(it.Value()));
            (*map)[filename] = base::WideToUTF8(it.Name());
        }
    }

    // Fills |font_names| with a list of font families found in the font file at
    // |filename|. Takes in a |font_map| from font filename to font families, which
    // is filled-in by querying the registry, if empty.
    void GetFontNamesFromFilename(const std::string& filename,
        std::map<std::string, std::string>* font_map,
        std::vector<std::string>* font_names)
    {
        if (font_map->empty())
            QueryFontsFromRegistry(font_map);

        std::map<std::string, std::string>::const_iterator it = font_map->find(base::ToLowerASCII(filename));
        if (it == font_map->end())
            return;

        internal::ParseFontFamilyString(it->second, font_names);
    }

    // Returns true if |text| contains only ASCII digits.
    bool ContainsOnlyDigits(const std::string& text)
    {
        return text.find_first_not_of("0123456789") == base::string16::npos;
    }

    // Appends a Font with the given |name| and |size| to |fonts| unless the last
    // entry is already a font with that name.
    void AppendFont(const std::string& name, int size, std::vector<Font>* fonts)
    {
        if (fonts->empty() || fonts->back().GetFontName() != name)
            fonts->push_back(Font(name, size));
    }

    // Queries the registry to get a list of linked fonts for |font|.
    void QueryLinkedFontsFromRegistry(const Font& font,
        std::map<std::string, std::string>* font_map,
        std::vector<Font>* linked_fonts)
    {
        const wchar_t* kSystemLink = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink";

        base::win::RegKey key;
        if (FAILED(key.Open(HKEY_LOCAL_MACHINE, kSystemLink, KEY_READ)))
            return;

        const std::wstring original_font_name = base::UTF8ToWide(font.GetFontName());
        std::vector<std::wstring> values;
        if (FAILED(key.ReadValues(original_font_name.c_str(), &values))) {
            key.Close();
            return;
        }

        std::string filename;
        std::string font_name;
        for (size_t i = 0; i < values.size(); ++i) {
            internal::ParseFontLinkEntry(
                base::WideToUTF8(values[i]), &filename, &font_name);
            // If the font name is present, add that directly, otherwise add the
            // font names corresponding to the filename.
            if (!font_name.empty()) {
                AppendFont(font_name, font.GetFontSize(), linked_fonts);
            } else if (!filename.empty()) {
                std::vector<std::string> font_names;
                GetFontNamesFromFilename(filename, font_map, &font_names);
                for (size_t i = 0; i < font_names.size(); ++i)
                    AppendFont(font_names[i], font.GetFontSize(), linked_fonts);
            }
        }

        key.Close();
    }

    // CachedFontLinkSettings is a singleton cache of the Windows font settings
    // from the registry. It maintains a cached view of the registry's list of
    // system fonts and their font link chains.
    class CachedFontLinkSettings {
    public:
        static CachedFontLinkSettings* GetInstance();

        // Returns the linked fonts list correspond to |font|. Returned value will
        // never be null.
        const std::vector<Font>* GetLinkedFonts(const Font& font);

    private:
        friend struct base::DefaultSingletonTraits<CachedFontLinkSettings>;

        CachedFontLinkSettings();
        virtual ~CachedFontLinkSettings();

        // Map of system fonts, from file names to font families.
        std::map<std::string, std::string> cached_system_fonts_;

        // Map from font names to vectors of linked fonts.
        std::map<std::string, std::vector<Font>> cached_linked_fonts_;

        DISALLOW_COPY_AND_ASSIGN(CachedFontLinkSettings);
    };

    // static
    CachedFontLinkSettings* CachedFontLinkSettings::GetInstance()
    {
        return base::Singleton<
            CachedFontLinkSettings,
            base::LeakySingletonTraits<CachedFontLinkSettings>>::get();
    }

    const std::vector<Font>* CachedFontLinkSettings::GetLinkedFonts(
        const Font& font)
    {
        const std::string& font_name = font.GetFontName();
        std::map<std::string, std::vector<Font>>::const_iterator it = cached_linked_fonts_.find(font_name);
        if (it != cached_linked_fonts_.end())
            return &it->second;

        cached_linked_fonts_[font_name] = std::vector<Font>();
        std::vector<Font>* linked_fonts = &cached_linked_fonts_[font_name];

        // TODO(ckocagil): Remove ScopedTracker below once crbug.com/441028 is fixed.
        tracked_objects::ScopedTracker tracking_profile(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "441028 QueryLinkedFontsFromRegistry()"));

        QueryLinkedFontsFromRegistry(font, &cached_system_fonts_, linked_fonts);
        return linked_fonts;
    }

    CachedFontLinkSettings::CachedFontLinkSettings()
    {
    }

    CachedFontLinkSettings::~CachedFontLinkSettings()
    {
    }

    // Callback to |EnumEnhMetaFile()| to intercept font creation.
    int CALLBACK MetaFileEnumProc(HDC hdc,
        HANDLETABLE* table,
        CONST ENHMETARECORD* record,
        int table_entries,
        LPARAM log_font)
    {
        if (record->iType == EMR_EXTCREATEFONTINDIRECTW) {
            const EMREXTCREATEFONTINDIRECTW* create_font_record = reinterpret_cast<const EMREXTCREATEFONTINDIRECTW*>(record);
            *reinterpret_cast<LOGFONT*>(log_font) = create_font_record->elfw.elfLogFont;
        }
        return 1;
    }

} // namespace

namespace internal {

    void ParseFontLinkEntry(const std::string& entry,
        std::string* filename,
        std::string* font_name)
    {
        std::vector<std::string> parts = base::SplitString(
            entry, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
        filename->clear();
        font_name->clear();
        if (parts.size() > 0)
            *filename = parts[0];
        // The second entry may be the font name or the first scaling factor, if the
        // entry does not contain a font name. If it contains only digits, assume it
        // is a scaling factor.
        if (parts.size() > 1 && !ContainsOnlyDigits(parts[1]))
            *font_name = parts[1];
    }

    void ParseFontFamilyString(const std::string& family,
        std::vector<std::string>* font_names)
    {
        // The entry is comma separated, having the font filename as the first value
        // followed optionally by the font family name and a pair of integer scaling
        // factors.
        // TODO(asvitkine): Should we support these scaling factors?
        *font_names = base::SplitString(
            family, "&", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
        if (!font_names->empty()) {
            const size_t index = font_names->back().find('(');
            if (index != std::string::npos) {
                font_names->back().resize(index);
                base::TrimWhitespaceASCII(font_names->back(), base::TRIM_TRAILING,
                    &font_names->back());
            }
        }
    }

    LinkedFontsIterator::LinkedFontsIterator(Font font)
        : original_font_(font)
        , next_font_set_(false)
        , linked_fonts_(NULL)
        , linked_font_index_(0)
    {
        SetNextFont(original_font_);
    }

    LinkedFontsIterator::~LinkedFontsIterator()
    {
    }

    void LinkedFontsIterator::SetNextFont(Font font)
    {
        next_font_ = font;
        next_font_set_ = true;
    }

    bool LinkedFontsIterator::NextFont(Font* font)
    {
        if (next_font_set_) {
            next_font_set_ = false;
            current_font_ = next_font_;
            *font = current_font_;
            return true;
        }

        // First time through, get the linked fonts list.
        if (linked_fonts_ == NULL)
            linked_fonts_ = GetLinkedFonts();

        if (linked_font_index_ == linked_fonts_->size())
            return false;

        current_font_ = linked_fonts_->at(linked_font_index_++);
        *font = current_font_;
        return true;
    }

    const std::vector<Font>* LinkedFontsIterator::GetLinkedFonts() const
    {
        CachedFontLinkSettings* font_link = CachedFontLinkSettings::GetInstance();

        // First, try to get the list for the original font.
        const std::vector<Font>* fonts = font_link->GetLinkedFonts(original_font_);

        // If there are no linked fonts for the original font, try querying the
        // ones for the current font. This may happen if the first font is a custom
        // font that has no linked fonts in the registry.
        //
        // Note: One possibility would be to always merge both lists of fonts,
        //       but it is not clear whether there are any real world scenarios
        //       where this would actually help.
        if (fonts->empty())
            fonts = font_link->GetLinkedFonts(current_font_);

        return fonts;
    }

} // namespace internal

std::vector<Font> GetFallbackFonts(const Font& font)
{
    std::string font_family = font.GetFontName();

    // LinkedFontsIterator doesn't care about the font size, so we always pass 10.
    internal::LinkedFontsIterator linked_fonts(Font(font_family, 10));
    std::vector<Font> fallback_fonts;
    Font current;
    while (linked_fonts.NextFont(&current))
        fallback_fonts.push_back(current);
    return fallback_fonts;
}

bool GetUniscribeFallbackFont(const Font& font,
    const wchar_t* text,
    int text_length,
    Font* result)
{
    // Adapted from WebKit's |FontCache::GetFontDataForCharacters()|.
    // Uniscribe doesn't expose a method to query fallback fonts, so this works by
    // drawing the text to an EMF object with Uniscribe's ScriptStringOut and then
    // inspecting the EMF object to figure out which font Uniscribe used.
    //
    // DirectWrite in Windows 8.1 provides a cleaner alternative:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dn280480.aspx

    static HDC hdc = CreateCompatibleDC(NULL);

    // Use a meta file to intercept the fallback font chosen by Uniscribe.
    HDC meta_file_dc = CreateEnhMetaFile(hdc, NULL, NULL, NULL);
    if (!meta_file_dc)
        return false;

    SelectObject(meta_file_dc, font.GetNativeFont());

    SCRIPT_STRING_ANALYSIS script_analysis;
    HRESULT hresult = ScriptStringAnalyse(meta_file_dc, text, text_length, 0, -1,
        SSA_METAFILE | SSA_FALLBACK | SSA_GLYPHS | SSA_LINK,
        0, NULL, NULL, NULL, NULL, NULL, &script_analysis);

    if (SUCCEEDED(hresult)) {
        hresult = ScriptStringOut(script_analysis, 0, 0, 0, NULL, 0, 0, FALSE);
        ScriptStringFree(&script_analysis);
    }

    bool found_fallback = false;
    HENHMETAFILE meta_file = CloseEnhMetaFile(meta_file_dc);
    if (SUCCEEDED(hresult)) {
        LOGFONT log_font;
        log_font.lfFaceName[0] = 0;
        EnumEnhMetaFile(0, meta_file, MetaFileEnumProc, &log_font, NULL);
        if (log_font.lfFaceName[0]) {
            *result = Font(base::UTF16ToUTF8(log_font.lfFaceName),
                font.GetFontSize());
            found_fallback = true;
        }
    }
    DeleteEnhMetaFile(meta_file);

    return found_fallback;
}

} // namespace gfx
