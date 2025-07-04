/* 
 * Lowri Jones 2025
 * 
 * Monumental thank you to the following people for so kindly producing and
 *   freely sharing the educational materials and example code that allowed
 *   me to learn enough SDL3 to get a texture onto the GPU and start drawing
 *   it to some tris, and eventually in the correct colours!
 * 
 * Sam Lantiga for 
 * + "hello.c" example code, my go-to template while learning the basics
 * + "testgpu_spinning_cube.c"
 * 
 * Hamdy Elzanqali for the article "Let there be triangles, SDL_gpu edition" 
 *   at 'hamdy-elzanqali.medium.com'
 * 
 * Evan Hemsley for
 * + Article titled "SDL GPU API Concepts: Sprite Batcher" at 'moonside.games'
 * + Article titled "SDL GPU API Concepts: Data Transfer and Cycling" 
 *     at 'moonside.games'
 * * Github repo "SDL_gpu_examples" (so many helpful examples)
 * 
 * Joey de Vries for the article "Rendering Sprites" at 'learnopengl.com'
 * 
 * Vulcain Plop for the article "Sprite Batching with SDL3_gpu for Efficient 
 *   Rendering" at 'glusoft.com'
 * 
 * Loïc Chen for the github repo "sdl3-gpu-experiments"
 * 
 * Snorri Sturluson for the github repo "vulkan-sprites"
 *   and especially for the shaders in this repo, these helped me so much 
 *   while trying to fumble my way through getting code to run on the GPU
 * 
 * and all the reddit threads and forum posts along the way.
 */

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  //import once to hook into sdl callbacks

#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-sdl-engine.h"
#include "../cpph/rsg-gui-engine.h"

#include "../cpph/rsg-window.hpp"

/* Static variables */
//list of verts
const rsd::uint2 display_size = { 100, 32 };

RsgEngine* rsg_sdl_eng;
RsgGuiEngine* rsg_gui_eng;

//static Uint32 n_chars = 0;  //total number of glyphs on screen
//rsd::CharData* debug;

/* This function is called once by SDL when program is started */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    rsg_sdl_eng = new RsgEngine();
    rsg_gui_eng = new RsgGuiEngine(rsg_sdl_eng);

    //Init SDL engine, handles window+GPU and streaming chardata
    SDL_AppResult rsg_result = 
        rsg_sdl_eng->Init(    
            display_size,   //create display with specified dims (chars, not px)
            2,              //scale of 2 because i can't see too good
            false,          //don't allow resizing
            "gtest32.bmp"   //using 32 glyphs per row test font
        );

    if (rsg_result != SDL_APP_CONTINUE) {
        return rsg_result;  //quit early on error
    }

    rsgui::Window* main_window = new rsgui::Window(
        rsd::uint2(0, 0),
        rsd::uint2(display_size.x, display_size.y),
        "This Is A Test Window",
        rsd::float4(0.8f, 0.8f, 0.8f, 1.0f),
        rsd::float4(0.2f, 0.2f, 0.2f, 1.0f)
    );
    //main_window->SetBorder(rsgui::RSG_WINDOW_BORDER_DOUBLE);

    rsg_gui_eng->SetRenderableComponent(main_window);

    SDL_Log("Ready to render!");

    //debug draw window
    /*
    rsg_gui_eng->drawWindow(
        rsd::uint2(0, 0),
        rsd::uint2(display_size.x, display_size.y), //display_size,
        rsd::float4(0.8f,0.8f,0.8f,1.0f),
        rsd::float4(0.1f,0.05f,0.05f,1.0f),
        rsd::float4(0.8f,0.8f,0.8f,1.0f),
        rsd::float4(0.2f,0.2f,0.2f,1.0f),
        "Test Window - hello"
    );*/

    /*rseng::SetCharacter(rseng::PointToIndex(
        rsd::uint2(3, 2)),
        new (rsd::CharData){
            0.0f,0.0f,0.0f,1.0f,
            1.0f,0.0f,0.0f,1.0f,
            1023,' ',0,0
        }
    );*/

    //SDL_Log("\\0: %u, \\n: %u, \\r: %u, \\t: %u", '\0', '\n', '\r', '\t');

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    //SDL_Log("Event: %i", event->type);

    //todo app events handler
    
    //todo update (repaint) graphics - use callback
    return rsg_sdl_eng->Event(event);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    //update char buffer before render pass?

    SDL_AppResult rsg_result = rsg_sdl_eng->Render();
    if (rsg_result != SDL_APP_CONTINUE) {
        return rsg_result;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    rsg_sdl_eng->Quit();
}