#pragma once
#include<SDL3/SDL.h>
//#include <map>
//#include <string>

/* Offsets for RSG_LINE_x */
#define LINE_DOT	0 
#define LINE_VERT	1 
#define LINE_HORIZ  2 
#define LINE_CROSS  3 
#define LINE_UP		4 
#define LINE_RIGHT	5 
#define LINE_DOWN	6 
#define LINE_LEFT	7 

#define LINE_RA_UP_RIGHT		8 
#define LINE_RA_DOWN_RIGHT		9 
#define LINE_RA_DOWN_LEFT		10 
#define LINE_RA_UP_LEFT			11 
#define LINE_TEE_VERT_RIGHT		12 
#define LINE_TEE_HORIZ_DOWN		13 
#define LINE_TEE_VERT_LEFT		14 
#define LINE_TEE_HORIZ_UP		15 

/* Offsets for RSG_CONN_x */
#define CONN_VERT_A_HORIZ_B_CLIP	0 
#define CONN_VERT_A_HORIZ_B_NOCLIP	1 
#define CONN_HORIZ_A_VERT_B_CLIP	2 
#define CONN_HORIZ_A_VERT_B_NOCLIP	3 
#define CONN_VERT_A_RIGHT_B			4 
#define CONN_VERT_A_LEFT_B			5
#define CONN_RIGHT_A_VERT_B			6 
#define CONN_LEFT_A_VERT_B			7 

#define CONN_UP_A_RIGHT_B		8 
#define CONN_UP_A_LEFT_B		9 
#define CONN_DOWN_A_RIGHT_B		10 
#define CONN_DOWN_A_LEFT_B		11 
#define CONN_RIGHT_A_UP_B		12 
#define CONN_LEFT_A_UP_B		13 
#define CONN_RIGHT_A_DOWN_B		14 
#define CONN_LEFT_A_DOWN_B		15 

/* Offsets for RSG_ICONS_1 */
#define IC1_SQUARE4			0 
#define IC1_DOT4			1 
#define IC1_ARROWS_RIGHT	2 
#define IC1_ARROWS_LEFT		3 
#define IC1_ARROWL_RIGHT	4 
#define IC1_ARROWL_LEFT		5
#define IC1_SQBOX7			6 
#define IC1_SQBOX5_2X		7 

#define IC1_SQBOX6			8 
#define IC1_SQBOX6_DOT		9 
#define IC1_SQBOX6_TICK		10 
#define IC1_SQBOX6_MARK		11 
#define IC1_CIRC8			12 
#define IC1_CIRC8_DOT		13 
#define IC1_CIRC8_MARK		14 
#define IC1_CIRC8_PIZZA		15 

/* Offsets for RSG_BLOCKS */
#define BLOCK_FILLED		0 
#define BLOCK_25PC			1 
#define BLOCK_50PC			2 
#define BLOCK_75PC			3 
#define BLOCK_HALF_UP		4 
#define BLOCK_HALF_RIGHT	5
#define BLOCK_HALF_DOWN		6 
#define BLOCK_HALF_LEFT		7 

/* Offsets for RSG_OUTER_BORDERS_1 */
#define OUTER_THIN_UP		0 
#define OUTER_THIN_RIGHT	1 
#define OUTER_THIN_DOWN		2 
#define OUTER_THIN_LEFT		3 
#define OUTER_THICK_UP		4 
#define OUTER_THICK_RIGHT	5
#define OUTER_THICK_DOWN	6 
#define OUTER_THICK_LEFT	7 

/* Offsets for RSG_TITLE_BORDERS */
#define TITLE_FULL				0 
#define TITLE_BREAK_BOTH		1 
#define TITLE_HALF_RIGHT		2 
#define TITLE_HALF_LEFT			3 
#define TITLE_HALF_RIGHT_END	4 
#define TITLE_HALF_LEFT_END		5
#define TITLE_BREAK_RIGHT		6 
#define TITLE_BREAK_LEFT		7 

#define TITLE_RIGHT_JOIN		8 //todo fix window
#define TITLE_LEFT_JOIN			9 //todo fix window

#define TITLE_RIGHT_JOIN_SINGLE		8 
#define TITLE_LEFT_JOIN_SINGLE		9 
#define TITLE_RIGHT_JOIN_DOUBLE		10 
#define TITLE_LEFT_JOIN_DOUBLE		11 
#define TITLE_RIGHT_JOIN_THIN		12 
#define TITLE_LEFT_JOIN_THIN		13 
#define TITLE_RIGHT_JOIN_THICK		14 
#define TITLE_LEFT_JOIN_THICK		15 

/* Base IDs for glyph sets */
#define RSG_LINE_SINGLE			256 
#define RSG_LINE_DOUBLE			272 
#define RSG_CONN_SINGLE_DOUBLE	288 
#define RSG_ICONS_1				304 
#define RSG_BLOCKS				320 
#define RSG_OUTER_BORDERS_1		328 
#define RSG_TITLE_BORDERS		336 

/*
namespace rsfonts {
	std::map<std::string, Uint32> F_ENC = {
		/ * Offsets for RSG_LINE_x * /
		{"LINE_DOT", 0},
		{"LINE_VERT", 1},
		{"LINE_HORIZ", 2},
		{"LINE_CROSS", 3},
		
		{"END_UP", 4},
		{"END_RIGHT", 5},
		{"END_DOWN", 6},
		{"END_LEFT", 7},

		{"CORNER_UP_RIGHT", 8},
		{"CORNER_DOWN_RIGHT", 9},
		{"CORNER_DOWN_LEFT", 10},
		{"CORNER_UP_LEFT", 11},

		{"TEE_VERT_RIGHT", 12},
		{"TEE_HORIZ_DOWN", 13},
		{"TEE_VERT_LEFT", 14},
		{"TEE_HORIZ_UP", 15},

		/ *Offsets for RSG_CONN_x* /
		{"VERT_A_HORIZ_B_CLIP", 0},
		{"VERT_A_HORIZ_B_NOCLIP", 1},
		{"HORIZ_A_VERT_B_CLIP", 2},
		{"HORIZ_A_VERT_B_NOCLIP", 3},

		{"VERT_A_RIGHT_B", 4},
		{"VERT_A_LEFT_B", 5},
		{"RIGHT_A_VERT_B", 6},
		{"LEFT_A_VERT_B", 7},

		{"UP_A_RIGHT_B", 8},
		{"UP_A_LEFT_B", 9},
		{"DOWN_A_RIGHT_B", 10},
		{"DOWN_A_LEFT_B", 11},

		{"RIGHT_A_UP_B", 12},
		{"LEFT_A_UP_B", 13},
		{"RIGHT_A_DOWN_B", 14},
		{"LEFT_A_DOWN_B", 15},

		/ * Base IDs for glyph sets * /
		{"RSG_LINE_SINGLE", 256},
		{"RSG_LINE_DOUBLE", 272},
		{"RSG_CONN_SINGLE_DOUBLE", 288},
		{"RSG_ICONS_1", 304},
		{"RSG_BLOCKS", 320},
		{"RSG_OUTER_BORDERS_1", 328},
		{"RSG_TITLE_BORDERS", 336}

	};
}
*/