#
# Copyright (C) 2013 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#         * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#         * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#         * Neither the name of Google Inc. nor the names of its
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
      'devtools.gypi',
    ],
    'targets': [
        {
            'target_name': 'devtools_frontend_resources',
            'type': 'none',
            'dependencies': [
                'supported_css_properties',
                'frontend_protocol_sources',
                'build_applications',
            ],
            'conditions': [
                ['debug_devtools==0', {
                    'dependencies': [
                        'concatenated_inspector_css',
                        'concatenated_toolbox_css',
                    ],
                }],
            ],
            'copies': [
                {
                    'destination': '<(PRODUCT_DIR)/resources/inspector/Images',
                    'files': [
                        '<@(devtools_image_files)',
                    ],
                },
                {
                    'destination': '<(PRODUCT_DIR)/resources/inspector/',
                    'files': [
                        '<@(devtools_compatibility_scripts)',
                    ],
                },
            ],
        },
        {
            'target_name': 'devtools_extension_api',
            'type': 'none',
            'actions': [{
                'action_name': 'devtools_extension_api',
                'script_name': 'scripts/generate_devtools_extension_api.py',
                'inputs': [
                    '<@(_script_name)',
                    '<@(devtools_extension_api_files)',
                ],
                'outputs': ['<(PRODUCT_DIR)/resources/inspector/devtools_extension_api.js'],
                'action': ['python', '<@(_script_name)', '<@(_outputs)', '<@(devtools_extension_api_files)'],
            }],
        },
        {
            'target_name': 'generate_devtools_grd',
            'type': 'none',
            'dependencies': [
                'devtools_extension_api',
                'devtools_frontend_resources',
            ],
            'conditions': [
                ['debug_devtools==0', {
                    'actions': [{
                        'action_name': 'generate_devtools_grd',
                        'script_name': 'scripts/generate_devtools_grd.py',
                        'relative_path_dirs': [
                            '<(PRODUCT_DIR)/resources/inspector',
                            'front_end'
                        ],
                        'static_files': [
                            # Intentionally empty. Should get rebuilt when switching from debug_devtools==1.
                        ],
                        'devtools_static_files_list': '<|(devtools_static_grd_files.tmp <@(_static_files))',
                        'generated_files': [
                            # Core and remote modules should not be listed here.
                            '<(PRODUCT_DIR)/resources/inspector/inspector.css',
                            '<(PRODUCT_DIR)/resources/inspector/inspector.html',
                            '<(PRODUCT_DIR)/resources/inspector/inspector.js',
                            '<(PRODUCT_DIR)/resources/inspector/toolbox.css',
                            '<(PRODUCT_DIR)/resources/inspector/toolbox.html',
                            '<(PRODUCT_DIR)/resources/inspector/toolbox.js',
                            '<(PRODUCT_DIR)/resources/inspector/accessibility_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/animation_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/audits_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/components_lazy_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/console_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/elements_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/heap_snapshot_worker_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/layers_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/network_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/profiler_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/promises_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/resources_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/security_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/script_formatter_worker_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/settings_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/snippets_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/source_frame_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/sources_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/temp_storage_shared_worker_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/timeline_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/ui_lazy_module.js',
                            '<(PRODUCT_DIR)/resources/inspector/devtools_extension_api.js',
                        ],
                        'inputs': [
                            '<@(_script_name)',
                            '<@(_static_files)',
                            '<@(devtools_compatibility_scripts)',
                            '<@(_generated_files)',
                            '<@(devtools_image_files)',
                            '<(_devtools_static_files_list)',
                        ],
                        'images_path': [
                            'front_end/Images',
                        ],
                        'outputs': ['<(SHARED_INTERMEDIATE_DIR)/devtools/devtools_resources.grd'],
                        'action': ['python', '<@(_script_name)', '<@(_generated_files)', '<@(devtools_compatibility_scripts)', '--static_files_list', '<(_devtools_static_files_list)', '--relative_path_dirs', '<@(_relative_path_dirs)', '--images', '<@(_images_path)', '--output', '<@(_outputs)'],
                    }],
                },
                {
                    # If we're not concatenating devtools files, we want to
                    # run after the original files have been copied to
                    # <(PRODUCT_DIR)/resources/inspector.
                    'dependencies': ['devtools_frontend_resources'],
                    'actions': [{
                        'action_name': 'generate_devtools_grd',
                        'script_name': 'scripts/generate_devtools_grd.py',
                        'relative_path_dirs': [
                            'front_end',
                            '<(PRODUCT_DIR)/resources/inspector',
                        ],
                        'static_files': [
                            '<@(all_devtools_files)',
                            'front_end/Runtime.js',
                        ],
                        'devtools_static_files_list': '<|(devtools_static_grd_files.tmp <@(_static_files))',
                        'generated_files': [
                            '<(PRODUCT_DIR)/resources/inspector/InspectorBackendCommands.js',
                            '<(PRODUCT_DIR)/resources/inspector/SupportedCSSProperties.js',
                            '<(PRODUCT_DIR)/resources/inspector/inspector.html',
                            '<(PRODUCT_DIR)/resources/inspector/toolbox.html',
                        ],
                        'inputs': [
                            '<@(_script_name)',
                            '<@(_static_files)',
                            '<@(devtools_compatibility_scripts)',
                            '<@(_generated_files)',
                            '<@(devtools_image_files)',
                            '<(_devtools_static_files_list)',
                        ],
                        'images_path': [
                            'front_end/Images',
                        ],
                        # Note that other files are put under /devtools directory, together with declared devtools_resources.grd
                        'outputs': ['<(SHARED_INTERMEDIATE_DIR)/devtools/devtools_resources.grd'],
                        'action': ['python', '<@(_script_name)', '<@(_generated_files)', '<@(devtools_compatibility_scripts)', '--static_files_list', '<(_devtools_static_files_list)', '--relative_path_dirs', '<@(_relative_path_dirs)', '--images', '<@(_images_path)', '--output', '<@(_outputs)'],
                    }],
                }],
            ],
        },
        {
          'target_name': 'frontend_protocol_sources',
          'type': 'none',
          'actions': [
            {
              'action_name': 'generateInspectorProtocolFrontendSources',
              'inputs': [
                # The python script in action below.
                'scripts/CodeGeneratorFrontend.py',
                # Input file for the script.
                'protocol.json',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/resources/inspector/InspectorBackendCommands.js',
              ],
              'action': [
                'python',
                'scripts/CodeGeneratorFrontend.py',
                'protocol.json',
                '--output_js_dir', '<(PRODUCT_DIR)/resources/inspector/',
              ],
              'message': 'Generating Inspector protocol frontend sources from protocol.json',
            },
          ]
        },
        {
          'target_name': 'supported_css_properties',
          'type': 'none',
          'actions': [
            {
              'action_name': 'generateSupportedCSSProperties',
              'inputs': [
                # The python script in action below.
                'scripts/generate_supported_css.py',
                # Input files for the script.
                '../core/css/CSSProperties.in',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/resources/inspector/SupportedCSSProperties.js',
              ],
              'action': [
                'python',
                '<@(_inputs)',
                '<@(_outputs)',
              ],
              'message': 'Generating supported CSS properties for front end',
            },
          ]
        },

        # Frontend applications and modules.
        {
            'target_name': 'build_applications',
            'type': 'none',
            'dependencies': [
                'supported_css_properties',
                'frontend_protocol_sources',
            ],
            'output_path': '<(PRODUCT_DIR)/resources/inspector/',
            'actions': [{
                'action_name': 'build_applications',
                'script_name': 'scripts/build_applications.py',
                'helper_scripts': [
                    'scripts/modular_build.py',
                    'scripts/concatenate_application_code.py',
                ],
                'inputs': [
                    '<@(_script_name)',
                    '<@(_helper_scripts)',
                    '<@(all_devtools_files)',
                    'front_end/inspector.html',
                    'front_end/toolbox.html',
                    '<(_output_path)/InspectorBackendCommands.js',
                    '<(_output_path)/SupportedCSSProperties.js',
                ],
                'action': ['python', '<@(_script_name)', 'inspector', 'toolbox', '--input_path', 'front_end', '--output_path', '<@(_output_path)', '--debug', '<@(debug_devtools)'],
                'conditions': [
                    ['debug_devtools==0', { # Release
                        'outputs': [
                            '<(_output_path)/inspector.html',
                            '<(_output_path)/inspector.js',
                            '<(_output_path)/toolbox.html',
                            '<(_output_path)/toolbox.js',
                            '<(_output_path)/accessibility_module.js',
                            '<(_output_path)/animation_module.js',
                            '<(_output_path)/audits_module.js',
                            '<(_output_path)/cm_modes_module.js',
                            '<(_output_path)/components_lazy_module.js',
                            '<(_output_path)/console_module.js',
                            '<(_output_path)/elements_module.js',
                            '<(_output_path)/emulated_devices_module.js',
                            '<(_output_path)/heap_snapshot_worker_module.js',
                            '<(_output_path)/layers_module.js',
                            '<(_output_path)/network_module.js',
                            '<(_output_path)/profiler_module.js',
                            '<(_output_path)/promises_module.js',
                            '<(_output_path)/resources_module.js',
                            '<(_output_path)/security_module.js',
                            '<(_output_path)/screencast_module.js',
                            '<(_output_path)/script_formatter_worker_module.js',
                            '<(_output_path)/settings_module.js',
                            '<(_output_path)/snippets_module.js',
                            '<(_output_path)/source_frame_module.js',
                            '<(_output_path)/sources_module.js',
                            '<(_output_path)/temp_storage_shared_worker_module.js',
                            '<(_output_path)/timeline_module.js',
                            '<(_output_path)/ui_lazy_module.js',
                        ],
                    },
                    { # Debug
                        'outputs': [
                            '<(_output_path)/inspector.html',
                            '<(_output_path)/toolbox.html',
                        ]
                    }]
                ]
            }],
            'conditions': [
                ['debug_devtools==0', { # Release
                },
                { # Debug
                  # Copy runtime core and non-module directories here.
                    'copies': [
                        {
                            'destination': '<(_output_path)',
                            'files': [
                                '<@(devtools_core_base_files)',
                                '<@(devtools_core_css_files)',
                            ],
                        },
                        {
                            'destination': '<(_output_path)/acorn',
                            'files': [
                                '<@(devtools_acorn_files)',
                            ],
                        },
                        {
                            'destination': '<(_output_path)/cm',
                            'files': [
                                '<@(devtools_cm_js_files)',
                                '<@(devtools_cm_css_files)',
                            ],
                        },
                    ]
                }]
            ]
        },
    ], # targets
    'conditions': [
        ['debug_devtools==0', {
            'targets': [
                {
                    'target_name': 'concatenated_inspector_css',
                    'type': 'none',
                    'actions': [{
                        'action_name': 'concatenate_inspector_css',
                        'script_name': 'scripts/concatenate_css_files.py',
                        'input_stylesheet': 'front_end/inspector.css',
                        'inputs': [
                            '<@(_script_name)',
                            '<@(_input_stylesheet)',
                            '<@(devtools_core_css_files)',
                        ],
                        'search_path': [ 'front_end' ],
                        'outputs': ['<(PRODUCT_DIR)/resources/inspector/inspector.css'],
                        'action': ['python', '<@(_script_name)', '<@(_input_stylesheet)', '<@(_outputs)'],
                    }],
                },
                {
                    'target_name': 'concatenated_toolbox_css',
                    'type': 'none',
                    'actions': [{
                        'action_name': 'concatenate_toolbox_css',
                        'script_name': 'scripts/concatenate_css_files.py',
                        'input_stylesheet': 'front_end/toolbox.css',
                        'inputs': [
                            '<@(_script_name)',
                            '<@(_input_stylesheet)',
                            '<@(devtools_core_css_files)',
                        ],
                        'search_path': [ 'front_end' ],
                        'outputs': ['<(PRODUCT_DIR)/resources/inspector/toolbox.css'],
                        'action': ['python', '<@(_script_name)', '<@(_input_stylesheet)', '<@(_outputs)'],
                    }],
                },
            ],
        }],
    ], # conditions
}
