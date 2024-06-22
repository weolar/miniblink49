#ifndef ui_gfx_win_dpi_h
#define ui_gfx_win_dpi_h

namespace gfx {

namespace win {

void InitDeviceScaleFactor();
float GetDeviceScaleFactor();
int GetSystemMetricsInDIP(int metric);

} // namespace win

} // namespace gfx

#endif // ui_gfx_win_dpi_h