// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/config/gpu_info_collector.h"

// This has to be included before windows.h.
#include "third_party/re2/re2/re2.h"

#include <windows.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi.h>
#include <setupapi.h>

#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/file_util.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "base/scoped_native_library.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread.h"
#include "base/threading/worker_pool.h"
#include "base/win/registry.h"
#include "base/win/scoped_com_initializer.h"
#include "base/win/scoped_comptr.h"
#include "base/win/windows_version.h"
#include "third_party/libxml/chromium/libxml_utils.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_egl.h"

namespace gpu {

namespace {

// This must be kept in sync with histograms.xml.
enum DisplayLinkInstallationStatus {
  DISPLAY_LINK_NOT_INSTALLED,
  DISPLAY_LINK_7_1_OR_EARLIER,
  DISPLAY_LINK_7_2_OR_LATER,
  DISPLAY_LINK_INSTALLATION_STATUS_MAX
};

float ReadXMLFloatValue(XmlReader* reader) {
  std::string score_string;
  if (!reader->ReadElementContent(&score_string))
    return 0.0;

  double score;
  if (!base::StringToDouble(score_string, &score))
    return 0.0;

  return static_cast<float>(score);
}

GpuPerformanceStats RetrieveGpuPerformanceStats() {
  TRACE_EVENT0("gpu", "RetrieveGpuPerformanceStats");

  // If the user re-runs the assessment without restarting, the COM API
  // returns WINSAT_ASSESSMENT_STATE_NOT_AVAILABLE. Because of that and
  // http://crbug.com/124325, read the assessment result files directly.
  GpuPerformanceStats stats;

  // Get path to WinSAT results files.
  wchar_t winsat_results_path[MAX_PATH];
  DWORD size = ExpandEnvironmentStrings(
      L"%WinDir%\\Performance\\WinSAT\\DataStore\\",
      winsat_results_path, MAX_PATH);
  if (size == 0 || size > MAX_PATH) {
    LOG(ERROR) << "The path to the WinSAT results is too long: "
               << size << " chars.";
    return stats;
  }

  // Find most recent formal assessment results.
  base::FileEnumerator file_enumerator(
      base::FilePath(winsat_results_path),
      false,  // not recursive
      base::FileEnumerator::FILES,
      FILE_PATH_LITERAL("* * Formal.Assessment (*).WinSAT.xml"));

  base::FilePath current_results;
  for (base::FilePath results = file_enumerator.Next(); !results.empty();
       results = file_enumerator.Next()) {
    // The filenames start with the date and time as yyyy-mm-dd hh.mm.ss.xxx,
    // so the greatest file lexicographically is also the most recent file.
    if (base::FilePath::CompareLessIgnoreCase(current_results.value(),
                                              results.value()))
      current_results = results;
  }

  std::string current_results_string = current_results.MaybeAsASCII();
  if (current_results_string.empty()) {
    LOG(ERROR) << "Can't retrieve a valid WinSAT assessment.";
    return stats;
  }

  // Get relevant scores from results file. XML schema at:
  // http://msdn.microsoft.com/en-us/library/windows/desktop/aa969210.aspx
  XmlReader reader;
  if (!reader.LoadFile(current_results_string)) {
    LOG(ERROR) << "Could not open WinSAT results file.";
    return stats;
  }
  // Descend into <WinSAT> root element.
  if (!reader.SkipToElement() || !reader.Read()) {
    LOG(ERROR) << "Could not read WinSAT results file.";
    return stats;
  }

  // Search for <WinSPR> element containing the results.
  do {
    if (reader.NodeName() == "WinSPR")
      break;
  } while (reader.Next());
  // Descend into <WinSPR> element.
  if (!reader.Read()) {
    LOG(ERROR) << "Could not find WinSPR element in results file.";
    return stats;
  }

  // Read scores.
  for (int depth = reader.Depth(); reader.Depth() == depth; reader.Next()) {
    std::string node_name = reader.NodeName();
    if (node_name == "SystemScore")
      stats.overall = ReadXMLFloatValue(&reader);
    else if (node_name == "GraphicsScore")
      stats.graphics = ReadXMLFloatValue(&reader);
    else if (node_name == "GamingScore")
      stats.gaming = ReadXMLFloatValue(&reader);
  }

  if (stats.overall == 0.0)
    LOG(ERROR) << "Could not read overall score from assessment results.";
  if (stats.graphics == 0.0)
    LOG(ERROR) << "Could not read graphics score from assessment results.";
  if (stats.gaming == 0.0)
    LOG(ERROR) << "Could not read gaming score from assessment results.";

  return stats;
}

GpuPerformanceStats RetrieveGpuPerformanceStatsWithHistograms() {
  base::TimeTicks start_time = base::TimeTicks::Now();

  GpuPerformanceStats stats = RetrieveGpuPerformanceStats();

  UMA_HISTOGRAM_TIMES("GPU.WinSAT.ReadResultsFileTime",
                      base::TimeTicks::Now() - start_time);
  UMA_HISTOGRAM_CUSTOM_COUNTS("GPU.WinSAT.OverallScore2",
                              stats.overall * 10, 10, 200, 50);
  UMA_HISTOGRAM_CUSTOM_COUNTS("GPU.WinSAT.GraphicsScore2",
                              stats.graphics * 10, 10, 200, 50);
  UMA_HISTOGRAM_CUSTOM_COUNTS("GPU.WinSAT.GamingScore2",
                              stats.gaming * 10, 10, 200, 50);
  UMA_HISTOGRAM_BOOLEAN(
      "GPU.WinSAT.HasResults",
      stats.overall != 0.0 && stats.graphics != 0.0 && stats.gaming != 0.0);

  return stats;
}

// Returns the display link driver version or an invalid version if it is
// not installed.
Version DisplayLinkVersion() {
  base::win::RegKey key;

  if (key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE", KEY_READ | KEY_WOW64_64KEY))
    return Version();

  if (key.OpenKey(L"DisplayLink", KEY_READ | KEY_WOW64_64KEY))
    return Version();

  if (key.OpenKey(L"Core", KEY_READ | KEY_WOW64_64KEY))
    return Version();

  base::string16 version;
  if (key.ReadValue(L"Version", &version))
    return Version();

  return Version(base::UTF16ToASCII(version));
}

// Returns whether Lenovo dCute is installed.
bool IsLenovoDCuteInstalled() {
  base::win::RegKey key;

  if (key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE", KEY_READ | KEY_WOW64_64KEY))
    return false;

  if (key.OpenKey(L"Lenovo", KEY_READ | KEY_WOW64_64KEY))
    return false;

  if (key.OpenKey(L"Lenovo dCute", KEY_READ | KEY_WOW64_64KEY))
    return false;

  return true;
}

// Determines whether D3D11 won't work, either because it is not supported on
// the machine or because it is known it is likely to crash.
bool D3D11ShouldWork(const GPUInfo& gpu_info) {
  // TODO(apatrick): This is a temporary change to see what impact disabling
  // D3D11 stats collection has on Canary.
#if 1
  return false;
#else
  // Windows XP never supports D3D11. It seems to be less stable that D3D9 on
  // Vista.
  if (base::win::GetVersion() <= base::win::VERSION_VISTA)
    return false;

  // http://crbug.com/175525.
  if (gpu_info.display_link_version.IsValid())
    return false;

  return true;
#endif
}

// Collects information about the level of D3D11 support and records it in
// the UMA stats. Records no stats when D3D11 in not supported at all.
void CollectD3D11SupportOnWorkerThread() {
  TRACE_EVENT0("gpu", "CollectD3D11Support");

  typedef HRESULT (WINAPI *D3D11CreateDeviceFunc)(
      IDXGIAdapter* adapter,
      D3D_DRIVER_TYPE driver_type,
      HMODULE software,
      UINT flags,
      const D3D_FEATURE_LEVEL* feature_levels,
      UINT num_feature_levels,
      UINT sdk_version,
      ID3D11Device** device,
      D3D_FEATURE_LEVEL* feature_level,
      ID3D11DeviceContext** immediate_context);

  // This enumeration must be kept in sync with histograms.xml. Do not reorder
  // the members; always add to the end.
  enum FeatureLevel {
    FEATURE_LEVEL_UNKNOWN,
    FEATURE_LEVEL_NO_D3D11_DLL,
    FEATURE_LEVEL_NO_CREATE_DEVICE_ENTRY_POINT,
    FEATURE_LEVEL_DEVICE_CREATION_FAILED,
    FEATURE_LEVEL_9_1,
    FEATURE_LEVEL_9_2,
    FEATURE_LEVEL_9_3,
    FEATURE_LEVEL_10_0,
    FEATURE_LEVEL_10_1,
    FEATURE_LEVEL_11_0,
    NUM_FEATURE_LEVELS
  };

  FeatureLevel feature_level = FEATURE_LEVEL_UNKNOWN;
  UINT bgra_support = 0;

  // This module is leaked in case it is hooked by third party software.
  base::NativeLibrary d3d11_module = base::LoadNativeLibrary(
      base::FilePath(L"d3d11.dll"),
      NULL);

  if (!d3d11_module) {
    feature_level = FEATURE_LEVEL_NO_D3D11_DLL;
  } else {
    D3D11CreateDeviceFunc create_func =
        reinterpret_cast<D3D11CreateDeviceFunc>(
            base::GetFunctionPointerFromNativeLibrary(d3d11_module,
                                                      "D3D11CreateDevice"));
    if (!create_func) {
      feature_level = FEATURE_LEVEL_NO_CREATE_DEVICE_ENTRY_POINT;
    } else {
      static const D3D_FEATURE_LEVEL d3d_feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
      };

      base::win::ScopedComPtr<ID3D11Device> device;
      D3D_FEATURE_LEVEL d3d_feature_level;
      base::win::ScopedComPtr<ID3D11DeviceContext> device_context;
      HRESULT hr = create_func(NULL,
                               D3D_DRIVER_TYPE_HARDWARE,
                               NULL,
                               0,
                               d3d_feature_levels,
                               arraysize(d3d_feature_levels),
                               D3D11_SDK_VERSION,
                               device.Receive(),
                               &d3d_feature_level,
                               device_context.Receive());
      if (FAILED(hr)) {
        feature_level = FEATURE_LEVEL_DEVICE_CREATION_FAILED;
      } else {
        switch (d3d_feature_level) {
          case D3D_FEATURE_LEVEL_11_0:
            feature_level = FEATURE_LEVEL_11_0;
            break;
          case D3D_FEATURE_LEVEL_10_1:
            feature_level = FEATURE_LEVEL_10_1;
            break;
          case D3D_FEATURE_LEVEL_10_0:
            feature_level = FEATURE_LEVEL_10_0;
            break;
          case D3D_FEATURE_LEVEL_9_3:
            feature_level = FEATURE_LEVEL_9_3;
            break;
          case D3D_FEATURE_LEVEL_9_2:
            feature_level = FEATURE_LEVEL_9_2;
            break;
          case D3D_FEATURE_LEVEL_9_1:
            feature_level = FEATURE_LEVEL_9_1;
            break;
          default:
            NOTREACHED();
            break;
        }

        hr = device->CheckFormatSupport(DXGI_FORMAT_B8G8R8A8_UNORM,
                                        &bgra_support);
        DCHECK(SUCCEEDED(hr));
      }
    }
  }

  UMA_HISTOGRAM_ENUMERATION("GPU.D3D11_FeatureLevel",
                            feature_level,
                            NUM_FEATURE_LEVELS);

  // ANGLE requires at least feature level 10.0. Do not record any further
  // stats if ANGLE would not work anyway.
  if (feature_level < FEATURE_LEVEL_10_0)
    return;

  UMA_HISTOGRAM_BOOLEAN(
      "GPU.D3D11_B8G8R8A8_Texture2DSupport",
      (bgra_support & D3D11_FORMAT_SUPPORT_TEXTURE2D) != 0);
  UMA_HISTOGRAM_BOOLEAN(
      "GPU.D3D11_B8G8R8A8_RenderTargetSupport",
      (bgra_support & D3D11_FORMAT_SUPPORT_RENDER_TARGET) != 0);
}

// Collects information about the level of D3D11 support and records it in
// the UMA stats. Records no stats when D3D11 in not supported at all.
void CollectD3D11Support() {
  // D3D11 takes about 50ms to initialize so do this on a worker thread.
  base::WorkerPool::PostTask(
      FROM_HERE,
      base::Bind(CollectD3D11SupportOnWorkerThread),
      false);
}
}  // namespace anonymous

#if !defined(GOOGLE_CHROME_BUILD)
void GetAMDVideocardInfo(GPUInfo* gpu_info) {
  DCHECK(gpu_info);
  return;
}
#else
// This function has a real implementation for official builds that can
// be found in src/third_party/amd.
void GetAMDVideocardInfo(GPUInfo* gpu_info);
#endif

bool CollectDriverInfoD3D(const std::wstring& device_id,
                          GPUInfo* gpu_info) {
  TRACE_EVENT0("gpu", "CollectDriverInfoD3D");

  // create device info for the display device
  HDEVINFO device_info = SetupDiGetClassDevsW(
      NULL, device_id.c_str(), NULL,
      DIGCF_PRESENT | DIGCF_PROFILE | DIGCF_ALLCLASSES);
  if (device_info == INVALID_HANDLE_VALUE) {
    LOG(ERROR) << "Creating device info failed";
    return false;
  }

  DWORD index = 0;
  bool found = false;
  SP_DEVINFO_DATA device_info_data;
  device_info_data.cbSize = sizeof(device_info_data);
  while (SetupDiEnumDeviceInfo(device_info, index++, &device_info_data)) {
    WCHAR value[255];
    if (SetupDiGetDeviceRegistryPropertyW(device_info,
                                        &device_info_data,
                                        SPDRP_DRIVER,
                                        NULL,
                                        reinterpret_cast<PBYTE>(value),
                                        sizeof(value),
                                        NULL)) {
      HKEY key;
      std::wstring driver_key = L"System\\CurrentControlSet\\Control\\Class\\";
      driver_key += value;
      LONG result = RegOpenKeyExW(
          HKEY_LOCAL_MACHINE, driver_key.c_str(), 0, KEY_QUERY_VALUE, &key);
      if (result == ERROR_SUCCESS) {
        DWORD dwcb_data = sizeof(value);
        std::string driver_version;
        result = RegQueryValueExW(
            key, L"DriverVersion", NULL, NULL,
            reinterpret_cast<LPBYTE>(value), &dwcb_data);
        if (result == ERROR_SUCCESS)
          driver_version = base::UTF16ToASCII(std::wstring(value));

        std::string driver_date;
        dwcb_data = sizeof(value);
        result = RegQueryValueExW(
            key, L"DriverDate", NULL, NULL,
            reinterpret_cast<LPBYTE>(value), &dwcb_data);
        if (result == ERROR_SUCCESS)
          driver_date = base::UTF16ToASCII(std::wstring(value));

        std::string driver_vendor;
        dwcb_data = sizeof(value);
        result = RegQueryValueExW(
            key, L"ProviderName", NULL, NULL,
            reinterpret_cast<LPBYTE>(value), &dwcb_data);
        if (result == ERROR_SUCCESS) {
          driver_vendor = base::UTF16ToASCII(std::wstring(value));
          if (driver_vendor == "Advanced Micro Devices, Inc." ||
              driver_vendor == "ATI Technologies Inc.") {
            // We are conservative and assume that in the absence of a clear
            // signal the videocard is assumed to be switchable. Additionally,
            // some switchable systems with Intel GPUs aren't correctly
            // detected, so always count them.
            GetAMDVideocardInfo(gpu_info);
            if (!gpu_info->amd_switchable &&
                gpu_info->gpu.vendor_id == 0x8086) {
              gpu_info->amd_switchable = true;
              gpu_info->secondary_gpus.push_back(gpu_info->gpu);
              gpu_info->gpu.vendor_id = 0x1002;
              gpu_info->gpu.device_id = 0;  // Unknown discrete AMD GPU.
            }
          }
        }

        gpu_info->driver_vendor = driver_vendor;
        gpu_info->driver_version = driver_version;
        gpu_info->driver_date = driver_date;
        found = true;
        RegCloseKey(key);
        break;
      }
    }
  }
  SetupDiDestroyDeviceInfoList(device_info);
  return found;
}

CollectInfoResult CollectContextGraphicsInfo(GPUInfo* gpu_info) {
  TRACE_EVENT0("gpu", "CollectGraphicsInfo");

  DCHECK(gpu_info);

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kUseGL)) {
    std::string requested_implementation_name =
        CommandLine::ForCurrentProcess()->GetSwitchValueASCII(switches::kUseGL);
    if (requested_implementation_name == "swiftshader") {
      gpu_info->software_rendering = true;
      return kCollectInfoNonFatalFailure;
    }
  }

  CollectInfoResult result = CollectGraphicsInfoGL(gpu_info);
  if (result != kCollectInfoSuccess)
    return result;

  // ANGLE's renderer strings are of the form:
  // ANGLE (<adapter_identifier> Direct3D<version> vs_x_x ps_x_x)
  std::string direct3d_version;
  int vertex_shader_major_version = 0;
  int vertex_shader_minor_version = 0;
  int pixel_shader_major_version = 0;
  int pixel_shader_minor_version = 0;
  gpu_info->adapter_luid = 0;
  if (RE2::FullMatch(gpu_info->gl_renderer,
                     "ANGLE \\(.*\\)") &&
      RE2::PartialMatch(gpu_info->gl_renderer,
                        " Direct3D(\\w+)",
                        &direct3d_version) &&
      RE2::PartialMatch(gpu_info->gl_renderer,
                        " vs_(\\d+)_(\\d+)",
                        &vertex_shader_major_version,
                        &vertex_shader_minor_version) &&
      RE2::PartialMatch(gpu_info->gl_renderer,
                        " ps_(\\d+)_(\\d+)",
                        &pixel_shader_major_version,
                        &pixel_shader_minor_version)) {
    gpu_info->can_lose_context = direct3d_version == "9";
    gpu_info->vertex_shader_version =
        base::StringPrintf("%d.%d",
                           vertex_shader_major_version,
                           vertex_shader_minor_version);
    gpu_info->pixel_shader_version =
        base::StringPrintf("%d.%d",
                           pixel_shader_major_version,
                           pixel_shader_minor_version);

    // ANGLE's EGL vendor strings are of the form:
    // Google, Inc. (adapter LUID: 0123456789ABCDEF)
    // The LUID is optional and identifies the GPU adapter ANGLE is using.
    const char* egl_vendor = eglQueryString(
        gfx::GLSurfaceEGL::GetHardwareDisplay(),
        EGL_VENDOR);
    RE2::PartialMatch(egl_vendor,
                      " \\(adapter LUID: ([0-9A-Fa-f]{16})\\)",
                      RE2::Hex(&gpu_info->adapter_luid));

    // DirectX diagnostics are collected asynchronously because it takes a
    // couple of seconds. Do not mark gpu_info as complete until that is done.
    gpu_info->finalized = false;
  } else {
    gpu_info->finalized = true;
  }

  return kCollectInfoSuccess;
}

GpuIDResult CollectGpuID(uint32* vendor_id, uint32* device_id) {
  DCHECK(vendor_id && device_id);
  *vendor_id = 0;
  *device_id = 0;

  // Taken from http://developer.nvidia.com/object/device_ids.html
  DISPLAY_DEVICE dd;
  dd.cb = sizeof(DISPLAY_DEVICE);
  std::wstring id;
  for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); ++i) {
    if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
      id = dd.DeviceID;
      break;
    }
  }

  if (id.length() > 20) {
    int vendor = 0, device = 0;
    std::wstring vendor_string = id.substr(8, 4);
    std::wstring device_string = id.substr(17, 4);
    base::HexStringToInt(base::UTF16ToASCII(vendor_string), &vendor);
    base::HexStringToInt(base::UTF16ToASCII(device_string), &device);
    *vendor_id = vendor;
    *device_id = device;
    if (*vendor_id != 0 && *device_id != 0)
      return kGpuIDSuccess;
  }
  return kGpuIDFailure;
}

CollectInfoResult CollectBasicGraphicsInfo(GPUInfo* gpu_info) {
  TRACE_EVENT0("gpu", "CollectPreliminaryGraphicsInfo");

  DCHECK(gpu_info);

  gpu_info->performance_stats = RetrieveGpuPerformanceStatsWithHistograms();

  // nvd3d9wrap.dll is loaded into all processes when Optimus is enabled.
  HMODULE nvd3d9wrap = GetModuleHandleW(L"nvd3d9wrap.dll");
  gpu_info->optimus = nvd3d9wrap != NULL;

  gpu_info->lenovo_dcute = IsLenovoDCuteInstalled();

  gpu_info->display_link_version = DisplayLinkVersion();

  if (!gpu_info->display_link_version .IsValid()) {
    UMA_HISTOGRAM_ENUMERATION("GPU.DisplayLinkInstallationStatus",
                              DISPLAY_LINK_NOT_INSTALLED,
                              DISPLAY_LINK_INSTALLATION_STATUS_MAX);
  } else if (gpu_info->display_link_version.IsOlderThan("7.2")) {
    UMA_HISTOGRAM_ENUMERATION("GPU.DisplayLinkInstallationStatus",
                              DISPLAY_LINK_7_1_OR_EARLIER,
                              DISPLAY_LINK_INSTALLATION_STATUS_MAX);
  } else {
    UMA_HISTOGRAM_ENUMERATION("GPU.DisplayLinkInstallationStatus",
                              DISPLAY_LINK_7_2_OR_LATER,
                              DISPLAY_LINK_INSTALLATION_STATUS_MAX);
  }

  // Taken from http://developer.nvidia.com/object/device_ids.html
  DISPLAY_DEVICE dd;
  dd.cb = sizeof(DISPLAY_DEVICE);
  std::wstring id;
  for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); ++i) {
    if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
      id = dd.DeviceID;
      break;
    }
  }

  if (id.length() <= 20)
    return kCollectInfoNonFatalFailure;

  int vendor_id = 0, device_id = 0;
  base::string16 vendor_id_string = id.substr(8, 4);
  base::string16 device_id_string = id.substr(17, 4);
  base::HexStringToInt(base::UTF16ToASCII(vendor_id_string), &vendor_id);
  base::HexStringToInt(base::UTF16ToASCII(device_id_string), &device_id);
  gpu_info->gpu.vendor_id = vendor_id;
  gpu_info->gpu.device_id = device_id;
  // TODO(zmo): we only need to call CollectDriverInfoD3D() if we use ANGLE.
  if (!CollectDriverInfoD3D(id, gpu_info))
    return kCollectInfoNonFatalFailure;

  // Collect basic information about supported D3D11 features. Delay for 45
  // seconds so as not to regress performance tests.
  if (D3D11ShouldWork(*gpu_info)) {
    // This is on a field trial so we can turn it off easily if it blows up
    // again in stable channel.
    scoped_refptr<base::FieldTrial> trial(
        base::FieldTrialList::FactoryGetFieldTrial(
            "D3D11Experiment", 100, "Disabled", 2015, 7, 8,
            base::FieldTrial::SESSION_RANDOMIZED, NULL));
    const int enabled_group =
        trial->AppendGroup("Enabled", 0);

    if (trial->group() == enabled_group) {
      base::MessageLoop::current()->PostDelayedTask(
          FROM_HERE,
          base::Bind(&CollectD3D11Support),
          base::TimeDelta::FromSeconds(45));
    }
  }

  return kCollectInfoSuccess;
}

CollectInfoResult CollectDriverInfoGL(GPUInfo* gpu_info) {
  TRACE_EVENT0("gpu", "CollectDriverInfoGL");

  if (!gpu_info->driver_version.empty())
    return kCollectInfoSuccess;

  bool parsed = RE2::PartialMatch(
      gpu_info->gl_version, "([\\d\\.]+)$", &gpu_info->driver_version);
  return parsed ? kCollectInfoSuccess : kCollectInfoNonFatalFailure;
}

void MergeGPUInfo(GPUInfo* basic_gpu_info,
                  const GPUInfo& context_gpu_info) {
  DCHECK(basic_gpu_info);

  if (context_gpu_info.software_rendering) {
    basic_gpu_info->software_rendering = true;
    return;
  }

  MergeGPUInfoGL(basic_gpu_info, context_gpu_info);

  basic_gpu_info->dx_diagnostics = context_gpu_info.dx_diagnostics;
}

}  // namespace gpu
