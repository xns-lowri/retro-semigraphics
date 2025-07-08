#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "rsg-datatypes.h"
//#include "rsg-component.hpp"

class RsgEngine;
namespace rsgui {
	class Component;
	class Window;
}

class RsgGuiEngine {
	/* Class variables */
	RsgEngine* sdlEngine;				//pointer to rsg-sdl engine instance
	rsgui::Component* renderableComponent;	//main renderable component
	//std::string titleText;		//window title text

	//todo soc?
	rsgui::Component* lastMouseOver;

public:
	RsgGuiEngine(RsgEngine* engine);
	rsgui::Window* InitEngineWindow(
		std::string title,
		rsd::float4 fgCol,
		rsd::float4 bgCol
	);

	//sdl-like functions
	SDL_AppResult Event(SDL_Event* event);
	void RequestClose();
	void RequestMinimise();
	void RequestMoveWindow(rsd::float2 relmove);

	void RequestTrapMouse(bool trapped);

	rsd::int2 GetParentWindowPosition();

	void Quit();

	//Render method called by sdl engine itself
	void Render(RsgEngine* engine);
	void RenderNext(RsgEngine* engine, rsgui::Component* component);

	void SetRenderableComponent(rsgui::Component* comp);
};
