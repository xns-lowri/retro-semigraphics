#pragma once
#include <string>
#include <functional>
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
	enum WindowBorder {
		RSG_WINDOW_BORDER_NONE,
		RSG_WINDOW_BORDER_SINGLE,
		RSG_WINDOW_BORDER_DOUBLE
	};

	class Window: 
		public Component, 
		public Container,
		public Selectable,
		public MouseListener
	{
		//rsd::uint2 position;
		//rsd::uint2 size;
		WindowType windowType;
		WindowBorder windowBorder;
		std::string windowTitle;
		//Canvas* canvas; //todo available internal drawing area??

		rsd::float4 titleFgCol;
		rsd::float4 titleBgCol;

		RsgGuiEngine* parent;

		Button* quit_btn;
		Button* minimise_btn;

		rsd::float2 windowDragOrigin;

	public:
		//todo other constructors probably
		/* Window Constructor */
		Window(
			rsd::uint2 position,
			rsd::uint2 size,
			std::string title,
			rsd::float4 fgCol,
			rsd::float4 bgCol,
			RsgGuiEngine* parent
			//may need to add WindowType param if reusing this class for sub-windows?
		) 
		{
			this->position = position;
			this->size = size;
			this->windowTitle = title;
			this->titleFgCol = this->fgCol = fgCol;
			this->titleBgCol = this->bgCol = bgCol;
			

			this->windowType = RSG_WINDOWTYPE_MAIN;
			this->windowBorder = RSG_WINDOW_BORDER_SINGLE;

			this->children = new ComponentList();

			this->quit_btn = NULL;
			this->minimise_btn = NULL;

			this->windowDragOrigin = rsd::float2(0, 0);

			this->parent = parent;
		}

		/* Create quit, minimise, maximise buttons for main window */
		void CreateMainWindowControls() {
			const rsd::uint2 btn_size = rsd::uint2(3, 1);

			/* Main window has quit button */
			std::string quit_btn_text = "x";
			quit_btn = new Button(
				rsd::uint2( //button pos
					(position.x + size.x) - (1 + btn_size.x),
					position.y
				),
				btn_size, //button size
				quit_btn_text,
				titleFgCol,
				titleBgCol
			);
			//set quit button properties
			quit_btn->SetBorder(RSG_BUTTON_BORDER_TITLE);
			quit_btn->SetHighlightedBackgroundColour(
				rsd::float4(0.5f, 0.0f, 0.0f, 1.0f)
			);
			quit_btn->SetSelectedForegroundColour(
				rsd::float4(0.5f, 0.0f, 0.0f, 1.0f)
			);
			quit_btn->SetSelectedBackgroundColour(
				rsd::float4(0.8f, 0.8f, 0.8f, 1.0f)
			);

			quit_btn->SetClickedCallback(
				this,
				&rsgui::MouseListener::MouseClicked //omg seriously hwut
			);

			/*quit_btn->SetClickedCallback(
				this,
				static_cast<rsgui::MouseCallback>(MouseClicked);
			);
			
			void(*callback)(Window*) = [](Window* thi) {
				thi->QuitButtonClicked();
			};
			quit_btn->SetClickedCallback(
				callback(this)
			);*/

			//std::function<void(void)> stdf_callback = &QuitButtonClicked;
			//quit_btn->SetClickedCallback(stdf_callback);

			//nearly there i swear

			children->AddComponent(quit_btn);

			/* Main window has minimise button */
			std::string minimise_btn_text = "_";
			minimise_btn = new Button(
				rsd::uint2( //button pos
					(position.x + size.x) - (1 + (2 * btn_size.x)),
					position.y
				),
				btn_size, //button size
				minimise_btn_text,
				titleFgCol,
				titleBgCol
			);
			//set minimise button properties
			minimise_btn->SetBorder(RSG_BUTTON_BORDER_TITLE);
			minimise_btn->SetHighlightedBackgroundColour(
				rsd::float4(0.02f, 0.05f, 0.5f, 1.0f)
			);
			minimise_btn->SetSelectedForegroundColour(
				rsd::float4(0.02f, 0.05f, 0.5f, 1.0f)
			);
			minimise_btn->SetSelectedBackgroundColour(
				rsd::float4(0.8f, 0.8f, 0.8f, 1.0f)
			);

			//plsplspls
			minimise_btn->SetClickedCallback(
				this,
				&rsgui::MouseListener::MouseClicked 
			);

			children->AddComponent(minimise_btn);

			//Todo maximise/restore? Deal with resizing l8r D=
		}

		void SetTitle(std::string title) {
			this->windowTitle = title;
		}

		void SetBorder(WindowBorder style) {
			this->windowBorder = style;
		}

		void SetTitleForegroundColour(
			rsd::float4 titleFgCol
		) {
			this->titleFgCol = titleFgCol;
		}
		void SetTitleBackgroundColour(
			rsd::float4 titleBgCol
		) {
			this->titleBgCol = titleBgCol;
		}
		void SetTitleColours(
			rsd::float4 titleFgCol,
			rsd::float4 titleBgCol
		) {
			this->titleFgCol = titleFgCol;
			this->titleBgCol = titleBgCol;
		}

		//Get appropriate single/double/no line glyph codes
		Uint32 GetBorderChar(Uint32 offset) {
			switch (windowBorder) {
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

		//todo doesn't always work reyt good like
		Uint32 GetTitleBorderChar(Uint32 offset) {
			//only works for corners atm b/c offsets
			switch (windowBorder) {
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

		//inherited from MouseListener
		void MouseClicked(
			SDL_Event* event, 
			rsgui::Component* element
		) {
			//SDL_Log("Nearly a working event system?");
			//SDL_Log("this: %i", this);
			if (element == quit_btn) {
				parent->RequestClose();
			}
			if (element == minimise_btn) {
				parent->RequestMinimise();
			}
		}

		//inherited from MouseListener
		void MouseDragged(
			SDL_Event* event, 
			rsgui::Component* element
		) {

			//SDL_Log("Mouse dragged!");
			parent->RequestMoveWindow(
				rsd::float2(
					event->motion.xrel,// - windowDragOrigin.x,
					event->motion.yrel // - windowDragOrigin.y
				)
			);
		}

		//inherited from Selectable
		void OnDragged(SDL_Event* event) {
			MouseDragged(event, this);
		}
		
		void OnMouseDown(SDL_Event* event) {
			//todo only if mouse is over title bar

			//ugh this is a rabbit hole in itself
			//whoever knew a gui engine would be so hard??
			parent->RequestTrapMouse(true);

			windowDragOrigin = rsd::float2(
				event->motion.x,
				event->motion.y
			);
			SDL_Log("Set drag origin");
			//todo
		}
		void OnMouseUp(SDL_Event* event) {
			parent->RequestTrapMouse(false);
		}

		/* Paint methods */
		void Repaint(RsgEngine* engine) {
			//todo only paint dirty cells??
			Paint(engine); //debug just paint
		}
		void Paint(RsgEngine* engine) { //todo
			/* Temporary character data object for blitting */
			rsd::CharData* tmp_char = new rsd::CharData(
				titleFgCol, titleBgCol, ' ', ' '
			);

			//todo better method for drawing various box components?
			//probably going to be a lot of duplicated code, hopefully
			//the solution becomes clear soon D:

			//todo need to be able to individually/randomly
			//draw certain (dirty) pixels

			//draw title row
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_FULL;
			engine->FillCharacter(
				engine->PointToIndex(position.x, position.y),
				size.x,
				tmp_char,
				this
			);
			//draw title corner joins
			tmp_char->char1 = GetTitleBorderChar(TITLE_RIGHT_JOIN);
			engine->SetCharacter(
				engine->PointToIndex(position.x, position.y),
				tmp_char,
				this
			);
			tmp_char->char1 = GetTitleBorderChar(TITLE_LEFT_JOIN);
			engine->SetCharacter(
				engine->PointToIndex(position.x + size.x - 1, position.y),
				tmp_char,
				this
			);

			//BBIG TODO how do I stop too long windowTitle from doing bad stufff???
			// ****************** must fix asap *******************
			
			//draw title text
			Uint32 title_rel_start = (size.x - (windowTitle.length() + 2)) / 2;
			//before title border join
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_LEFT;
			engine->SetCharacter(
				engine->PointToIndex(position.x + title_rel_start, position.y),
				tmp_char,
				this
			);
			//after title border joint
			tmp_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_RIGHT;
			engine->SetCharacter(
				engine->PointToIndex(
					position.x + title_rel_start + windowTitle.length() + 1, 
					position.y),
				tmp_char,
				this
			);
			//insert text
			for (Uint32 c = 0; c < windowTitle.length(); ++c) {
				tmp_char->char1 = windowTitle.at(c);
				engine->SetCharacter(
					position.x + title_rel_start + 1 + c,
					tmp_char,
					this
				);
			}

			tmp_char->SetForegroundColour(fgCol);
			tmp_char->SetBackgroundColour(bgCol);

			//draw middle rows
			for (Uint32 iy = position.y + 1; iy < position.y + size.y - 1; ++iy) {
				/*Uint32 cur_ind = position.x
					+ (position.y * engine->GetDisplaySize()->x);*/
				tmp_char->char1 = ' ';
				engine->FillCharacter(
					engine->PointToIndex(position.x + 1, iy), 
					size.x-2, 
					tmp_char,
					this
				);

				tmp_char->char1 = GetBorderChar(LINE_VERT);
				engine->SetCharacter(
					engine->PointToIndex(position.x, iy), 
					tmp_char,
					this
				);
				engine->SetCharacter(
					engine->PointToIndex(position.x + size.x - 1, iy), 
					tmp_char,
					this
				);
			}

			//draw bottom row
			tmp_char->char1 = GetBorderChar(LINE_HORIZ);
			engine->FillCharacter(
				engine->PointToIndex(position.x + 1, position.y + size.y - 1), 
				size.x - 2, 
				tmp_char,
				this
			);

			//draw bottom corners
			tmp_char->char1 = GetBorderChar(LINE_RA_UP_RIGHT);
			engine->SetCharacter(
				engine->PointToIndex(position.x, position.y + size.y - 1),
				tmp_char,
				this
			);
			tmp_char->char1 = GetBorderChar(LINE_RA_UP_LEFT);
			engine->SetCharacter(
				engine->PointToIndex(position.x + size.x - 1, position.y + size.y - 1),
				tmp_char,
				this
			);
		}
	};
};