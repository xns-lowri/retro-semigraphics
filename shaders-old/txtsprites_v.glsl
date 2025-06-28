#version 460

layout (set=1, binding=0) uniform UniformBufferObject {
	vec2 windowSize;
} ubo;

//todo calculate texture co-ords on GPU from glyph ID
layout (location=0) in vec2 charPosition;
layout (location=1) in vec2 glyphCoords;
layout (location=2) in vec4 glyphTint;

layout (location=0) out vec2 outGlyphCoords;
layout (location=1) out vec4 outGlyphTint;

const vec2 one = vec2(1.0f, 1.0f);

void main() {
	//get half window size for converting 0..w_window to -1.0f..1.0f
	//0,0 is at bottom left
	vec2 halfSize = ubo.windowSize / 2.0f;

	//set vert position
	gl_Position = vec4((charPosition / halfSize) - one, 0.0, 1.0);
	
	//pass to frag shader
	outGlyphCoords = glyphCoords;
	outGlyphTint = glyphTint;
}