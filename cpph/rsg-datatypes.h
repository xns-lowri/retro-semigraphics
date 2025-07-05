#pragma once
#include <SDL3/SDL.h>
//soz, am an old C coder stuck in a slightly younger C coder's body

namespace rsgui {
    class Component;
}

namespace rsd {
    //typedef bool (*callback_ptr)(SDL_Event* event); //wtf was this

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
    //idk how this still aligns for gpu when it used to be a struct?
    //c++ is magic? woooOOoooOOO)Ooo
    class CharData {
    public:
        float r, g, b, a;       //vec4 glyph color
        float r1, g1, b1, a1;   //vec4 bg color
        Uint32 char1;   //raw char-ish code, inprog expand > 255
        Uint32 char2;   //second char to mix with first
        Uint32 p1;
        Uint32 p2;
        //padding to align, todo use for useful st00fs

        CharData();

        CharData(
            float r, float g, float b, float a,     //fgcol
            float r1, float g1, float b1, float a1, //bgcol
            Uint32 char1, 
            Uint32 char2
        );

        CharData(
            rsd::float4 fgCol,
            rsd::float4 bgCol,
            Uint32 char1,
            Uint32 char2
        );

        void SetForegroundColour(rsd::float4 fgCol);
        void SetBackgroundColour(rsd::float4 bgCol);
    };

    class CharMetadata {
    public:
        rsgui::Component* charOwner;
        bool isDirty;

        CharMetadata();

        //void SetOwner(rsgui::Component* newOwner);
        //void SetDirty(bool dirty);
        //bool GetDirty();
    };
}
