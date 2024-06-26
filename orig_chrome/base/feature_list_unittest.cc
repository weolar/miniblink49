// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/feature_list.h"

#include "base/format_macros.h"
#include "base/metrics/field_trial.h"
#include "base/strings/stringprintf.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

namespace {

    const char kFeatureOnByDefaultName[] = "OnByDefault";
    struct Feature kFeatureOnByDefault {
        kFeatureOnByDefaultName, FEATURE_ENABLED_BY_DEFAULT
    };

    const char kFeatureOffByDefaultName[] = "OffByDefault";
    struct Feature kFeatureOffByDefault {
        kFeatureOffByDefaultName, FEATURE_DISABLED_BY_DEFAULT
    };

} // namespace

class FeatureListTest : public testing::Test {
public:
    FeatureListTest()
        : feature_list_(nullptr)
    {
        RegisterFeatureListInstance(make_scoped_ptr(new FeatureList));
    }
    ~FeatureListTest() override { ClearFeatureListInstance(); }

    void RegisterFeatureListInstance(scoped_ptr<FeatureList> feature_list)
    {
        FeatureList::ClearInstanceForTesting();
        feature_list_ = feature_list.get();
        FeatureList::SetInstance(feature_list.Pass());
    }
    void ClearFeatureListInstance()
    {
        FeatureList::ClearInstanceForTesting();
        feature_list_ = nullptr;
    }

    FeatureList* feature_list() { return feature_list_; }

private:
    // Weak. Owned by the FeatureList::SetInstance().
    FeatureList* feature_list_;

    DISALLOW_COPY_AND_ASSIGN(FeatureListTest);
};

TEST_F(FeatureListTest, DefaultStates)
{
    EXPECT_TRUE(FeatureList::IsEnabled(kFeatureOnByDefault));
    EXPECT_FALSE(FeatureList::IsEnabled(kFeatureOffByDefault));
}

TEST_F(FeatureListTest, InitializeFromCommandLine)
{
    struct {
        const char* enable_features;
        const char* disable_features;
        bool expected_feature_on_state;
        bool expected_feature_off_state;
    } test_cases[] = {
        { "", "", true, false },
        { "OffByDefault", "", true, true },
        { "OffByDefault", "OnByDefault", false, true },
        { "OnByDefault,OffByDefault", "", true, true },
        { "", "OnByDefault,OffByDefault", false, false },
        // In the case an entry is both, disable takes precedence.
        { "OnByDefault", "OnByDefault,OffByDefault", false, false },
    };

    for (size_t i = 0; i < arraysize(test_cases); ++i) {
        const auto& test_case = test_cases[i];
        SCOPED_TRACE(base::StringPrintf("Test[%" PRIuS "]: [%s] [%s]", i,
            test_case.enable_features,
            test_case.disable_features));

        ClearFeatureListInstance();
        scoped_ptr<FeatureList> feature_list(new FeatureList);
        feature_list->InitializeFromCommandLine(test_case.enable_features,
            test_case.disable_features);
        RegisterFeatureListInstance(feature_list.Pass());

        EXPECT_EQ(test_case.expected_feature_on_state,
            FeatureList::IsEnabled(kFeatureOnByDefault))
            << i;
        EXPECT_EQ(test_case.expected_feature_off_state,
            FeatureList::IsEnabled(kFeatureOffByDefault))
            << i;
    }
}

TEST_F(FeatureListTest, CheckFeatureIdentity)
{
    // Tests that CheckFeatureIdentity() correctly detects when two different
    // structs with the same feature name are passed to it.

    // Call it twice for each feature at the top of the file, since the first call
    // makes it remember the entry and the second call will verify it.
    EXPECT_TRUE(feature_list()->CheckFeatureIdentity(kFeatureOnByDefault));
    EXPECT_TRUE(feature_list()->CheckFeatureIdentity(kFeatureOnByDefault));
    EXPECT_TRUE(feature_list()->CheckFeatureIdentity(kFeatureOffByDefault));
    EXPECT_TRUE(feature_list()->CheckFeatureIdentity(kFeatureOffByDefault));

    // Now, call it with a distinct struct for |kFeatureOnByDefaultName|, which
    // should return false.
    struct Feature kFeatureOnByDefault2 {
        kFeatureOnByDefaultName, FEATURE_ENABLED_BY_DEFAULT
    };
    EXPECT_FALSE(feature_list()->CheckFeatureIdentity(kFeatureOnByDefault2));
}

TEST_F(FeatureListTest, FieldTrialOverrides)
{
    struct {
        FeatureList::OverrideState trial1_state;
        FeatureList::OverrideState trial2_state;
    } test_cases[] = {
        { FeatureList::OVERRIDE_DISABLE_FEATURE,
            FeatureList::OVERRIDE_DISABLE_FEATURE },
        { FeatureList::OVERRIDE_DISABLE_FEATURE,
            FeatureList::OVERRIDE_ENABLE_FEATURE },
        { FeatureList::OVERRIDE_ENABLE_FEATURE,
            FeatureList::OVERRIDE_DISABLE_FEATURE },
        { FeatureList::OVERRIDE_ENABLE_FEATURE,
            FeatureList::OVERRIDE_ENABLE_FEATURE },
    };

    FieldTrial::ActiveGroup active_group;
    for (size_t i = 0; i < arraysize(test_cases); ++i) {
        const auto& test_case = test_cases[i];
        SCOPED_TRACE(base::StringPrintf("Test[%" PRIuS "]", i));

        ClearFeatureListInstance();

        FieldTrialList field_trial_list(nullptr);
        scoped_ptr<FeatureList> feature_list(new FeatureList);

        FieldTrial* trial1 = FieldTrialList::CreateFieldTrial("TrialExample1", "A");
        FieldTrial* trial2 = FieldTrialList::CreateFieldTrial("TrialExample2", "B");
        feature_list->RegisterFieldTrialOverride(kFeatureOnByDefaultName,
            test_case.trial1_state, trial1);
        feature_list->RegisterFieldTrialOverride(kFeatureOffByDefaultName,
            test_case.trial2_state, trial2);
        RegisterFeatureListInstance(feature_list.Pass());

        // Initially, neither trial should be active.
        EXPECT_FALSE(FieldTrialList::IsTrialActive(trial1->trial_name()));
        EXPECT_FALSE(FieldTrialList::IsTrialActive(trial2->trial_name()));

        const bool expected_enabled_1 = (test_case.trial1_state == FeatureList::OVERRIDE_ENABLE_FEATURE);
        EXPECT_EQ(expected_enabled_1, FeatureList::IsEnabled(kFeatureOnByDefault));
        // The above should have activated |trial1|.
        EXPECT_TRUE(FieldTrialList::IsTrialActive(trial1->trial_name()));
        EXPECT_FALSE(FieldTrialList::IsTrialActive(trial2->trial_name()));

        const bool expected_enabled_2 = (test_case.trial2_state == FeatureList::OVERRIDE_ENABLE_FEATURE);
        EXPECT_EQ(expected_enabled_2, FeatureList::IsEnabled(kFeatureOffByDefault));
        // The above should have activated |trial2|.
        EXPECT_TRUE(FieldTrialList::IsTrialActive(trial1->trial_name()));
        EXPECT_TRUE(FieldTrialList::IsTrialActive(trial2->trial_name()));
    }
}

TEST_F(FeatureListTest, CommandLineTakesPrecedenceOverFieldTrial)
{
    ClearFeatureListInstance();

    FieldTrialList field_trial_list(nullptr);
    scoped_ptr<FeatureList> feature_list(new FeatureList);

    // The feature is explicitly enabled on the command-line.
    feature_list->InitializeFromCommandLine(kFeatureOffByDefaultName, "");

    // But the FieldTrial would set the feature to disabled.
    FieldTrial* trial = FieldTrialList::CreateFieldTrial("TrialExample2", "A");
    feature_list->RegisterFieldTrialOverride(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE, trial);
    RegisterFeatureListInstance(feature_list.Pass());

    EXPECT_FALSE(FieldTrialList::IsTrialActive(trial->trial_name()));
    // Command-line should take precedence.
    EXPECT_TRUE(FeatureList::IsEnabled(kFeatureOffByDefault));
    // Since the feature is on due to the command-line, and not as a result of the
    // field trial, the field trial should not be activated (since the Associate*
    // API wasn't used.)
    EXPECT_FALSE(FieldTrialList::IsTrialActive(trial->trial_name()));
}

TEST_F(FeatureListTest, IsFeatureOverriddenFromCommandLine)
{
    ClearFeatureListInstance();

    FieldTrialList field_trial_list(nullptr);
    scoped_ptr<FeatureList> feature_list(new FeatureList);

    // No features are overridden from the command line yet
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOnByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOnByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE));

    // Now, enable |kFeatureOffByDefaultName| via the command-line.
    feature_list->InitializeFromCommandLine(kFeatureOffByDefaultName, "");

    // It should now be overridden for the enabled group.
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE));
    EXPECT_TRUE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE));

    // Register a field trial to associate with the feature and ensure that the
    // results are still the same.
    feature_list->AssociateReportingFieldTrial(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE,
        FieldTrialList::CreateFieldTrial("Trial1", "A"));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE));
    EXPECT_TRUE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE));

    // Now, register a field trial to override |kFeatureOnByDefaultName| state
    // and check that the function still returns false for that feature.
    feature_list->RegisterFieldTrialOverride(
        kFeatureOnByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE,
        FieldTrialList::CreateFieldTrial("Trial2", "A"));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOnByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE));
    EXPECT_FALSE(feature_list->IsFeatureOverriddenFromCommandLine(
        kFeatureOnByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE));
    RegisterFeatureListInstance(feature_list.Pass());

    // Check the expected feature states for good measure.
    EXPECT_TRUE(FeatureList::IsEnabled(kFeatureOffByDefault));
    EXPECT_FALSE(FeatureList::IsEnabled(kFeatureOnByDefault));
}

TEST_F(FeatureListTest, AssociateReportingFieldTrial)
{
    struct {
        const char* enable_features;
        const char* disable_features;
        bool expected_enable_trial_created;
        bool expected_disable_trial_created;
    } test_cases[] = {
        // If no enable/disable flags are specified, no trials should be created.
        { "", "", false, false },
        // Enabling the feature should result in the enable trial created.
        { kFeatureOffByDefaultName, "", true, false },
        // Disabling the feature should result in the disable trial created.
        { "", kFeatureOffByDefaultName, false, true },
    };

    const char kTrialName[] = "ForcingTrial";
    const char kForcedOnGroupName[] = "ForcedOn";
    const char kForcedOffGroupName[] = "ForcedOff";

    for (size_t i = 0; i < arraysize(test_cases); ++i) {
        const auto& test_case = test_cases[i];
        SCOPED_TRACE(base::StringPrintf("Test[%" PRIuS "]: [%s] [%s]", i,
            test_case.enable_features,
            test_case.disable_features));

        ClearFeatureListInstance();

        FieldTrialList field_trial_list(nullptr);
        scoped_ptr<FeatureList> feature_list(new FeatureList);
        feature_list->InitializeFromCommandLine(test_case.enable_features,
            test_case.disable_features);

        FieldTrial* enable_trial = nullptr;
        if (feature_list->IsFeatureOverriddenFromCommandLine(
                kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE)) {
            enable_trial = base::FieldTrialList::CreateFieldTrial(kTrialName,
                kForcedOnGroupName);
            feature_list->AssociateReportingFieldTrial(
                kFeatureOffByDefaultName, FeatureList::OVERRIDE_ENABLE_FEATURE,
                enable_trial);
        }
        FieldTrial* disable_trial = nullptr;
        if (feature_list->IsFeatureOverriddenFromCommandLine(
                kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE)) {
            disable_trial = base::FieldTrialList::CreateFieldTrial(
                kTrialName, kForcedOffGroupName);
            feature_list->AssociateReportingFieldTrial(
                kFeatureOffByDefaultName, FeatureList::OVERRIDE_DISABLE_FEATURE,
                disable_trial);
        }
        EXPECT_EQ(test_case.expected_enable_trial_created, enable_trial != nullptr);
        EXPECT_EQ(test_case.expected_disable_trial_created,
            disable_trial != nullptr);
        RegisterFeatureListInstance(feature_list.Pass());

        EXPECT_FALSE(FieldTrialList::IsTrialActive(kTrialName));
        if (disable_trial) {
            EXPECT_FALSE(FeatureList::IsEnabled(kFeatureOffByDefault));
            EXPECT_TRUE(FieldTrialList::IsTrialActive(kTrialName));
            EXPECT_EQ(kForcedOffGroupName, disable_trial->group_name());
        } else if (enable_trial) {
            EXPECT_TRUE(FeatureList::IsEnabled(kFeatureOffByDefault));
            EXPECT_TRUE(FieldTrialList::IsTrialActive(kTrialName));
            EXPECT_EQ(kForcedOnGroupName, enable_trial->group_name());
        }
    }
}

} // namespace base
