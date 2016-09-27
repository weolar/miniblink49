#
# Copyright (C) 2013 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

{
  'includes': [
    'core.gypi',
    '../bindings/scripts/scripts.gypi',
    '../build/features.gypi',
    '../build/scripts/scripts.gypi',
  ],

  'targets': [
    {
      # GN version: //third_party/WebKit/Source/core:core_event_interfaces
      'target_name': 'core_event_interfaces',
      'type': 'none',
      'actions': [
        {
          'action_name': 'make_core_event_interfaces',
          'variables': {
            'event_idl_files': [
              '<@(core_event_idl_files)',
            ],
            'event_idl_files_list':
                '<|(event_idl_files_list.tmp <@(event_idl_files))',
          },
          'inputs': [
            # FIXME: should be in build/scripts, not bindings/scripts
            '../bindings/scripts/generate_event_interfaces.py',
            '../bindings/scripts/utilities.py',
            '<(event_idl_files_list)',
            '<@(event_idl_files)',
          ],
          'outputs': [
            '<(blink_core_output_dir)/EventInterfaces.in',
          ],
          'action': [
            'python',
            '../bindings/scripts/generate_event_interfaces.py',
            '--event-idl-files-list',
            '<(event_idl_files_list)',
            '--event-interfaces-file',
            '<(blink_core_output_dir)/EventInterfaces.in',
            '--write-file-only-if-changed',
            '<(write_file_only_if_changed)',
          ],
        },
      ],
    },
    {
      # GN version: //third_party/WebKit/Sources/core:generated_testing_idls
      'target_name': 'generated_testing_idls',
      'type': 'none',
      'actions': [
        {
          'action_name': 'Settings',
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_settings.py',
            '../build/scripts/templates/SettingsMacros.h.tmpl',
            'frame/Settings.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/SettingsMacros.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_settings.py',
            'frame/Settings.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'InternalSettings',
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_internal_settings.py',
            '../build/scripts/templates/InternalSettingsGenerated.idl.tmpl',
            '../build/scripts/templates/InternalSettingsGenerated.cpp.tmpl',
            '../build/scripts/templates/InternalSettingsGenerated.h.tmpl',
            'frame/Settings.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/testing/InternalSettingsGenerated.idl',
            '<(blink_core_output_dir)/testing/InternalSettingsGenerated.cpp',
            '<(blink_core_output_dir)/testing/InternalSettingsGenerated.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_internal_settings.py',
            'frame/Settings.in',
            '--output_dir',
            '<(blink_core_output_dir)/testing',
          ],
        },
        {
          'action_name': 'InternalRuntimeFlags',
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_internal_runtime_flags.py',
            '../platform/RuntimeEnabledFeatures.in',
            '../build/scripts/templates/InternalRuntimeFlags.h.tmpl',
            '../build/scripts/templates/InternalRuntimeFlags.idl.tmpl',
          ],
          'outputs': [
            '<(blink_core_output_dir)/testing/InternalRuntimeFlags.idl',
            '<(blink_core_output_dir)/testing/InternalRuntimeFlags.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_internal_runtime_flags.py',
            '../platform/RuntimeEnabledFeatures.in',
            '--output_dir',
            '<(blink_core_output_dir)/testing',
          ],
        },
      ]
    },
    {
      # GN version: //third_party/WebKit/Source/core:make_core_generated
      'target_name': 'make_core_generated',
      'type': 'none',
      'hard_dependency': 1,
      'dependencies': [
        'generated_testing_idls',
        'core_event_interfaces',
        '../config.gyp:config',
      ],
      'sources': [
        # bison rule
        'xml/XPathGrammar.y',
      ],
      'actions': [
        {
          'action_name': 'generatePrivateScript',
          # FIXME: The implementation of Blink-in-JS is not yet mature.
          # You can use Blink-in-JS in your local experiment, but don't ship it.
          # crbug.com/341031
          'private_script_files': [
             '../bindings/core/v8/PrivateScriptRunner.js',
             'html/HTMLMarqueeElement.js',
             'html/shadow/PluginPlaceholderElement.js',
             'xml/DocumentXMLTreeViewer.js',
          ],
          'inputs': [
             '../build/scripts/make_private_script_source.py',
             '<@(_private_script_files)',
          ],
          'outputs': [
            '<(blink_core_output_dir)/PrivateScriptSources.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_private_script_source.py',
            '<@(_outputs)',
            '<@(_private_script_files)'
          ],
        },
        {
          'action_name': 'generatePrivateScriptForTesting',
          'private_script_files': [
            'testing/PartialPrivateScriptTest.js',
            'testing/PrivateScriptTest.js',
           ],
          'inputs': [
             '../build/scripts/make_private_script_source.py',
             '<@(_private_script_files)',
           ],
           'outputs': [
             '<(blink_core_output_dir)/PrivateScriptSourcesForTesting.h',
           ],
           'action': [
             'python',
             '../build/scripts/make_private_script_source.py',
             '--for-testing',
             '<@(_outputs)',
             '<@(_private_script_files)'
           ],
        },
        {
          'action_name': 'HTMLEntityTable',
          'inputs': [
            'html/parser/create-html-entity-table',
            'html/parser/HTMLEntityNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLEntityTable.cpp'
          ],
          'action': [
            'python',
            'html/parser/create-html-entity-table',
            '-o',
            '<@(_outputs)',
            '<@(_inputs)'
          ],
        },
        {
          'action_name': 'CSSPropertyNames',
          'inputs': [
            '<@(css_properties_files)',
            '../build/scripts/make_css_property_names.py',
          ],
          'outputs': [
            '<(blink_core_output_dir)/CSSPropertyNames.cpp',
            '<(blink_core_output_dir)/CSSPropertyNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_css_property_names.py',
            'css/CSSProperties.in',
            '--output_dir',
            '<(blink_core_output_dir)',
            '--gperf', '<(gperf_exe)',
          ],
        },
        {
          'action_name': 'MediaFeatureNames',
          'variables': {
            'in_files': [
              'css/MediaFeatureNames.in',
            ],
          },
          'inputs': [
            '<@(make_names_files)',
            '../build/scripts/make_media_feature_names.py',
            '<@(in_files)'
          ],
          'outputs': [
            '<(blink_core_output_dir)/MediaFeatureNames.cpp',
            '<(blink_core_output_dir)/MediaFeatureNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_media_feature_names.py',
            '<@(in_files)',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'MediaFeatures',
          'variables': {
            'in_files': [
              'css/MediaFeatureNames.in',
            ],
          },
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_media_features.py',
            '../build/scripts/templates/MediaFeatures.h.tmpl',
            '<@(in_files)'
          ],
          'outputs': [
            '<(blink_core_output_dir)/MediaFeatures.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_media_features.py',
            '<@(in_files)',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'MediaTypeNames',
          'variables': {
            'in_files': [
              'css/MediaTypeNames.in',
            ],
          },
          'inputs': [
            '<@(make_names_files)',
            '<@(in_files)'
          ],
          'outputs': [
            '<(blink_core_output_dir)/MediaTypeNames.cpp',
            '<(blink_core_output_dir)/MediaTypeNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            '<@(in_files)',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'CSSTokenizerCodepoints',
          'inputs': [
            '../build/scripts/make_css_tokenizer_codepoints.py',
          ],
          'outputs': [
            '<(blink_core_output_dir)/CSSTokenizerCodepoints.cpp',
          ],
          'action': [
            'python',
            '../build/scripts/make_css_tokenizer_codepoints.py',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'StylePropertyShorthand',
          'inputs': [
            '<@(css_properties_files)',
            '../build/scripts/make_style_shorthands.py',
            '../build/scripts/templates/StylePropertyShorthand.cpp.tmpl',
            '../build/scripts/templates/StylePropertyShorthand.h.tmpl',
          ],
          'outputs': [
            '<(blink_core_output_dir)/StylePropertyShorthand.cpp',
            '<(blink_core_output_dir)/StylePropertyShorthand.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_style_shorthands.py',
            'css/CSSProperties.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'StyleBuilder',
          'inputs': [
            '<@(css_properties_files)',
            '../build/scripts/make_style_builder.py',
            '../build/scripts/templates/StyleBuilder.cpp.tmpl',
            '../build/scripts/templates/StyleBuilderFunctions.cpp.tmpl',
            '../build/scripts/templates/StyleBuilderFunctions.h.tmpl',
          ],
          'outputs': [
            '<(blink_core_output_dir)/StyleBuilder.cpp',
            '<(blink_core_output_dir)/StyleBuilderFunctions.h',
            '<(blink_core_output_dir)/StyleBuilderFunctions.cpp',
          ],
          'action': [
            'python',
            '../build/scripts/make_style_builder.py',
            'css/CSSProperties.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'CSSPropertyMetadata',
          'inputs': [
            '<@(css_properties_files)',
            '../build/scripts/make_css_property_metadata.py',
            '../build/scripts/templates/CSSPropertyMetadata.cpp.tmpl',
          ],
          'outputs': [
            '<(blink_core_output_dir)/CSSPropertyMetadata.cpp',
          ],
          'action': [
            'python',
            '../build/scripts/make_css_property_metadata.py',
            'css/CSSProperties.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'CSSValueKeywords',
          'variables': {
            'in_files': [
              'css/CSSValueKeywords.in',
              'css/SVGCSSValueKeywords.in',
            ],
          },
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_css_value_keywords.py',
            '<@(in_files)'
          ],
          'outputs': [
            '<(blink_core_output_dir)/CSSValueKeywords.cpp',
            '<(blink_core_output_dir)/CSSValueKeywords.h',
          ],
          'action': [
             'python',
             '../build/scripts/make_css_value_keywords.py',
             '<@(in_files)',
             '--output_dir',
             '<(blink_core_output_dir)',
            '--gperf', '<(gperf_exe)',
          ],
        },
        {
          'action_name': 'HTMLElementFactory',
          'inputs': [
            '<@(make_element_factory_files)',
            'html/HTMLTagNames.in',
            'html/HTMLAttributeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLElementFactory.cpp',
            '<(blink_core_output_dir)/HTMLElementFactory.h',
            '<(blink_core_output_dir)/HTMLNames.cpp',
            '<(blink_core_output_dir)/HTMLNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_element_factory.py',
            'html/HTMLTagNames.in',
            'html/HTMLAttributeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'HTMLElementTypeHelpers',
          'inputs': [
            '<@(make_element_type_helpers_files)',
            'html/HTMLTagNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLElementTypeHelpers.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_element_type_helpers.py',
            'html/HTMLTagNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'SVGNames',
          'inputs': [
            '<@(make_element_factory_files)',
            'svg/SVGTagNames.in',
            'svg/SVGAttributeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/SVGElementFactory.cpp',
            '<(blink_core_output_dir)/SVGElementFactory.h',
            '<(blink_core_output_dir)/SVGNames.cpp',
            '<(blink_core_output_dir)/SVGNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_element_factory.py',
            'svg/SVGTagNames.in',
            'svg/SVGAttributeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'SVGElementTypeHelpers',
          'inputs': [
            '<@(make_element_type_helpers_files)',
            'svg/SVGTagNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/SVGElementTypeHelpers.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_element_type_helpers.py',
            'svg/SVGTagNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'EventFactory',
          'inputs': [
            '<@(make_event_factory_files)',
            '<(blink_core_output_dir)/EventInterfaces.in',
            'events/EventAliases.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/Event.cpp',
            '<(blink_core_output_dir)/EventHeaders.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_event_factory.py',
            '<(blink_core_output_dir)/EventInterfaces.in',
            'events/EventAliases.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'EventNames',
          'inputs': [
            '<@(make_names_files)',
            '<(blink_core_output_dir)/EventInterfaces.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/EventNames.cpp',
            '<(blink_core_output_dir)/EventNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            '<(blink_core_output_dir)/EventInterfaces.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'EventTargetNames',
          'inputs': [
            '<@(make_names_files)',
            'events/EventTargetFactory.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/EventTargetNames.cpp',
            '<(blink_core_output_dir)/EventTargetNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'events/EventTargetFactory.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'MathMLNames',
          'inputs': [
            '<@(make_qualified_names_files)',
            'html/parser/MathMLTagNames.in',
            'html/parser/MathMLAttributeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/MathMLNames.cpp',
            '<(blink_core_output_dir)/MathMLNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_qualified_names.py',
            'html/parser/MathMLTagNames.in',
            'html/parser/MathMLAttributeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'FetchInitiatorTypeNames',
          'inputs': [
            '<@(make_names_files)',
            'fetch/FetchInitiatorTypeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/fetch/FetchInitiatorTypeNames.cpp',
            '<(blink_core_output_dir)/fetch/FetchInitiatorTypeNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'fetch/FetchInitiatorTypeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)/fetch',
          ],
        },
        {
          'action_name': 'EventTypeNames',
          'inputs': [
            '<@(make_names_files)',
            'events/EventTypeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/EventTypeNames.cpp',
            '<(blink_core_output_dir)/EventTypeNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'events/EventTypeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'HTMLTokenizerNames',
          'inputs': [
            '<@(make_names_files)',
            'html/parser/HTMLTokenizerNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLTokenizerNames.cpp',
            '<(blink_core_output_dir)/HTMLTokenizerNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'html/parser/HTMLTokenizerNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'InputTypeNames',
          'inputs': [
            '<@(make_names_files)',
            'html/forms/InputTypeNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/InputTypeNames.cpp',
            '<(blink_core_output_dir)/InputTypeNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_names.py',
            'html/forms/InputTypeNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'XLinkNames',
          'inputs': [
            '<@(make_qualified_names_files)',
            'svg/xlinkattrs.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/XLinkNames.cpp',
            '<(blink_core_output_dir)/XLinkNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_qualified_names.py',
            'svg/xlinkattrs.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'XMLNSNames',
          'inputs': [
            '<@(make_qualified_names_files)',
            'xml/xmlnsattrs.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/XMLNSNames.cpp',
            '<(blink_core_output_dir)/XMLNSNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_qualified_names.py',
            'xml/xmlnsattrs.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'XMLNames',
          'inputs': [
            '<@(make_qualified_names_files)',
            'xml/xmlattrs.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/XMLNames.cpp',
            '<(blink_core_output_dir)/XMLNames.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_qualified_names.py',
            'xml/xmlattrs.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
        {
          'action_name': 'MakeTokenMatcherForViewport',
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_token_matcher.py',
            '../core/html/HTMLMetaElement-in.cpp',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLMetaElement.cpp',
          ],
          'action': [
            'python',
            '../build/scripts/make_token_matcher.py',
            '../core/html/HTMLMetaElement-in.cpp',
            '<(blink_core_output_dir)/HTMLMetaElement.cpp',
          ],
        },
        {
          'action_name': 'HTMLElementLookupTrie',
          'inputs': [
            '<@(scripts_for_in_files)',
            '../build/scripts/make_element_lookup_trie.py',
            '../build/scripts/templates/ElementLookupTrie.cpp.tmpl',
            '../build/scripts/templates/ElementLookupTrie.h.tmpl',
            'html/HTMLTagNames.in',
          ],
          'outputs': [
            '<(blink_core_output_dir)/HTMLElementLookupTrie.cpp',
            '<(blink_core_output_dir)/HTMLElementLookupTrie.h',
          ],
          'action': [
            'python',
            '../build/scripts/make_element_lookup_trie.py',
            'html/HTMLTagNames.in',
            '--output_dir',
            '<(blink_core_output_dir)',
          ],
        },
      ],
      'rules': [
        {
          'rule_name': 'bison',
          'extension': 'y',
          'outputs': [
            '<(blink_core_output_dir)/<(RULE_INPUT_ROOT).cpp',
            '<(blink_core_output_dir)/<(RULE_INPUT_ROOT).h'
          ],
          'action': [
            'python',
            '../build/scripts/rule_bison.py',
            '<(RULE_INPUT_PATH)',
            '<(blink_core_output_dir)',
            '<(bison_exe)',
          ],
        },
      ],
    },
  ],
}
