#pragma once
#include <SDL3/SDL.h>
//soz, am an old C coder stuck in a slightly younger C coder's body

namespace rsd {
    //typedef bool (*callback_ptr)(SDL_Event* event);


    //float2, simple struct of two floats
    struct float2 
    {
        float x, y;
    };

    //float 4, simple struct of four floats
    struct float4
    {
        float x, y, z, w;
    };

    //uint2, simple struct of two Uint32s
    struct uint2 
    {
        //uint2 composed of two Uint32 variables
        Uint32 x, y;

        //constructor with default values
        uint2(Uint32 w=0, Uint32 h=0) : x(w), y(h) {}

        //overload set equal operator
        uint2& operator=(const uint2& a) {
            x = a.x;
            y = a.y;
            return *this;
        }
        //overload quick add operator
        uint2& operator+=(const uint2& a) {
            x = x+a.x;
            y = y+a.y;
            return *this;
        }
        //overload regular add operator
        uint2 operator+(const uint2& a) {
            return uint2(x+a.x, y+a.y);
        }
        //overload greater than check
        //NOTE: returns true if EITHER dim is greater
        //todo might change if it ever comes up
        bool operator>(const uint2& a) {
            return (x > a.x) || (y > a.y);
        }
    };

    //character data class, layout for vert shader
    class CharData
    {
    public:
        float r, g, b, a;       //vec4 glyph color
        float r1, g1, b1, a1;   //vec4 bg color
        Uint32 char1;   //raw char-ish code, inprog expand > 255
        Uint32 char2;   //second char to mix with first
        Uint32 p1;
        Uint32 p2;

        CharData() {
            r = g = b = a = 0.0f;
            r1 = g1 = b1 = a1 = 0.0f;
            char1 = char2 = ' ';
            p1 = p2 = 0;
        };

        CharData(float r, float g, float b, float a,
            float r1, float g1, float b1, float a1,
            Uint32 char1, Uint32 char2, Uint32 p1, Uint32 p2) :
            r(r), g(g), b(b), a(a),
            r1(r1), g1(g1), b1(b1), a1(a1),
            char1(char1), char2(char2), p1(p1), p2(p2) 
        {};
        //padding to align, todo use for useful st00fs
        CharData(
            rsd::float4 fgCol, 
            rsd::float4 bgCol, 
            Uint32 char1, 
            Uint32 char2
        ) :
            r(fgCol.x), g(fgCol.y), b(fgCol.z), a(fgCol.w),
            r1(bgCol.x), g1(bgCol.y), b1(bgCol.z), a1(bgCol.w),
            char1(char1), char2(char2)
        {
            p1 = 0;
            p2 = 0;
        };
    };
}
