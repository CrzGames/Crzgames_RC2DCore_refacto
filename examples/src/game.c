#include <mygame/game.h>
#include <RC2D/RC2D.h>
#include <RC2D/RC2D_internal.h>

static RC2D_GPUShader* fragmentShader;
static RC2D_GPUShader* vertexShader;
static RC2D_GPUGraphicsPipeline graphicsPipeline;
static SDL_GPUColorTargetDescription colorTargetDesc;
static SDL_GPUGraphicsPipelineTargetInfo targetInfo;

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
}

void rc2d_load(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");

    fragmentShader = rc2d_gpu_loadShader("test.fragment");
    RC2D_assert_release(fragmentShader != NULL, RC2D_LOG_CRITICAL, "Failed to load fragment shader");

    vertexShader = rc2d_gpu_loadShader("test.vertex");
    RC2D_assert_release(vertexShader != NULL, RC2D_LOG_CRITICAL, "Failed to load vertex shader");

    // Aucune entrée de vertex buffer utilisée ici (full screen quad généré via SV_VertexID)
    SDL_GPUVertexInputState vertexInput = {
        .vertex_buffer_descriptions = NULL,
        .num_vertex_buffers = 0,
        .vertex_attributes = NULL,
        .num_vertex_attributes = 0
    };

    SDL_GPURasterizerState rasterizer = {
        .fill_mode = SDL_GPU_FILLMODE_FILL,
        .cull_mode = SDL_GPU_CULLMODE_NONE,
        .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        .enable_depth_bias = false,
        .enable_depth_clip = true
    };

    SDL_GPUMultisampleState multisample = {
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .sample_mask = 0,
        .enable_mask = false
    };

    SDL_GPUStencilOpState defaultStencil = {
        .fail_op = SDL_GPU_STENCILOP_KEEP,
        .pass_op = SDL_GPU_STENCILOP_KEEP,
        .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
        .compare_op = SDL_GPU_COMPAREOP_ALWAYS
    };

    SDL_GPUDepthStencilState depthStencil = {
        .back_stencil_state = defaultStencil,
        .front_stencil_state = defaultStencil,
        .compare_mask = 0xFF,
        .write_mask = 0xFF,
        .enable_depth_test = false,
        .enable_depth_write = false,
        .enable_stencil_test = false
    };

    SDL_GPUTextureFormat swapchainFormat = SDL_GetGPUSwapchainTextureFormat(
        rc2d_engine_state.gpu_device,
        rc2d_engine_state.window
    );

    colorTargetDesc = (SDL_GPUColorTargetDescription){
        .format = swapchainFormat,
        .blend_state = { 0 },
    };
    
    targetInfo = (SDL_GPUGraphicsPipelineTargetInfo){
        .color_target_descriptions = &colorTargetDesc,
        .num_color_targets = 1,
        .depth_stencil_format = 0,
        .has_depth_stencil_target = false
    };    

    graphicsPipeline.create_info = (RC2D_GPUGraphicsPipelineCreateInfo){
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = vertexInput,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = rasterizer,
        .multisample_state = multisample,
        .depth_stencil_state = depthStencil,
        .target_info = targetInfo,
        .props = 0
    };

    graphicsPipeline.debug_name = "FullscreenShaderPipeline";
    graphicsPipeline.vertex_shader_filename = SDL_strdup("test.vertex");
    graphicsPipeline.fragment_shader_filename = SDL_strdup("test.fragment");

    bool success = rc2d_gpu_createGraphicsPipeline(&graphicsPipeline, true);
    RC2D_assert_release(success, RC2D_LOG_CRITICAL, "Failed to create full screen shader pipeline");
}

void rc2d_update(double dt) 
{
    // Pas de logique de mise à jour pour l'instant
}

void rc2d_draw(void) 
{
    rc2d_gpu_bindGraphicsPipeline(&graphicsPipeline);

    SDL_DrawGPUPrimitives(
        rc2d_engine_state.gpu_current_render_pass,
        3, 1, 0, 0
    );
}
