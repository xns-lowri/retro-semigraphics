#include <SDL3/SDL.h>
#include "../cpph/rsg-datatypes.h"

namespace rsd {
    //class CharData
    //ugh maybe put in header, datatypes becomes .hpp? :/
    //or split out hpps into h and cpp?
    //i'd need to know more about c/c++ compatibility

    /* Constructor - empty, init zero */
    CharData::CharData() {
        r = g = b = 0.0f;
        r1 = g1 = b1 = 0.0f;
        a = a1 = 1.0f;
        char1 = char2 = ' ';
        p1 = p2 = 0;
    };

    /* Constructor - individual floats */
    //not used, deprecate?
    //is actually used by sdl engine to init char struct
    //^ should be using empty constructor?
    CharData::CharData(
        float r, float g, float b, float a,
        float r1, float g1, float b1, float a1,
        Uint32 char1, Uint32 char2) :
        r(r), g(g), b(b), a(a),
        r1(r1), g1(g1), b1(b1), a1(a1),
        char1(char1), char2(char2)
    {
        p1 = 0;
        p2 = 0;
    };

    /* Constructor - float structs */
    CharData::CharData(
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

    /* Colour setters */
    void CharData::SetForegroundColour(rsd::float4 fgCol) {
        r = fgCol.x;
        g = fgCol.y;
        b = fgCol.z;
        a = fgCol.w;
    }
    void CharData::SetBackgroundColour(rsd::float4 bgCol) {
        r1 = bgCol.x;
        g1 = bgCol.y;
        b1 = bgCol.z;
        a1 = bgCol.w;
    }

    CharMetadata::CharMetadata() {
        charOwner = NULL;
        isDirty = false;
    }

    /* ugh i typed out all these helpers for nowt :(
    void CharMetadata::SetOwner(rsgui::Component* newOwner) {
        charOwner = newOwner;
        isDirty = true;
    }
    void CharMetadata::SetDirty(bool dirty) {
        isDirty = dirty;
    }
    bool CharMetadata::GetDirty() {
        return isDirty;
    }*/
}