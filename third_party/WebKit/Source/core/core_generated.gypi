# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../bindings/core/core.gypi',  # for bindings_core_v8_output_dir
  ],

  'variables': {
    'blink_core_output_dir': '<(SHARED_INTERMEDIATE_DIR)/blink/core',

    'generated_webcore_testing_idl_files': [
      '<(blink_core_output_dir)/testing/InternalRuntimeFlags.idl',
      '<(blink_core_output_dir)/testing/InternalSettingsGenerated.idl',
    ],

    'generated_core_testing_dictionary_files': [
      '<(blink_core_output_dir)/testing/InternalDictionary.cpp',
      '<(blink_core_output_dir)/testing/InternalDictionary.h',
      '<(blink_core_output_dir)/testing/InternalDictionaryDerived.cpp',
      '<(blink_core_output_dir)/testing/InternalDictionaryDerived.h',
      '<(blink_core_output_dir)/testing/PluginPlaceholderOptions.cpp',
      '<(blink_core_output_dir)/testing/PluginPlaceholderOptions.h',
    ],

    'generated_bindings_core_testing_files': [
      '<(bindings_core_v8_output_dir)/V8DictionaryTest.cpp',
      '<(bindings_core_v8_output_dir)/V8DictionaryTest.h',
      '<(bindings_core_v8_output_dir)/V8GarbageCollectedScriptWrappable.cpp',
      '<(bindings_core_v8_output_dir)/V8GarbageCollectedScriptWrappable.h',
      '<(bindings_core_v8_output_dir)/V8GCObservation.cpp',
      '<(bindings_core_v8_output_dir)/V8GCObservation.h',
      '<(bindings_core_v8_output_dir)/V8PrivateScriptTest.cpp',
      '<(bindings_core_v8_output_dir)/V8PrivateScriptTest.h',
      '<(bindings_core_v8_output_dir)/V8TypeConversions.cpp',
      '<(bindings_core_v8_output_dir)/V8TypeConversions.h',
      '<(bindings_core_v8_output_dir)/V8UnionTypesTest.cpp',
      '<(bindings_core_v8_output_dir)/V8UnionTypesTest.h',
      '<(bindings_core_v8_output_dir)/V8Internals.cpp',
      '<(bindings_core_v8_output_dir)/V8Internals.h',
      '<(bindings_core_v8_output_dir)/V8InternalSettings.cpp',
      '<(bindings_core_v8_output_dir)/V8InternalSettings.h',
      '<(bindings_core_v8_output_dir)/V8InternalSettingsGenerated.cpp',
      '<(bindings_core_v8_output_dir)/V8InternalSettingsGenerated.h',
      '<(bindings_core_v8_output_dir)/V8InternalRuntimeFlags.cpp',
      '<(bindings_core_v8_output_dir)/V8InternalRuntimeFlags.h',
      '<(bindings_core_v8_output_dir)/V8LayerRect.cpp',
      '<(bindings_core_v8_output_dir)/V8LayerRect.h',
      '<(bindings_core_v8_output_dir)/V8LayerRectList.cpp',
      '<(bindings_core_v8_output_dir)/V8LayerRectList.h',
      '<(bindings_core_v8_output_dir)/V8RefCountedScriptWrappable.cpp',
      '<(bindings_core_v8_output_dir)/V8RefCountedScriptWrappable.h',
      '<(bindings_core_v8_output_dir)/V8InternalDictionary.cpp',
      '<(bindings_core_v8_output_dir)/V8InternalDictionary.h',
      '<(bindings_core_v8_output_dir)/V8InternalDictionaryDerived.cpp',
      '<(bindings_core_v8_output_dir)/V8InternalDictionaryDerived.h',
      '<(bindings_core_v8_output_dir)/V8PluginPlaceholderOptions.cpp',
      '<(bindings_core_v8_output_dir)/V8PluginPlaceholderOptions.h',
    ],

    'generated_core_additional_files': [
      # Additional .cpp files for HashTools.h
      '<(blink_core_output_dir)/CSSPropertyNames.cpp',
      '<(blink_core_output_dir)/CSSValueKeywords.cpp',

      # Additional .cpp files from make_core_generated actions.
      '<(blink_core_output_dir)/Event.cpp',
      '<(blink_core_output_dir)/EventHeaders.h',
      '<(blink_core_output_dir)/EventNames.cpp',
      '<(blink_core_output_dir)/EventNames.h',
      '<(blink_core_output_dir)/EventTargetNames.cpp',
      '<(blink_core_output_dir)/EventTargetNames.h',
      '<(blink_core_output_dir)/EventTypeNames.cpp',
      '<(blink_core_output_dir)/EventTypeNames.h',
      '<(blink_core_output_dir)/HTMLElementFactory.cpp',
      '<(blink_core_output_dir)/HTMLElementFactory.h',
      '<(blink_core_output_dir)/HTMLElementLookupTrie.cpp',
      '<(blink_core_output_dir)/HTMLElementLookupTrie.h',
      '<(blink_core_output_dir)/HTMLNames.cpp',
      '<(blink_core_output_dir)/HTMLTokenizerNames.cpp',
      '<(blink_core_output_dir)/InputTypeNames.cpp',
      '<(blink_core_output_dir)/MathMLNames.cpp',
      '<(blink_core_output_dir)/SVGNames.cpp',
      '<(blink_core_output_dir)/XLinkNames.cpp',
      '<(blink_core_output_dir)/XMLNSNames.cpp',
      '<(blink_core_output_dir)/XMLNames.cpp',
      '<(blink_core_output_dir)/fetch/FetchInitiatorTypeNames.cpp',

      # Generated from HTMLEntityNames.in
      '<(blink_core_output_dir)/HTMLEntityTable.cpp',

      # Generated from MediaFeatureNames.in
      '<(blink_core_output_dir)/MediaFeatureNames.cpp',

      # Generated from MediaTypeNames.in
      '<(blink_core_output_dir)/MediaTypeNames.cpp',

      # Generated from HTMLMetaElement-in.cpp
      '<(blink_core_output_dir)/HTMLMetaElement.cpp',

      # Additional .cpp files from the make_core_generated rules.
      '<(blink_core_output_dir)/XPathGrammar.cpp',

      # Additional .cpp files from the inspector_protocol_sources list.
      '<(blink_core_output_dir)/InspectorFrontend.cpp',
      '<(blink_core_output_dir)/InspectorBackendDispatcher.cpp',
      '<(blink_core_output_dir)/InspectorTypeBuilder.cpp',

      # Additional .cpp files from the inspector_instrumentation_sources list.
      '<(blink_core_output_dir)/InspectorConsoleInstrumentationInl.h',
      '<(blink_core_output_dir)/InspectorInstrumentationInl.h',
      '<(blink_core_output_dir)/InspectorOverridesInl.h',
      '<(blink_core_output_dir)/InstrumentingAgentsInl.h',
      '<(blink_core_output_dir)/InspectorInstrumentationImpl.cpp',

      # Additional .cpp files for SVG.
      '<(blink_core_output_dir)/SVGElementFactory.cpp',

      # Generated from make_style_shorthands.py
      '<(blink_core_output_dir)/StylePropertyShorthand.cpp',

      # Generated from make_style_builder.py
      '<(blink_core_output_dir)/StyleBuilder.cpp',
      '<(blink_core_output_dir)/StyleBuilderFunctions.cpp',

      # Generated from make_css_property_metadata.py
      '<(blink_core_output_dir)/CSSPropertyMetadata.cpp',
    ],
  },
}
