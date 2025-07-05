#include <string>
#include <SDL3/SDL.h>
#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-sdl-engine.h"
#include "../cpph/rsg-gui-engine.h"
#include "../cpph/rsg-component.hpp"
#include "../cpph/rsg-window.hpp"
#include "../cpph/rsfonts.h"

/* RsgGuiEngine Class constructor */
RsgGuiEngine::RsgGuiEngine(RsgEngine* engine) {
	sdlEngine = engine;
	sdlEngine->SetGuiEngine(this);

	renderableComponent = NULL;
}

rsgui::Window* RsgGuiEngine::InitEngineWindow(
	std::string windowTitle,
	rsd::float4 fgCol,
	rsd::float4 bgCol
) {
	rsgui::Window* main_window = new rsgui::Window(
		rsd::uint2(0, 0),
		sdlEngine->GetDisplaySize(),
		windowTitle,
		fgCol,
		bgCol
	);
	SetRenderableComponent(main_window);
	return main_window;
}

/* Gui engine event handler - called by sdl event handler */
SDL_AppResult RsgGuiEngine::Event(SDL_Event* event) {
	//SDL_Log("Graphics event!");
	
	//todo handle various events
	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		SDL_MouseMotionEvent mouse_event = event->motion;
		rsd::uint2 mouse_charpos =
			sdlEngine->ScreenPointToCharXY(
				rsd::float2(mouse_event.x, mouse_event.y)
			);
		Uint32 mouse_charindex = sdlEngine->PointToIndex(mouse_charpos);

		//SDL_Log("Mouse is at char x:%i, y:%i", mouse_charpos.x, mouse_charpos.y);

		rsgui::Component* thisMouseOver = 
			sdlEngine->charMetadata[mouse_charindex].charOwner;

		if (lastMouseOver != thisMouseOver) {
			//SDL_Log("Moused over new element?");
			rsgui::Selectable* lastSelectable =
				dynamic_cast<rsgui::Selectable*>(lastMouseOver);
			if (lastSelectable != NULL) {
				lastSelectable->SetHighlighted(false);
				lastSelectable->SetSelected(false);
			}

			rsgui::Selectable* thisSelectable = 
				dynamic_cast<rsgui::Selectable*>(thisMouseOver);
			if (thisSelectable != NULL) {
				thisSelectable->SetHighlighted(true);
			}

			lastMouseOver = thisMouseOver;
		}
		//lastMouseOver;
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		//SDL_Log("Mouse button down event");
		rsgui::Selectable* lastSelectable =
			dynamic_cast<rsgui::Selectable*>(lastMouseOver);
		if (lastSelectable != NULL) {
			lastSelectable->SetSelected(true);
		}
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		//SDL_Log("Mouse button up event");
		rsgui::Selectable* lastSelectable =
			dynamic_cast<rsgui::Selectable*>(lastMouseOver);
		if (lastSelectable != NULL) {
			if (lastSelectable->GetSelected()) {
				SDL_Log("Mouse clicked event!");
				//todo callback
				lastSelectable->SetSelected(false);
			}
		}
	}

	return SDL_APP_CONTINUE;
};

void RsgGuiEngine::Quit() {
	//todo delete all children
	delete(this);
}

/* Main render function - called from sdl engine */
void RsgGuiEngine::Render(RsgEngine* engine) {
	if (renderableComponent == NULL) {
		return;
	}

	//render prioritising children first
	RenderNext(engine, renderableComponent);
}

/* Render components recursively */
void RsgGuiEngine::RenderNext(RsgEngine* engine, rsgui::Component* component) {
	component->Repaint(engine); //repaint current component

	//check if current component is a container, and has child components
	rsgui::Container* container = dynamic_cast<rsgui::Container*>(component);
	if (container == NULL) { return; } //not container
	if (container->children == NULL) { return; } //no kids

	//call RenderNext on children in turn
	//prioritises child components over sibling components at every level
	for (Uint32 i = 0; i < container->children->Length(); ++i) {
		//recursion :( 
		rsgui::Component* next_kiddo = container->children->GetComponent(i);
		if (next_kiddo == NULL) { continue; } //shouldn't happen?
		//Render next child object
		RenderNext(engine, next_kiddo);
	}
}

//set main renderable component, add all renderables to comp
//to-don't preclude swapping renderable component to change screens
void RsgGuiEngine::SetRenderableComponent(rsgui::Component* comp) {
	renderableComponent = comp;
}

//debug function, todo refactor
//commented pending removal
/*bool RsgGuiEngine::drawWindow(
	rsd::uint2 origin,
	rsd::uint2 size,
	rsd::float4 titlefgcol,
	rsd::float4 titlebgcol,
	rsd::float4 windowfgcol,
	rsd::float4 windowbgcol,
	std::string titletext
)
{
	if (sdlEngine == NULL) 
	{ return false; }

	if ( (origin + size) > sdlEngine->GetDisplaySize() 
		|| (titletext.length() + 2) > size.x + 6 ) 
	{ return false; }

	Uint32 start_index = sdlEngine->PointToIndex(origin);

	//SDL_Log("Point at (%i, %i) = %i", origin.x, origin.y, start_index);

	//create windowTitle bg char
	rsd::CharData* title_char = new (rsd::CharData){
		titlefgcol.x, titlefgcol.y, titlefgcol.z, titlefgcol.w,
		titlebgcol.x, titlebgcol.y, titlebgcol.z, titlebgcol.w,
		RSG_TITLE_BORDERS + TITLE_FULL, ' ', 0, 0
	};
	//draw windowTitle bar bg
	sdlEngine->FillCharacter(start_index, size.x, title_char);

	//draw windowTitle text
	Uint32 title_rel_start = (size.x - titletext.length() + 2) / 2;

	//engine left joiner
	title_char->char1 = RSG_TITLE_BORDERS + TITLE_RIGHT_JOIN_SINGLE; //todo consts/defs for nonstandard chars
	sdlEngine->SetCharacter(
		start_index,
		title_char
	);
	//before windowTitle text joiner
	title_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_LEFT; //todo consts/defs for nonstandard chars
	sdlEngine->SetCharacter(
		start_index + title_rel_start,
		title_char
	);
	//after windowTitle text joiner
	title_char->char1 = RSG_TITLE_BORDERS + TITLE_HALF_RIGHT;
	sdlEngine->SetCharacter(
		start_index + 1 + title_rel_start + titletext.length(),
		title_char
	);
	//engine right joiner
	title_char->char1 = RSG_TITLE_BORDERS + TITLE_LEFT_JOIN_SINGLE; //todo consts/defs for nonstandard chars
	sdlEngine->SetCharacter(
		start_index + size.x - 1,
		title_char
	);

	for (Uint32 c = 0; c < titletext.length(); ++c) {
		title_char->char1 = titletext.at(c);
		sdlEngine->SetCharacter(
			start_index + 1 + title_rel_start + c, 
			title_char
		);
	}

	rsd::CharData* bg_char = new rsd::CharData(
		windowfgcol.x, windowfgcol.y, windowfgcol.z, windowfgcol.w,
		windowbgcol.x, windowbgcol.y, windowbgcol.z, windowbgcol.w,
		' ', ' ', 0, 0
	);
	for (Uint32 bgline = 1;
		bgline < size.y - 1;
		++bgline) 
	{
		rsd::uint2 next_origin = { 0, bgline };
		next_origin += origin;
		Uint32 line_index = sdlEngine->PointToIndex(next_origin);
		/ *SDL_Log("Line at (%i, %i) = %i",
			next_origin.x, next_origin.y, line_index);* /
		bg_char->char1 = ' ';
		sdlEngine->FillCharacter(line_index + 1, size.x - 2, bg_char);
		bg_char->char1 = RSG_LINE_SINGLE + LINE_VERT;
		sdlEngine->SetCharacter(line_index, bg_char);
		sdlEngine->SetCharacter(line_index + size.x - 1, bg_char);
	}

	rsd::uint2 next_origin = { 0, size.y - 1 };
	next_origin += origin;
	Uint32 line_index = sdlEngine->PointToIndex(next_origin);
	bg_char->char1 = RSG_LINE_SINGLE + LINE_HORIZ;
	sdlEngine->FillCharacter(line_index + 1, size.x - 2, bg_char);
	bg_char->char1 = RSG_LINE_SINGLE + LINE_RA_UP_RIGHT;
	sdlEngine->SetCharacter(line_index, bg_char);
	bg_char->char1 = RSG_LINE_SINGLE + LINE_RA_UP_LEFT;
	sdlEngine->SetCharacter(line_index + size.x - 1, bg_char);



	return true;
}*/