#version 460

layout (set=1, binding=0) uniform UniformBufferObject {
	vec2 windowSize;
	vec2 glyphMapSize;
	vec2 glyphSize;
} ubo;

struct glyphdata {
	vec2 charPosition;
	vec2 glyphCoords;
	vec4 glyphTint;
};

layout (set=0, binding=0) readonly buffer CharBuffer {
	//vec2 charPosition[9600]; //screen position of char
	//vec2 glyphCoords[9600];  //texture co-ords of char, todo calc fully
	//vec4 glyphTint[9600];
	glyphdata chars[];
} cbf;

//todo calculate texture co-ords on GPU from glyph ID
//layout (location=0) in vec2 charPosition;
//layout (location=1) in vec2 glyphCoords;
//layout (location=2) in vec4 glyphTint;

layout (location=0) out vec2 outGlyphCoords;
layout (location=1) out vec4 outGlyphTint;

const vec2 one = vec2(1.0f, 1.0f);

const uint triIndices[6] = {0,1,2,3,2,1};
const vec2 triVertPos[4] = {
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{0.0f, 1.0f},
	{1.0f, 1.0f}
};
const vec2 triTexPos[4] = {
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{0.0f, 0.0f},
	{1.0f, 0.0f}
};

void main() {
	//get half window size for converting 0..w_window to -1.0f..1.0f
	//0,0 is at bottom left
	uint charID = gl_VertexIndex / 6;
	uint vertID = triIndices[gl_VertexIndex % 6];
	
	vec2 windHalfSize = ubo.windowSize / 2.0f;
	
	vec2 thisPtRel = triVertPos[vertID] * ubo.glyphSize;

	vec2 vertPos = ((cbf.chars[charID].charPosition + thisPtRel) / windHalfSize) - one;
	//set vert position
	gl_Position = vec4(vertPos, 0.0, 1.0);
	
	vec2 thisTexRel = triTexPos[vertID] * ubo.glyphSize;
	vec2 texPos = (cbf.chars[charID].glyphCoords + thisTexRel) / ubo.glyphMapSize;
	//vec2 texPos = thisPtRel / ubo.glyphMapSize;
	//pass to frag shader
	outGlyphCoords = texPos;
	outGlyphTint = cbf.chars[charID].glyphTint;
}