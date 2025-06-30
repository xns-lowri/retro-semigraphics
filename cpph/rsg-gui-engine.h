#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "rsg-datatypes.h"

class RsgEngine;

class RsgGuiEngine {
	/* Class variables */
	RsgEngine* parentWindow;	//pointer to rsg-sdl engine instance

	std::string titleText;		//window title text

public:
	RsgGuiEngine(RsgEngine* window);

	SDL_AppResult Event(SDL_Event* event);
	void Quit();

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
