
#include "config.h"


#if !(((defined ENABLE_WKE) && (ENABLE_WKE == 1)) || ((defined ENABLE_CEF) && (ENABLE_CEF == 1)))
	#error	"Please select an interface     请选择接口"
#endif
