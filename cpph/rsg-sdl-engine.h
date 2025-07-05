#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "rsg-datatypes.h"

class RsgGuiEngine;

class RsgEngine {
private:
    //todo clean up vars? :/

    /* Main character data for GPU */
    rsd::CharData* charData; //pointer to char1 data array

    //SDL Engine resources
    Uint32 scale;               //render scale
    Uint32 r_width, r_height;   //'render' width = GPU output texture size
    Uint32 w_width, w_height;   //window size = render size * scale;

    Uint32 chars_per_line;  //number of characters per row/line of text
    Uint32 n_lines;         //number of rows in window
    Uint32 n_chars;         //total number of glyphs on screen

    //other engine resources
    Uint64 last_tick; //use for framerate calc, timestamping
    float framerate; //fps counter variable

    //GPU resources
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

    RsgGuiEngine* guiEngine;

public:
    /* public variables */
    rsd::CharMetadata* charMetadata;		//character metadata array (owner, dirty)

    /* Method declarations */
    RsgEngine();

    /* SDL mirror functions */
    SDL_AppResult Init(
        rsd::uint2 display_size,
        Uint32 rscale,
        bool allowResize,
        std::string defaultFont
    );
    SDL_AppResult Render();
    SDL_AppResult Event(SDL_Event* event);
    void Quit();
    /* End SDL functions */

    bool SetGuiEngine(RsgGuiEngine* engine);

    /* Getters and helpers */
    //Get x,y size of display in characters
    rsd::uint2 GetDisplaySize();
    //Get number of characters in display
    Uint32 GetCharacterCount();

    //Get chardata array index from char x,y points in display 
    Uint32 PointToIndex(Uint32 x, Uint32 y);
    //Get chardata array index from char x,y uint2
    Uint32 PointToIndex(rsd::uint2 point);

    //get char index from pixel position in window
    rsd::uint2 ScreenPointToCharXY(rsd::float2 point);

    /* Data setters */
    //character setters setters
    //set data for single char from input char data
    bool SetCharacter(
        Uint32 index, 
        rsd::CharData* data,
        rsgui::Component* owner
    );

    //fill multiple positions from input char data
    Uint32 FillCharacter(
        Uint32 index,
        Uint32 count,
        rsd::CharData* data,
        rsgui::Component* owner
    );
};