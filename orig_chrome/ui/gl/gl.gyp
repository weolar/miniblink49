# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },

  'targets': [
    {
      'target_name': 'gl',
      'type': '<(component)',
      'product_name': 'gl_wrapper',  # Avoid colliding with OS X's libGL.dylib
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/blpwtk2/blpwtk2.gyp:blpwtk2_generate_sources',
        '<(DEPTH)/gpu/command_buffer/command_buffer.gyp:gles2_utils',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/mesa/mesa.gyp:mesa_headers',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx_geometry',
      ],
      'defines': [
        'GL_IMPLEMENTATION',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/swiftshader/include',
        '<(DEPTH)/third_party/khronos',
      ],
      'export_dependent_settings': [
        '<(DEPTH)/third_party/mesa/mesa.gyp:mesa_headers',
      ],
      'sources': [
        'android/gl_jni_registrar.cc',
        'android/gl_jni_registrar.h',
        'android/scoped_java_surface.cc',
        'android/scoped_java_surface.h',
        'android/surface_texture.cc',
        'android/surface_texture.h',
        'android/surface_texture_listener.cc',
        'android/surface_texture_listener.h',
        'gl_bindings.cc',
        'gl_bindings.h',
        'gl_bindings_autogen_gl.cc',
        'gl_bindings_autogen_gl.h',
        'gl_bindings_autogen_osmesa.cc',
        'gl_bindings_autogen_osmesa.h',
        'gl_bindings_skia_in_process.cc',
        'gl_bindings_skia_in_process.h',
        'gl_context.cc',
        'gl_context.h',
        'gl_context_android.cc',
        'gl_context_mac.mm',
        'gl_context_osmesa.cc',
        'gl_context_osmesa.h',
        'gl_context_ozone.cc',
        'gl_context_stub.cc',
        'gl_context_stub.h',
        'gl_context_stub_with_extensions.cc',
        'gl_context_stub_with_extensions.h',
        'gl_context_win.cc',
        'gl_context_x11.cc',
        'gl_enums.cc',
        'gl_enums.h',
        'gl_enums_implementation_autogen.h',
        'gl_export.h',
        'gl_fence.cc',
        'gl_fence.h',
        'gl_fence_arb.cc',
        'gl_fence_arb.h',
        'gl_fence_nv.cc',
        'gl_fence_nv.h',
        'gl_gl_api_implementation.cc',
        'gl_gl_api_implementation.h',
        'gl_image.h',
        'gl_image_memory.cc',
        'gl_image_memory.h',
        'gl_image_ref_counted_memory.cc',
        'gl_image_ref_counted_memory.h',
        'gl_image_shared_memory.cc',
        'gl_image_shared_memory.h',
        'gl_image_stub.cc',
        'gl_image_stub.h',
        'gl_implementation.cc',
        'gl_implementation.h',
        'gl_implementation_android.cc',
        'gl_implementation_mac.cc',
        'gl_implementation_ozone.cc',
        'gl_implementation_win.cc',
        'gl_implementation_x11.cc',
        'gl_osmesa_api_implementation.cc',
        'gl_osmesa_api_implementation.h',
        'gl_share_group.cc',
        'gl_share_group.h',
        'gl_state_restorer.cc',
        'gl_state_restorer.h',
        'gl_surface.cc',
        'gl_surface.h',
        'gl_surface_android.cc',
        'gl_surface_mac.cc',
        'gl_surface_osmesa.cc',
        'gl_surface_osmesa.h',
        'gl_surface_ozone.cc',
        'gl_surface_stub.cc',
        'gl_surface_stub.h',
        'gl_surface_win.cc',
        'gl_surface_x11.cc',
        'gl_switches.cc',
        'gl_switches.h',
        'gl_version_info.cc',
        'gl_version_info.h',
        'gpu_switching_manager.cc',
        'gpu_switching_manager.h',
        'gpu_switching_observer.h',
        'gpu_timing.cc',
        'gpu_timing.h',
        'scoped_api.cc',
        'scoped_api.h',
        'scoped_binders.cc',
        'scoped_binders.h',
        'scoped_make_current.cc',
        'scoped_make_current.h',
        'sync_control_vsync_provider.cc',
        'sync_control_vsync_provider.h',
        'trace_util.cc',
        'trace_util.h',
      ],
      'conditions': [
        ['OS in ("win", "android", "linux")', {
          'sources': [
            'egl_util.cc',
            'egl_util.h',
            'gl_bindings_autogen_egl.cc',
            'gl_bindings_autogen_egl.h',
            'gl_context_egl.cc',
            'gl_context_egl.h',
            'gl_egl_api_implementation.cc',
            'gl_egl_api_implementation.h',
            'gl_fence_egl.cc',
            'gl_fence_egl.h',
            'gl_image_egl.cc',
            'gl_image_egl.h',
            'gl_surface_egl.cc',
            'gl_surface_egl.h',
          ],
          'include_dirs': [
            '<(DEPTH)/third_party/khronos',
        ],
        }],
        ['OS in ("android", "linux")', {
          'sources': [
            'gl_implementation_osmesa.cc',
            'gl_implementation_osmesa.h',
          ],
        }],
        ['use_x11 == 1', {
          'sources': [
            'gl_bindings_autogen_glx.cc',
            'gl_bindings_autogen_glx.h',
            'gl_context_glx.cc',
            'gl_context_glx.h',
            'gl_egl_api_implementation.cc',
            'gl_egl_api_implementation.h',
            'gl_glx_api_implementation.cc',
            'gl_glx_api_implementation.h',
            'gl_image_glx.cc',
            'gl_image_glx.h',
            'gl_surface_glx.cc',
            'gl_surface_glx.h',
          ],
          'all_dependent_settings': {
            'defines': [
              'GL_GLEXT_PROTOTYPES',
            ],
          },
          'dependencies': [
            '<(DEPTH)/build/linux/system.gyp:x11',
            '<(DEPTH)/build/linux/system.gyp:xcomposite',
            '<(DEPTH)/build/linux/system.gyp:xext',
            '<(DEPTH)/ui/events/platform/events_platform.gyp:events_platform',
            '<(DEPTH)/ui/gfx/x/gfx_x11.gyp:gfx_x11',
          ],
        }],
        ['OS=="win"', {
          'sources': [
            'angle_platform_impl.cc',
            'angle_platform_impl.h',
            'gl_bindings_autogen_wgl.cc',
            'gl_bindings_autogen_wgl.h',
            'gl_context_wgl.cc',
            'gl_context_wgl.h',
            'gl_egl_api_implementation.cc',
            'gl_egl_api_implementation.h',
            'gl_surface_wgl.cc',
            'gl_surface_wgl.h',
            'gl_wgl_api_implementation.cc',
            'gl_wgl_api_implementation.h',
          ],
          'msvs_settings': {
            'VCLinkerTool': {
              'DelayLoadDLLs': [
                'dwmapi.dll',
              ],
              'AdditionalDependencies': [
                'dwmapi.lib',
              ],
            },
          },
          'link_settings': {
            'libraries': [
              '-ldwmapi.lib',
            ],
          },
          'dependencies': [
            '<(DEPTH)/third_party/angle/src/angle.gyp:libEGL',
            '<(DEPTH)/third_party/angle/src/angle.gyp:libGLESv2',
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'gl_context_cgl.cc',
            'gl_context_cgl.h',
            'gl_fence_apple.cc',
            'gl_fence_apple.h',
            'gl_image_io_surface.mm',
            'gl_image_io_surface.h',
            'scoped_cgl.cc',
            'scoped_cgl.h',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/IOSurface.framework',
              '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
              '$(SDKROOT)/System/Library/Frameworks/Quartz.framework',
            ],
          },
        }],
        ['OS=="android"', {
          'dependencies': [
            'gl_jni_headers',
            '../android/ui_android.gyp:ui_java',
          ],
          'sources': [
            'gl_image_surface_texture.cc',
            'gl_image_surface_texture.h',
          ],
          'link_settings': {
            'libraries': [
              '-landroid',
            ],
          },
          'sources!': [
            'system_monitor_posix.cc',
          ],
          'defines': [
            'GL_GLEXT_PROTOTYPES',
            'EGL_EGLEXT_PROTOTYPES',
          ],
        }],
        ['OS!="android"', {
          'sources/': [ ['exclude', '^android/'] ],
        }],
        ['use_ozone==1', {
          'sources': [
            'gl_image_ozone_native_pixmap.cc',
            'gl_image_ozone_native_pixmap.h',
          ],
          'dependencies': [
            '../ozone/ozone.gyp:ozone',
            '../ozone/ozone.gyp:ozone_base',
          ],
        }],
      ],
    },
    {
      'target_name': 'gl_unittest_utils',
      'type': 'static_library',
      'dependencies': [
        '../../third_party/khronos/khronos.gyp:khronos_headers',
        'gl',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'gl_bindings_autogen_mock.cc',
        'gl_bindings_autogen_mock.h',
        'gl_mock.cc',
        'gl_mock.h',
        'gl_mock_autogen_gl.h',
        'gpu_timing_fake.cc',
        'gpu_timing_fake.h',
      ],
    },
    {
      'target_name': 'gl_test_support',
      'type': 'static_library',
      'dependencies': [
        '../../third_party/khronos/khronos.gyp:khronos_headers',
        'gl',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'test/gl_image_test_support.cc',
        'test/gl_image_test_support.h',
        'test/gl_image_test_template.h',
        'test/gl_surface_test_support.cc',
        'test/gl_surface_test_support.h',
        'test/gl_test_helper.cc',
        'test/gl_test_helper.h',
      ],
      'conditions': [
        ['use_x11==1', {
          'dependencies': [
            '../../build/linux/system.gyp:x11',
            '../gfx/x/gfx_x11.gyp:gfx_x11',
            '../platform_window/x11/x11_window.gyp:x11_window',
          ],
        }],
      ],
    },
  ],
  'conditions': [
    ['OS=="android"' , {
      'targets': [
        {
          'target_name': 'surface_jni_headers',
          'type': 'none',
          'variables': {
            'jni_gen_package': 'ui/gl',
            'input_java_class': 'android/view/Surface.class',
          },
          'includes': [ '../../build/jar_file_jni_generator.gypi' ],
        },
        {
          'target_name': 'gl_jni_headers',
          'type': 'none',
          'dependencies': [
            'surface_jni_headers',
          ],
          'sources': [
            '../android/java/src/org/chromium/ui/gl/SurfaceTexturePlatformWrapper.java',
            '../android/java/src/org/chromium/ui/gl/SurfaceTextureListener.java',
          ],
          'variables': {
            'jni_gen_package': 'ui/gl',
          },
          'includes': [ '../../build/jni_generator.gypi' ],
        },
      ],
    }],
  ],
}
