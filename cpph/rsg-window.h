#pragma once
#include <string>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"

class Canvas;
class HScroll;
class VScroll;

namespace rsgui {
	enum WindowType {
		RSG_WINDOWTYPE_MAIN
		//todo external sub-windows?
	};
	enum WindowDecoration {
		RSG_WINDOW_DECORATE_NONE,
		RSG_WINDOW_DECORATE_TITLE,
		RSG_WINDOW_DECORATE_ALL
	};

	class Window: public Component {
		//rsd::uint2 position;
		//rsd::uint2 size;
		WindowType windowType;
		WindowDecoration windowDecoration;

		std::string title;

		Canvas* canvas; //todo??

	public:
		Window();

	};
};