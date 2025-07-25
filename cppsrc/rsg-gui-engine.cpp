#include <string>
#include <SDL3/SDL.h>
#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-sdl-engine.h"
#include "../cpph/rsg-gui-engine.h"
#include "../cpph/rsg-component.hpp"
#include "../cpph/rsg-window.hpp"
#include "../cpph/rsfonts.h"

//comment

/* RsgGuiEngine Class constructor */
RsgGuiEngine::RsgGuiEngine(RsgEngine* engine) {
	sdlEngine = engine;
	sdlEngine->SetGuiEngine(this);

	renderableComponent = NULL;
	lastMouseOver = NULL;
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
		bgCol,
		this
	);
	SetRenderableComponent(main_window);
	return main_window;
}

/* Gui engine event handler - called by sdl event handler */
SDL_AppResult RsgGuiEngine::Event(SDL_Event* event) {
	//SDL_Log("Graphics event!");
	//todo handle various events

	//todo this one's getting hefty - move to new class?

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		/* Get char under current mouse position */
		SDL_MouseMotionEvent mouse_event = event->motion;
		rsd::uint2 mouse_charpos =
			sdlEngine->ScreenPointToCharXY(
				rsd::float2(mouse_event.x, mouse_event.y)
			);
		Uint32 mouse_charindex = sdlEngine->PointToIndex(mouse_charpos);
		//SDL_Log("Mouse is at char x:%i, y:%i", mouse_charpos.x, mouse_charpos.y);

		/* Get component at char under mouse */
		rsgui::Component* thisMouseOver = NULL;
		if (mouse_charindex < sdlEngine->GetCharacterCount()) {
			//mouse left window
			thisMouseOver = sdlEngine->charMetadata[mouse_charindex].charOwner;
		}
			

		/* Check if element under mouse has changed */
		if (lastMouseOver != thisMouseOver) {
			//SDL_Log("Moused over new element?");
			rsgui::Selectable* lastSelectable =
				dynamic_cast<rsgui::Selectable*>(lastMouseOver);
			//Clear mouse state if mouse has left last selectable element
			if (lastSelectable != NULL) {
				//Clear highlight and selected colour style
				lastSelectable->SetHighlighted(false);
				lastSelectable->SetSelected(false);
			}

			//Last selectable element is now the new element
			if (thisMouseOver != NULL) {
				lastMouseOver = thisMouseOver;
			}

			rsgui::Selectable* thisSelectable = 
				dynamic_cast<rsgui::Selectable*>(thisMouseOver);
			//Set mouse state for new element
			//ignores mouse button already down
			if (thisSelectable != NULL) {
				//Set using highlighted colour style
				thisSelectable->SetHighlighted(true); 
				//Callback: Mouse entered element
				thisSelectable->OnHighlighted(event);
			}
		}

		//mouse movement event if already selected
		rsgui::Selectable* thisSelectable =
			dynamic_cast<rsgui::Selectable*>(lastMouseOver);
		bool isSelected = false;
		//Set mouse state for new element
		//ignores mouse button already down
		if (thisSelectable != NULL) {
			isSelected = thisSelectable->GetSelected();
			if (isSelected) {
				thisSelectable->OnDragged(event);
			}
		}
		
		//lastMouseOver;
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		//SDL_Log("Mouse button down event");
		rsgui::Selectable* lastSelectable =
			dynamic_cast<rsgui::Selectable*>(lastMouseOver);
		if (lastSelectable != NULL) {
			lastSelectable->SetSelected(true);
			lastSelectable->OnMouseDown(event);
		}
		//todo left/right/middle click?
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		//SDL_Log("Mouse button up event");
		rsgui::Selectable* lastSelectable =
			dynamic_cast<rsgui::Selectable*>(lastMouseOver);
		if (lastSelectable != NULL) {
			//mouse up event
			lastSelectable->OnMouseUp(event);

			//Accept full button 'click' cycle
			if (lastSelectable->GetSelected()) {
				//SDL_Log("Mouse clicked event!");
				//Callback: mouse button clicked (todo left/primary)
				lastSelectable->OnSelected(event);
				//Clear selected colour style
				lastSelectable->SetSelected(false);
			}
		}
		//todo left/right/middle click?
	}

	//todo pass event to event listeners

	return SDL_APP_CONTINUE;
};

void RsgGuiEngine::RequestClose() {
	/*SDL_Event quit_event{
		.type = SDL_EVENT_QUIT
	};

	sdlEngine->PushEvent(&quit_event);*/

	sdlEngine->CloseWindow();

}

void RsgGuiEngine::RequestMinimise() {
	/*SDL_Event minimise_event{
		.type = SDL_EVENT_WINDOW_MINIMIZED //doesn't work lol
	};

	sdlEngine->PushEvent(&minimise_event);*/

	sdlEngine->MinimiseWindow();
	//feels a bit cheaty but i guess it fits abstraction/SoC
}

void RsgGuiEngine::RequestMoveWindow(rsd::float2 relmove) {
	sdlEngine->MoveWindow(relmove);
}

void RsgGuiEngine::RequestTrapMouse(bool trapped) {
	sdlEngine->TrapMouse(trapped);
}

rsd::int2 RsgGuiEngine::GetParentWindowPosition() {
	return sdlEngine->GetWindowPosition();
}

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