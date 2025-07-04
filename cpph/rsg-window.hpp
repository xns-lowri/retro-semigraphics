#pragma once
#include <string>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"

//class Canvas;
//class HScroll;
//class VScroll;

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

	class Window: public Component, public Container {
		//rsd::uint2 position;
		//rsd::uint2 size;
		WindowType windowType;
		WindowDecoration windowDecoration;
		std::string windowTitle;
		//Canvas* canvas; //todo??

	public:
		Window(
			rsd::uint2 position,
			rsd::uint2 size,
			std::string title,
			rsd::float4 fgCol,
			rsd::float4 bgCol
		) 
		{
			this->position = position;
			this->size = size;
			this->windowTitle = title;
			this->fgCol = fgCol;
			this->bgCol = bgCol;

			this->windowType = RSG_WINDOWTYPE_MAIN;
			this->windowDecoration = RSG_WINDOW_DECORATE_ALL;
		}

		void SetTitle(std::string title) {
			this->windowTitle = title;
		}

		void Repaint() { //todo

		}
		void Paint() { //todo

		}
	};
};