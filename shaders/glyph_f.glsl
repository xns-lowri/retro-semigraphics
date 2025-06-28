#version 460

//font sheet image from app
layout (set=2, binding=0) uniform sampler2D fontImage;

//glyph co-ords from vert shader
//tint passed from app
layout (location=0) in vec2 glyphCoords;
layout (location=1) in vec2 glyph2Coords;
layout (location=2) in vec4 glyphTint;
layout (location=3) in vec4 glyphBG;

layout (location=0) out vec4 outColour;

void main() {
	vec4 texColour = texture(fontImage, glyphCoords) 
		+ texture(fontImage, glyph2Coords) ;
		
	outColour = (glyphTint * texColour) + (glyphBG * (1.0f - texColour.a));
}