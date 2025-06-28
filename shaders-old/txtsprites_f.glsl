#version 460

//font sheet image from app
layout (set=2, binding=0) uniform sampler2D fontImage;

//glyph co-ords from vert shader
//tint passed from app
layout (location=0) in vec2 glyphCoords;
layout (location=1) in vec4 glyphTint;

layout (location=0) out vec4 outColour;

void main() {
	outColour = glyphTint * texture(fontImage, glyphCoords);
}