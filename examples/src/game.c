#include <mygame/game.h>
#include <RC2D/RC2D.h>
#include <RC2D/RC2D_internal.h>

static RC2D_GPUShader* fragmentShader;
static RC2D_GPUShader* vertexShader;
static RC2D_GPUGraphicsPipeline graphicsPipeline;
static SDL_GPUColorTargetDescription colorTargetDesc;
static SDL_GPUGraphicsPipelineTargetInfo targetInfo;

typedef struct UniformBlock {
    float resolution[2]; // float2
    float time;          // float
    float padding;       // align to 16 bytes (std140)
} UniformBlock;

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
}

void rc2d_load(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");

    rc2d_window_setTitle("Test jeu");
    rc2d_window_setMinimumSize(1280, 720);

    fragmentShader = rc2d_gpu_loadShader("test.fragment");
    RC2D_assert_release(fragmentShader != NULL, RC2D_LOG_CRITICAL, "Failed to load fragment shader");

    vertexShader = rc2d_gpu_loadShader("test.vertex");
    RC2D_assert_release(vertexShader != NULL, RC2D_LOG_CRITICAL, "Failed to load vertex shader");

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
        rc2d_gpu_getDevice(),
        rc2d_window_getWindow()
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

    /**
     * Test de l'API ONNX Runtime, pour l'inférence d'un modèle ONNX.
     */
    RC2D_OnnxTensor inputs[] = {
        {
            .name = "input_tensor_1",
            .data = my_input_data,
            .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
            .shape = {1, 3},
            .dims = 2
        },
        {
            .name = "input_tensor_2",
            .data = my_bool_flag,
            .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL,
            .shape = {1},
            .dims = 1
        },
        {
            .name = "input_tensor_3",
            .data = (void*)my_strings,
            .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING,
            .shape = {1, 2},
            .dims = 2
        }
    };

    RC2D_OnnxTensor outputs[] = {
        {
            .name = "output_tensor_scores", // Pour te repérer dans outputs[0]
            .data = my_output_scores        // float[10] déjà alloué
            // type / shape / dims seront remplis automatiquement
        },
        {
            .name = "output_tensor_labels", // Pour te repérer dans outputs[1]
            .data = my_output_labels        // char* labels[2]; initialisé mais non alloué
            // type / shape / dims seront remplis automatiquement
        }
    };

    RC2D_OnnxModel model = {
        .path = "models-onnx/my_model.onnx",
        .session = NULL
    };

    if (!rc2d_onnx_loadModel(&model)) {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to load ONNX model");
        return;
    }

    if (!rc2d_onnx_run(&model, inputs, outputs)) {
        RC2D_log(RC2D_LOG_CRITICAL, "ONNX inference failed");
        return;
    }

    // Affiche les résultats de l'inférence
    RC2D_log(RC2D_LOG_INFO, "Output scores:");
    size_t count = rc2d_onnx_computeElementCount(outputs[0].shape, outputs[0].dims);
    float* scores = (float*)outputs[0].data;

    for (size_t i = 0; i < count; ++i) {
        RC2D_log(RC2D_LOG_INFO, "%f", scores[i]);
    }
}

void rc2d_update(double dt) 
{
    // rien pour l'instant
}

void rc2d_draw(void)
{
    rc2d_gpu_bindGraphicsPipeline(&graphicsPipeline);

    UniformBlock ubo = {
        .resolution = {
            (float)rc2d_window_getWidth(),
            (float)rc2d_window_getHeight()
        },
        .time = (float)(SDL_GetTicks() / 1000.0f),
        .padding = 0.0f
    };

    SDL_PushGPUFragmentUniformData(
        rc2d_engine_state.gpu_current_command_buffer,
        0, // slot b0
        &ubo,
        sizeof(UniformBlock)
    );

    SDL_DrawGPUPrimitives(
        rc2d_engine_state.gpu_current_render_pass,
        3, 1, 0, 0
    );
}
