#pragma once
#include <string>
#include <functional>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"
#include "rsfonts.h"

namespace rsgui {
	typedef void(rsgui::MouseListener::* MouseCallback )(rsgui::Component*);

	enum ButtonBorder {
		RSG_BUTTON_BORDER_NONE,
		RSG_BUTTON_BORDER_SINGLE,
		RSG_BUTTON_BORDER_DOUBLE,
		RSG_BUTTON_BORDER_TITLE
	};

	class Button : 
		public Component,
		public Selectable,
		public Inhibitable
	{
	private:
		ButtonBorder buttonBorder;
		std::string buttonText;
		//void (*mouseClickCallback)();
		//specifically using a pointer to member of MouseListener class???
		MouseListener* mouseListener;
		MouseCallback mouseClickCallback;
	public:
		Button(
			rsd::uint2 position,
			rsd::uint2 size,
			std::string label,
			rsd::float4 fgCol,
			rsd::float4 bgCol
		) : 
			buttonText(label)
		{
			this->position = position;
			this->size = size;

			this->fgCol = fgCol;
			this->highlightedFgCol = fgCol;
			this->selectedFgCol = fgCol;
			this->inhibitedFgCol = fgCol;

			this->bgCol = bgCol;
			this->highlightedBgCol = bgCol;
			this->selectedBgCol = bgCol;
			this->inhibitedBgCol = bgCol;

			this->highlightable = true;
			this->highlighted = false;

			this->selectable = true;
			this->selected = false;

			this->inhibited = false;

			buttonBorder = RSG_BUTTON_BORDER_NONE;
		}

		void SetBorder(ButtonBorder border) {
			buttonBorder = border;
		}

		void SetText(std::string text) {
			buttonText = text;
		}

		void SetClickedCallback(MouseListener* mouseListener, MouseCallback mouseClickCallback) {
			this->mouseListener = mouseListener;
			this->mouseClickCallback = mouseClickCallback;
		}

		//todo?
		/*void OnHighlighted() {

		}*/

		void OnSelected() {
			if (mouseListener == NULL || mouseClickCallback == NULL) { return; }
			(mouseListener->*mouseClickCallback)(this);
		}

		Uint32 GetBorderChar(Uint32 offset) {
			Uint32 ret_code = ' ';
			switch (buttonBorder) {
			case RSG_BUTTON_BORDER_SINGLE:
				ret_code = RSG_LINE_SINGLE + offset;
				break;
			case RSG_BUTTON_BORDER_DOUBLE:
				ret_code = RSG_LINE_DOUBLE + offset;
				break;
			case RSG_BUTTON_BORDER_TITLE:
				ret_code = RSG_TITLE_BORDERS + offset;
				break;
			}
			return ret_code;
		}

		/* Component rendering interface methods */
		void Repaint(RsgEngine* engine) {
			//todo dirty chars
			Paint(engine); //debug just paint for now
		}
		void Paint(RsgEngine* engine) {
			/* Temporary character data object for blitting */
			rsd::CharData* tmp_char = new rsd::CharData(
				fgCol, bgCol, ' ', ' '
			);

			if (highlighted) {
				tmp_char->SetForegroundColour(highlightedFgCol);
				tmp_char->SetBackgroundColour(highlightedBgCol);
			}
			if (selected) {
				tmp_char->SetForegroundColour(selectedFgCol);
				tmp_char->SetBackgroundColour(selectedBgCol);
			}

			//todo actually draw the button

			//todo also draw borders if needed

			for (Uint32 iy = position.y; iy < position.y + size.y; ++iy) {
				//draw each line of the button
				engine->FillCharacter(
					engine->PointToIndex(position.x, iy),
					size.x,
					tmp_char,
					this
				);
			}
			//insert text
			Uint32 text_rel_start = ((size.x - buttonText.length()) / 2) 
				+ (position.y * engine->GetDisplaySize().x);

			for (Uint32 c = 0; c < buttonText.length(); ++c) {
				tmp_char->char1 = buttonText.at(c);
				engine->SetCharacter(
					position.x + text_rel_start + c,
					tmp_char,
					this
				);
			}

			//todo clean up
			if (buttonBorder == RSG_BUTTON_BORDER_TITLE) {
				//draw preceeding title break
				tmp_char->char1 = GetBorderChar(TITLE_BREAK_RIGHT);
				engine->SetCharacter(
					position.x,
					tmp_char,
					this
				);
				//draw proceeding title break
				tmp_char->char1 = GetBorderChar(TITLE_BREAK_LEFT);
				engine->SetCharacter(
					position.x + size.x - 1,
					tmp_char,
					this
				);
			}
		}
	};
}