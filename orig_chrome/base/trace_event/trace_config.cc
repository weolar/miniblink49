// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/trace_config.h"

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/pattern.h"
#include "base/strings/string_split.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/memory_dump_request_args.h"
#include "base/trace_event/trace_event.h"

namespace base {
namespace trace_event {

    namespace {

        // String options that can be used to initialize TraceOptions.
        const char kRecordUntilFull[] = "record-until-full";
        const char kRecordContinuously[] = "record-continuously";
        const char kRecordAsMuchAsPossible[] = "record-as-much-as-possible";
        const char kTraceToConsole[] = "trace-to-console";
        const char kEnableSampling[] = "enable-sampling";
        const char kEnableSystrace[] = "enable-systrace";
        const char kEnableArgumentFilter[] = "enable-argument-filter";

        // String parameters that can be used to parse the trace config string.
        const char kRecordModeParam[] = "record_mode";
        const char kEnableSamplingParam[] = "enable_sampling";
        const char kEnableSystraceParam[] = "enable_systrace";
        const char kEnableArgumentFilterParam[] = "enable_argument_filter";
        const char kIncludedCategoriesParam[] = "included_categories";
        const char kExcludedCategoriesParam[] = "excluded_categories";
        const char kSyntheticDelaysParam[] = "synthetic_delays";

        const char kSyntheticDelayCategoryFilterPrefix[] = "DELAY(";

        // String parameters that is used to parse memory dump config in trace config
        // string.
        const char kMemoryDumpConfigParam[] = "memory_dump_config";
        const char kTriggersParam[] = "triggers";
        const char kPeriodicIntervalParam[] = "periodic_interval_ms";
        const char kModeParam[] = "mode";

        // Default configuration of memory dumps.
        const TraceConfig::MemoryDumpTriggerConfig kDefaultHeavyMemoryDumpTrigger = {
            2000, // periodic_interval_ms
            MemoryDumpLevelOfDetail::DETAILED
        };
        const TraceConfig::MemoryDumpTriggerConfig kDefaultLightMemoryDumpTrigger = {
            250, // periodic_interval_ms
            MemoryDumpLevelOfDetail::LIGHT
        };

        class ConvertableTraceConfigToTraceFormat
            : public base::trace_event::ConvertableToTraceFormat {
        public:
            explicit ConvertableTraceConfigToTraceFormat(const TraceConfig& trace_config)
                : trace_config_(trace_config)
            {
            }
            void AppendAsTraceFormat(std::string* out) const override
            {
                out->append(trace_config_.ToString());
            }

        protected:
            ~ConvertableTraceConfigToTraceFormat() override { }

        private:
            const TraceConfig trace_config_;
        };

    } // namespace

    TraceConfig::TraceConfig()
    {
        InitializeDefault();
    }

    TraceConfig::TraceConfig(const std::string& category_filter_string,
        const std::string& trace_options_string)
    {
        InitializeFromStrings(category_filter_string, trace_options_string);
    }

    TraceConfig::TraceConfig(const std::string& category_filter_string,
        TraceRecordMode record_mode)
    {
        std::string trace_options_string;
        switch (record_mode) {
        case RECORD_UNTIL_FULL:
            trace_options_string = kRecordUntilFull;
            break;
        case RECORD_CONTINUOUSLY:
            trace_options_string = kRecordContinuously;
            break;
        case RECORD_AS_MUCH_AS_POSSIBLE:
            trace_options_string = kRecordAsMuchAsPossible;
            break;
        case ECHO_TO_CONSOLE:
            trace_options_string = kTraceToConsole;
            break;
        default:
            NOTREACHED();
        }
        InitializeFromStrings(category_filter_string, trace_options_string);
    }

    TraceConfig::TraceConfig(const std::string& config_string)
    {
        if (!config_string.empty())
            InitializeFromConfigString(config_string);
        else
            InitializeDefault();
    }

    TraceConfig::TraceConfig(const TraceConfig& tc)
        : record_mode_(tc.record_mode_)
        , enable_sampling_(tc.enable_sampling_)
        , enable_systrace_(tc.enable_systrace_)
        , enable_argument_filter_(tc.enable_argument_filter_)
        , memory_dump_config_(tc.memory_dump_config_)
        , included_categories_(tc.included_categories_)
        , disabled_categories_(tc.disabled_categories_)
        , excluded_categories_(tc.excluded_categories_)
        , synthetic_delays_(tc.synthetic_delays_)
    {
    }

    TraceConfig::~TraceConfig()
    {
    }

    TraceConfig& TraceConfig::operator=(const TraceConfig& rhs)
    {
        if (this == &rhs)
            return *this;

        record_mode_ = rhs.record_mode_;
        enable_sampling_ = rhs.enable_sampling_;
        enable_systrace_ = rhs.enable_systrace_;
        enable_argument_filter_ = rhs.enable_argument_filter_;
        memory_dump_config_ = rhs.memory_dump_config_;
        included_categories_ = rhs.included_categories_;
        disabled_categories_ = rhs.disabled_categories_;
        excluded_categories_ = rhs.excluded_categories_;
        synthetic_delays_ = rhs.synthetic_delays_;
        return *this;
    }

    const TraceConfig::StringList& TraceConfig::GetSyntheticDelayValues() const
    {
        return synthetic_delays_;
    }

    std::string TraceConfig::ToString() const
    {
        base::DictionaryValue dict;
        ToDict(dict);

        std::string json;
        base::JSONWriter::Write(dict, &json);

        return json;
    }

    scoped_refptr<ConvertableToTraceFormat>
    TraceConfig::AsConvertableToTraceFormat() const
    {
        return new ConvertableTraceConfigToTraceFormat(*this);
    }

    std::string TraceConfig::ToCategoryFilterString() const
    {
        std::string filter_string;
        WriteCategoryFilterString(included_categories_, &filter_string, true);
        WriteCategoryFilterString(disabled_categories_, &filter_string, true);
        WriteCategoryFilterString(excluded_categories_, &filter_string, false);
        WriteCategoryFilterString(synthetic_delays_, &filter_string);
        return filter_string;
    }

    bool TraceConfig::IsCategoryGroupEnabled(
        const char* category_group_name) const
    {
        // TraceLog should call this method only as part of enabling/disabling
        // categories.

        bool had_enabled_by_default = false;
        DCHECK(category_group_name);
        CStringTokenizer category_group_tokens(
            category_group_name, category_group_name + strlen(category_group_name),
            ",");
        while (category_group_tokens.GetNext()) {
            std::string category_group_token = category_group_tokens.token();
            // Don't allow empty tokens, nor tokens with leading or trailing space.
            DCHECK(!TraceConfig::IsEmptyOrContainsLeadingOrTrailingWhitespace(
                category_group_token))
                << "Disallowed category string";
            if (IsCategoryEnabled(category_group_token.c_str())) {
                return true;
            }
            //     if (!base::MatchPattern(category_group_token.c_str(),
            //                             TRACE_DISABLED_BY_DEFAULT("*")))
            //       had_enabled_by_default = true;
            DebugBreak();
        }
        // Do a second pass to check for explicitly disabled categories
        // (those explicitly enabled have priority due to first pass).
        category_group_tokens.Reset();
        bool category_group_disabled = false;
        while (category_group_tokens.GetNext()) {
            std::string category_group_token = category_group_tokens.token();
            for (StringList::const_iterator ci = excluded_categories_.begin();
                 ci != excluded_categories_.end();
                 ++ci) {
                if (base::MatchPattern(category_group_token.c_str(), ci->c_str())) {
                    // Current token of category_group_name is present in excluded_list.
                    // Flag the exclusion and proceed further to check if any of the
                    // remaining categories of category_group_name is not present in the
                    // excluded_ list.
                    category_group_disabled = true;
                    break;
                }
                // One of the category of category_group_name is not present in
                // excluded_ list. So, it has to be included_ list. Enable the
                // category_group_name for recording.
                category_group_disabled = false;
            }
            // One of the categories present in category_group_name is not present in
            // excluded_ list. Implies this category_group_name group can be enabled
            // for recording, since one of its groups is enabled for recording.
            if (!category_group_disabled)
                break;
        }
        // If the category group is not excluded, and there are no included patterns
        // we consider this category group enabled, as long as it had categories
        // other than disabled-by-default.
        return !category_group_disabled && included_categories_.empty() && had_enabled_by_default;
    }

    void TraceConfig::Merge(const TraceConfig& config)
    {
        if (record_mode_ != config.record_mode_
            || enable_sampling_ != config.enable_sampling_
            || enable_systrace_ != config.enable_systrace_
            || enable_argument_filter_ != config.enable_argument_filter_) {
            DLOG(ERROR) << "Attempting to merge trace config with a different "
                        << "set of options.";
        }

        // Keep included patterns only if both filters have an included entry.
        // Otherwise, one of the filter was specifying "*" and we want to honor the
        // broadest filter.
        if (HasIncludedPatterns() && config.HasIncludedPatterns()) {
            included_categories_.insert(included_categories_.end(),
                config.included_categories_.begin(),
                config.included_categories_.end());
        } else {
            included_categories_.clear();
        }

        memory_dump_config_.insert(memory_dump_config_.end(),
            config.memory_dump_config_.begin(),
            config.memory_dump_config_.end());

        disabled_categories_.insert(disabled_categories_.end(),
            config.disabled_categories_.begin(),
            config.disabled_categories_.end());
        excluded_categories_.insert(excluded_categories_.end(),
            config.excluded_categories_.begin(),
            config.excluded_categories_.end());
        synthetic_delays_.insert(synthetic_delays_.end(),
            config.synthetic_delays_.begin(),
            config.synthetic_delays_.end());
    }

    void TraceConfig::Clear()
    {
        record_mode_ = RECORD_UNTIL_FULL;
        enable_sampling_ = false;
        enable_systrace_ = false;
        enable_argument_filter_ = false;
        included_categories_.clear();
        disabled_categories_.clear();
        excluded_categories_.clear();
        synthetic_delays_.clear();
        memory_dump_config_.clear();
    }

    void TraceConfig::InitializeDefault()
    {
        record_mode_ = RECORD_UNTIL_FULL;
        enable_sampling_ = false;
        enable_systrace_ = false;
        enable_argument_filter_ = false;
        excluded_categories_.push_back("*Debug");
        excluded_categories_.push_back("*Test");
    }

    void TraceConfig::InitializeFromConfigString(const std::string& config_string)
    {
        //   scoped_ptr<base::Value> value(base::JSONReader::Read(config_string));
        //   if (!value || !value->IsType(base::Value::TYPE_DICTIONARY)) {
        //     InitializeDefault();
        //     return;
        //   }
        //   scoped_ptr<base::DictionaryValue> dict(
        //         static_cast<base::DictionaryValue*>(value.release()));
        //
        //   record_mode_ = RECORD_UNTIL_FULL;
        //   std::string record_mode;
        //   if (dict->GetString(kRecordModeParam, &record_mode)) {
        //     if (record_mode == kRecordUntilFull) {
        //       record_mode_ = RECORD_UNTIL_FULL;
        //     } else if (record_mode == kRecordContinuously) {
        //       record_mode_ = RECORD_CONTINUOUSLY;
        //     } else if (record_mode == kTraceToConsole) {
        //       record_mode_ = ECHO_TO_CONSOLE;
        //     } else if (record_mode == kRecordAsMuchAsPossible) {
        //       record_mode_ = RECORD_AS_MUCH_AS_POSSIBLE;
        //     }
        //   }
        //
        //   bool enable_sampling;
        //   if (!dict->GetBoolean(kEnableSamplingParam, &enable_sampling))
        //     enable_sampling_ = false;
        //   else
        //     enable_sampling_ = enable_sampling;
        //
        //   bool enable_systrace;
        //   if (!dict->GetBoolean(kEnableSystraceParam, &enable_systrace))
        //     enable_systrace_ = false;
        //   else
        //     enable_systrace_ = enable_systrace;
        //
        //   bool enable_argument_filter;
        //   if (!dict->GetBoolean(kEnableArgumentFilterParam, &enable_argument_filter))
        //     enable_argument_filter_ = false;
        //   else
        //     enable_argument_filter_ = enable_argument_filter;
        //
        //   base::ListValue* category_list = nullptr;
        //   if (dict->GetList(kIncludedCategoriesParam, &category_list))
        //     SetCategoriesFromIncludedList(*category_list);
        //   if (dict->GetList(kExcludedCategoriesParam, &category_list))
        //     SetCategoriesFromExcludedList(*category_list);
        //   if (dict->GetList(kSyntheticDelaysParam, &category_list))
        //     SetSyntheticDelaysFromList(*category_list);
        //
        //   if (IsCategoryEnabled(MemoryDumpManager::kTraceCategory)) {
        //     // If dump triggers not set, the client is using the legacy with just
        //     // category enabled. So, use the default periodic dump config.
        //     base::DictionaryValue* memory_dump_config = nullptr;
        //     if (dict->GetDictionary(kMemoryDumpConfigParam, &memory_dump_config))
        //       SetMemoryDumpConfig(*memory_dump_config);
        //     else
        //       SetDefaultMemoryDumpConfig();
        //   }
        DebugBreak();
    }

    void TraceConfig::InitializeFromStrings(
        const std::string& category_filter_string,
        const std::string& trace_options_string)
    {
        if (!category_filter_string.empty()) {
            std::vector<std::string> split = base::SplitString(
                category_filter_string, ",", base::TRIM_WHITESPACE,
                base::SPLIT_WANT_ALL);
            std::vector<std::string>::iterator iter;
            for (iter = split.begin(); iter != split.end(); ++iter) {
                std::string category = *iter;
                // Ignore empty categories.
                if (category.empty())
                    continue;
                // Synthetic delays are of the form 'DELAY(delay;option;option;...)'.
                if (category.find(kSyntheticDelayCategoryFilterPrefix) == 0 && category.at(category.size() - 1) == ')') {
                    category = category.substr(
                        strlen(kSyntheticDelayCategoryFilterPrefix),
                        category.size() - strlen(kSyntheticDelayCategoryFilterPrefix) - 1);
                    size_t name_length = category.find(';');
                    if (name_length != std::string::npos && name_length > 0 && name_length != category.size() - 1) {
                        synthetic_delays_.push_back(category);
                    }
                } else if (category.at(0) == '-') {
                    // Excluded categories start with '-'.
                    // Remove '-' from category string.
                    category = category.substr(1);
                    excluded_categories_.push_back(category);
                } else if (category.compare(0, strlen(TRACE_DISABLED_BY_DEFAULT("")),
                               TRACE_DISABLED_BY_DEFAULT(""))
                    == 0) {
                    disabled_categories_.push_back(category);
                } else {
                    included_categories_.push_back(category);
                }
            }
        }

        record_mode_ = RECORD_UNTIL_FULL;
        enable_sampling_ = false;
        enable_systrace_ = false;
        enable_argument_filter_ = false;
        if (!trace_options_string.empty()) {
            std::vector<std::string> split = base::SplitString(
                trace_options_string, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
            std::vector<std::string>::iterator iter;
            for (iter = split.begin(); iter != split.end(); ++iter) {
                if (*iter == kRecordUntilFull) {
                    record_mode_ = RECORD_UNTIL_FULL;
                } else if (*iter == kRecordContinuously) {
                    record_mode_ = RECORD_CONTINUOUSLY;
                } else if (*iter == kTraceToConsole) {
                    record_mode_ = ECHO_TO_CONSOLE;
                } else if (*iter == kRecordAsMuchAsPossible) {
                    record_mode_ = RECORD_AS_MUCH_AS_POSSIBLE;
                } else if (*iter == kEnableSampling) {
                    enable_sampling_ = true;
                } else if (*iter == kEnableSystrace) {
                    enable_systrace_ = true;
                } else if (*iter == kEnableArgumentFilter) {
                    enable_argument_filter_ = true;
                }
            }
        }

        if (IsCategoryEnabled(MemoryDumpManager::kTraceCategory)) {
            SetDefaultMemoryDumpConfig();
        }
    }

    void TraceConfig::SetCategoriesFromIncludedList(
        const base::ListValue& included_list)
    {
        included_categories_.clear();
        for (size_t i = 0; i < included_list.GetSize(); ++i) {
            std::string category;
            if (!included_list.GetString(i, &category))
                continue;
            if (category.compare(0, strlen(TRACE_DISABLED_BY_DEFAULT("")),
                    TRACE_DISABLED_BY_DEFAULT(""))
                == 0) {
                disabled_categories_.push_back(category);
            } else {
                included_categories_.push_back(category);
            }
        }
    }

    void TraceConfig::SetCategoriesFromExcludedList(
        const base::ListValue& excluded_list)
    {
        excluded_categories_.clear();
        for (size_t i = 0; i < excluded_list.GetSize(); ++i) {
            std::string category;
            if (excluded_list.GetString(i, &category))
                excluded_categories_.push_back(category);
        }
    }

    void TraceConfig::SetSyntheticDelaysFromList(const base::ListValue& list)
    {
        synthetic_delays_.clear();
        for (size_t i = 0; i < list.GetSize(); ++i) {
            std::string delay;
            if (!list.GetString(i, &delay))
                continue;
            // Synthetic delays are of the form "delay;option;option;...".
            size_t name_length = delay.find(';');
            if (name_length != std::string::npos && name_length > 0 && name_length != delay.size() - 1) {
                synthetic_delays_.push_back(delay);
            }
        }
    }

    void TraceConfig::AddCategoryToDict(base::DictionaryValue& dict,
        const char* param,
        const StringList& categories) const
    {
        //   if (categories.empty())
        //     return;
        //
        //   scoped_ptr<base::ListValue> list(new base::ListValue());
        //   for (StringList::const_iterator ci = categories.begin();
        //        ci != categories.end();
        //        ++ci) {
        //     list->AppendString(*ci);
        //   }
        //
        //   dict.Set(param, list.Pass());
        DebugBreak();
    }

    void TraceConfig::SetMemoryDumpConfig(
        const base::DictionaryValue& memory_dump_config)
    {
        memory_dump_config_.clear();

        const base::ListValue* trigger_list = nullptr;
        if (!memory_dump_config.GetList(kTriggersParam, &trigger_list) || trigger_list->GetSize() == 0) {
            return;
        }

        for (size_t i = 0; i < trigger_list->GetSize(); ++i) {
            const base::DictionaryValue* trigger = nullptr;
            if (!trigger_list->GetDictionary(i, &trigger))
                continue;

            MemoryDumpTriggerConfig dump_config;
            int interval = 0;

            if (!trigger->GetInteger(kPeriodicIntervalParam, &interval)) {
                continue;
            }
            DCHECK_GT(interval, 0);
            dump_config.periodic_interval_ms = static_cast<uint32>(interval);
            std::string level_of_detail_str;
            trigger->GetString(kModeParam, &level_of_detail_str);
            dump_config.level_of_detail = StringToMemoryDumpLevelOfDetail(level_of_detail_str);
            memory_dump_config_.push_back(dump_config);
        }
    }

    void TraceConfig::SetDefaultMemoryDumpConfig()
    {
        memory_dump_config_.clear();
        memory_dump_config_.push_back(kDefaultHeavyMemoryDumpTrigger);
        memory_dump_config_.push_back(kDefaultLightMemoryDumpTrigger);
    }

    void TraceConfig::ToDict(base::DictionaryValue& dict) const
    {
        //   switch (record_mode_) {
        //     case RECORD_UNTIL_FULL:
        //       dict.SetString(kRecordModeParam, kRecordUntilFull);
        //       break;
        //     case RECORD_CONTINUOUSLY:
        //       dict.SetString(kRecordModeParam, kRecordContinuously);
        //       break;
        //     case RECORD_AS_MUCH_AS_POSSIBLE:
        //       dict.SetString(kRecordModeParam, kRecordAsMuchAsPossible);
        //       break;
        //     case ECHO_TO_CONSOLE:
        //       dict.SetString(kRecordModeParam, kTraceToConsole);
        //       break;
        //     default:
        //       NOTREACHED();
        //   }
        //
        //   if (enable_sampling_)
        //     dict.SetBoolean(kEnableSamplingParam, true);
        //   else
        //     dict.SetBoolean(kEnableSamplingParam, false);
        //
        //   if (enable_systrace_)
        //     dict.SetBoolean(kEnableSystraceParam, true);
        //   else
        //     dict.SetBoolean(kEnableSystraceParam, false);
        //
        //   if (enable_argument_filter_)
        //     dict.SetBoolean(kEnableArgumentFilterParam, true);
        //   else
        //     dict.SetBoolean(kEnableArgumentFilterParam, false);
        //
        //   StringList categories(included_categories_);
        //   categories.insert(categories.end(),
        //                     disabled_categories_.begin(),
        //                     disabled_categories_.end());
        //   AddCategoryToDict(dict, kIncludedCategoriesParam, categories);
        //   AddCategoryToDict(dict, kExcludedCategoriesParam, excluded_categories_);
        //   AddCategoryToDict(dict, kSyntheticDelaysParam, synthetic_delays_);
        //
        //   if (IsCategoryEnabled(MemoryDumpManager::kTraceCategory)) {
        //     scoped_ptr<base::DictionaryValue> memory_dump_config(
        //         new base::DictionaryValue());
        //     scoped_ptr<base::ListValue> triggers_list(new base::ListValue());
        //     for (const MemoryDumpTriggerConfig& config : memory_dump_config_) {
        //       scoped_ptr<base::DictionaryValue> trigger_dict(
        //           new base::DictionaryValue());
        //       trigger_dict->SetInteger(kPeriodicIntervalParam,
        //                                static_cast<int>(config.periodic_interval_ms));
        //       trigger_dict->SetString(
        //           kModeParam, MemoryDumpLevelOfDetailToString(config.level_of_detail));
        //       triggers_list->Append(trigger_dict.Pass());
        //     }
        //
        //     // Empty triggers will still be specified explicitly since it means that
        //     // the periodic dumps are not enabled.
        //     memory_dump_config->Set(kTriggersParam, triggers_list.Pass());
        //     dict.Set(kMemoryDumpConfigParam, memory_dump_config.Pass());
        //   }
        DebugBreak();
    }

    std::string TraceConfig::ToTraceOptionsString() const
    {
        std::string ret;
        switch (record_mode_) {
        case RECORD_UNTIL_FULL:
            ret = kRecordUntilFull;
            break;
        case RECORD_CONTINUOUSLY:
            ret = kRecordContinuously;
            break;
        case RECORD_AS_MUCH_AS_POSSIBLE:
            ret = kRecordAsMuchAsPossible;
            break;
        case ECHO_TO_CONSOLE:
            ret = kTraceToConsole;
            break;
        default:
            NOTREACHED();
        }
        if (enable_sampling_)
            ret = ret + "," + kEnableSampling;
        if (enable_systrace_)
            ret = ret + "," + kEnableSystrace;
        if (enable_argument_filter_)
            ret = ret + "," + kEnableArgumentFilter;
        return ret;
    }

    void TraceConfig::WriteCategoryFilterString(const StringList& values,
        std::string* out,
        bool included) const
    {
        bool prepend_comma = !out->empty();
        int token_cnt = 0;
        for (StringList::const_iterator ci = values.begin();
             ci != values.end(); ++ci) {
            if (token_cnt > 0 || prepend_comma)
                StringAppendF(out, ",");
            StringAppendF(out, "%s%s", (included ? "" : "-"), ci->c_str());
            ++token_cnt;
        }
    }

    void TraceConfig::WriteCategoryFilterString(const StringList& delays,
        std::string* out) const
    {
        bool prepend_comma = !out->empty();
        int token_cnt = 0;
        for (StringList::const_iterator ci = delays.begin();
             ci != delays.end(); ++ci) {
            if (token_cnt > 0 || prepend_comma)
                StringAppendF(out, ",");
            StringAppendF(out, "%s%s)", kSyntheticDelayCategoryFilterPrefix,
                ci->c_str());
            ++token_cnt;
        }
    }

    bool TraceConfig::IsCategoryEnabled(const char* category_name) const
    {
        StringList::const_iterator ci;

        // Check the disabled- filters and the disabled-* wildcard first so that a
        // "*" filter does not include the disabled.
        for (ci = disabled_categories_.begin();
             ci != disabled_categories_.end();
             ++ci) {
            if (base::MatchPattern(category_name, ci->c_str()))
                return true;
        }

        if (base::MatchPattern(category_name, TRACE_DISABLED_BY_DEFAULT("*")))
            return false;

        for (ci = included_categories_.begin();
             ci != included_categories_.end();
             ++ci) {
            if (base::MatchPattern(category_name, ci->c_str()))
                return true;
        }

        return false;
    }

    bool TraceConfig::IsEmptyOrContainsLeadingOrTrailingWhitespace(
        const std::string& str)
    {
        return str.empty() || str.at(0) == ' ' || str.at(str.length() - 1) == ' ';
    }

    bool TraceConfig::HasIncludedPatterns() const
    {
        return !included_categories_.empty();
    }

} // namespace trace_event
} // namespace base
