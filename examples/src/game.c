#include <mygame/game.h>
#include <RC2D/RC2D.h>
#include <RC2D/RC2D_time.h>
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

// Structure pour un PNJ
typedef struct {
    float x, y, health; // État du PNJ : position x, y et santé
    int action;         // Action prédite (0=left, 1=right, 2=jump)
} NPC;

#define MAX_NPCS 150
static NPC npcs[MAX_NPCS];
static size_t npc_count = 150; // Nombre de PNJ actifs
static RC2D_OnnxModel model;
static float* npc_states = NULL;   // Buffer préalloué pour les états des PNJ
static float* npc_actions = NULL;  // Buffer préalloué pour les scores d'action

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
    // Libère les buffers préalloués
    SDL_free(npc_states);
    SDL_free(npc_actions);
    // Libère la session ONNX
    rc2d_onnx_unloadModel(&model);
}

void rc2d_load(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");

    rc2d_window_setTitle("Test jeu");
    rc2d_window_setMinimumSize(1280, 720);

    RC2D_DateFormat *dateFormat;
    RC2D_TimeFormat *timeFormat;
    rc2d_time_getDateTimeLocalePreferences(&dateFormat, &timeFormat);
    RC2D_log(RC2D_LOG_INFO, "Date format: %d, Time format: %d", dateFormat, timeFormat);

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
     * Initialisation des PNJ et chargement du modèle ONNX
     */
    // Initialisation des états des PNJ (exemple avec valeurs aléatoires)
    for (size_t i = 0; i < npc_count; ++i) {
        npcs[i].x = (float)(rand() % 100) / 100.0f; // Position x entre 0 et 1
        npcs[i].y = (float)(rand() % 100) / 100.0f; // Position y entre 0 et 1
        npcs[i].health = (float)(rand() % 100) / 100.0f; // Santé entre 0 et 1
        npcs[i].action = 0; // Action initiale
    }

    // Préalloue les buffers pour les états et les actions
    npc_states = SDL_malloc(MAX_NPCS * 3 * sizeof(float));
    npc_actions = SDL_malloc(MAX_NPCS * 3 * sizeof(float));
    if (!npc_states || !npc_actions) {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to allocate NPC buffers");
        return;
    }

    // Charge le modèle ONNX avec support des batchs dynamiques
    model.path = "models-onnx/game_ai_model.onnx";
    model.session = NULL;
    model.dynamic_batch = true; // Active les formes dynamiques [N, 3]
    if (!rc2d_onnx_loadModel(&model)) {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to load ONNX model");
        SDL_free(npc_states);
        SDL_free(npc_actions);
        return;
    }

    /**
     * Test de l'API ONNX Runtime pour un seul PNJ (comme dans le script Python)
     */
    // Données simulées : position x, y et santé (comme dans le test Python)
    float test_state[3] = { 0.5f, 0.5f, 0.2f }; // milieu, faible santé → devrait "sauter" (classe 2)
    RC2D_OnnxTensor input = {
        .name = "player_state",
        .data = test_state,
        .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        .shape = {1, 3},
        .dims = 2
    };
    float output_logits[3];
    RC2D_OnnxTensor output = {
        .name = "action_logits",
        .data = output_logits
        // type/shape/dims remplis automatiquement
    };
    if (!rc2d_onnx_run(&model, &input, &output)) {
        SDL_free(npc_states);
        SDL_free(npc_actions);
        return;
    }
    int best_action = 0;
    float max_score = output_logits[0];
    for (int i = 1; i < 3; ++i) {
        if (output_logits[i] > max_score) {
            max_score = output_logits[i];
            best_action = i;
        }
    }
    RC2D_log(RC2D_LOG_INFO, "Test predicted action: %d (0=left, 1=right, 2=jump)", best_action);
}

void rc2d_update(double dt) 
{
    /**
     * Inférence ONNX pour 150 PNJ
     */
    // Remplit le buffer d'entrée avec les états des PNJ
    for (size_t i = 0; i < npc_count; ++i) {
        npc_states[i * 3 + 0] = npcs[i].x;      // Position x
        npc_states[i * 3 + 1] = npcs[i].y;      // Position y
        npc_states[i * 3 + 2] = npcs[i].health; // Santé
    }

    RC2D_OnnxTensor input = {
        .name = "player_state",
        .data = npc_states,
        .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        .shape = {(int64_t)npc_count, 3}, // [150, 3] pour 150 PNJ et 3 valeurs par PNJ
        .dims = 2
    };

    // Prépare le tenseur de sortie
    RC2D_OnnxTensor output = {
        .name = "action_logits",
        .data = npc_actions
        // type/shape/dims remplis automatiquement par rc2d_onnx_run
    };

    // Exécute l'inférence
    if (!rc2d_onnx_run(&model, &input, &output)) {
        return;
    }

    // Traite les résultats pour assigner des actions à chaque PNJ
    for (size_t i = 0; i < npc_count; ++i) {
        int best_action = 0;
        float max_score = npc_actions[i * 3 + 0];
        for (int j = 1; j < 3; ++j) {
            if (npc_actions[i * 3 + j] > max_score) {
                max_score = npc_actions[i * 3 + j];
                best_action = j;
            }
        }
        npcs[i].action = best_action;
        // Log des actions pour les 5 premiers PNJ (débogage)
        if (i < 5) {
            RC2D_log(RC2D_LOG_DEBUG, "NPC %zu: x=%.2f, y=%.2f, health=%.2f, action=%d",
                     i, npcs[i].x, npcs[i].y, npcs[i].health, npcs[i].action);
        }
    }

    RC2D_log(RC2D_LOG_INFO, "Predicted actions for %zu NPCs (0=left, 1=right, 2=jump)", npc_count);

    // Mise à jour des états des PNJ (exemple simplifié)
    for (size_t i = 0; i < npc_count; ++i) {
        // Exemple : déplacer les PNJ selon leur action
        switch (npcs[i].action) {
            case 0: // left
                npcs[i].x -= 0.01f * dt;
                break;
            case 1: // right
                npcs[i].x += 0.01f * dt;
                break;
            case 2: // jump
                npcs[i].y += 0.02f * dt;
                break;
        }
        // Limiter les positions (exemple)
        npcs[i].x = SDL_clamp(npcs[i].x, 0.0f, 1.0f);
        npcs[i].y = SDL_clamp(npcs[i].y, 0.0f, 1.0f);
    }
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