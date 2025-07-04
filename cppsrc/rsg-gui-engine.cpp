#include <string>
#include <SDL3/SDL.h>
#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-sdl-engine.h"
#include "../cpph/rsg-gui-engine.h"
#include "../cpph/rsfonts.h"

/* RsgGuiEngine Class constructor */
	RsgGuiEngine::RsgGuiEngine(RsgEngine* window) {
		parentWindow = window;
		parentWindow->SetGuiEngine(this);
	}

/* Gui engine event handler - called by sdl event handler */
	SDL_AppResult RsgGuiEngine::Event(SDL_Event* event) {
		//SDL_Log("Graphics event!");
		return SDL_APP_CONTINUE;
	};

	void RsgGuiEngine::Quit() {
		//todo delete all children
		delete(this);
	}

	void RsgGuiEngine::Render(RsgEngine* engine) {
		if (renderableComponent == NULL) {
			return;
		}
		renderableComponent->Repaint(engine);
	}

	//set main renderable component
	void RsgGuiEngine::SetRenderableComponent(rsgui::Component* comp) {
		renderableComponent = comp;
	}

	//debug function, todo refactor
	bool RsgGuiEngine::drawWindow(
		rsd::uint2 origin,
		rsd::uint2 size,
		rsd::float4 titlefgcol,
		rsd::float4 titlebgcol,
		rsd::float4 windowfgcol,
		rsd::float4 windowbgcol,
		std::string titletext
	)
	{
		if (parentWindow == NULL) {
			return false;
		}

		rsd::uint2* display_extents = parentWindow->GetDisplaySize();

		if ((origin + size) > *display_extents ||
			(titletext.length() + 2) > size.x + 6) {
			return false;
		}

		Uint32 start_index = parentWindow->PointToIndex(origin);

		//SDL_Log("Point at (%i, %i) = %i", origin.x, origin.y, start_index);

		//create title bg char
		rsd::CharData* title_char = new (rsd::CharData){
			titlefgcol.x, titlefgcol.y, titlefgcol.z, titlefgcol.w,
			titlebgcol.x, titlebgcol.y, titlebgcol.z, titlebgcol.w,
			RSG_TITLE_BORDERS + TITLE_FULL, ' ', 0, 0
		};
		//draw title bar bg
		parentWindow->FillCharacter(start_index, size.x, title_char);

		//draw title text
		Uint32 title_rel_start = (size.x - titletext.length() + 2) / 2;

		//window left joiner
		title_char->char1 = RSG_TITLE_BORDERS + TITLE_RIGHT_JOIN_SINGLE; //todo consts/defs for nonstandard chars
		parentWindow->SetCharacter(
			start_index,
			title_char
		);
		//before title text joiner
		title_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_LEFT; //todo consts/defs for nonstandard chars
		parentWindow->SetCharacter(
			start_index + title_rel_start,
			title_char
		);
		//after title text joiner
		title_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_RIGHT;
		parentWindow->SetCharacter(
			start_index + 1 + title_rel_start + titletext.length(),
			title_char
		);
		//window right joiner
		title_char->char1 = RSG_TITLE_BORDERS + TITLE_LEFT_JOIN_SINGLE; //todo consts/defs for nonstandard chars
		parentWindow->SetCharacter(
			start_index + size.x - 1,
			title_char
		);

		for (Uint32 c = 0; c < titletext.length(); ++c) {
			title_char->char1 = titletext.at(c);
			parentWindow->SetCharacter(
				start_index + 1 + title_rel_start + c, 
				title_char
			);
		}

		rsd::CharData* bg_char = new (rsd::CharData){
			windowfgcol.x, windowfgcol.y, windowfgcol.z, windowfgcol.w,
			windowbgcol.x, windowbgcol.y, windowbgcol.z, windowbgcol.w,
			' ', ' ', 0, 0
		};
		for (Uint32 bgline = 1;
			bgline < size.y - 1;
			++bgline) 
		{
			rsd::uint2 next_origin = { 0, bgline };
			next_origin += origin;
			Uint32 line_index = parentWindow->PointToIndex(next_origin);
			/*SDL_Log("Line at (%i, %i) = %i",
				next_origin.x, next_origin.y, line_index);*/
			bg_char->char1 = ' ';
			parentWindow->FillCharacter(line_index + 1, size.x - 2, bg_char);
			bg_char->char1 = RSG_LINE_SINGLE + LINE_VERT;
			parentWindow->SetCharacter(line_index, bg_char);
			parentWindow->SetCharacter(line_index + size.x - 1, bg_char);
		}

		rsd::uint2 next_origin = { 0, size.y - 1 };
		next_origin += origin;
		Uint32 line_index = parentWindow->PointToIndex(next_origin);
		bg_char->char1 = RSG_LINE_SINGLE + LINE_HORIZ;
		parentWindow->FillCharacter(line_index + 1, size.x - 2, bg_char);
		bg_char->char1 = RSG_LINE_SINGLE + LINE_RA_UP_RIGHT;
		parentWindow->SetCharacter(line_index, bg_char);
		bg_char->char1 = RSG_LINE_SINGLE + LINE_RA_UP_LEFT;
		parentWindow->SetCharacter(line_index + size.x - 1, bg_char);



		return true;
	}