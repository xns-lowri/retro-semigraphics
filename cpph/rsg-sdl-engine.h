#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "rsg-datatypes.h"


class RsgEngine {
    //static variables
    Uint64 last_tick; //use for framerate calc
    Uint32 scale;                   //render scale
    Uint32 r_width, r_height;   //640x350?
    Uint32 w_width, w_height;   //1280x700?

    Uint32 chars_per_line;    //number of characters per row/line of text
    Uint32 n_lines;           //number of rows in window

    Uint32 n_chars;  //total number of glyphs on screen

    float framerate;

    /* public variables */
    rsd::CharData* charData; //pointer to char1 data array

    rsd::float2 render_extents; //window extents for vert transform
    rsd::float2 glyphsize;
    rsd::float2 glyphmapsize;

    //window resources
    SDL_Window* window;
    SDL_GPUDevice* device;

    //vert shader resources
    SDL_GPUBuffer* charDataBuffer;
    rsd::CharData* charDataPtr;

    //frag shader resources
    SDL_GPUTexture* fontMapTexture;
    SDL_GPUSampler* fontMapSampler;

    //GPU transfer buffers and pipeline
    SDL_GPUTransferBuffer* charTransferBuffer;
    SDL_GPUTransferBuffer* texTransferBuffer;
    SDL_GPUGraphicsPipeline* graphicsPipeline;

public:
    RsgEngine();
    RsgEngine(Uint32 rscale);

    /* SDL mirror functions */
    SDL_AppResult Init(
        rsd::uint2 display_size,
        bool allowResize,
        std::string defaultFont
    );
    SDL_AppResult Render();
    SDL_AppResult Event(SDL_Event* event);
    void Quit();
    /* End SDL functions */

    //character setters setters
    //set data for single char from input char data
    bool SetCharacter(Uint32 index, rsd::CharData* data);

    //fill multiple positions from input char data
    Uint32 FillCharacter(
        Uint32 index,
        Uint32 count,
        rsd::CharData* data
    );

    //Get size of display in characters
    rsd::uint2* GetDisplaySize();

    //Get chardata array index from x,y point in display 
    Uint32 PointToIndex(rsd::uint2 point);
};