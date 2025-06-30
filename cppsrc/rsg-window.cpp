#include <string>
#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-window.h"

namespace rsgui {
	Window::Window() {
		position = rsd::uint2{ 0,0 };
		size = rsd::uint2{ 0,0 };
		windowType = RSG_WINDOWTYPE_MAIN;
		windowDecoration = RSG_WINDOW_DECORATE_ALL;

		title = "";

		canvas = NULL;
	};
};