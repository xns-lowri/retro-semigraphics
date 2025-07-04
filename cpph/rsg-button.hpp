#pragma once
#include <string>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"

namespace rsgui {
	enum ButtonDecoration {
		RSG_BUTTON_BORDER_NONE,
		RSG_BUTTON_BORDER_SINGLE,
		RSG_BUTTON_BORDER_DOUBLE
	};

	class Button : 
		public Component,
		public Highlightable,
		public Enableable
	{
		ButtonDecoration buttonDecoration;
		std::string buttonText;
	public:
		Button(
			rsd::uint2 position,
			rsd::uint2 size,
			std::string label,
			rsd::float4 fgCol,
			rsd::float4 bgCol,
			bool enabled
		) : 
			buttonText(label)
		{
			this->position = position;
			this->size = size;

			this->fgCol = fgCol;
			this->fgColHighlight = fgCol;
			this->fgColDisabled = fgCol;

			this->bgCol = bgCol;
			this->bgColHighlight = bgCol;
			this->bgColDisabled = bgCol;

			this->enabled = enabled;
			buttonDecoration = RSG_BUTTON_BORDER_NONE;
		}

		void SetText(std::string label) {
			buttonText = label;
		}
	};
}