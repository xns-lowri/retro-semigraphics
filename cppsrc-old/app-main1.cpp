/* 
 * Lowri Jones 2025
 * 
 * Monumental thank you to the following people for so kindly producing and
 *   freely sharing the educational materials and example code that allowed
 *   me to learn enough SDL3 to get a texture onto the GPU and start drawing
 *   it to some tris, and eventually in the correct colours!
 * 
 * Sam Lantiga for 
 * * "hello.c" example code, my go-to template while learning the basics
 * * "testgpu_spinning_cube.c"
 * 
 * Hamdy Elzanqali for the article "Let there be triangles, SDL_gpu edition" 
 *   at 'hamdy-elzanqali.medium.com'
 * 
 * Evan Hemsley for
 * * Article titled "SDL GPU API Concepts: Sprite Batcher" at 'moonside.games'
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

#include <filesystem>
#include <string>

#define BG_COL 0.1f     //todo remove
#define FG_COL 0.8f     //todo remove

#define fGLYPH_W 8.0f   //todo remove
#define fGLYPH_H 14.0f  //todo remove

#define uGLYPH_W 8   //todo remove
#define uGLYPH_H 14  //todo remove

//todo glyph input layout struct

//vertex input layout struct
struct CharData  
{
    //float x, y;         //vec2 position
    //float u, v;         //vec2 texture
    float r, g, b, a;   //vec4 tint color
    float r1, g1, b1, a1;   //vec4 tint color
    Uint32 character;
    Uint32 p, p1, p2;
};
//typedef struct Vertex Vertex;


Uint32 scale = 2;
Uint32 r_width = 0, r_height = 0; //640x350
Uint32 w_width = 0, w_height = 0; //1280x700

//list of verts
static Uint32 n_chars = 0;          //total number of glyphs on screen
static Uint32 chars_per_line = 100;   //todo implementation, number of characters per row
static Uint32 n_lines = 35;          //todo implementation, number of rows in window
CharData* charData;                 //pointer to character data array
/*
{
    /*
    {-1.0f,   1.0f,  0.0f,      1.0f, 0.0f, 1.0f, 1.0f},     // top vertex
    {-1.0f,   0.92f, 0.0f,      1.0f, 0.0f, 0.0f, 1.0f},   // bottom left vertex
    {-0.975f, 0.92f, 0.0f,      1.0f, 0.0f, 1.0f, 1.0f},     // bottom right vertex

    { -1.0f,  1.0f,  0.0f,      0.0f, 0.0f, 1.0f, 1.0f },     // top vertex
    {-0.975f, 0.92f, 0.0f,      0.0f, 0.0f, 1.0f, 1.0f},     // bottom right vertex
    {-0.975f,   1.0f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f},   // bottom left vertex

    {-0.975f,   1.0f,  0.0f,      1.0f, 0.0f, 1.0f, 1.0f },     // top vertex
    {-0.975f,   0.92f, 0.0f,      1.0f, 0.0f, 0.0f, 1.0f},   // bottom left vertex
    {-0.95f,  0.92f, 0.0f,      1.0f, 0.0f, 1.0f, 1.0f},     // bottom right vertex

    {-0.975f,  1.0f,  0.0f,      0.0f, 0.0f, 1.0f, 1.0f },     // top vertex
    {-0.95f, 0.92f, 0.0f,      0.0f, 0.0f, 1.0f, 1.0f},     // bottom right vertex
    {-0.95f,   1.0f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f}   // bottom left vertex
    * /

    {0.0f,       0.0f,    0.0f,     1.0f,     FG_COL, FG_COL, FG_COL, 1.0f},      //idek at this poiint lol
    {0.0f,    fGLYPH_H,    0.0f,     0.0f,     FG_COL, FG_COL, FG_COL, 1.0f},     //idek at this poiint lol
    {fGLYPH_W, fGLYPH_H,    1.0f,     0.0f,     FG_COL, FG_COL, FG_COL, 1.0f},    //idek at this poiint lol

    { fGLYPH_W,   0.0f,    1.0f,     1.0f,     FG_COL, FG_COL, FG_COL, 1.0f },      //idek at this poiint lol
    {0.0f,       0.0f,    0.0f,     1.0f,     FG_COL, FG_COL, FG_COL, 1.0f},    //idek at this poiint lol
    {fGLYPH_W, fGLYPH_H,    1.0f,     0.0f,     FG_COL, FG_COL, FG_COL, 1.0f}     //idek at this poiint lol
};
*/

struct WindowExtents {
    float w, h;
} winext;

struct GlyphSize {
    float x, y;
} glyphsize;

struct GlyphMapSize {
    float x, y;
} glyphmapsize;

struct UniformBuf {
    float win_w, win_h;
    float map_w, map_h;
    float glyph_w, glyph_h;
    Uint32 cpr, gpr; //chars per row (screen), glyphs per row (map)
} uniformbuf;

SDL_Window* window;
SDL_GPUDevice* device;

//vert shader resources
SDL_GPUBuffer* charDataBuffer;
CharData* charDataPtr;

//frag shader resources
SDL_GPUTexture* fontMapTexture;
SDL_GPUSampler* fontMapSampler;

//tx buffers and pipeline
SDL_GPUTransferBuffer* charTransferBuffer;
SDL_GPUTransferBuffer* texTransferBuffer;
SDL_GPUGraphicsPipeline* graphicsPipeline;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    //todo request screen size and scale
    //system screen scaling?

    r_width = uGLYPH_W * chars_per_line; //window width from glyph width and line len
    r_height = uGLYPH_H * n_lines;       //window height from glyph height and line count

    n_chars = chars_per_line * n_lines;  //calculate total number of chars in window

    w_width = r_width * scale; w_height = r_height * scale;
    winext = { (float)r_width, (float)r_height };
    glyphsize = { fGLYPH_W, fGLYPH_H };

    //n_chars = 2;

    charData = new CharData[n_chars]; //create char (vertex) buffer
    if (charData == NULL) {
        SDL_Log("Couldn't allocate memory for char array: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    for (Uint32 i = 0; i < n_chars; ++i) {
        //float col = i % chars_per_line;
        //float row = i / chars_per_line;

        //line 0 at top
        //float linepos = (r_height - glyphsize.y) - (glyphsize.y * row);
        //float xpos = glyphsize.x * col;

        Uint32 char_id = i % 256;
        if (i > 255) {
            char_id = rand() % 256; //debug random chars after full glyphset
        }

        //float tex_x = (char_id % 16) * fGLYPH_W;
        //float tex_y = (char_id / 16) * fGLYPH_H;

        charData[i] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 0.0f,
            char_id
        };
    }

    charData[0].character = 48;
    charData[0].r = 0.f;

    charData[1].character = 49;
    charData[1].a1 = 1.0f;

    const char* helo = "Hello Borld :)";

    for (int x = 0; x < 256; ++x) {
        char next = *(helo + (sizeof(char) * x));
        if (next == '\0') {
            break;
        }
        charData[x + 2].character = next;
    }

    charData[n_chars - 1].g = 0.f;
    charData[n_chars - 1].b = 0.f;

    /* Create the window */
    window = SDL_CreateWindow("Oldskool Fonts Utility", w_width, w_height, NULL);
    if (window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

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

    /* Create vertex shader */
    //load vertex shader code
    size_t vertexCodeSize;
    void* vertexCode = SDL_LoadFile(
        (std::filesystem::current_path().string() 
            + "\\shaders\\glyph2_v.spv").c_str(), 
        &vertexCodeSize);
    SDL_Log("Vertex code size: %i", vertexCodeSize);

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

    //free code file
    SDL_free(vertexCode);


    /* Create fragment shader */
    //load fragment code
    size_t fragmentCodeSize;
    void* fragmentCode = SDL_LoadFile(
        (std::filesystem::current_path().string() 
            + "\\shaders\\glyph_f.spv").c_str(), 
        &fragmentCodeSize);
    SDL_Log("Fragment code size: %i", fragmentCodeSize);

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

    //free code file
    SDL_free(fragmentCode);

    /* Load texture and create sampler */
    SDL_Surface* font_in_1 = SDL_LoadBMP((std::filesystem::current_path().string()
        + "\\shaders\\gtest1a.bmp").c_str());
    if (font_in_1 == NULL) {
        SDL_Log("Couldn't load image: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Got image format: %i", font_in_1->format);

    SDL_Surface* font_in = SDL_ConvertSurface(font_in_1, SDL_PIXELFORMAT_ABGR8888); //SDL_PIXELFORMAT_RGB565
    SDL_Log("Converted to image format: %i", font_in->format);

    SDL_GPUTextureCreateInfo textureInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, //SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = (Uint32) font_in->w,
        .height = (Uint32) font_in->h,
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
    /*
    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
    vertexBufferDescriptions[0] = {
        .slot = 0,
        .pitch = sizeof(Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;


    //describe vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[] = {
        //charPosition
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = 0
        },
        //glyphCoords
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = sizeof(float) * 2
        },
        //glyphTint
        {
            .location = 2,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
            .offset = sizeof(float) * 4
        }
    };

    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    */

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
        .size = n_chars * sizeof(CharData)
    };
    charDataBuffer = SDL_CreateGPUBuffer(device, &vertBufferInfo);

    //create transfer buffer to upload to vertex buffer
    SDL_GPUTransferBufferCreateInfo vertTransferInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = n_chars * sizeof(CharData) 
    };
    charTransferBuffer = SDL_CreateGPUTransferBuffer(device, &vertTransferInfo);

    //fill transfer buffer
    charDataPtr = (CharData*)SDL_MapGPUTransferBuffer(
        device, 
        charTransferBuffer, 
        false
    );
    SDL_memcpy(
        charDataPtr, 
        (void*)charData, 
        n_chars * sizeof(CharData)
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
        .size = n_chars * sizeof(CharData)
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

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    //for (Uint32 n = 256; n < n_chars; ++n) {
    //    charBuffer[n].character = rand() % 256;
    //}

    //fill transfer buffer
    charDataPtr = (CharData*)SDL_MapGPUTransferBuffer(
        device,
        charTransferBuffer,
        false
    );
    SDL_memcpy(
        charDataPtr,
        (void*)charData,
        n_chars * sizeof(CharData)
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
        .size = n_chars * sizeof(CharData)
    };

    //upload
    SDL_UploadToGPUBuffer(copyPass, &vertLocation, &vertRegion, true);

    //SDL_Log("copied vertex data");
    SDL_EndGPUCopyPass(copyPass);


    //get swapchain texture
    SDL_GPUTexture* swapchainTexture;
    Uint32 width, height;

    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height);
    if (swapchainTexture == NULL) {
        SDL_SubmitGPUCommandBuffer(commandBuffer); //always submit command buffer
        SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
        return SDL_APP_CONTINUE;
    }
    

    //create colour target
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = { BG_COL, BG_COL, BG_COL, 1.0f };
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    uniformbuf = { 
        winext.w, winext.h, 
        glyphmapsize.x, glyphmapsize.y, 
        glyphsize.x, glyphsize.y,
        chars_per_line, (Uint32)16
    };

    //push uniform data before render pass
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniformbuf, sizeof(uniformbuf));

    //start render pass
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

    //draw something
    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

    /*SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer;
    bufferBindings[0].offset = 0;
    SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);*/

    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &charDataBuffer, 1);
    //SDL_BindGPUVertexStorageBuffers;


    SDL_GPUTextureSamplerBinding textureSamplerBindings[1];
    textureSamplerBindings[0].texture = fontMapTexture;
    textureSamplerBindings[0].sampler = fontMapSampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, textureSamplerBindings, 1);


    SDL_DrawGPUPrimitives( renderPass, 
        n_chars * 6,    //draw 6 verts for every char
        1, //draw all chars as separate instaces? 
        0, 0);


    //end render pass
    SDL_EndGPURenderPass(renderPass);

    //submit command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_Log("Last error: %s", SDL_GetError());

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
}