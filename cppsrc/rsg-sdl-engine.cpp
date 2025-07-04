#include <SDL3/SDL.h>

#include <filesystem>
#include <string>

#include "../cpph/rsg-datatypes.h"
#include "../cpph/rsg-sdl-engine.h"
#include "../cpph/rsg-gui-engine.h"

#define BG_COL 0.1f     //todo replace
#define FG_COL 0.9f     //todo replace

//#define fGLYPH_W 8.0f   //todo replace
//#define fGLYPH_H 14.0f  //todo repalce

#define uGLYPH_W 8   //todo replace
#define uGLYPH_H 14  //todo replace

#define W_GLYPHMAP 32 //width of glyph map in chars

//todo tidy up, this is the struct for uniform buffer data
//this gets pushed to the vert shader before each render pass
struct UniformBuf {
    float win_w, win_h;
    float map_w, map_h;
    float glyph_w, glyph_h;
    Uint32 cpr, gpr; //chars per row (screen), glyphs per row (map)
} uniformbuf;
/* End structs */



RsgEngine::RsgEngine() 
{
    scale = 2;  //default scale

    //init class variables to _something_
    last_tick = 0;
    r_width = 0; r_height = 0;
    w_width = 0; w_height = 0;
    chars_per_line = 0; n_lines = 0;
    n_chars = 0;  //total number of glyphs on screen
    framerate = 0.f;

    charData = NULL;
    render_extents = { 0.f,0.f };
    glyphsize = { 0.f,0.f };
    glyphmapsize = { 0.f,0.f };

    window = NULL; device = NULL;

    charDataBuffer = NULL;
    charDataPtr = NULL;

    fontMapTexture = NULL;
    fontMapSampler = NULL;
    charTransferBuffer = NULL;
    texTransferBuffer = NULL;
    graphicsPipeline = NULL;
}

/* Call in SDL_AppInit to create window */
SDL_AppResult RsgEngine::Init(
    rsd::uint2 display_size, 
    Uint32 rscale,
    bool allowResize, 
    std::string defaultFont
)
{
    //todo request screen size and scale?
    //system screen scaling?
    scale = rscale;

    chars_per_line = display_size.x;
    n_lines = display_size.y;

    r_width = uGLYPH_W * chars_per_line; //rendering width from glyph width and line len
    r_height = uGLYPH_H * n_lines;       //rendering height from glyph height and line count

    n_chars = chars_per_line * n_lines;  //calculate total number of chars in window

    //calculate real window extents, update render_extents,glyphsize for GPU
    w_width = r_width * scale; 
    w_height = r_height * scale;
        
    render_extents = { 
        static_cast<float>(r_width), 
        static_cast<float>(r_height) 
    };
        
    glyphsize = { 
        static_cast<float>(uGLYPH_W), 
        static_cast<float>(uGLYPH_H) 
    };

    SDL_Log("n_chars = %i", n_chars);

    //create char (vertex) buffer array
    charData = new rsd::CharData[n_chars];
    if (charData == NULL) {
        SDL_Log("Couldn't allocate memory for char array: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    //initialise char buffer data
    FillCharacter(
        0, n_chars, 
        new (rsd::CharData){
            FG_COL, FG_COL, FG_COL, 1.0f,   //fg colour with full opacity
            0.0f, 0.0f, 0.0f, 0.0f,         //transparent bg
            ' ', ' ',   //init all data to space
            0, 0        //init padding
        });

    /* Create the window */
    window = SDL_CreateWindow("Oldskool Fonts Utility", w_width, w_height, NULL);
    if (window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    //allow resizing or not
    //todo enum resize mode: disallow, stretch, resize (char buffer)?
    SDL_SetWindowResizable(window, allowResize);

    //SDL_Window* window2 = SDL_CreateWindow("Oldskool Fonts Utility 2", w_width, w_height, NULL);
    //SDL_SetWindowMinimumSize(window, w_width/2, w_height/2);
    //SDL_SetWindowMaximumSize(window, w_width, w_height);

    //SDL_SetWindowSize(window, w_width / 2, w_height / 2);

    SDL_Log("GPU shader format support:");
    SDL_Log("Invalid: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_INVALID, NULL));
    SDL_Log("Private: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_PRIVATE, NULL));
    SDL_Log("SPIR-V: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_SPIRV, NULL));
    SDL_Log("DX12 DXBC: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_DXBC, NULL));
    SDL_Log("DX12 DXIL: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_DXIL, NULL));
    SDL_Log("MSL Metal: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_MSL, NULL));
    SDL_Log("metallib: %i", SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_METALLIB, NULL));

    /* Create GPU device*/
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (device == NULL) {
        //handle error
        SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Claim window for GPU */
    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_Log("Couldn't claim window for GPU device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Base path: %s", SDL_GetBasePath());

    //debug allow uncapped framerate, more calls to iterate
    /*if (!SDL_SetGPUSwapchainParameters(device, window,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        SDL_GPU_PRESENTMODE_IMMEDIATE))*/
    if (!SDL_SetGPUSwapchainParameters(device, window,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        SDL_GPU_PRESENTMODE_VSYNC))
    {
        SDL_Log("Couldn't set present mode: %s", SDL_GetError());
    }

    //SDL_SetWindowFullscreenMode(window, SDL_DisplayMode);

    /* Create vertex shader */
    //load vertex shader code
    size_t vertexCodeSize;
    void* vertexCode = SDL_LoadFile(
        (std::filesystem::current_path().string()
            + "\\shaders\\glyph2_v.spv").c_str(),
        &vertexCodeSize);
    if (vertexCode == NULL) {
        SDL_Log("Couldn't load vertex shader file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Vertex code size: %zu", vertexCodeSize);

    //create vertex shader
    SDL_GPUShaderCreateInfo vertexInfo{
        .code_size = vertexCodeSize,
        .code = (Uint8*)vertexCode,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV, //.spv shader
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,  //vertex shader (vert positioning)
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 1,
        .num_uniform_buffers = 1
    };
    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);
    if (vertexShader == NULL) {
        SDL_Log("Couldn't create fragment shader: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    //free code file
    SDL_free(vertexCode);


    /* Create fragment shader */
    //load fragment code
    size_t fragmentCodeSize;
    void* fragmentCode = SDL_LoadFile(
        (std::filesystem::current_path().string()
            + "\\shaders\\glyph_f.spv").c_str(),
        &fragmentCodeSize);
    if (fragmentCode == NULL) {
        SDL_Log("Couldn't load fragment shader file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Fragment code size: %zu", fragmentCodeSize);

    //create fragment shader
    SDL_GPUShaderCreateInfo fragmentInfo{
        .code_size = fragmentCodeSize,
        .code = (Uint8*)fragmentCode,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers = 1,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0
    };
    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);
    if (fragmentShader == NULL) {
        SDL_Log("Couldn't create fragment shader: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    //free code file
    SDL_free(fragmentCode);

    /* Load texture and create sampler */
    std::string font_filepath = (std::filesystem::current_path().string()
        + "\\fonts\\" + defaultFont);
    SDL_Surface* font_in_1 = SDL_LoadBMP(font_filepath.c_str());
    if (font_in_1 == NULL) {
        SDL_Log("Error reading file at %s", font_filepath.c_str());
        SDL_Log("Couldn't load font image: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    //SDL_Log("Got image format: %i", font_in_1->format);
    SDL_Surface* font_in = SDL_ConvertSurface(font_in_1, SDL_PIXELFORMAT_ABGR8888); //SDL_PIXELFORMAT_RGB565
    SDL_DestroySurface(font_in_1); //release original image
    //SDL_Log("Converted to image format: %i", font_in->format);

    SDL_GPUTextureCreateInfo textureInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, //SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = (Uint32)font_in->w,
        .height = (Uint32)font_in->h,
        .layer_count_or_depth = 1,
        .num_levels = 1
    };
    fontMapTexture = SDL_CreateGPUTexture(device, &textureInfo);

    SDL_GPUSamplerCreateInfo samplerInfo{
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT

    };
    fontMapSampler = SDL_CreateGPUSampler(device, &samplerInfo);

    glyphmapsize = { (float)font_in->w, (float)font_in->h };
    SDL_Log("Glyph size: %f x %f", glyphsize.x, glyphsize.y);
    SDL_Log("Glyph map size: %f x %f", glyphmapsize.x, glyphmapsize.y);

    /* Create graphics pipeline */
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;


    //describe vertex buffers
    /* ~~ there aren't any :D ~~ */

    //describe color target
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    //release shaders
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    SDL_Log("Create and fill transfer buffers");

    //create char buffer
    SDL_GPUBufferCreateInfo vertBufferInfo{
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = n_chars * sizeof(rsd::CharData)
    };
    charDataBuffer = SDL_CreateGPUBuffer(device, &vertBufferInfo);

    //create transfer buffer to upload to vertex buffer
    SDL_GPUTransferBufferCreateInfo vertTransferInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = n_chars * sizeof(rsd::CharData)
    };
    charTransferBuffer = SDL_CreateGPUTransferBuffer(device, &vertTransferInfo);

    //fill transfer buffer
    charDataPtr = (rsd::CharData*)SDL_MapGPUTransferBuffer(
        device,
        charTransferBuffer,
        false
    );
    SDL_memcpy(
        charDataPtr,
        (void*)charData,
        n_chars * sizeof(rsd::CharData)
    );
    SDL_UnmapGPUTransferBuffer(device, charTransferBuffer);

    SDL_Log("filled vertex transfer buffer");


    /* Start copy pass */
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    //where data is
    SDL_GPUTransferBufferLocation vertLocation{
        .transfer_buffer = charTransferBuffer,
        .offset = 0
    };

    //where to upload data
    SDL_GPUBufferRegion vertRegion{
        .buffer = charDataBuffer,
        .offset = 0,
        .size = n_chars * sizeof(rsd::CharData)
    };

    //upload
    SDL_UploadToGPUBuffer(copyPass, &vertLocation, &vertRegion, true);

    SDL_Log("copied vertex data");


    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    /* End of copy pass */


    //create texture transfer buffer
    SDL_GPUTransferBufferCreateInfo texTransferBufferInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<Uint32>(font_in->w * font_in->h * 4)
    };
    texTransferBuffer = SDL_CreateGPUTransferBuffer(device, &texTransferBufferInfo);
    SDL_Log("created texture transfer buffer");

    Uint8* texTransferPtr = (Uint8*)SDL_MapGPUTransferBuffer(
        device,
        texTransferBuffer,
        false);

    if (texTransferPtr == NULL) {
        SDL_Log("Couldn't get tex ptr: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Format: %i", font_in->format);
    SDL_Log("Got pointer, transferring %i B", texTransferBufferInfo.size);
    SDL_memcpy(
        texTransferPtr,
        (Uint8*)font_in->pixels,
        font_in->w * font_in->h * 4
    );
    SDL_Log("Transferred");
    SDL_UnmapGPUTransferBuffer(device, texTransferBuffer);

    SDL_Log("filled texture transfer buffer");

    commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    //where to upload data
    SDL_GPUTextureRegion texRegion{};
    texRegion.texture = fontMapTexture;
    texRegion.h = font_in->h;
    texRegion.w = font_in->w;
    texRegion.d = 1;


    SDL_GPUTextureTransferInfo texTransferInfo{
        .transfer_buffer = texTransferBuffer,
        .offset = 0,
        .pixels_per_row = (Uint32)font_in->w,
        .rows_per_layer = (Uint32)font_in->h
    };

    SDL_UploadToGPUTexture(copyPass, &texTransferInfo, &texRegion, true);

    SDL_Log("copied texture data");

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    SDL_DestroySurface(font_in);

    SDL_Log("GPU setup complete");

    return SDL_APP_CONTINUE;
}

/* Call in SDL_AppIterate to render next frame */
SDL_AppResult RsgEngine::Render() {
    //debug changing chars each frame
    /*for (Uint32 n = 256; n < n_chars; ++n) {
        charData[n].char1++;
    }*/

    //calc framerate
    Uint64 this_tick = SDL_GetTicksNS();
    if (this_tick > last_tick) {
        framerate = 1000000000.f / (float)(this_tick - last_tick);
    }
    else {
        framerate = std::nanf(""); //invalid
    }
    last_tick = this_tick;

    //push characters from string, todo make function for this
    /*std::string framerate_str = std::format("{:.2f}", framerate) + " FPS ";
    for (Uint32 i = 0; i < framerate_str.length(); ++i) {
        charData[i].char1 = framerate_str.at(i);
    }*/

    //fill transfer buffer
    charDataPtr = (rsd::CharData*)SDL_MapGPUTransferBuffer(
        device, charTransferBuffer, false
    );
    SDL_memcpy(
        charDataPtr,
        (void*)charData,
        n_chars * sizeof(rsd::CharData)
    );
    SDL_UnmapGPUTransferBuffer(device, charTransferBuffer);

    //SDL_Log("filled vertex transfer buffer");


    //acquire command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    //where data is
    SDL_GPUTransferBufferLocation vertLocation{
        .transfer_buffer = charTransferBuffer,
        .offset = 0
    };

    //where to upload data
    SDL_GPUBufferRegion vertRegion{
        .buffer = charDataBuffer,
        .offset = 0,
        .size = n_chars * sizeof(rsd::CharData)
    };

    //upload
    SDL_UploadToGPUBuffer(copyPass, &vertLocation, &vertRegion, true);

    //SDL_Log("copied vertex data");
    SDL_EndGPUCopyPass(copyPass);


    //get swapchain texture
    SDL_GPUTexture* swapchainTexture = NULL;
    Uint32 width, height; //not used 4 now
    //acquire gpu swapchain texture, blocks thread, returns false on error
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height)) {
        SDL_SubmitGPUCommandBuffer(commandBuffer); //always submit command buffer
        SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
        return SDL_APP_FAILURE; //todo???
    }
    //check null swapchain texture, not error but can't render
    if (swapchainTexture == NULL) {
        SDL_SubmitGPUCommandBuffer(commandBuffer); //always submit command buffer
        return SDL_APP_CONTINUE; //continue without rendering
    }

    //create colour target for render pass
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = { BG_COL, BG_COL, BG_COL, 0.0f }; //TODO variable for bgcol?
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    //pass uniform data to GPU
    //this data == 'constant' data for all verts in next render pass
    uniformbuf = {
        render_extents.x, render_extents.y,
        glyphmapsize.x, glyphmapsize.y,
        glyphsize.x, glyphsize.y,
        chars_per_line, static_cast<Uint32>(W_GLYPHMAP) //TODO n chars per line of glyph map
    };

    //push uniform data before render pass
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniformbuf, sizeof(uniformbuf));

    /***** Begin the render pass *****/
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);
    //we 'can' 'draw' things now

    /* Bind resources for draw calls within render pass */

    //bind the gpu pipeline for the next render pass
    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

    /* No vcertex buffers to bind */

    //bind the char data buffer to [ssbo?] in vert shader
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &charDataBuffer, 1);

    //create and bind texture and sampler to frag shader
    // - todon't: make new binding every frame?
    // - alt todo: is this good practice? should i usse more stack vars??
    SDL_GPUTextureSamplerBinding textureSamplerBindings[1];
    textureSamplerBindings[0].texture = fontMapTexture;
    textureSamplerBindings[0].sampler = fontMapSampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, textureSamplerBindings, 1);

    //ok we can actually _draw_ things now

    //draw the primitives
    SDL_DrawGPUPrimitives(renderPass, //this render pass
        n_chars * 6,    //draw 6 verts (= 2 tris = 1 quad) for every char
        1,              //TODO: draw some/all chars as separate instaces? 
        0,              //first vert always = 0
        0               //first instance always = 0
    );

    //no more drawing to do
    SDL_EndGPURenderPass(renderPass);
    /***** End the render pass *****/

    //submit (release) the command buffer and continue
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult RsgEngine::Event(SDL_Event* event) {
    //return early if quit event
    //todo may need to intercept for e.g. save changes alert
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    SDL_AppResult appResult = SDL_APP_CONTINUE;
    if (guiEngine != NULL) {
        appResult = guiEngine->Event(event);
    }

    //todo window resize
    /*switch (event->type) {
        case SDL_EVENT_WINDOW_RESIZED:
            SDL_Log("Window resized");
            break;
    }*/

    return appResult;
}

void RsgEngine::Quit() {
    //release buffers
    SDL_ReleaseGPUBuffer(device, charDataBuffer);
    SDL_ReleaseGPUTransferBuffer(device, charTransferBuffer);
    SDL_ReleaseGPUTransferBuffer(device, texTransferBuffer);
    SDL_ReleaseGPUTexture(device, fontMapTexture);
    SDL_ReleaseGPUSampler(device, fontMapSampler);

    //reelease the pipeline
    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

    //destroy GPU device
    SDL_DestroyGPUDevice(device);

    //destroy window
    SDL_DestroyWindow(window);

    if (guiEngine != NULL) {
        guiEngine->Quit();
    }

    delete(this);
}

/*CharData* GetCharDataPtr() {
    return charData;
}*/

bool RsgEngine::SetGuiEngine(RsgGuiEngine* engine) {
    bool updated = (guiEngine != NULL);
    guiEngine = engine;
    return updated;
}

rsd::uint2* RsgEngine::GetDisplaySize() {
    return new (rsd::uint2){chars_per_line, n_lines};
}

Uint32 RsgEngine::PointToIndex(rsd::uint2 point) {
    return (point.y * GetDisplaySize()->x) + point.x;
}

bool RsgEngine::SetCharacter(Uint32 index, rsd::CharData* data) {
    if (index >= n_chars) {
        return false;
    }
    charData[index] = *data;
    return true;
}

Uint32 RsgEngine::FillCharacter(Uint32 index, Uint32 count, rsd::CharData* data) {
    Uint32 chars_modified = 0;
    for (Uint32 i = index; i < index + count; ++i) {
        if (i >= n_chars) {
            break;
        }
        charData[i] = *data;
        ++chars_modified;
    }
    return chars_modified;
}