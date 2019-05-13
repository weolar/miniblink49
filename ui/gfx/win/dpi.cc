#include "dpi.h"
#include "windows.h"

float g_device_scale_factor = 0.0f;

namespace gfx {

    namespace win {

        int kDefaultDPIX = 96;
        int kDefaultDPIY = 96;

        SIZE GetDPI() {
            static int dpi_x = 0;
            static int dpi_y = 0;
            static bool should_initialize = true;

            if (should_initialize) {
                should_initialize = false;
                HDC screen_dc = ::GetDC(NULL);
                // This value is safe to cache for the life time of the app since the
                // user must logout to change the DPI setting. This value also applies
                // to all screens.
                if (NULL != screen_dc) {
                    dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
                    dpi_y = GetDeviceCaps(screen_dc, LOGPIXELSY);
                    ::ReleaseDC(NULL, screen_dc);
                } else {
                    dpi_x = 120;
                    dpi_y = 120;
                } 
            }
            SIZE size = { dpi_x, dpi_y };
            return size;
        }

        void InitDeviceScaleFactor() {
            g_device_scale_factor = static_cast<float>(GetDPI().cx) / static_cast<float>(kDefaultDPIX);
        }

        float GetDeviceScaleFactor() {
            return g_device_scale_factor;
        }

        int GetSystemMetricsInDIP(int metric) {
            return static_cast<int>(GetSystemMetrics(metric) / GetDeviceScaleFactor() + 0.5);
        }

    } // namespace win

} // namespace gfx