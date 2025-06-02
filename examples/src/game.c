#include <mygame/game.h>

#include <RC2D/RC2D.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_memory.h>
#include <RC2D/RC2D_imgui.h>

#include <cimgui.h>

static RC2D_GPUComputePipeline* computeShader;
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

    /* Libérer les ressources allouées */
    if (graphicsPipeline.vertex_shader_filename) {
        RC2D_free(graphicsPipeline.vertex_shader_filename); 
        graphicsPipeline.vertex_shader_filename = NULL;
    }
    if (graphicsPipeline.fragment_shader_filename) {
        RC2D_free(graphicsPipeline.fragment_shader_filename);
        graphicsPipeline.fragment_shader_filename = NULL;
    }
    if (graphicsPipeline.pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(rc2d_gpu_getDevice(), graphicsPipeline.pipeline);
        graphicsPipeline.pipeline = NULL;
    }
    if (vertexShader) {
        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), vertexShader);
        vertexShader = NULL;
    }
    if (fragmentShader) {
        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), fragmentShader);
        fragmentShader = NULL;
    }
    if (computeShader) {
        SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), computeShader);
        computeShader = NULL;
    }
}

void rc2d_load(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");

    // Charger les shaders graphiques
    fragmentShader = rc2d_gpu_loadGraphicsShader("test.fragment");
    RC2D_assert_release(fragmentShader != NULL, RC2D_LOG_CRITICAL, "Failed to load fragment shader");

    vertexShader = rc2d_gpu_loadGraphicsShader("test.vertex");
    RC2D_assert_release(vertexShader != NULL, RC2D_LOG_CRITICAL, "Failed to load vertex shader");

    // Charger le compute shader
    computeShader = rc2d_gpu_loadComputeShader("test.compute");
    RC2D_assert_release(computeShader != NULL, RC2D_LOG_CRITICAL, "Failed to load compute shader");

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
    graphicsPipeline.vertex_shader_filename = RC2D_strdup("test.vertex");
    graphicsPipeline.fragment_shader_filename = RC2D_strdup("test.fragment");

    bool success = rc2d_gpu_createGraphicsPipeline(&graphicsPipeline, true);
    RC2D_assert_release(success, RC2D_LOG_CRITICAL, "Failed to create full screen shader pipeline");
}

void rc2d_update(double dt) 
{

}

void rc2d_draw(void)
{
    // -------------------- SHADER COMPUTE -------------------
    // (Votre code compute shader inchangé)

    // ------------------- SHADER GRAPHICS -------------------
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

    // ------------------- IMGUI INTERFACE -------------------
    static bool show_editor = true;
    static float effect_value = 0.5f;
    static char map_name[128] = "New Map";
    static int current_tile_type = 0;
    static int current_layer = 0;
    static const char* tile_types[] = {"Grass", "Water", "Stone", "Sand"};
    static const char* layers[] = {"Background", "Foreground", "Collision"};

    rc2d_cimgui_setNextWindowPos(0.0f, 0.0f, RC2D_IMGUI_SET_CONDITION_ALWAYS);
    if (rc2d_cimgui_begin("Map Editor", &show_editor, 
        RC2D_IMGUI_WINDOW_FLAGS_NO_DOCKING)) 
    {
        // Champ de texte pour le nom de la map
        if (rc2d_cimgui_input_text("Map Name", map_name, sizeof(map_name), RC2D_IMGUI_INPUT_TEXT_FLAGS_NONE)) {
            RC2D_log(RC2D_LOG_INFO, "Map name changed to: %s", map_name);
        }

        // Combo box pour le type de tuile
        if (rc2d_cimgui_combo("Tile Type", &current_tile_type, tile_types, 4, -1)) {
            RC2D_log(RC2D_LOG_INFO, "Selected tile type: %s", tile_types[current_tile_type]);
        }

        // List box pour les couches
        if (rc2d_cimgui_list_box("Layers", &current_layer, layers, 3, -1)) {
            RC2D_log(RC2D_LOG_INFO, "Selected layer: %s", layers[current_layer]);
        }

        // Bouton pour ouvrir un popup contextuel
        if (rc2d_cimgui_button("Open Context Menu", 0.0f, 0.0f)) {
            rc2d_cimgui_open_popup("ContextMenu", RC2D_IMGUI_POPUP_FLAGS_NONE);
        }

        // Popup contextuel
        if (rc2d_cimgui_begin_popup("ContextMenu", RC2D_IMGUI_WINDOW_FLAGS_NONE)) {
            rc2d_cimgui_text("Context Menu");
            if (rc2d_cimgui_button("Add Layer", 0.0f, 0.0f)) {
                RC2D_log(RC2D_LOG_INFO, "Add Layer clicked");
            }
            if (rc2d_cimgui_button("Remove Layer", 0.0f, 0.0f)) {
                RC2D_log(RC2D_LOG_INFO, "Remove Layer clicked");
            }
            rc2d_cimgui_end();
        }

        rc2d_cimgui_checkbox("Show Metrics", &show_editor);
        rc2d_cimgui_sliderFloat("Effect Value", &effect_value, 0.0f, 1.0f, "%.3f", RC2D_IMGUI_SLIDER_FLAGS_CLAMP_ON_INPUT);

        rc2d_cimgui_end();
    }

    if (show_editor) {
        igShowMetricsWindow(&show_editor);
    }
}