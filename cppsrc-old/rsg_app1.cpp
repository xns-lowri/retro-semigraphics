/* 
 * Thanks to Sam Lantiga for "hello.c"
 * Thanks to Hamdy Elzanqali for "Let there be triangles, SDL_gpu edition"
 * Thanks to Evan Hemsley for "SDL GPU API Concepts: Sprite Batcher"
 * 
 * TODO info
 */

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include <string>


//vertex input layout struct
struct Vertex 
{
    float x, y, z;      //vec3 position
    float r, g, b, a;   //vec4 color
};

//list of verts
static Vertex vertices[]
{
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
};

SDL_Window* window;
SDL_GPUDevice* device;

SDL_GPUBuffer* vertexBuffer;
SDL_GPUTransferBuffer* transferBuffer;
SDL_GPUGraphicsPipeline* graphicsPipeline;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    /* Create the window */
    window = SDL_CreateWindow("Oldskool Fonts Utility", 1280, 700, NULL);
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
            + "\\shaders\\vertex.spv").c_str(), 
        &vertexCodeSize);

    SDL_Log("Vertex code size: %i", vertexCodeSize);

    //create vertex shader
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8*)vertexCode;
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV; //.spv shader
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;  //vertex shader (vert positioning)
    vertexInfo.num_samplers = 0;
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 0;

    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);

    //free code file
    SDL_free(vertexCode);


    /* Create fragment shader */
    //load fragment code
    size_t fragmentCodeSize;
    void* fragmentCode = SDL_LoadFile(
        (std::filesystem::current_path().string() 
            + "\\shaders\\fragment.spv").c_str(), 
        &fragmentCodeSize);

    SDL_Log("Fragment code size: %i", fragmentCodeSize);

    //create fragment shader
    SDL_GPUShaderCreateInfo fragmentInfo{};
    fragmentInfo.code = (Uint8*)fragmentCode;
    fragmentInfo.code_size = fragmentCodeSize;
    fragmentInfo.entrypoint = "main";
    fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragmentInfo.num_samplers = 0;
    fragmentInfo.num_storage_buffers = 0;
    fragmentInfo.num_storage_textures = 0;
    fragmentInfo.num_uniform_buffers = 0;

    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

    //free code file
    SDL_free(fragmentCode);


    /* Create graphics pipeline */
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    //describe vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
    vertexBufferDescriptions[0].slot = 0;
    vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescriptions[0].instance_step_rate = 0;
    vertexBufferDescriptions[0].pitch = sizeof(Vertex);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;

    //describe vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];

    //a_position
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset = 0;

    //a_color
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].offset = sizeof(float) * 3;

    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

    //describe color target
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    //fgd
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    //dfg
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    //release shaders
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    //create vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    //create transfer buffer to upload to vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = sizeof(vertices);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    //fill transfer buffer
    Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    SDL_memcpy(data, (void*)vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);


    /* Start copy pass */
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    //where data is
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    //where to upload data
    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size = sizeof(vertices);
    region.offset = 0;

    //upload
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    /* End of copy pass */

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
    //acquire command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

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
    colorTargetInfo.clear_color = { 240 / 255.f, 240 / 255.f, 240 / 255.f, 255 / 255.f };
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    //start render pass
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

    //draw something
    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);
    SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer;
    bufferBindings[0].offset = 0;
    SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

    SDL_DrawGPUPrimitives(renderPass, sizeof(vertices)/sizeof(Vertex), 1, 0, 0);

    //end render pass
    SDL_EndGPURenderPass(renderPass);

    //submit command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    //release buffers
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    //reelease the pipeline
    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

    //destroy GPU device
    SDL_DestroyGPUDevice(device);

    //destroy window
    SDL_DestroyWindow(window);
}