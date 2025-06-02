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

    // Si la swapchain texture est NULL, définir skip_rendering et retourner
    if (rc2d_engine_state.gpu_current_swapchain_texture == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Swapchain texture is NULL (window may be minimized). Skipping frame rendering.");
        rc2d_engine_state.skip_rendering = true;
        return;
    }

    // Si nous sommes ici, le rendu peut continuer
    rc2d_engine_state.skip_rendering = false;

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

    if (!rc2d_engine_state.skip_rendering) 
    {
        rc2d_cimgui_newFrame();
    }
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
     * \brief Étape 2 : Rendu des letterboxes
     *
     * Les letterboxes sont rendues après la fin du render pass, car SDL_BlitGPUTexture
     * ne peut pas être appelé à l'intérieur d'un render pass.
     */
    if (rc2d_engine_state.gpu_current_command_buffer && !rc2d_engine_state.skip_rendering)    
    {
        //rc2d_letterbox_draw();
    }

    
    if (!rc2d_engine_state.skip_rendering) 
    {
        rc2d_cimgui_draw();
    }

    /**
     * \brief Étape 2 : Soumettre le command buffer
     *
     * C’est ici que le GPU exécute réellement toutes les commandes encodées pendant cette frame.
     * SDL_SubmitGPUCommandBuffer() envoie le tout pour traitement asynchrone.
     */
    if (rc2d_engine_state.gpu_current_command_buffer && !rc2d_engine_state.skip_rendering)
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