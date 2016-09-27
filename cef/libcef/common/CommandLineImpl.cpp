
#include "include/capi/cef_command_line_capi.h"
#include "base/file_path.h"
#include "cef/libcef/common/CommonBase.h"

#include "wtf/text/WTFString.h"
#include "wtf/text/StringHash.h"
#include "wtf/Vector.h"
#include "wtf/HashMap.h"

namespace cef {

static const WCHAR kSwitchTerminator[] = FILE_PATH_LITERAL("--");
static const WCHAR kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");
static const WCHAR* const kSwitchPrefixes[] = { L"--", L"-", L"/" };
static const size_t switch_prefix_count = arraysize(kSwitchPrefixes);

class CommandLineImpl : public CefCppBase<cef_command_line_t, CommandLineImpl> {
public:
    CommandLineImpl();
    void willDeleted()
    {
        DebugBreak();
    }
private:
    bool m_bIsValid;
    bool m_bReadOnly;
    WTF::Vector<String> m_argv;
    WTF::HashMap<String, String> m_switches;

    static int CEF_CALLBACK isValid(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, 0);
        return self->m_bIsValid;
    }

    static int CEF_CALLBACK CommandLineImplIsReadOnly(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, 0);
        return self->m_bReadOnly;
    }

    static size_t getSwitchPrefixLength(const WTF::String& string)
    {
        for (size_t i = 0; i < switch_prefix_count; ++i) {
            WTF::String prefix(kSwitchPrefixes[i]);
            if (string.startsWith(prefix))
                return prefix.length();
        }
        return 0;
    }

    // Fills in |switch_string| and |switch_value| if |string| is a switch.
    // This will preserve the input switch prefix in the output |switch_string|.
    static bool isSwitch(const WTF::String& string,
        WTF::String* switch_string,
        WTF::String* switch_value) {
        *switch_string = StringImpl::empty();
        *switch_value = StringImpl::empty();
        size_t prefix_length = getSwitchPrefixLength(string);
        if (prefix_length == 0 || prefix_length == string.length())
            return false;

        const size_t equals_position = string.find(kSwitchValueSeparator);
        *switch_string = string.substring(0, equals_position);
        if (equals_position != WTF::kNotFound)
            *switch_value = string.substring(equals_position + 1);
        return true;
    }

    static cef_command_line_t* CEF_CALLBACK copy(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, nullptr);
        CommandLineImpl* newSelf = new CommandLineImpl();
        newSelf->m_bIsValid = self->m_bIsValid;
        newSelf->m_bReadOnly = self->m_bReadOnly;
        newSelf->m_argv = self->m_argv;
        newSelf->m_switches = self->m_switches;
        return (cef_command_line_t*)newSelf;
    }

    void appendSwitchNative(const String& switch_string, const String& value)
    {
        String switch_key(switch_string.lower());
        String combined_switch_string(switch_key);

        size_t prefix_length = getSwitchPrefixLength(combined_switch_string);
        m_switches.set(switch_key.substring(prefix_length), value);
        // Preserve existing switch prefixes in |argv_|; only append one if necessary.
        if (prefix_length == 0)
            combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
        if (!value.isEmpty())
            combined_switch_string.append(kSwitchValueSeparator + value);
        // Append the switch and update the switches/arguments divider |begin_args_|.
        m_argv.append(combined_switch_string);
    }

    void appendArgNative(const String& value)
    {
        m_argv.append(value);
    }

    void appendSwitchesAndArguments(const WTF::Vector<String>& argv)
    {
        bool parse_switches = true;
        for (size_t i = 1; i < argv.size(); ++i) {
            String arg = argv[i];
            //TrimWhitespace(arg, TRIM_ALL, &arg);

            String switch_string;
            String switch_value;
            parse_switches &= (arg != String(kSwitchTerminator));
            if (parse_switches && isSwitch(arg, &switch_string, &switch_value)) {
                appendSwitchNative(switch_string, switch_value);
            }
            else {
                appendArgNative(arg);
            }
        }
    }

    void InitFromArgvV(const WTF::Vector<String>& argv)
    {
        m_argv.clear();
        m_switches.clear();

        m_argv.append(argv.isEmpty() ? StringImpl::empty() : argv[0]);
        appendSwitchesAndArguments(argv);
    }

    static void CEF_CALLBACK initFromArgv(cef_command_line_t* s, int argc, const char* const* argv)
    {
        DebugBreak();
    }

    void initFromArgvW(int argc, wchar_t** argv)
    {
        WTF::Vector<String> newArgv;
        for (int i = 0; i < argc; ++i)
            newArgv.append(String(argv[i]));
        InitFromArgvV(newArgv);
    }

public:
    void initFromWTFString(const WTF::String& commandLineString)
    {
        //TrimWhitespace(command_line, TRIM_ALL, &command_line_string);
        if (commandLineString.isEmpty())
            return;

        int numArgs = 0;
        wchar_t** args = NULL;
        args = ::CommandLineToArgvW(commandLineString.charactersWithNullTermination().data(), &numArgs);
        initFromArgvW(numArgs, args);
        ::LocalFree(args);
    }

private:
    static void CEF_CALLBACK initFromString(cef_command_line_t* s, const cef_string_t* command_line)
    {
        CEF_CHECK_ARGS_CAST_NORET(CommandLineImpl);
        WTF::String commandLineString(command_line->str, command_line->length);
        self->initFromWTFString(commandLineString);
    }

    static void CEF_CALLBACK reset(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST_NORET(CommandLineImpl);
        String argv;
        if (self->m_argv.size() >= 1)
            argv = self->m_argv[0];
        self->m_argv.clear();
        self->m_switches.clear();

        self->m_argv.append(argv);
    }

    static void CEF_CALLBACK getArgv(cef_command_line_t* s, cef_string_list_t argv)
    {
        CEF_CHECK_ARGS_CAST_NORET(CommandLineImpl);

        const WTF::Vector<String>& cmdArgv = self->m_argv;
        WTF::Vector<String>::const_iterator it = cmdArgv.begin();
        for (; it != cmdArgv.end(); ++it) {
            cef_string_utf16_t* value = cef_string_userfree_utf16_alloc();
            Vector<UChar> cmd = it->charactersWithNullTermination();
            cef_string_utf16_set(cmd.data(), cmd.size(), value, true);
            cef_string_list_append(argv, value);
        }
    }

    static WTF::String getArgumentsString(CommandLineImpl* self)
    {
        WTF::String params;
        // Append switches and arguments.
        bool parse_switches = true;
        for (size_t i = 1; i < self->m_argv.size(); ++i) {
            WTF::String arg = self->m_argv[i];
            WTF::String switch_string;
            WTF::String switch_value;
            parse_switches &= arg != WTF::String(kSwitchTerminator);
            if (i > 1)
                params.append((FILE_PATH_LITERAL(" ")));
            if (parse_switches && isSwitch(arg, &switch_string, &switch_value)) {
                params.append(switch_string);
                if (!switch_value.isEmpty()) {
                    // #if defined(OS_WIN)
                    //                 switch_value = QuoteForCommandLineToArgvW(switch_value);
                    // #endif
                    params.append(kSwitchValueSeparator + switch_value);
                }
            }
            else {
                // #if defined(OS_WIN)
                //             arg = QuoteForCommandLineToArgvW(arg);
                // #endif
                params.append(arg);
            }
        }
        return params;
    }

    static cef_string_userfree_t CEF_CALLBACK getCommandLineString(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, nullptr);
        WTF::String string;
        if (self->m_argv.size() > 0)
            string = self->m_argv[0];
        // #if defined(OS_WIN)
        //     string = QuoteForCommandLineToArgvW(string);
        // #endif
        WTF::String params(getArgumentsString(self));
        if (!params.isEmpty()) {
            string.append(WTF::String(FILE_PATH_LITERAL(" ")));
            string.append(params);
        }

        cef_string_userfree_utf16_t retVal = cef_string_userfree_utf16_alloc();
        Vector<UChar> str = string.charactersWithNullTermination();
        cef_string_utf16_set(str.data(), str.size(), retVal, true);
        return retVal;
    }

    static cef_string_userfree_t CEF_CALLBACK getProgram(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, nullptr);

        cef_string_userfree_utf16_t retVal = cef_string_userfree_utf16_alloc();
        if (self->m_argv.size() == 0)
            cef_string_utf16_set(L"", 0, retVal, false);
        else {
            Vector<UChar> str = self->m_argv[0].charactersWithNullTermination();
            cef_string_utf16_set(str.data(), str.size(), retVal, true);
        }
        return retVal;
    }

    static void CEF_CALLBACK setProgram(cef_command_line_t* s, const cef_string_t* program)
    {
        CEF_CHECK_ARGS_CAST_NORET(CommandLineImpl);

        String programStr(program->str, program->length);
        if (self->m_argv.size() == 0)
            self->m_argv.append(programStr);
        else
            self->m_argv[0] = programStr;
    }

    static int CEF_CALLBACK hasSwitches(cef_command_line_t* s)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, 0);
        return self->m_switches.size() > 0;
    }

    static int CEF_CALLBACK hasSwitch(cef_command_line_t* s, const cef_string_t* name)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, 0);
        String str(name->str, name->length);
        WTF::HashMap<String, String>::const_iterator it = self->m_switches.find(str.lower());
        return it != self->m_switches.end();
    }

    static cef_string_userfree_t CEF_CALLBACK getSwitchValue(cef_command_line_t* s, const cef_string_t* name)
    {
        CEF_CHECK_ARGS_CAST(CommandLineImpl, nullptr);
        String str(name->str, name->length);
        WTF::HashMap<String, String>::const_iterator it = self->m_switches.find(str.lower());

        cef_string_userfree_utf16_t retVal = cef_string_userfree_utf16_alloc();
        if (it != self->m_switches.end()) {
            Vector<UChar> str = it->value.charactersWithNullTermination();
            cef_string_utf16_set(str.data(), str.size(), retVal, true);
        }
        else
            cef_string_utf16_set(L"", 0, retVal, false);

        return retVal;
    }

    static void CEF_CALLBACK get_switches(struct _cef_command_line_t* self, cef_string_map_t switches) { DebugBreak(); }
    static void CEF_CALLBACK append_switch(struct _cef_command_line_t* self, const cef_string_t* name) { DebugBreak(); }
    static int CEF_CALLBACK has_arguments(struct _cef_command_line_t* self) { DebugBreak(); return 0; }
    static void CEF_CALLBACK get_arguments(struct _cef_command_line_t* self, cef_string_list_t arguments) { DebugBreak(); }
    static void CEF_CALLBACK append_argument(struct _cef_command_line_t* self, const cef_string_t* argument) { DebugBreak(); }
    static void CEF_CALLBACK append_switch_with_value(struct _cef_command_line_t* self, const cef_string_t* name, const cef_string_t* value) { DebugBreak(); }
    static void CEF_CALLBACK prepend_wrapper(struct _cef_command_line_t* self, const cef_string_t* wrapper) { DebugBreak(); }
};

CommandLineImpl::CommandLineImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.is_valid = isValid;
    m_baseClass.is_read_only = CommandLineImplIsReadOnly;
    m_baseClass.copy = copy;
    m_baseClass.init_from_argv = initFromArgv;
    m_baseClass.init_from_string = initFromString;
    m_baseClass.reset = reset;
    m_baseClass.get_argv = getArgv;
    m_baseClass.get_command_line_string = getCommandLineString;
    m_baseClass.get_program = getProgram;
    m_baseClass.set_program = setProgram;
    m_baseClass.has_switches = hasSwitches;
    m_baseClass.has_switch = hasSwitch;
    m_baseClass.get_switch_value = getSwitchValue;
    m_baseClass.get_switches = get_switches;
    m_baseClass.append_switch = append_switch;
    m_baseClass.append_switch_with_value = append_switch_with_value;
    m_baseClass.has_arguments = has_arguments;
    m_baseClass.get_arguments = get_arguments;
    m_baseClass.append_argument = append_argument;
    m_baseClass.prepend_wrapper = prepend_wrapper;

    CefCppBase<cef_command_line_t, CommandLineImpl>::init(this);
}

} // cef

CEF_EXPORT cef_command_line_t* cef_command_line_create()
{
    cef::CommandLineImpl* ptr = new cef::CommandLineImpl();
    ptr->ref();

    return (cef_command_line_t*)(ptr);
}

///
// Returns the singleton global cef_command_line_t object. The returned object
// will be read-only.
///
CEF_EXPORT cef_command_line_t* cef_command_line_get_global()
{
    static cef::CommandLineImpl* cmdLine = nullptr;
    if (!cmdLine) {
        cmdLine = new cef::CommandLineImpl();
        cef_string_t commandLineStr = {0};
        String str(::GetCommandLineW());
        cmdLine->initFromWTFString(str);
    }

    cmdLine->ref();
    return (cef_command_line_t*)(cmdLine);
}