#include <mygame/game.h>
#include <RC2D/RC2D.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_camera.h>

static RC2D_GPUShader* fragmentShader;
static RC2D_GPUShader* vertexShader;
static RC2D_GPUGraphicsPipeline graphicsPipeline;
static SDL_GPUColorTargetDescription colorTargetDesc;
static SDL_GPUGraphicsPipelineTargetInfo targetInfo;

// Variables globales pour le test de la caméra
static RC2D_Camera* test_camera = NULL;
static bool camera_permission_granted = false;

typedef struct UniformBlock {
    float resolution[2]; // float2
    float time;          // float
    float padding;       // align to 16 bytes (std140)
} UniformBlock;

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");

    // Ferme la caméra si ouverte
    if (test_camera) {
        rc2d_camera_close(test_camera);
        test_camera = NULL;
    }
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

    // Test du module RC2D_camera
    // Liste les caméras disponibles
    int count;
    RC2D_CameraID *devices = rc2d_camera_getDevices(&count);
    if (devices && count > 0) {
        RC2D_log(RC2D_LOG_INFO, "Caméras détectées : %d", count);
        for (int i = 0; devices[i]; i++) {
            const char *name = rc2d_camera_getName(devices[i]);
            RC2D_log(RC2D_LOG_INFO, "Caméra %d : %s (ID %u)", i, name ? name : "Inconnu", devices[i]);
        }

        // Ouvre la première caméra
        RC2D_CameraOptions options = {
            .spec = &(RC2D_CameraSpec){
                .format = SDL_PIXELFORMAT_RGB24,
                .width = 1280,
                .height = 720,
                .framerate = 30
            },
            .position = RC2D_CAMERA_POSITION_BACK_FACING
        };
        test_camera = rc2d_camera_open(devices[0], &options);
        if (test_camera) {
            RC2D_log(RC2D_LOG_INFO, "Caméra ouverte avec succès, en attente de permission");
            // Récupère et logue la spécification
            RC2D_CameraSpec spec;
            if (rc2d_camera_getSpec(test_camera, &spec)) {
                RC2D_log(RC2D_LOG_INFO, "Spécification : %dx%d, format %u, %d FPS", 
                         spec.width, spec.height, spec.format, spec.framerate);
            }
        } else {
            RC2D_log(RC2D_LOG_ERROR, "Échec de l'ouverture de la caméra");
        }
        SDL_free(devices);
    } else {
        RC2D_log(RC2D_LOG_WARN, "Aucune caméra détectée");
        if (devices) SDL_free(devices);
    }

    // Test de cas d'erreur : ID invalide
    test_camera = rc2d_camera_open(0, NULL);
    if (!test_camera) {
        RC2D_log(RC2D_LOG_INFO, "Échec attendu de l'ouverture avec ID invalide");
    }
}

void rc2d_update(double dt) 
{
    // Vérifie la permission et capture une image si approuvé
    if (test_camera && !camera_permission_granted) {
        int permission = rc2d_camera_getPermission(test_camera);
        if (permission == 1) {
            camera_permission_granted = true;
            RC2D_log(RC2D_LOG_INFO, "Permission caméra accordée");
        } else if (permission == -1) {
            RC2D_log(RC2D_LOG_ERROR, "Permission caméra refusée");
            rc2d_camera_close(test_camera);
            test_camera = NULL;
        }
    }

    // Capture une image si la permission est accordée
    if (test_camera && camera_permission_granted) {
        Uint64 timestamp_ns;
        SDL_Surface *frame = rc2d_camera_getFrame(test_camera, &timestamp_ns);
        if (frame) {
            RC2D_log(RC2D_LOG_INFO, "Image capturée : %dx%d à %llu ns", frame->w, frame->h, timestamp_ns);
            rc2d_camera_releaseFrame(test_camera, frame);
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