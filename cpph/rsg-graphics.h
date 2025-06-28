#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "../cpph/rsg-datatypes.h"

class RsgGuiEngine {
	RsgEngine* parent_window;
public:
	RsgGuiEngine(RsgEngine* window);
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