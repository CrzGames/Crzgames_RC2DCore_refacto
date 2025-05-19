#include <mygame/game.h>
#include <RC2D/RC2D.h>
#include <RC2D/RC2D_time.h>
#include <RC2D/RC2D_thread.h>
#include <RC2D/RC2D_internal.h>

static RC2D_GPUShader* fragmentShader;
static RC2D_GPUShader* vertexShader;
static RC2D_GPUGraphicsPipeline graphicsPipeline;
static SDL_GPUColorTargetDescription colorTargetDesc;
static SDL_GPUGraphicsPipelineTargetInfo targetInfo;

// Variable globale pour le thread de test
static RC2D_Thread* testThread = NULL;
static bool threadDetached = false;

typedef struct UniformBlock {
    float resolution[2]; // float2
    float time;          // float
    float padding;       // align to 16 bytes (std140)
} UniformBlock;

// Fonction exécutée par le thread de test
static int testThreadFunction(void *data) {
    RC2D_log(RC2D_LOG_INFO, "Thread de test démarré\n");

    // Définit une priorité haute pour simuler une tâche critique
    if (!rc2d_thread_setPriority(RC2D_THREAD_PRIORITY_HIGH)) {
        RC2D_log(RC2D_LOG_WARN, "Échec de la définition de la priorité haute pour le thread\n");
    }

    // Simule un chargement de ressources (attente de 5 secondes)
    RC2D_DateTime startTime;
    rc2d_time_getCurrentTime(&startTime);
    for (int i = 0; i < 5; i++) {
        SDL_Delay(1000); // Attend 1 seconde
        RC2D_log(RC2D_LOG_INFO, "Thread de test : progression %d/5\n", i + 1);
    }

    RC2D_DateTime endTime;
    rc2d_time_getCurrentTime(&endTime);
    RC2D_log(RC2D_LOG_INFO, "Thread de test terminé en %d secondes\n", endTime.second - startTime.second);

    return 42; // Code de retour arbitraire
}

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");

    // Nettoie le thread s'il n'est pas détaché
    if (testThread && !threadDetached) {
        int status;
        rc2d_thread_wait(testThread, &status);
        RC2D_log(RC2D_LOG_INFO, "Thread de test terminé avec le code de retour : %d\n", status);
    }
    testThread = NULL;
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

    // Crée un thread de test avec des options personnalisées
    RC2D_ThreadOptions options = {
        .stack_size = 0, // Taille par défaut
        .priority = RC2D_THREAD_PRIORITY_NORMAL,
        .auto_detach = false // Ne pas détacher automatiquement pour tester rc2d_thread_wait
    };
    testThread = rc2d_thread_newWithOptions(testThreadFunction, "TestLoadingThread", NULL, &options);
    if (!testThread) {
        RC2D_log(RC2D_LOG_CRITICAL, "Échec de la création du thread de test\n");
    } else {
        RC2D_log(RC2D_LOG_INFO, "Thread de test créé avec succès\n");
    }
}

void rc2d_update(double dt) 
{
    // Surveille l'état du thread
    if (testThread) {
        RC2D_ThreadState state = rc2d_thread_getState(testThread);
        const char *stateStr;
        switch (state) {
            case RC2D_THREAD_STATE_UNKNOWN:
                stateStr = "Inconnu";
                break;
            case RC2D_THREAD_STATE_ALIVE:
                stateStr = "En cours";
                break;
            case RC2D_THREAD_STATE_DETACHED:
                stateStr = "Détaché";
                break;
            case RC2D_THREAD_STATE_COMPLETE:
                stateStr = "Terminé";
                break;
            default:
                stateStr = "Erreur";
        }
        RC2D_log(RC2D_LOG_DEBUG, "État du thread de test : %s\n", stateStr);

        // Teste le détachement si le thread est terminé
        if (state == RC2D_THREAD_STATE_COMPLETE && !threadDetached) {
            RC2D_log(RC2D_LOG_INFO, "Détachement du thread de test\n");
            rc2d_thread_detach(testThread);
            threadDetached = true;
            testThread = NULL;
        }
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