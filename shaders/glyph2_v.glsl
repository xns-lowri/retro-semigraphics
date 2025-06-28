#version 460

layout (set=1, binding=0) uniform UniformBufferObject {
	vec2 windowSize;
	vec2 glyphMapSize;
	vec2 glyphSize;
	uint chars_per_row;
	uint glyphs_per_row;
} ubo;

struct charData_t {
	vec4 glyphTint;
	vec4 glyphBG;
	uint char1;
	uint char2;
	uint p1;
	uint p2;
};

layout (set=0, binding=0) readonly buffer CharBuffer {
	//vec2 charPosition[9600]; //screen position of char
	//vec2 glyphCoords[9600];  //texture co-ords of char, todo calc fully
	//vec4 glyphTint[9600];
	charData_t chars[];
} cbf;

//todo calculate texture co-ords on GPU from glyph ID
//layout (location=0) in vec2 charPosition;
//layout (location=1) in vec2 glyphCoords;
//layout (location=2) in vec4 glyphTint;

layout (location=0) out vec2 outGlyphCoords;
layout (location=1) out vec2 outGlyph2Coords;
layout (location=2) out vec4 outGlyphTint;
layout (location=3) out vec4 outGlyphBG;

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
	
	
	uint row = charID / ubo.chars_per_row;
	uint col = charID - (row * ubo.chars_per_row);
	
	vec2 vertPos = vec2(0, ubo.windowSize.y - ubo.glyphSize.y); 
	//vertPos.x += float(col) * ubo.glyphSize.x;
	//vertPos.y -= float(row) * ubo.glyphSize.y;
	vertPos += vec2(float(col),-float(row)) * ubo.glyphSize;
	vertPos += thisPtRel;
	vertPos /= windHalfSize;
	vertPos	-= one;
	
	//set vert position
	gl_Position = vec4(vertPos, 0.0, 1.0);
	
	uint pos = cbf.chars[charID].char1;
	uint g_col = pos % ubo.glyphs_per_row;
	uint g_row = pos / ubo.glyphs_per_row; //hard coded 16 per row?
	
	uint pos2 = cbf.chars[charID].char2;
	uint g_col2 = pos2 % ubo.glyphs_per_row;
	uint g_row2 = pos2 / ubo.glyphs_per_row; //hard coded 16 per row?
	
	vec2 texOrigin = vec2(g_col, g_row) * ubo.glyphSize;
	vec2 thisTexRel = triTexPos[vertID] * ubo.glyphSize;
	vec2 texPos = (texOrigin + thisTexRel) / ubo.glyphMapSize;
	
	vec2 texOrigin2 = vec2(g_col2, g_row2) * ubo.glyphSize;
	vec2 thisTexRel2 = triTexPos[vertID] * ubo.glyphSize;
	vec2 texPos2 = (texOrigin2 + thisTexRel2) / ubo.glyphMapSize;
	//vec2 texPos = thisPtRel / ubo.glyphMapSize;
	//pass to frag shader
	outGlyphCoords = texPos;
	outGlyph2Coords = texPos2;
	outGlyphTint = cbf.chars[charID].glyphTint;
	outGlyphBG = cbf.chars[charID].glyphBG;
}