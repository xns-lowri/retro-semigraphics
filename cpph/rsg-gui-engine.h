#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "rsg-datatypes.h"
#include "rsg-component.hpp"

class RsgEngine;

class RsgGuiEngine {
	/* Class variables */
	RsgEngine* parentWindow;	//pointer to rsg-sdl engine instance
	rsgui::Component* renderableComponent; //main renderable component
	//std::string titleText;		//window title text

public:
	RsgGuiEngine(RsgEngine* window);

	//sdl-like functions
	SDL_AppResult Event(SDL_Event* event);
	void Quit();

	//Render method called by sdl engine itself
	void Render(RsgEngine* engine);

	void SetRenderableComponent(rsgui::Component* comp);


	//debug, TODO: object-based rendering
	bool drawWindow(
		rsd::uint2 origin,
		rsd::uint2 extent,
		rsd::float4 titlefgcol,
		rsd::float4 titlebgcol,
		rsd::float4 windowfgcol,
		rsd::float4 windowbgcol,
		std::string titletext
	);
};
