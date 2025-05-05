#include <RC2D/RC2D_graphics.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_math.h>

#include <SDL3/SDL_gpu.h>

void rc2d_graphics_clear(void)
{
    /**
     * \brief Étape 1 : Acquisition d’un GPUCommandBuffer
     *
     * Le GPUCommandBuffer est une structure temporaire qui enregistre toutes les commandes de rendu
     * (comme le début de render pass, les draw calls, les changements de pipeline, etc.) pour cette frame.
     * On ne peut soumettre les commandes au GPU qu’après avoir fini d’enregistrer dans ce buffer.
     * SDL nous fournit ce buffer via SDL_AcquireGPUCommandBuffer(), qui doit être appelé avant toute commande GPU.
     */
    rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_engine_state.gpu_device);
    RC2D_assert_release(rc2d_engine_state.gpu_current_command_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to acquire GPU command buffer");

    /**
     * \brief Étape 2 : Acquisition de la texture de swapchain
     *
     * La swapchain texture est une image représentative du contenu de la fenêtre.
     * C’est sur cette texture que le moteur va rendre le contenu de la frame actuelle.
     * SDL_WaitAndAcquireGPUSwapchainTexture() bloque jusqu’à ce qu’une texture soit prête à l’utilisation,
     * puis la lie au command buffer courant. La fonction remplit également les dimensions de la texture.
     *
     * Attention : la texture peut être NULL (par exemple si la fenêtre est minimisée).
     */
    Uint32 swapchainTextureWidth = 0;
	Uint32 swapchainTextureHeight = 0;
    SDL_WaitAndAcquireGPUSwapchainTexture(
        rc2d_engine_state.gpu_current_command_buffer,
        rc2d_engine_state.window,
        &rc2d_engine_state.gpu_current_swapchain_texture,
        &swapchainTextureWidth,
        &swapchainTextureHeight
    );
    RC2D_assert_release(rc2d_engine_state.gpu_current_swapchain_texture != NULL, RC2D_LOG_CRITICAL, "Swapchain texture is NULL (window may be minimized)");

    /**
     * \brief Étape 3 : Création du ColorTargetInfo pour le render pass
     *
     * Cette structure décrit **quelle texture** sera utilisée comme cible de rendu (ici la swapchain),
     * 
     * Mais aussi **comment** le GPU doit l’utiliser :
     *  - texture : La texture qui sera utilisée comme cible de couleur par une passe de rendu.
     *  - mip_level : Le niveau mip à utiliser comme cible de couleur. 
     *  - layer_or_depth_plane : L'indice de couche ou le plan de profondeur à utiliser comme cible de couleur. 
     *                           Cette valeur est traitée comme un indice de couche sur les tableaux 2D et les textures cubiques, 
     *                           et comme un plan de profondeur sur les textures 3D.
     *  - clear_color : La couleur à laquelle la cible de couleur doit être réinitialisée au début de la passe de rendu. 
     *                  Ignoré si SDL_GPU_LOADOP_CLEAR n'est pas utilisé.
     *  - load_op : Ce qui est fait avec le contenu de la cible de couleur au début de la passe de rendu.
     *  - store_op : Ce qui est fait avec le contenu de la cible de couleur à la fin de la passe de rendu.
     *  - resolve_texture : La texture qui recevra les résultats d'une opération de résolution multi-échantillon. 
     *                      Ignorée si un RESOLVE* store_op n'est pas utilisé.
     * - resolve_mip_level : Le niveau mip de la texture de résolution à utiliser pour l'opération de résolution. 
     *                       Ignoré si un store_op RESOLVE* n'est pas utilisé.
     * - resolve_layer : L'indice de couche de la texture à utiliser pour l'opération de résolution. 
     *                   Ignoré si un RESOLVE* store_op n'est pas utilisé.
     */
    SDL_GPUColorTargetInfo colorTargetInfo = {0};
    colorTargetInfo.texture = rc2d_engine_state.gpu_current_swapchain_texture;
    colorTargetInfo.mip_level = 0;
    colorTargetInfo.layer_or_depth_plane = 0;
    colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.resolve_texture = NULL;
    colorTargetInfo.resolve_mip_level = 0;
    colorTargetInfo.resolve_layer = 0;
    colorTargetInfo.cycle = true;
    colorTargetInfo.cycle_resolve_texture = false;
    colorTargetInfo.padding1 = 0;
    colorTargetInfo.padding2 = 0;

    /**
     * \brief Étape 4 : Début d’un Render Pass
     *
     * Le render pass est une section logique dans le GPUCommandBuffer où toutes les opérations de rendu sont encodées.
     * On passe ici le color_target pour spécifier que l’on souhaite dessiner sur la texture de swapchain,
     * avec les réglages définis ci-dessus. Aucune texture de depth/stencil n’est utilisée ici.
     */
    rc2d_engine_state.gpu_current_render_pass = SDL_BeginGPURenderPass(
        rc2d_engine_state.gpu_current_command_buffer,
        &colorTargetInfo, 
        1, 
        NULL
    );
    RC2D_assert_release(rc2d_engine_state.gpu_current_render_pass != NULL, RC2D_LOG_CRITICAL, "Failed to begin GPU render pass");

    /**
     * \brief Étape 5 : Définir le viewport
     *
     * Le viewport est une zone rectangulaire de la cible de rendu où le dessin est autorisé.
     * SDL ne définit pas de viewport par défaut dans le render pass, donc tu dois toujours
     * l’assigner manuellement. Il est impératif que `gpu_current_viewport` ait été
     * correctement calculé avant l’appel à cette fonction.
     */
    RC2D_assert_release(rc2d_engine_state.gpu_current_viewport != NULL, RC2D_LOG_CRITICAL, "No viewport set in rc2d_engine_state");
    SDL_SetGPUViewport(rc2d_engine_state.gpu_current_render_pass, rc2d_engine_state.gpu_current_viewport);
}

void rc2d_graphics_present(void)
{
    /**
     * \brief Étape 1 : Terminer le render pass
     *
     * Tous les draw calls encodés doivent être finalisés avant de pouvoir soumettre le command buffer.
     * SDL_EndGPURenderPass() clôt le bloc d’enregistrement des commandes de rendu.
     */
    if (rc2d_engine_state.gpu_current_render_pass)
    {
        SDL_EndGPURenderPass(rc2d_engine_state.gpu_current_render_pass);
        rc2d_engine_state.gpu_current_render_pass = NULL;
    }

    /**
     * \brief Étape 2 : Soumettre le command buffer
     *
     * C’est ici que le GPU exécute réellement toutes les commandes encodées pendant cette frame.
     * SDL_SubmitGPUCommandBuffer() envoie le tout pour traitement asynchrone.
     */
    if (rc2d_engine_state.gpu_current_command_buffer)
    {
        SDL_SubmitGPUCommandBuffer(rc2d_engine_state.gpu_current_command_buffer);
        rc2d_engine_state.gpu_current_command_buffer = NULL;
    }

    /**
     * \brief Étape 3 : Nettoyage de la swapchain texture
     *
     * La texture est gérée par SDL, mais on libère notre pointeur par sécurité.
     * Cela évite toute utilisation incorrecte au prochain frame.
     */
    rc2d_engine_state.gpu_current_swapchain_texture = NULL;
}

/* Draw a rectangle (filled or outlined) */
void rc2d_graphics_rectangle(const char* mode, RC2D_Rect* rect, RC2D_Color color)
{
    RC2D_assert_release(rc2d_engine_state.gpu_current_render_pass != NULL, RC2D_LOG_CRITICAL, "No active render pass");
    RC2D_assert_release(rect != NULL, RC2D_LOG_CRITICAL, "Rectangle is NULL");
    RC2D_assert_release(mode != NULL, RC2D_LOG_CRITICAL, "Mode is NULL");

    /* Define vertices for a rectangle */
    RC2D_Vertex vertices[] = {
        { rect->x, rect->y },                              /* Top-left */
        { rect->x + rect->width, rect->y },                /* Top-right */
        { rect->x + rect->width, rect->y + rect->height }, /* Bottom-right */
        { rect->x, rect->y + rect->height }                /* Bottom-left */
    };

    /* Create vertex buffer */
    SDL_GPUBufferCreateInfo vertex_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(vertices)
    };
    SDL_GPUBuffer* vertex_buffer = SDL_CreateGPUBuffer(rc2d_engine_state.gpu_device, &vertex_buffer_info);
    RC2D_assert_release(vertex_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create vertex buffer");

    /* Upload vertex data */
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(vertices)
    };
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(rc2d_engine_state.gpu_device, &transfer_buffer_info);
    RC2D_assert_release(transfer_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create transfer buffer");

    void* transfer_data = SDL_MapGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer, true);
    RC2D_assert_release(transfer_data != NULL, RC2D_LOG_CRITICAL, "Failed to map transfer buffer");
    memcpy(transfer_data, vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer);

    SDL_GPUBufferRegion vertex_buffer_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(vertices)
    };
    SDL_UploadToGPUBuffer(rc2d_engine_state.gpu_current_command_buffer, &(SDL_GPUTransferBufferLocation){
        .transfer_buffer = transfer_buffer,
        .offset = 0
    }, &vertex_buffer_region, true);

    /* Normalize color (0-255 to 0.0-1.0) and push as uniform data */
    float normalized_color[4] = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };
    SDL_PushGPUFragmentUniformData(rc2d_engine_state.gpu_current_render_pass, 0, normalized_color, sizeof(normalized_color));

    /* Bind graphics pipeline */
    SDL_BindGPUGraphicsPipeline(rc2d_engine_state.gpu_current_render_pass, rc2d_engine_state.gpu_pipeline);

    /* Bind vertex buffer */
    SDL_GPUBufferBinding vertex_binding = {
        .buffer = vertex_buffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(rc2d_engine_state.gpu_current_render_pass, 0, &vertex_binding, 1);

    /* Draw based on mode */
    if (strcmp(mode, "fill") == 0) 
	{
        /* Filled rectangle: draw two triangles */
        Uint16 indices[] = { 0, 1, 2, 0, 2, 3 };
        SDL_GPUBufferCreateInfo index_buffer_info = {
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = sizeof(indices)
        };
        SDL_GPUBuffer* index_buffer = SDL_CreateGPUBuffer(rc2d_engine_state.gpu_device, &index_buffer_info);
        RC2D_assert_release(index_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create index buffer");

        /* Upload index data */
        transfer_buffer_info.size = sizeof(indices);
        SDL_GPUTransferBuffer* index_transfer_buffer = SDL_CreateGPUTransferBuffer(rc2d_engine_state.gpu_device, &transfer_buffer_info);
        RC2D_assert_release(index_transfer_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create index transfer buffer");

        transfer_data = SDL_MapGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer, true);
        RC2D_assert_release(transfer_data != NULL, RC2D_LOG_CRITICAL, "Failed to map index transfer buffer");
        memcpy(transfer_data, indices, sizeof(indices));
        SDL_UnmapGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer);

        SDL_GPUBufferRegion index_buffer_region = {
            .buffer = index_buffer,
            .offset = 0,
            .size = sizeof(indices)
        };
        SDL_UploadToGPUBuffer(rc2d_engine_state.gpu_current_command_buffer, &(SDL_GPUTransferBufferLocation){
            .transfer_buffer = index_transfer_buffer,
            .offset = 0
        }, &index_buffer_region, true);

        /* Bind index buffer */
        SDL_BindGPUIndexBuffer(rc2d_engine_state.gpu_current_render_pass, &(SDL_GPUBufferBinding){
            .buffer = index_buffer,
            .offset = 0
        }, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        /* Draw */
        SDL_DrawGPUIndexedPrimitives(rc2d_engine_state.gpu_current_render_pass, 6, 1, 0, 0, 0);

        /* Clean up index buffer */
        SDL_ReleaseGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer);
        SDL_ReleaseGPUBuffer(rc2d_engine_state.gpu_device, index_buffer);
    } 
	else if (strcmp(mode, "line") == 0) 
	{
        /* Outlined rectangle: draw as line strip (loop back to first vertex) */
        Uint16 indices[] = { 0, 1, 2, 3, 0 };
        SDL_GPUBufferCreateInfo index_buffer_info = {
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = sizeof(indices)
        };
        SDL_GPUBuffer* index_buffer = SDL_CreateGPUBuffer(rc2d_engine_state.gpu_device, &index_buffer_info);
        RC2D_assert_release(index_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create index buffer");

        /* Upload index data */
        transfer_buffer_info.size = sizeof(indices);
        SDL_GPUTransferBuffer* index_transfer_buffer = SDL_CreateGPUTransferBuffer(rc2d_engine_state.gpu_device, &transfer_buffer_info);
        RC2D_assert_release(index_transfer_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create index transfer buffer");

        transfer_data = SDL_MapGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer, true);
        RC2D_assert_release(transfer_data != NULL, RC2D_LOG_CRITICAL, "Failed to map index transfer buffer");
        memcpy(transfer_data, indices, sizeof(indices));
        SDL_UnmapGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer);

        SDL_GPUBufferRegion index_buffer_region = {
            .buffer = index_buffer,
            .offset = 0,
            .size = sizeof(indices)
        };
        SDL_UploadToGPUBuffer(rc2d_engine_state.gpu_current_command_buffer, &(SDL_GPUTransferBufferLocation){
            .transfer_buffer = index_transfer_buffer,
            .offset = 0
        }, &index_buffer_region, true);

        /* Bind index buffer */
        SDL_BindGPUIndexBuffer(rc2d_engine_state.gpu_current_render_pass, &(SDL_GPUBufferBinding){
            .buffer = index_buffer,
            .offset = 0
        }, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        /* Draw */
        SDL_DrawGPUIndexedPrimitives(rc2d_engine_state.gpu_current_render_pass, 5, 1, 0, 0, 0);

        /* Clean up index buffer */
        SDL_ReleaseGPUTransferBuffer(rc2d_engine_state.gpu_device, index_transfer_buffer);
        SDL_ReleaseGPUBuffer(rc2d_engine_state.gpu_device, index_buffer);
    } 
    else 
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Invalid mode: must be 'fill' or 'line'");
    }

    /* Clean up */
    SDL_ReleaseGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer);
    SDL_ReleaseGPUBuffer(rc2d_engine_state.gpu_device, vertex_buffer);
}

/* Draw a point */
void rc2d_graphics_point(RC2D_Vertex point, RC2D_Color color)
{
    RC2D_assert_release(rc2d_engine_state.gpu_current_render_pass != NULL, RC2D_LOG_CRITICAL, "No active render pass");

    /* Define vertex for a point */
    RC2D_Vertex vertex = { point.x, point.y };

    /* Create vertex buffer */
    SDL_GPUBufferCreateInfo vertex_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(vertex)
    };
    SDL_GPUBuffer* vertex_buffer = SDL_CreateGPUBuffer(rc2d_engine_state.gpu_device, &vertex_buffer_info);
    RC2D_assert_release(vertex_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create vertex buffer");

    /* Upload vertex data */
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(vertex)
    };
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(rc2d_engine_state.gpu_device, &transfer_buffer_info);
    RC2D_assert_release(transfer_buffer != NULL, RC2D_LOG_CRITICAL, "Failed to create transfer buffer");

    void* transfer_data = SDL_MapGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer, true);
    RC2D_assert_release(transfer_data != NULL, RC2D_LOG_CRITICAL, "Failed to map transfer buffer");
    memcpy(transfer_data, &vertex, sizeof(vertex));
    SDL_UnmapGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer);

    SDL_GPUBufferRegion vertex_buffer_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(vertex)
    };
    SDL_UploadToGPUBuffer(rc2d_engine_state.gpu_current_command_buffer, &(SDL_GPUTransferBufferLocation){
        .transfer_buffer = transfer_buffer,
        .offset = 0
    }, &vertex_buffer_region, true);

    /* Normalize color (0-255 to 0.0-1.0) and push as uniform data */
    float normalized_color[4] = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };
    SDL_PushGPUFragmentUniformData(rc2d_engine_state.gpu_current_render_pass, 0, normalized_color, sizeof(normalized_color));

    /* Bind graphics pipeline */
    SDL_BindGPUGraphicsPipeline(rc2d_engine_state.gpu_current_render_pass, rc2d_engine_state.gpu_pipeline);

    /* Bind vertex buffer */
    SDL_GPUBufferBinding vertex_binding = {
        .buffer = vertex_buffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(rc2d_engine_state.gpu_current_render_pass, 0, &vertex_binding, 1);

    /* Draw the point */
    SDL_DrawGPUPrimitives(rc2d_engine_state.gpu_current_render_pass, 1, 1, 0, 0);

    /* Clean up */
    SDL_ReleaseGPUTransferBuffer(rc2d_engine_state.gpu_device, transfer_buffer);
    SDL_ReleaseGPUBuffer(rc2d_engine_state.gpu_device, vertex_buffer);
}