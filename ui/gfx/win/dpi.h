#ifndef dpi_h
#define dpi_h

namespace gfx {

    namespace win {

        void InitDeviceScaleFactor();

        float GetDeviceScaleFactor();

        int GetSystemMetricsInDIP(int metric);

    } // namespace win

} // namespace gfx

#endif // dpi_h