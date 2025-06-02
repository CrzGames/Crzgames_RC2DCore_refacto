#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>

#include <SDL3_image/SDL_image.h>

static void create_color_texture(SDL_GPUCommandBuffer *cmd, Uint8 r, Uint8 g, Uint8 b, Uint8 a, RC2D_GPUTexture *texture_info) {
    SDL_GPUDevice *device = rc2d_engine_state.gpu_device;

    // Créer une texture 1x1 pixel
    SDL_GPUTextureCreateInfo create_info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET,
        .width = 1,
        .height = 1,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0
    };

    texture_info->gpu_texture = SDL_CreateGPUTexture(device, &create_info);
    texture_info->width = 1;
    texture_info->height = 1;

    if (!texture_info->gpu_texture) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create color texture: %s", SDL_GetError());
        return;
    }

    // Créer un transfer buffer pour uploader la couleur
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = 4, // 4 bytes pour RGBA
        .props = 0
    };

    SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);
    if (!transfer_buffer) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device, texture_info->gpu_texture);
        texture_info->gpu_texture = NULL;
        return;
    }

    // Remplir le transfer buffer avec la couleur
    Uint8 *data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    if (!data) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to map transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
        SDL_ReleaseGPUTexture(device, texture_info->gpu_texture);
        texture_info->gpu_texture = NULL;
        return;
    }

    data[0] = r;
    data[1] = g;
    data[2] = b;
    data[3] = a;

    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // Uploader la couleur dans la texture
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
    if (!copy_pass) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to begin copy pass: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
        SDL_ReleaseGPUTexture(device, texture_info->gpu_texture);
        texture_info->gpu_texture = NULL;
        return;
    }

    SDL_GPUTextureTransferInfo source = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
        .pixels_per_row = 1,
        .rows_per_layer = 1
    };

    SDL_GPUTextureRegion destination = {
        .texture = texture_info->gpu_texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = 1,
        .h = 1,
        .d = 1
    };

    SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);
    SDL_EndGPUCopyPass(copy_pass);

    // Libérer le transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}

RC2D_GPUTexture rc2d_letterbox_loadTexture(SDL_GPUCommandBuffer *cmd, const char *filename, int letterbox_side) {
    RC2D_GPUTexture texture_info = {0};

    if (!filename) {
        RC2D_log(RC2D_LOG_WARN, "No texture filename provided");
        return texture_info;
    }

    RC2D_log(RC2D_LOG_DEBUG, "Attempting to load texture: %s (side: %d)", filename, letterbox_side);

    // Vérifier si c'est une couleur unie
    if (SDL_strncmp(filename, "color:", 6) == 0) {
        Uint8 r, g, b, a;
        if (SDL_sscanf(filename + 6, "%hhu,%hhu,%hhu,%hhu", &r, &g, &b, &a) == 4) {
            create_color_texture(cmd, r, g, b, a, &texture_info);
            if (texture_info.gpu_texture) {
                RC2D_log(RC2D_LOG_INFO, "Created color texture: %s", filename);
            }
            return texture_info;
        } else {
            RC2D_log(RC2D_LOG_ERROR, "Invalid color format: %s", filename);
            return texture_info;
        }
    }

    // Construire le chemin complet
    const char *basePath = SDL_GetBasePath();
    if (!basePath) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get base path for letterbox texture");
        return texture_info;
    }

    char fullPath[512];
    SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/images/%s", basePath, filename);
    RC2D_log(RC2D_LOG_DEBUG, "Loading image from: %s", fullPath);

    SDL_Surface *surface = NULL;

    // Charger SVG ou image raster
    if (SDL_strstr(filename, ".svg") != NULL) {
        SDL_IOStream *src = SDL_IOFromFile(fullPath, "rb");
        if (!src) {
            RC2D_log(RC2D_LOG_ERROR, "Failed to open SVG file %s: %s", fullPath, SDL_GetError());
            return texture_info;
        }

        // Récupérer les dimensions de la fenêtre et de la zone sûre
        int window_width, window_height;
        rc2d_window_getSize(&window_width, &window_height);
        RC2D_Rect safe_area;
        rc2d_window_getSafeArea(&safe_area);
        float pixel_density = rc2d_window_getPixelDensity();

        int svg_width = 0;
        int svg_height = (int)(safe_area.height * pixel_density);

        // Ajuster les dimensions selon le mode et le côté
        if (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_PER_SIDE) {
            if (letterbox_side == 2 || letterbox_side == 3) { // Haut ou bas
                svg_width = (int)(safe_area.width * pixel_density);
                svg_height = 0;
            }
        } else if (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_BACKGROUND_FULL) {
            svg_width = (int)(window_width * pixel_density);
            svg_height = (int)(window_height * pixel_density);
        } else if (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_UNIFORM) {
            // Utiliser une taille générique pour UNIFORM
            svg_width = 0;
            svg_height = (int)(safe_area.height * pixel_density);
        }

        RC2D_log(RC2D_LOG_DEBUG, "Rasterizing SVG: width=%d, height=%d", svg_width, svg_height);
        surface = IMG_LoadSizedSVG_IO(src, svg_width, svg_height);
        SDL_CloseIO(src);
        if (!surface) {
            RC2D_log(RC2D_LOG_ERROR, "Failed to load SVG %s: %s", fullPath, SDL_GetError());
            return texture_info;
        }
    } else {
        surface = IMG_Load(fullPath);
        if (!surface) {
            RC2D_log(RC2D_LOG_ERROR, "Failed to load image %s: %s", fullPath, SDL_GetError());
            return texture_info;
        }
    }

    // Vérifier si la surface est déjà en SDL_PIXELFORMAT_RGBA8888
    SDL_Surface *converted = surface;
    if (surface->format != SDL_PIXELFORMAT_RGBA8888) {
        converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
        SDL_DestroySurface(surface);
        if (!converted) {
            RC2D_log(RC2D_LOG_ERROR, "Failed to convert surface to RGBA8888: %s", SDL_GetError());
            return texture_info;
        }
        RC2D_log(RC2D_LOG_DEBUG, "Converted surface to RGBA8888");
    }

    SDL_GPUDevice *device = rc2d_engine_state.gpu_device;
    if (!device) {
        RC2D_log(RC2D_LOG_ERROR, "GPU device is NULL");
        SDL_DestroySurface(converted);
        return texture_info;
    }

    // Créer la texture GPU
    SDL_GPUTextureCreateInfo create_info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET,
        .width = (Uint32)converted->w,
        .height = (Uint32)converted->h,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0
    };

    texture_info.gpu_texture = SDL_CreateGPUTexture(device, &create_info);
    texture_info.width = converted->w;
    texture_info.height = converted->h;

    if (!texture_info.gpu_texture) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU texture: %s", SDL_GetError());
        SDL_DestroySurface(converted);
        return texture_info;
    }

    // Créer un transfer buffer
    Uint32 size = converted->w * converted->h * 4; // 4 bytes par pixel (RGBA)
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = size,
        .props = 0
    };

    SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);
    if (!transfer_buffer) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device, texture_info.gpu_texture);
        texture_info.gpu_texture = NULL;
        SDL_DestroySurface(converted);
        return texture_info;
    }

    // Copier les données de l'image dans le transfer buffer
    Uint8 *data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    if (!data) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to map transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
        SDL_ReleaseGPUTexture(device, texture_info.gpu_texture);
        texture_info.gpu_texture = NULL;
        SDL_DestroySurface(converted);
        return texture_info;
    }

    SDL_memcpy(data, converted->pixels, size);
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
    SDL_DestroySurface(converted);

    // Uploader les données dans la texture
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
    if (!copy_pass) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to begin copy pass: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
        SDL_ReleaseGPUTexture(device, texture_info.gpu_texture);
        texture_info.gpu_texture = NULL;
        return texture_info;
    }

    SDL_GPUTextureTransferInfo source = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
        .pixels_per_row = texture_info.width,
        .rows_per_layer = texture_info.height
    };

    SDL_GPUTextureRegion destination = {
        .texture = texture_info.gpu_texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = texture_info.width,
        .h = texture_info.height,
        .d = 1
    };

    SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);
    SDL_EndGPUCopyPass(copy_pass);

    // Libérer le transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    RC2D_log(RC2D_LOG_INFO, "Loaded texture: %s (%dx%d)", fullPath, texture_info.width, texture_info.height);
    return texture_info;
}

void rc2d_letterbox_init(void) 
{
    if (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_NONE) {
        RC2D_log(RC2D_LOG_INFO, "Letterbox mode is NONE, no textures will be loaded");
        return;
    }

    SDL_GPUDevice *device = rc2d_engine_state.gpu_device;
    if (!device) {
        RC2D_log(RC2D_LOG_CRITICAL, "GPU device is NULL");
        return;
    }

    // Libérer les textures GPU existantes
    if (rc2d_engine_state.letterbox_uniform_texture && rc2d_engine_state.letterbox_uniform_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_uniform_texture->gpu_texture);
        rc2d_engine_state.letterbox_uniform_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_uniform_texture->width = 0;
        rc2d_engine_state.letterbox_uniform_texture->height = 0;
    }
    if (rc2d_engine_state.letterbox_top_texture && rc2d_engine_state.letterbox_top_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_top_texture->gpu_texture);
        rc2d_engine_state.letterbox_top_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_top_texture->width = 0;
        rc2d_engine_state.letterbox_top_texture->height = 0;
    }
    if (rc2d_engine_state.letterbox_bottom_texture && rc2d_engine_state.letterbox_bottom_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_bottom_texture->gpu_texture);
        rc2d_engine_state.letterbox_bottom_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_bottom_texture->width = 0;
        rc2d_engine_state.letterbox_bottom_texture->height = 0;
    }
    if (rc2d_engine_state.letterbox_left_texture && rc2d_engine_state.letterbox_left_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_left_texture->gpu_texture);
        rc2d_engine_state.letterbox_left_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_left_texture->width = 0;
        rc2d_engine_state.letterbox_left_texture->height = 0;
    }
    if (rc2d_engine_state.letterbox_right_texture && rc2d_engine_state.letterbox_right_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_right_texture->gpu_texture);
        rc2d_engine_state.letterbox_right_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_right_texture->width = 0;
        rc2d_engine_state.letterbox_right_texture->height = 0;
    }
    if (rc2d_engine_state.letterbox_background_texture && rc2d_engine_state.letterbox_background_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_background_texture->gpu_texture);
        rc2d_engine_state.letterbox_background_texture->gpu_texture = NULL;
        rc2d_engine_state.letterbox_background_texture->width = 0;
        rc2d_engine_state.letterbox_background_texture->height = 0;
    }

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to acquire command buffer for letterbox init: %s", SDL_GetError());
        return;
    }

    RC2D_log(RC2D_LOG_DEBUG, "Letterbox textures mode: %d", rc2d_engine_state.letterbox_textures.mode);
    RC2D_log(RC2D_LOG_DEBUG, "Filenames: left=%s, right=%s, top=%s, bottom=%s, uniform=%s, background=%s",
             rc2d_engine_state.letterbox_textures.left_filename ? rc2d_engine_state.letterbox_textures.left_filename : "NULL",
             rc2d_engine_state.letterbox_textures.right_filename ? rc2d_engine_state.letterbox_textures.right_filename : "NULL",
             rc2d_engine_state.letterbox_textures.top_filename ? rc2d_engine_state.letterbox_textures.top_filename : "NULL",
             rc2d_engine_state.letterbox_textures.bottom_filename ? rc2d_engine_state.letterbox_textures.bottom_filename : "NULL",
             rc2d_engine_state.letterbox_textures.uniform_filename ? rc2d_engine_state.letterbox_textures.uniform_filename : "NULL",
             rc2d_engine_state.letterbox_textures.background_filename ? rc2d_engine_state.letterbox_textures.background_filename : "NULL");

    switch (rc2d_engine_state.letterbox_textures.mode) {
        case RC2D_LETTERBOX_UNIFORM:
            if (rc2d_engine_state.letterbox_textures.uniform_filename) {
                *rc2d_engine_state.letterbox_uniform_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.uniform_filename, -1);
                RC2D_log(RC2D_LOG_DEBUG, "Uniform texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.uniform_filename,
                         rc2d_engine_state.letterbox_uniform_texture->gpu_texture ? "yes" : "no");
            }
            break;

        case RC2D_LETTERBOX_PER_SIDE:
            if (rc2d_engine_state.letterbox_textures.left_filename) {
                *rc2d_engine_state.letterbox_left_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.left_filename, 0);
                RC2D_log(RC2D_LOG_DEBUG, "Left texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.left_filename,
                         rc2d_engine_state.letterbox_left_texture->gpu_texture ? "yes" : "no");
            }
            if (rc2d_engine_state.letterbox_textures.right_filename) {
                *rc2d_engine_state.letterbox_right_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.right_filename, 1);
                RC2D_log(RC2D_LOG_DEBUG, "Right texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.right_filename,
                         rc2d_engine_state.letterbox_right_texture->gpu_texture ? "yes" : "no");
            }
            if (rc2d_engine_state.letterbox_textures.top_filename) {
                *rc2d_engine_state.letterbox_top_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.top_filename, 2);
                RC2D_log(RC2D_LOG_DEBUG, "Top texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.top_filename,
                         rc2d_engine_state.letterbox_top_texture->gpu_texture ? "yes" : "no");
            }
            if (rc2d_engine_state.letterbox_textures.bottom_filename) {
                *rc2d_engine_state.letterbox_bottom_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.bottom_filename, 3);
                RC2D_log(RC2D_LOG_DEBUG, "Bottom texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.bottom_filename,
                         rc2d_engine_state.letterbox_bottom_texture->gpu_texture ? "yes" : "no");
            }
            break;

        case RC2D_LETTERBOX_BACKGROUND_FULL:
            if (rc2d_engine_state.letterbox_textures.background_filename) {
                *rc2d_engine_state.letterbox_background_texture = rc2d_letterbox_loadTexture(cmd, rc2d_engine_state.letterbox_textures.background_filename, -1);
                RC2D_log(RC2D_LOG_DEBUG, "Background texture loaded: %s (valid: %s)",
                         rc2d_engine_state.letterbox_textures.background_filename,
                         rc2d_engine_state.letterbox_background_texture->gpu_texture ? "yes" : "no");
            }
            break;

        default:
            RC2D_log(RC2D_LOG_WARN, "Unknown letterbox mode: %d", rc2d_engine_state.letterbox_textures.mode);
            break;
    }

    SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    if (!fence) {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to submit command buffer for letterbox init: %s", SDL_GetError());
        return;
    }

    if (!SDL_WaitForGPUFences(rc2d_engine_state.gpu_device, true, &fence, 1)) {
        RC2D_log(RC2D_LOG_ERROR, "Failed to wait for fence: %s", SDL_GetError());
    }

    SDL_ReleaseGPUFence(rc2d_engine_state.gpu_device, fence);
    RC2D_log(RC2D_LOG_INFO, "Letterbox textures initialized successfully");
}

void rc2d_letterbox_cleanup(void) {
    SDL_GPUDevice *device = rc2d_engine_state.gpu_device;

    if (rc2d_engine_state.letterbox_uniform_texture && rc2d_engine_state.letterbox_uniform_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_uniform_texture->gpu_texture);
        rc2d_engine_state.letterbox_uniform_texture->gpu_texture = NULL;
    }
    if (rc2d_engine_state.letterbox_top_texture && rc2d_engine_state.letterbox_top_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_top_texture->gpu_texture);
        rc2d_engine_state.letterbox_top_texture->gpu_texture = NULL;
    }
    if (rc2d_engine_state.letterbox_bottom_texture && rc2d_engine_state.letterbox_bottom_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_bottom_texture->gpu_texture);
        rc2d_engine_state.letterbox_bottom_texture->gpu_texture = NULL;
    }
    if (rc2d_engine_state.letterbox_left_texture && rc2d_engine_state.letterbox_left_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_left_texture->gpu_texture);
        rc2d_engine_state.letterbox_left_texture->gpu_texture = NULL;
    }
    if (rc2d_engine_state.letterbox_right_texture && rc2d_engine_state.letterbox_right_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_right_texture->gpu_texture);
        rc2d_engine_state.letterbox_right_texture->gpu_texture = NULL;
    }
    if (rc2d_engine_state.letterbox_background_texture && rc2d_engine_state.letterbox_background_texture->gpu_texture) {
        SDL_ReleaseGPUTexture(device, rc2d_engine_state.letterbox_background_texture->gpu_texture);
        rc2d_engine_state.letterbox_background_texture->gpu_texture = NULL;
    }

    rc2d_engine_state.letterbox_textures.mode = RC2D_LETTERBOX_NONE;
    rc2d_engine_state.letterbox_count = 0;
}

void rc2d_letterbox_draw(void) {
    if (rc2d_engine_state.skip_rendering || rc2d_engine_state.letterbox_count == 0 || rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_NONE) {
        RC2D_log(RC2D_LOG_DEBUG, "Skipping letterbox draw: skip_rendering=%d, letterbox_count=%d, mode=%d",
                 rc2d_engine_state.skip_rendering, rc2d_engine_state.letterbox_count, rc2d_engine_state.letterbox_textures.mode);
        return;
    }

    SDL_GPUCommandBuffer *cmd = rc2d_engine_state.gpu_current_command_buffer;
    if (!cmd) {
        RC2D_log(RC2D_LOG_ERROR, "No command buffer available for letterbox drawing");
        return;
    }

    RC2D_log(RC2D_LOG_DEBUG, "Drawing letterboxes: count=%d, mode=%d", rc2d_engine_state.letterbox_count, rc2d_engine_state.letterbox_textures.mode);

    if (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_BACKGROUND_FULL && rc2d_engine_state.letterbox_background_texture->gpu_texture) {
        int window_width, window_height;
        rc2d_window_getSize(&window_width, &window_height);
        RC2D_log(RC2D_LOG_DEBUG, "Rendering background texture: window=%dx%d", window_width, window_height);

        SDL_GPUBlitInfo blit_info = {
            .source = {
                .texture = rc2d_engine_state.letterbox_background_texture->gpu_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = 0,
                .y = 0,
                .w = rc2d_engine_state.letterbox_background_texture->width,
                .h = rc2d_engine_state.letterbox_background_texture->height
            },
            .destination = {
                .texture = rc2d_engine_state.gpu_current_swapchain_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = 0,
                .y = 0,
                .w = window_width,
                .h = window_height
            },
            .load_op = SDL_GPU_LOADOP_DONT_CARE,
            .flip_mode = SDL_FLIP_NONE,
            .filter = SDL_GPU_FILTER_LINEAR,
            .cycle = false
        };

        SDL_BlitGPUTexture(cmd, &blit_info);
        return;
    }

    const struct {
        int index;
        RC2D_GPUTexture *texture;
        const char *side;
    } letterbox_mappings[] = {
        {0, rc2d_engine_state.letterbox_left_texture, "left"},
        {1, rc2d_engine_state.letterbox_right_texture, "right"},
        {2, rc2d_engine_state.letterbox_top_texture, "top"},
        {3, rc2d_engine_state.letterbox_bottom_texture, "bottom"}
    };

    // Vérifier les chevauchements potentiels
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            RC2D_Rect *area1 = &rc2d_engine_state.letterbox_areas[i];
            RC2D_Rect *area2 = &rc2d_engine_state.letterbox_areas[j];
            if (area1->width > 0 && area1->height > 0 && area2->width > 0 && area2->height > 0) {
                if (area1->x < area2->x + area2->width &&
                    area1->x + area1->width > area2->x &&
                    area1->y < area2->y + area2->height &&
                    area1->y + area1->height > area2->y) {
                    RC2D_log(RC2D_LOG_WARN, "Overlap detected between letterbox %d (%s) and %d (%s)",
                             i, letterbox_mappings[i].side, j, letterbox_mappings[j].side);
                }
            }
        }
    }

    // Rendre d'abord les letterboxes horizontales (haut, bas)
    for (int i = 2; i <= 3; i++) {
        RC2D_Rect *area = &rc2d_engine_state.letterbox_areas[i];
        if (area->width <= 0 || area->height <= 0) {
            RC2D_log(RC2D_LOG_DEBUG, "Skipping letterbox %d (%s): invalid area (x=%f, y=%f, w=%f, h=%f)",
                     i, letterbox_mappings[i].side, area->x, area->y, area->width, area->height);
            continue;
        }

        RC2D_GPUTexture *texture = (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_UNIFORM)
            ? rc2d_engine_state.letterbox_uniform_texture
            : letterbox_mappings[i].texture;

        if (!texture || !texture->gpu_texture) {
            RC2D_log(RC2D_LOG_DEBUG, "Skipping letterbox %d (%s): invalid texture", i, letterbox_mappings[i].side);
            continue;
        }

        RC2D_log(RC2D_LOG_DEBUG, "Rendering letterbox %d (%s): area=(x=%f, y=%f, w=%f, h=%f), texture_size=(%dx%d)",
                 i, letterbox_mappings[i].side, area->x, area->y, area->width, area->height, texture->width, texture->height);

        SDL_GPUBlitInfo blit_info = {
            .source = {
                .texture = texture->gpu_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = 0,
                .y = 0,
                .w = texture->width,
                .h = texture->height
            },
            .destination = {
                .texture = rc2d_engine_state.gpu_current_swapchain_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = (int)area->x,
                .y = (int)area->y,
                .w = (int)area->width,
                .h = (int)area->height
            },
            .load_op = SDL_GPU_LOADOP_DONT_CARE,
            .flip_mode = SDL_FLIP_NONE,
            .filter = SDL_GPU_FILTER_LINEAR,
            .cycle = false
        };

        SDL_BlitGPUTexture(cmd, &blit_info);
    }

    // Ensuite, rendre les letterboxes verticales (gauche, droite)
    for (int i = 0; i <= 1; i++) {
        RC2D_Rect *area = &rc2d_engine_state.letterbox_areas[i];
        if (area->width <= 0 || area->height <= 0) {
            RC2D_log(RC2D_LOG_DEBUG, "Skipping letterbox %d (%s): invalid area (x=%f, y=%f, w=%f, h=%f)",
                     i, letterbox_mappings[i].side, area->x, area->y, area->width, area->height);
            continue;
        }

        RC2D_GPUTexture *texture = (rc2d_engine_state.letterbox_textures.mode == RC2D_LETTERBOX_UNIFORM)
            ? rc2d_engine_state.letterbox_uniform_texture
            : letterbox_mappings[i].texture;

        if (!texture || !texture->gpu_texture) {
            RC2D_log(RC2D_LOG_DEBUG, "Skipping letterbox %d (%s): invalid texture", i, letterbox_mappings[i].side);
            continue;
        }

        RC2D_log(RC2D_LOG_DEBUG, "Rendering letterbox %d (%s): area=(x=%f, y=%f, w=%f, h=%f), texture_size=(%dx%d)",
                 i, letterbox_mappings[i].side, area->x, area->y, area->width, area->height, texture->width, texture->height);

        SDL_GPUBlitInfo blit_info = {
            .source = {
                .texture = texture->gpu_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = 0,
                .y = 0,
                .w = texture->width,
                .h = texture->height
            },
            .destination = {
                .texture = rc2d_engine_state.gpu_current_swapchain_texture,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .x = (int)area->x,
                .y = (int)area->y,
                .w = (int)area->width,
                .h = (int)area->height
            },
            .load_op = SDL_GPU_LOADOP_DONT_CARE,
            .flip_mode = SDL_FLIP_NONE,
            .filter = SDL_GPU_FILTER_LINEAR,
            .cycle = false
        };

        SDL_BlitGPUTexture(cmd, &blit_info);
    }
}