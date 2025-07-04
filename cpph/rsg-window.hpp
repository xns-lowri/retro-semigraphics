#pragma once
#include <string>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"
#include "rsg-button.hpp"
#include "rsfonts.h"

//class Canvas;
//class HScroll;
//class VScroll;

namespace rsgui {
	enum WindowType {
		RSG_WINDOWTYPE_MAIN
		//todo external sub-windows?
	};
	enum WindowDecoration {
		RSG_WINDOW_BORDER_NONE,
		RSG_WINDOW_BORDER_SINGLE,
		RSG_WINDOW_BORDER_DOUBLE
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
			this->windowDecoration = RSG_WINDOW_BORDER_SINGLE;
		}

		void SetTitle(std::string title) {
			this->windowTitle = title;
		}

		void SetBorder(WindowDecoration style) {
			this->windowDecoration = style;
		}

		Uint32 GetDecorationChar(Uint32 offset) {
			switch (windowDecoration) {
			case RSG_WINDOW_BORDER_SINGLE:
				return(RSG_LINE_SINGLE + offset);
				break;
			case RSG_WINDOW_BORDER_DOUBLE:
				return(RSG_LINE_DOUBLE + offset);
				break;
			default:
				return ' ';
				break;
			}
		}

		Uint32 GetTitleDecorationChar(Uint32 offset) {
			//todo only works for corners atm
			switch (windowDecoration) {
			case RSG_WINDOW_BORDER_SINGLE:
				return(RSG_TITLE_BORDERS + offset);
				break;
			case RSG_WINDOW_BORDER_DOUBLE:
				return(RSG_TITLE_BORDERS + offset + 2);
				break;
			default:
				return RSG_TITLE_BORDERS + TITLE_FULL;
				break;
			}
		}

		void Repaint(RsgEngine* engine) {
			//todo only paint dirty cells??
			Paint(engine); //debug just paint
		}
		void Paint(RsgEngine* engine) { //todo
			rsd::CharData* tmp_char = new rsd::CharData(
				fgCol, bgCol, ' ', ' '
			);

			//draw title row
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_FULL;
			engine->FillCharacter(
				engine->PointToIndex(position.x, position.y),
				size.x,
				tmp_char);

			tmp_char->char1 = GetTitleDecorationChar(TITLE_RIGHT_JOIN);
			engine->SetCharacter(
				engine->PointToIndex(position.x, position.y),
				tmp_char);

			tmp_char->char1 = GetTitleDecorationChar(TITLE_LEFT_JOIN);
			engine->SetCharacter(
				engine->PointToIndex(position.x + size.x - 1, position.y),
				tmp_char);

			//draw title text
			Uint32 title_rel_start = (size.x - windowTitle.length() + 2) / 2;
			//before title border join
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_LEFT;
			engine->SetCharacter(
				engine->PointToIndex(position.x + title_rel_start, position.y),
				tmp_char);
			//after title border joint
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_RIGHT;
			engine->SetCharacter(
				engine->PointToIndex(
					position.x + title_rel_start + windowTitle.length() + 1, 
					position.y),
				tmp_char);

			for (Uint32 c = 0; c < windowTitle.length(); ++c) {
				tmp_char->char1 = windowTitle.at(c);
				engine->SetCharacter(
					position.x + title_rel_start + 1 + c,
					tmp_char
				);
			}

			//draw middle rows
			for (Uint32 iy = position.y + 1; iy < position.y + size.y - 1; ++iy) {
				/*Uint32 cur_ind = position.x
					+ (position.y * engine->GetDisplaySize()->x);*/
				tmp_char->char1 = ' ';
				engine->FillCharacter(
					engine->PointToIndex(position.x + 1, iy), 
					size.x-2, 
					tmp_char);

				tmp_char->char1 = GetDecorationChar(LINE_VERT);
				engine->SetCharacter(
					engine->PointToIndex(position.x, iy), 
					tmp_char);
				engine->SetCharacter(
					engine->PointToIndex(position.x + size.x - 1, iy), 
					tmp_char);
			}

			//draw bottom row
			tmp_char->char1 = GetDecorationChar(LINE_HORIZ);
			engine->FillCharacter(
				engine->PointToIndex(position.x + 1, position.y + size.y - 1), 
				size.x - 2, 
				tmp_char);

			//draw bottom corners
			tmp_char->char1 = GetDecorationChar(LINE_RA_UP_RIGHT);
			engine->SetCharacter(
				engine->PointToIndex(position.x, position.y + size.y - 1),
				tmp_char
			);

			tmp_char->char1 = GetDecorationChar(LINE_RA_UP_LEFT);
			engine->SetCharacter(
				engine->PointToIndex(position.x + size.x - 1, position.y + size.y - 1),
				tmp_char
			);
		}
	};
};