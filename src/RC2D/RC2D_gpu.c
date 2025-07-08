#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_platform_defines.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_filesystem.h>
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
#include <SDL3_shadercross/SDL_shadercross.h>
#endif

/**
 * Récupère le timestamp de la dernière modification d'un fichier.
 * 
 * @param {const char*} path - Le chemin du fichier dont on veut connaître la date de dernière modification.
 * @returns {SDL_Time} - Le timestamp de la dernière modification du fichier, ou 0 en cas d'erreur.
 */
static SDL_Time rc2d_gpu_getFileModificationTime(const char* path) 
{
    SDL_PathInfo info;
    if (SDL_GetPathInfo(path, &info)) 
    {
        // Retourne le timestamp de la dernière modification du fichier
        return info.modify_time;
    }
    else
    {
        // Si l'appel échoue, on log l'erreur et on retourne 0
        RC2D_log(RC2D_LOG_ERROR, "Failed to get path info for %s, SDL_Error : %s", path, SDL_GetError());
        return 0;
    }
}

RC2D_GPUShader* rc2d_gpu_loadGraphicsShader(const char* filename) 
{
    /**
     * Vérification des paramètres d'entrée
     */
    RC2D_assert_release(filename != NULL, RC2D_LOG_CRITICAL, "Graphics Shader filename is NULL");

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    /**
     * Locker le mutex pour éviter les accès concurrents au cache des shaders graphiques
     */
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);

    /**
     * Vérifier si le shader graphique est déjà dans le cache (donc déjà chargé une fois)
     */
    for (int i = 0; i < rc2d_engine_state.gpu_graphics_shader_count; i++) 
    {
        if (SDL_strcmp(rc2d_engine_state.gpu_graphics_shaders_cache[i].filename, filename) == 0) 
        {
            RC2D_GPUShader* graphicsShader = rc2d_engine_state.gpu_graphics_shaders_cache[i].shader;
            SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);

            RC2D_log(RC2D_LOG_INFO, "Graphics Shader already loaded from cache: %s", filename);
            return graphicsShader;
        }
    }

    /**
     * On unlock le mutex après avoir vérifié le cache des shaders graphiques.
     * Cela permet aux autres threads d'accéder au cache des shaders graphiques.
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
#endif

    // Récupérer le chemin de base de l'application (où est exécuté l'exécutable)
    const char* basePath = SDL_GetBasePath();
    RC2D_assert_release(basePath != NULL, RC2D_LOG_CRITICAL, "SDL_GetBasePath() failed, SDL_Error: %s", SDL_GetError());

    /**
     * Déterminer le stage en fonction du suffixe (vertex ou fragment)
     * On utilise SDL_strstr pour vérifier la présence de ".vertex" ou ".fragment" dans le nom du fichier
     */
    SDL_GPUShaderStage stage;
    if (SDL_strstr(filename, ".vertex")) 
    {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } 
    else if (SDL_strstr(filename, ".fragment")) 
    {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } 
    else 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Unknown shader stage suffix: expected .vertex or .fragment.");
        return NULL;
    }

    /**
     * fullPath : Chemin d'accès complet au fichier binaire du shader compilé ou au fichier HLSL source.
     */
    char fullPath[512];

#if !RC2D_GPU_SHADER_HOT_RELOAD_ENABLED // Compilation hors ligne des shaders graphics en HLSL
    /**
     * entrypoint : Point d'entrée du shader (main pour SPIR-V, DXIL et main0 pour MSL).
     */
    const char* entrypoint = NULL;

    // Récupérer les formats supportés par le backend actuel
    SDL_GPUShaderFormat backendFormatsSupported = SDL_GetGPUSupportedShaderFormats(rc2d_gpu_getDevice());

    // Le format de shader à utiliser pour la compilation hors ligne
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

    // Générer le chemin d'accès au fichier binaire du shader compilé en fonction du backend
    if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_SPIRV) 
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/spirv/%s.spv", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    }
    /**
     * Priorisé dans l'ordre des conditions le format : METALLIB avant MSL pour les appareils Apple.
     * METALLIB est le format précompilé pour Metal, tandis que MSL pour Metal est compilé à l'exécution.
     */
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_METALLIB)
    {
#ifdef RC2D_PLATFORM_IOS
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/metallib/ios/%s.metallib", basePath, filename);
#else
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/metallib/macos/%s.metallib", basePath, filename);
#endif // RC2D_PLATFORM_IOS
        format = SDL_GPU_SHADERFORMAT_METALLIB;
        entrypoint = "main";
    }
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_MSL)
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/msl/%s.msl", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0"; // SDL_shadercross requiert "main0" pour MSL
    } 
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_DXIL) 
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/dxil/%s.dxil", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } 
    else 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "No compatible shader format for this backend");
        return NULL;
    }

    /**
     * Charger le fichier du shader compilé
     * On utilise SDL_LoadFile pour charger le fichier binaire du shader compilé
     */
    size_t codeShaderCompiledSize = 0;
    void* codeShaderCompiled = SDL_LoadFile(fullPath, &codeShaderCompiledSize);
    if (codeShaderCompiled == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load compiled shader: %s, SDL_Error: %s", fullPath, SDL_GetError());
        return NULL;
    }

    /**
     * En mode compilation hors ligne des shaders, on utilise un fichier JSON généré par le script de compilation des shaders.
     * On génère le chemin d'accès au fichier JSON de réflexion en fonction du nom du shader et de son stage.
     */
    char jsonPath[512];
    SDL_snprintf(jsonPath, sizeof(jsonPath), "%sshaders/reflection/%s.json", basePath, filename);

    /**
     * On ouvre le fichier JSON de réflexion pour récupérer les informations de réflexions sur le shader.
     * On utilise SDL_LoadFile pour charger le fichier JSON de réflexion.
     * On utilise SDL_sscanf pour extraire les valeurs des champs : samplers, uniform_buffers, storage_buffers et storage_textures.
     * On ferme le fichier JSON après la lecture.
     * 
     * Exemple de contenu JSON attendu :
     * ```json
     * {
     *   "samplers": 0,
     *   "uniform_buffers": 0,
     *   "storage_buffers": 0,
     *   "storage_textures": 0
     * }
     * ```
     */
    Uint32 numSamplers = 0;
    Uint32 numUniformBuffers = 0;
    Uint32 numStorageBuffers = 0;
    Uint32 numStorageTextures = 0;
    void* jsonContent = SDL_LoadFile(jsonPath, NULL);
    if (jsonContent) 
    {
        // Cast le contenu du fichier JSON en chaîne de caractères
        const char* content = (const char*)jsonContent;
    
        // Essayer les différentes clés connues, y compris alternatives
        if (SDL_strstr(content, "\"samplers\""))
            SDL_sscanf(content, "%*[^\"samplers\"]\"samplers\"%*[: ]%u", &numSamplers);
        if (SDL_strstr(content, "\"uniform_buffers\""))
            SDL_sscanf(content, "%*[^\"uniform_buffers\"]\"uniform_buffers\"%*[: ]%u", &numUniformBuffers);
        else if (SDL_strstr(content, "\"uniformBuffers\""))
            SDL_sscanf(content, "%*[^\"uniformBuffers\"]\"uniformBuffers\"%*[: ]%u", &numUniformBuffers);
        if (SDL_strstr(content, "\"storage_buffers\""))
            SDL_sscanf(content, "%*[^\"storage_buffers\"]\"storage_buffers\"%*[: ]%u", &numStorageBuffers);
        else if (SDL_strstr(content, "\"readOnlyStorageBuffers\""))
            SDL_sscanf(content, "%*[^\"readOnlyStorageBuffers\"]\"readOnlyStorageBuffers\"%*[: ]%u", &numStorageBuffers);
        if (SDL_strstr(content, "\"storage_textures\""))
            SDL_sscanf(content, "%*[^\"storage_textures\"]\"storage_textures\"%*[: ]%u", &numStorageTextures);
        else if (SDL_strstr(content, "\"readOnlyStorageTextures\""))
            SDL_sscanf(content, "%*[^\"readOnlyStorageTextures\"]\"readOnlyStorageTextures\"%*[: ]%u", &numStorageTextures);
    
        // Libérer le contenu JSON après la lecture
        if (jsonContent) 
        {
            RC2D_free(jsonContent);
            jsonContent = NULL;
        }
    }
    else 
    {
        RC2D_log(RC2D_LOG_ERROR, "Shader reflection file not found: %s, SDL_Error: %s", jsonPath, SDL_GetError());
    }
    
    // Création du shader GPU avec les informations de réflexion récupérées depuis le fichier JSON
    SDL_GPUShaderCreateInfo info = {
        .code = codeShaderCompiled,
        .code_size = codeShaderCompiledSize,
        .entrypoint = entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = numSamplers,
        .num_uniform_buffers = numUniformBuffers,
        .num_storage_buffers = numStorageBuffers,
        .num_storage_textures = numStorageTextures,
        .props = 0
    };

    // Créer le shader graphique à partir du code compilé du shader
    SDL_GPUShader* graphicsShader = SDL_CreateGPUShader(rc2d_gpu_getDevice(), &info);
    if (codeShaderCompiled != NULL)
    {
        // Libérer le code du shader compilé après la création du shader
        RC2D_free(codeShaderCompiled);
        codeShaderCompiled = NULL;
    }
    if (graphicsShader == NULL) 
    {
        // Si la création du shader graphique échoue, on log l'erreur et on retourne NULL
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU graphics shader from file %s, SDL_Error: %s", filename, SDL_GetError());
        return NULL;
    }

#else // Compilation en ligne des shaders graphics en HLSL
    /**
     * On génère le chemin d'accès au fichier HLSL source en fonction du nom du shader et de son stage pour la compilation en ligne des shaders.
     * On utilise SDL_snprintf pour formater le chemin d'accès au fichier HLSL source.
     */
    SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, filename);

    /**
     * Charger le fichier HLSL source
     * On utilise SDL_LoadFile pour charger le fichier HLSL source.
     */
    char* codeHLSLSource = SDL_LoadFile(fullPath, NULL);
    if (codeHLSLSource == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source: %s, SDL_Error: %s", fullPath, SDL_GetError()); 
        return NULL;
    }

    // Préparer les informations pour la compilation HLSL vers SPIR-V
    SDL_ShaderCross_HLSL_Info hlslInfo = {
        .source = codeHLSLSource,
        .entrypoint = "main",
        .include_dir = NULL,
        .defines = NULL,
        .shader_stage = (SDL_ShaderCross_ShaderStage)stage,
        .enable_debug = true,
        .name = filename,
        .props = 0
    };

    // Compiler HLSL vers SPIR-V
    size_t spirvByteCodeSize = 0;
    void* spirvByteCode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &spirvByteCodeSize);
    if (codeHLSLSource != NULL)
    {
        // Libérer le code HLSL source après la compilation
        RC2D_free(codeHLSLSource);
        codeHLSLSource = NULL;
    }
    if (spirvByteCode == NULL || spirvByteCodeSize == 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to compile HLSL to SPIR-V: %s", filename);
        return NULL;
    }

    // Réfléchir les métadonnées du shader graphique
    SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
        spirvByteCode, spirvByteCodeSize, 0
    );
    if (metadata == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to reflect graphics shader metadata: %s", filename);
        if (spirvByteCode) 
        {
            RC2D_free(spirvByteCode);
            spirvByteCode = NULL;
        }
        return NULL;
    }

    // Préparer les informations SPIR-V pour la compilation du shader
    SDL_ShaderCross_SPIRV_Info spirvInfo = {
        .bytecode = spirvByteCode,
        .bytecode_size = spirvByteCodeSize,
        .entrypoint = "main",
        .shader_stage = (SDL_ShaderCross_ShaderStage)stage,
        .enable_debug = true,
        .name = filename,
        .props = 0
    };

    // Compiler le shader graphique
    SDL_GPUShader* graphicsShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
        rc2d_gpu_getDevice(),
        &spirvInfo,
        metadata,
        0
    );

    // Libérer les ressources allouées pour les métadonnées et le code SPIR-V
    if (metadata != NULL) 
    {
        RC2D_free(metadata);
        metadata = NULL;
    }
    if (spirvByteCode != NULL) 
    {
        RC2D_free(spirvByteCode);
        spirvByteCode = NULL;
    }

    // Vérifier si la compilation du shader graphique a réussi
    if (graphicsShader == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU graphics shader from SPIR-V: %s", filename);
        return NULL;
    }

    /**
     * On lock le temps d'ajouter le shader graphique au cache des shaders graphiques,
     * et pour éviter les accès concurrents.
     * 
     * On utilise un mutex pour protéger l'accès au cache des shaders graphiques.
     */
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);

    // On réalloue le cache des shaders graphiques pour ajouter le nouveau shader graphique (on dois augmenter la taille du cache)
    RC2D_GraphicsShaderEntry* newShaders = RC2D_realloc(
        rc2d_engine_state.gpu_graphics_shaders_cache,
        (rc2d_engine_state.gpu_graphics_shader_count + 1) * sizeof(RC2D_GraphicsShaderEntry)
    );

    // Vérifier si la réallocation a réussi
    RC2D_assert_release(newShaders != NULL, RC2D_LOG_CRITICAL, "Failed to realloc shader cache");

    // Mettre à jour le cache des shaders graphiques avec le nouveau shader graphique
    rc2d_engine_state.gpu_graphics_shaders_cache = newShaders;
    RC2D_GraphicsShaderEntry* entry = &rc2d_engine_state.gpu_graphics_shaders_cache[rc2d_engine_state.gpu_graphics_shader_count++];
    entry->filename = RC2D_strdup(filename);
    entry->shader = graphicsShader;
    entry->lastModified = rc2d_gpu_getFileModificationTime(fullPath);

    /**
     * On unlock le mutex après avoir ajouté le shader graphique au cache.
     * Cela permet aux autres threads d'accéder au cache des shaders graphiques.
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
#endif

    RC2D_log(RC2D_LOG_INFO, "Graphics Shader loaded and cached: %s", filename);
    return graphicsShader;
}

RC2D_GPUComputePipeline* rc2d_gpu_loadComputeShader(const char* filename) 
{
    /**
     * Vérification des paramètres d'entrée
     */
    RC2D_assert_release(filename != NULL, RC2D_LOG_CRITICAL, "Compute Shader filename is NULL");

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    /**
     * Locker le mutex pour éviter les accès concurrents au cache des shaders de calcul
     */
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

    // Vérifier si le shader compute est déjà dans le cache (donc déjà chargé une fois)
    for (int i = 0; i < rc2d_engine_state.gpu_compute_shader_count; i++) 
    {
        if (SDL_strcmp(rc2d_engine_state.gpu_compute_shaders_cache[i].filename, filename) == 0) 
        {
            RC2D_GPUComputePipeline* computeShader = rc2d_engine_state.gpu_compute_shaders_cache[i].shader;
            SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

            RC2D_log(RC2D_LOG_INFO, "Compute Shader already loaded from cache: %s", filename);
            return computeShader;
        }
    }

    /**
     * On unlock le mutex après avoir vérifié le cache des shaders de calcul.
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
#endif

    // Récupérer le chemin de base de l'application (où est exécuté l'exécutable)
    const char* basePath = SDL_GetBasePath();
    RC2D_assert_release(basePath != NULL, RC2D_LOG_CRITICAL, "SDL_GetBasePath() failed");

    /**
     * fullPath : Chemin d'accès complet au fichier binaire du shader compilé ou au fichier HLSL source.
     */
    char fullPath[512];

#if !RC2D_GPU_SHADER_HOT_RELOAD_ENABLED // Compilation hors ligne des shaders compute en HLSL
    /**
     * entrypoint : Point d'entrée du shader (main pour SPIR-V, DXIL et main0 pour MSL).
     */
    const char* entrypoint = NULL;

    // Récupérer les formats supportés par le backend actuel
    SDL_GPUShaderFormat backendFormatsSupported = SDL_GPU_GetSupportedShaderFormats(rc2d_gpu_getDevice());

    // Le format de shader à utiliser pour la compilation hors ligne
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

    // Générer le chemin d'accès au fichier binaire du shader compilé en fonction du backend
    if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_SPIRV) 
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/spirv/%s.spv", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    }
    /**
     * Priorisé dans l'ordre des conditions le format : METALLIB avant MSL pour les appareils Apple.
     * METALLIB est le format précompilé pour Metal, tandis que MSL pour Metal est compilé à l'exécution.
     */
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_METALLIB)
    {
#ifdef RC2D_PLATFORM_IOS
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/metallib/ios/%s.metallib", basePath, filename);
#else
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/metallib/macos/%s.metallib", basePath, filename);
#endif // RC2D_PLATFORM_IOS
        format = SDL_GPU_SHADERFORMAT_METALLIB;
        entrypoint = "main";
    }
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_MSL)
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/msl/%s.msl", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0"; // SDL_shadercross requiert "main0" pour MSL
    } 
    else if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_DXIL) 
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/dxil/%s.dxil", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } 
    else 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "No compatible shader format for this backend");
        return NULL;
    }

    /**
     * Charger le fichier du shader compilé
     * On utilise SDL_LoadFile pour charger le fichier binaire du shader compilé
     */
    size_t codeShaderCompiledSize = 0;
    void* codeShaderCompiled = SDL_LoadFile(fullPath, &codeShaderCompiledSize);
    if (codeShaderCompiled == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load compiled shader: %s, SDL_Error: %s", fullPath, SDL_GetError());
        return NULL;
    }

    /**
     * En mode compilation hors ligne des shaders, on utilise un fichier JSON généré par le script de compilation des shaders.
     * On génère le chemin d'accès au fichier JSON de réflexion en fonction du nom du shader et de son stage.
     */
    char jsonPath[512];
    SDL_snprintf(jsonPath, sizeof(jsonPath), "%sshaders/reflection/%s.json", basePath, filename);

    /**
     * On ouvre le fichier JSON de réflexion pour récupérer les informations de réflexions sur le shader.
     * On utilise SDL_LoadFile pour charger le fichier JSON de réflexion.
     * On utilise SDL_sscanf pour extraire les valeurs des champs : "samplers", "readonly_storage_textures", "readonly_storage_buffers", "readwrite_storage_textures", "readwrite_storage_buffers", "uniform_buffers", "threadcount_x", "threadcount_y" et "threadcount_z".
     * On ferme le fichier JSON après la lecture.
     * 
     * Exemple de contenu JSON attendu :
     * ```json
     * {
     *   "samplers": 0,
     *   "readonly_storage_textures": 0,
     *   "readonly_storage_buffers": 1,
     *   "readwrite_storage_textures": 0,
     *   "readwrite_storage_buffers": 1,
     *   "uniform_buffers": 0,
     *   "threadcount_x": 64,
     *   "threadcount_y": 1,
     *   "threadcount_z": 1
     * }
     * ```
     */
    Uint32 numSamplers = 0;
    Uint32 numReadonlyStorageTextures = 0;
    Uint32 numReadonlyStorageBuffers = 0;
    Uint32 numReadwriteStorageTextures = 0;
    Uint32 numReadwriteStorageBuffers = 0;
    Uint32 numUniformBuffers = 0;
    Uint32 numThreadCountX = 0;
    Uint32 numThreadCountY = 0;
    Uint32 numThreadCountZ = 0;
    void* jsonContent = SDL_LoadFile(jsonPath, NULL);
    if (jsonContent) 
    {
        // Cast le contenu du fichier JSON en chaîne de caractères
        const char* content = (const char*)jsonContent;
    
        // Essayer les différentes clés connues, y compris alternatives
        if (SDL_strstr(content, "\"samplers\""))
            SDL_sscanf(content, "%*[^\"samplers\"]\"samplers\"%*[: ]%u", &numSamplers);
        if (SDL_strstr(content, "\"readonly_storage_textures\""))
            SDL_sscanf(content, "%*[^\"readonly_storage_textures\"]\"readonly_storage_textures\"%*[: ]%u", &numReadonlyStorageTextures);
        else if (SDL_strstr(content, "\"readOnlyStorageTextures\""))
            SDL_sscanf(content, "%*[^\"readOnlyStorageTextures\"]\"readOnlyStorageTextures\"%*[: ]%u", &numReadonlyStorageTextures);
        if (SDL_strstr(content, "\"readonly_storage_buffers\""))
            SDL_sscanf(content, "%*[^\"readonly_storage_buffers\"]\"readonly_storage_buffers\"%*[: ]%u", &numReadonlyStorageBuffers);
        else if (SDL_strstr(content, "\"readOnlyStorageBuffers\""))
            SDL_sscanf(content, "%*[^\"readOnlyStorageBuffers\"]\"readOnlyStorageBuffers\"%*[: ]%u", &numReadonlyStorageBuffers);
        if (SDL_strstr(content, "\"readwrite_storage_textures\""))
            SDL_sscanf(content, "%*[^\"readwrite_storage_textures\"]\"readwrite_storage_textures\"%*[: ]%u", &numReadwriteStorageTextures);
        else if (SDL_strstr(content, "\"readWriteStorageTextures\""))
            SDL_sscanf(content, "%*[^\"readWriteStorageTextures\"]\"readWriteStorageTextures\"%*[: ]%u", &numReadwriteStorageTextures);
        if (SDL_strstr(content, "\"readwrite_storage_buffers\""))
            SDL_sscanf(content, "%*[^\"readwrite_storage_buffers\"]\"readwrite_storage_buffers\"%*[: ]%u", &numReadwriteStorageBuffers);
        else if (SDL_strstr(content, "\"readWriteStorageBuffers\""))
            SDL_sscanf(content, "%*[^\"readWriteStorageBuffers\"]\"readWriteStorageBuffers\"%*[: ]%u", &numReadwriteStorageBuffers);
        if (SDL_strstr(content, "\"uniform_buffers\""))
            SDL_sscanf(content, "%*[^\"uniform_buffers\"]\"uniform_buffers\"%*[: ]%u", &numUniformBuffers);
        else if (SDL_strstr(content, "\"uniformBuffers\""))
            SDL_sscanf(content, "%*[^\"uniformBuffers\"]\"uniformBuffers\"%*[: ]%u", &numUniformBuffers);
        if (SDL_strstr(content, "\"threadcount_x\""))
            SDL_sscanf(content, "%*[^\"threadcount_x\"]\"threadcount_x\"%*[: ]%u", &numThreadCountX);
        else if (SDL_strstr(content, "\"threadCountX\""))
            SDL_sscanf(content, "%*[^\"threadCountX\"]\"threadCountX\"%*[: ]%u", &numThreadCountX);
        if (SDL_strstr(content, "\"threadcount_y\""))
            SDL_sscanf(content, "%*[^\"threadcount_y\"]\"threadcount_y\"%*[: ]%u", &numThreadCountY);
        else if (SDL_strstr(content, "\"threadCountY\""))
            SDL_sscanf(content, "%*[^\"threadCountY\"]\"threadCountY\"%*[: ]%u", &numThreadCountY);
        if (SDL_strstr(content, "\"threadcount_z\""))
            SDL_sscanf(content, "%*[^\"threadcount_z\"]\"threadcount_z\"%*[: ]%u", &numThreadCountZ);
        else if (SDL_strstr(content, "\"threadCountZ\""))
            SDL_sscanf(content, "%*[^\"threadCountZ\"]\"threadCountZ\"%*[: ]%u", &numThreadCountZ);
        
        // Libérer le contenu JSON après la lecture
        if (jsonContent) 
        {
            RC2D_free(jsonContent);
            jsonContent = NULL;
        }
    }
    else 
    {
        RC2D_log(RC2D_LOG_WARN, "Shader reflection file not found: %s", jsonPath);
    }
    
    // Création du shader compute avec les informations de réflexion récupérées depuis le fichier JSON
    SDL_GPUComputePipelineCreateInfo info = {
        .code = codeShaderCompiled,
        .code_size = codeShaderCompiledSize,
        .entrypoint = entrypoint,
        .format = format,
        .num_samplers = numSamplers,
        .num_readonly_storage_textures = numReadonlyStorageTextures,
        .num_readonly_storage_buffers = numReadonlyStorageBuffers,
        .num_readwrite_storage_textures = numReadwriteStorageTextures,
        .num_readwrite_storage_buffers = numReadwriteStorageBuffers,
        .num_uniform_buffers = numUniformBuffers,
        .threadcount_x = numThreadCountX,
        .threadcount_y = numThreadCountY,
        .threadcount_z = numThreadCountZ,
        .props = 0
    };

    // Créer le shader de calcul à partir du code compilé du shader
    SDL_GPUComputePipeline* computePipelineShader = SDL_CreateGPUComputePipeline(
        rc2d_gpu_getDevice(),
        &info
    );
    if (codeShaderCompiled != NULL)
    {
        // Libérer le code du shader compilé après la création du shader
        RC2D_free(codeShaderCompiled);
        codeShaderCompiled = NULL;
    }
    if (computePipelineShader == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU compute shader from file %s, error SDL_Error: %s", filename, SDL_GetError());
        return NULL;
    }

#else // Compilation en ligne des shaders compute en HLSL
    /**
     * On génère le chemin d'accès au fichier HLSL source en fonction du nom du shader et de son stage pour la compilation en ligne des shaders.
     * On utilise SDL_snprintf pour formater le chemin d'accès au fichier HLSL source.
     */
    SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, filename);

    /**
     * Charger le fichier HLSL source
     * On utilise SDL_LoadFile pour charger le fichier HLSL source.
     */
    char* codeHLSLSource = SDL_LoadFile(fullPath, NULL);
    if (codeHLSLSource == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source: %s, SDL_Error: %s", fullPath, SDL_GetError());
        return NULL;
    }

    // Préparer les informations pour la compilation HLSL vers SPIR-V
    SDL_ShaderCross_HLSL_Info hlslInfo = {
        .source = codeHLSLSource,
        .entrypoint = "main",
        .include_dir = NULL,
        .defines = NULL,
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE,
        .enable_debug = true,
        .name = filename,
        .props = 0
    };

    // Compiler HLSL vers SPIR-V
    size_t spirvByteCodeSize = 0;
    void* spirvByteCode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &spirvByteCodeSize);

    // Libérer le code HLSL source après la compilation
    if (codeHLSLSource != NULL)
    {
        RC2D_free(codeHLSLSource);
        codeHLSLSource = NULL;
    }

    // Vérifier si la compilation HLSL vers SPIR-V a réussi
    if (spirvByteCode == NULL || spirvByteCodeSize == 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to compile HLSL to SPIR-V: %s", filename);
        return NULL;
    }

    // Réfléchir les métadonnées du pipeline de calcul
    SDL_ShaderCross_ComputePipelineMetadata* metadata = SDL_ShaderCross_ReflectComputeSPIRV(
        spirvByteCode, 
        spirvByteCodeSize,
        0
    );
    if (metadata == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to reflect compute pipeline metadata: %s", filename);

        // Si la réflexion échoue, on libère le code SPIR-V et on retourne NULL
        if (spirvByteCode != NULL) 
        {
            RC2D_free(spirvByteCode);
            spirvByteCode = NULL;
        }
        return NULL;
    }

    // Préparer les informations SPIR-V pour la compilation du pipeline de calcul 
    SDL_ShaderCross_SPIRV_Info spirvInfo = {
        .bytecode = spirvByteCode,
        .bytecode_size = spirvByteCodeSize,
        .entrypoint = "main",
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE,
        .enable_debug = true,
        .name = filename,
        .props = 0
    };

    // Compiler le pipeline de calcul
    SDL_GPUComputePipeline* computePipelineShader = SDL_ShaderCross_CompileComputePipelineFromSPIRV(
        rc2d_gpu_getDevice(),
        &spirvInfo,
        metadata,
        0
    );

    // Libérer les ressources allouées pour le code SPIR-V
    if (spirvByteCode != NULL) 
    {
        RC2D_free(spirvByteCode);
        spirvByteCode = NULL;
    }

    // Vérifier si la compilation du pipeline de calcul a réussi
    if (computePipelineShader == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU compute pipeline from SPIR-V: %s", filename);
        return NULL;
    }

    /**
     * On lock le temps d'ajouter le shader de calcul au cache des shaders de calcul,
     * et pour éviter les accès concurrents.
     */
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

    // On réalloue le cache des shaders de calcul pour ajouter le nouveau shader de calcul plus bas
    RC2D_ComputeShaderEntry* newShaders = RC2D_realloc(
        rc2d_engine_state.gpu_compute_shaders_cache,
        (rc2d_engine_state.gpu_compute_shader_count + 1) * sizeof(RC2D_ComputeShaderEntry)
    );

    // Vérifier si la réallocation a réussi
    RC2D_assert_release(newShaders != NULL, RC2D_LOG_CRITICAL, "Failed to realloc compute shader cache");

    // Mettre à jour le cache des shaders de calcul avec le nouveau shader de calcul
    rc2d_engine_state.gpu_compute_shaders_cache = newShaders;
    RC2D_ComputeShaderEntry* entry = &rc2d_engine_state.gpu_compute_shaders_cache[rc2d_engine_state.gpu_compute_shader_count++];
    entry->filename = RC2D_strdup(filename);
    entry->shader = computePipelineShader;
    entry->lastModified = rc2d_gpu_getFileModificationTime(fullPath);

    /**
     * On unlock le mutex après avoir ajouté le shader de calcul au cache.
     * Cela permet aux autres threads d'accéder au cache des shaders de calcul.
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
#endif

    // Log l'information que le shader de calcul a été chargé et mis en cache
    RC2D_log(RC2D_LOG_INFO, "Compute Shader loaded and cached: %s", filename);

    // Retourner le shader de calcul chargé
    return computePipelineShader;
}

void rc2d_gpu_hotReloadGraphicsShadersAndGraphicsPipeline(void)
{
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    /**
     * Vérifier si les mutex pour les shaders graphiques et pipelines graphiques sont initialisés
     */
    if (!rc2d_engine_state.gpu_graphics_shader_mutex)
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "gpu_graphics_shader_mutex is NULL");
    }
    if (!rc2d_engine_state.gpu_graphics_pipeline_mutex)
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "gpu_graphics_pipeline_mutex is NULL");
    }

    /**
     * Récupérer le chemin de base de l'application (où est exécuté l'exécutable)
     */
    const char* basePath = SDL_GetBasePath();
    if (basePath == NULL) 
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "SDL_GetBasePath() failed, SDL_Error: %s", SDL_GetError());
        return;
    }

    /**
     * On verrouille les mutex pour éviter les conflits d'accès concurrents
     * lors de la mise à jour des shaders graphiques et des pipelines graphiques.
     */
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);

    /**
     * Parcourir tous les shaders graphiques dans le cache
     * et vérifier s'ils ont été modifiés depuis leur dernière compilation.
     */
    for (int i = 0; i < rc2d_engine_state.gpu_graphics_shader_count; i++) 
    {
        // Récupérer le shader graphique à partir du cache
        RC2D_GraphicsShaderEntry* entry = &rc2d_engine_state.gpu_graphics_shaders_cache[i];

        /**
         * Générer le chemin d'accès complet au fichier HLSL source
         * 
         * En utilisant le nom du fichier du shader graphique récupéré du cache,
         * via `entry->filename`.
         * 
         * On utilise SDL_snprintf pour formater le chemin d'accès au fichier HLSL source.
         */
        char fullPath[512];
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, entry->filename);

        /**
         * Vérifier si le fichier HLSL source a été modifié depuis la dernière compilation
         * 
         * On utilise `rc2d_gpu_getFileModificationTime` pour obtenir le temps de modification du fichier.
         */
        SDL_Time currentModified = rc2d_gpu_getFileModificationTime(fullPath);

        // Si le fichier HLSL source a été modifié depuis la dernière compilation
        if (currentModified > entry->lastModified) 
        {            
            // Déterminer le stage
            SDL_GPUShaderStage stage;
            if (SDL_strstr(entry->filename, ".vertex")) 
            {
                stage = SDL_GPU_SHADERSTAGE_VERTEX;
            } 
            else if (SDL_strstr(entry->filename, ".fragment")) 
            {
                stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
            } 
            else 
            {
                RC2D_log(RC2D_LOG_ERROR, "Unknown shader stage for %s during reload", entry->filename);
                continue;
            }

            /**
             * Charger le fichier HLSL
             * 
             * Attend un peu que l'os ou l'ide est le temps d'écrire le 
             * fichier avant de le lire
             */
            char* codeHLSLSource = NULL;
            for (int attempt = 0; attempt < 3; attempt++) {
                codeHLSLSource = SDL_LoadFile(fullPath, NULL);
                if (codeHLSLSource != NULL) break;
                SDL_Delay(20);
            }

            /**
             * Si le code HLSL n'a pas pu être chargé après plusieurs tentatives, log l'erreur
             * et continue avec le prochain shader graphique.
             */
            if (!codeHLSLSource) 
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source after retries: %s", fullPath);
                continue;
            }

            // Préparer les informations HLSL
            SDL_ShaderCross_HLSL_Info hlslInfo = {
                .source = codeHLSLSource,
                .entrypoint = "main",
                .include_dir = NULL,
                .defines = NULL,
                .shader_stage = (SDL_ShaderCross_ShaderStage)stage,
                .enable_debug = true,
                .name = entry->filename,
                .props = 0
            };

            // Temps de début pour mesurer la compilation
            Uint64 t0 = SDL_GetPerformanceCounter();

            // Compiler HLSL vers SPIR-V
            size_t spirvByteCodeSize = 0;
            void* spirvByteCode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &spirvByteCodeSize);

            // Libérer le code HLSL source après la compilation
            if (codeHLSLSource != NULL)
            {
                RC2D_free(codeHLSLSource);
                codeHLSLSource = NULL;
            }

            // Vérifier si la compilation HLSL vers SPIR-V a réussi
            if (spirvByteCode == NULL || spirvByteCodeSize == 0)
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to compile HLSL to SPIR-V during reload: %s", entry->filename);
                continue;
            }

            // Réfléchir les métadonnées
            SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
                spirvByteCode, 
                spirvByteCodeSize, 
                0
            );
            // Vérifier si la réflexion des métadonnées a réussi
            if (!metadata) 
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to reflect graphics shader metadata during reload: %s", entry->filename);
                if (spirvByteCode) 
                {
                    RC2D_free(spirvByteCode);
                    spirvByteCode = NULL;
                }
                continue;
            }

            // Préparer les informations SPIR-V
            SDL_ShaderCross_SPIRV_Info spirvInfo = {
                .bytecode = spirvByteCode,
                .bytecode_size = spirvByteCodeSize,
                .entrypoint = "main",
                .shader_stage = (SDL_ShaderCross_ShaderStage)stage,
                .enable_debug = true,
                .name = entry->filename,
                .props = 0
            };

            // Compiler le shader
            SDL_GPUShader* newShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
                rc2d_gpu_getDevice(),
                &spirvInfo,
                metadata,
                0
            );

            // Temps de fin pour mesurer la compilation
            Uint64 t1 = SDL_GetPerformanceCounter();
            double compileTimeMs = (double)(t1 - t0) * 1000.0 / SDL_GetPerformanceFrequency();

            // Libérer les ressources allouées pour les métadonnées et le code SPIR-V
            if (metadata != NULL) 
            {
                RC2D_free(metadata);
                metadata = NULL;
            }
            if (spirvByteCode != NULL) 
            {
                RC2D_free(spirvByteCode);
                spirvByteCode = NULL;
            }

            if (newShader) 
            {
                /**
                 * Soumettre le buffer de commandes actuel et attendre que le GPU ait fini de traiter
                 * On utilise SDL_SubmitGPUCommandBufferAndAcquireFence pour soumettre le buffer de commandes
                 * et acquérir une fence pour synchroniser l'attente.
                 */
                SDL_GPUFence* fence = NULL;
                if (rc2d_engine_state.gpu_current_command_buffer) 
                {
                    // Soumettre le buffer de commandes actuel et acquérir une fence
                    fence = SDL_SubmitGPUCommandBufferAndAcquireFence(rc2d_engine_state.gpu_current_command_buffer);
                    if (!fence) 
                    {
                        RC2D_log(RC2D_LOG_ERROR, "Failed to submit command buffer and acquire fence");
                        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), newShader);
                        continue;
                    }

                    // Attendre que le GPU ait fini de traiter le buffer de commandes
                    if (!SDL_WaitForGPUFences(rc2d_gpu_getDevice(), true, &fence, 1))
                    {
                        RC2D_log(RC2D_LOG_ERROR, "Failed to wait for GPU fence: %s", SDL_GetError());
                        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), newShader);
                        continue;
                    }

                    // Libérer la fence après utilisation
                    SDL_ReleaseGPUFence(rc2d_gpu_getDevice(), fence);
                }

                // Acquérir un nouveau buffer de commandes
                rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_gpu_getDevice());
                if (!rc2d_engine_state.gpu_current_command_buffer) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "Failed to acquire new command buffer after shader reload : %s", SDL_GetError());
                    SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), newShader);
                    continue;
                }

                // Libérer l'ancien shader
                if (entry->shader) 
                {
                    SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), entry->shader);
                }

                // Remplacer l'ancien shader graphique par le nouveau shader graphique, dans le cache de RC2D
                entry->shader = newShader;

                // Mettre à jour le timestamp de dernière modification, c'est le moment où le shader a été rechargé (donc le timestamp actuel)
                entry->lastModified = currentModified;

                // Log la réussite du rechargement du shader
                RC2D_log(RC2D_LOG_INFO, "Successfully Shader %s reloaded in %.2f ms", entry->filename, compileTimeMs);

                // Mettre à jour tous les pipelines graphiques qui utilisent ce shader
                for (int j = 0; j < rc2d_engine_state.gpu_graphics_pipeline_count; j++) 
                {
                    // Récupérer le pipeline graphique à partir du cache
                    RC2D_GraphicsPipelineEntry* pipeline = &rc2d_engine_state.gpu_graphics_pipelines_cache[j];

                    // Check si le pipeline graphique utilise le shader graphique actuel
                    if ((stage == SDL_GPU_SHADERSTAGE_VERTEX && SDL_strcmp(pipeline->vertex_shader_filename, entry->filename) == 0) ||
                        (stage == SDL_GPU_SHADERSTAGE_FRAGMENT && SDL_strcmp(pipeline->fragment_shader_filename, entry->filename) == 0)) 
                    {
                        /**
                         * Si le pipeline graphique utilise le shader graphique actuel, on détruit l'ancien pipeline graphique
                         * et on le recrée avec le nouveau shader graphique.
                         */
                        SDL_ReleaseGPUGraphicsPipeline(rc2d_gpu_getDevice(), pipeline->graphicsPipeline->pipeline);
                        pipeline->graphicsPipeline->pipeline = NULL;

                        // Mettre à jour le shader graphique dans le pipeline graphique
                        if (stage == SDL_GPU_SHADERSTAGE_VERTEX) 
                        {
                            pipeline->graphicsPipeline->create_info.vertex_shader = newShader;
                        } 
                        else if (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) 
                        {
                            pipeline->graphicsPipeline->create_info.fragment_shader = newShader;
                        }

                        // Recréer le pipeline graphique avec le nouveau shader graphique
                        bool ok = rc2d_gpu_createGraphicsPipeline(pipeline->graphicsPipeline, false);
                        if (!ok || pipeline->graphicsPipeline->pipeline == NULL) {
                            RC2D_log(RC2D_LOG_CRITICAL, "Failed to rebuild pipeline for shader: %s (resulting pipeline is NULL)", entry->filename);
                            continue;
                        }

                        // Log optionnel pour debug
                        RC2D_log(RC2D_LOG_DEBUG, "Successfully rebuilt graphics pipeline using shader: %s", entry->filename);
                    }
                }
            } 
            else 
            {
                // Log l'erreur si la compilation du shader graphique a échoué
                RC2D_log(RC2D_LOG_ERROR, "Failed to reload shader: %s", entry->filename);
            }
        }
    }

    /**
     * Unlock les mutex pour permettre l'accès concurrent aux shaders graphiques et pipelines graphiques
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);
    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
#endif
}

void rc2d_gpu_hotReloadComputeShader(void)
{
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    // Vérifier que le mutex pour les compute shaders est valide
    if (!rc2d_engine_state.gpu_compute_shader_mutex)
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "gpu_compute_shader_mutex is NULL");
        return;
    }

    // Récupérer le chemin de base de l'application (chemin où est exécuté l'exécutable)
    const char* basePath = SDL_GetBasePath();
    if (basePath == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get base path for shader updates");
        return;
    }

    /**
     * On lock le mutex pour éviter les accès concurrents
     * lors de la mise à jour des compute shaders.
     */
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

    /**
     * Parcourir tous les compute shaders dans le cache
     * et vérifier s'ils ont été modifiés depuis leur dernière compilation.
     */
    for (int i = 0; i < rc2d_engine_state.gpu_compute_shader_count; i++) 
    {
        // Récupérer le shader de calcul à partir du cache
        RC2D_ComputeShaderEntry* entry = &rc2d_engine_state.gpu_compute_shaders_cache[i];

        // Construire le chemin complet vers le fichier HLSL source
        char fullPath[512];
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, entry->filename);

        // Vérifier le timestamp de dernière modification
        SDL_Time currentModified = rc2d_gpu_getFileModificationTime(fullPath);
        if (currentModified <= entry->lastModified) 
        {
            // Pas de modification, passer au shader suivant
            continue;
        }

        /**
         * Charger le fichier HLSL
         * 
         * Attend un peu que l'os ou l'ide est le temps d'écrire le 
         * fichier avant de le lire
         */
        char* codeHLSLSource = NULL;
        for (int attempt = 0; attempt < 3; attempt++) 
        {
            codeHLSLSource = SDL_LoadFile(fullPath, NULL);
            if (codeHLSLSource != NULL) break;
            SDL_Delay(20); // Attendre un peu au cas où le fichier est en cours d'écriture
        }

        /**
         * Si le code HLSL n'a pas pu être chargé après plusieurs tentatives, log l'erreur
         * et continuer avec le prochain shader de calcul.
         */
        if (!codeHLSLSource) 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source after retries: %s", fullPath);
            continue;
        }

        // Préparer les informations HLSL
        SDL_ShaderCross_HLSL_Info hlslInfo = {
            .source = codeHLSLSource,
            .entrypoint = "main",
            .include_dir = NULL,
            .defines = NULL,
            .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE,
            .enable_debug = true,
            .name = entry->filename,
            .props = 0
        };

        // Mesurer le temps de compilation
        Uint64 t0 = SDL_GetPerformanceCounter();

        // Compiler HLSL vers SPIR-V
        size_t spirvSize = 0;
        void* spirvBytecode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &spirvSize);
    
        // Libérer le code HLSL source après la compilation
        if (codeHLSLSource != NULL)
        {
            RC2D_free(codeHLSLSource);
            codeHLSLSource = NULL;
        }

        // Vérifier si la compilation HLSL vers SPIR-V a réussi
        if (spirvBytecode == NULL || spirvSize == 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to compile HLSL to SPIR-V during reload: %s", entry->filename);
            continue;
        }

        // Réfléchir les métadonnées
        SDL_ShaderCross_ComputePipelineMetadata* metadata = SDL_ShaderCross_ReflectComputeSPIRV(
            spirvBytecode, spirvSize, 0
        );
        // Vérifier si la réflexion des métadonnées a réussi
        if (!metadata) 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to reflect compute pipeline metadata during reload: %s", entry->filename);
            if (spirvBytecode) 
            {
                RC2D_free(spirvBytecode);
                spirvBytecode = NULL;
            }
            continue;
        }

        // Préparer les informations SPIR-V
        SDL_ShaderCross_SPIRV_Info spirvInfo = {
            .bytecode = spirvBytecode,
            .bytecode_size = spirvSize,
            .entrypoint = "main",
            .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE,
            .enable_debug = true,
            .name = entry->filename,
            .props = 0
        };

        // Compiler le pipeline de calcul
        SDL_GPUComputePipeline* newShader = SDL_ShaderCross_CompileComputePipelineFromSPIRV(
            rc2d_gpu_getDevice(),
            &spirvInfo,
            metadata,
            0
        );

        // Calculer le temps de compilation
        Uint64 t1 = SDL_GetPerformanceCounter();
        double compileTimeMs = (double)(t1 - t0) * 1000.0 / SDL_GetPerformanceFrequency();

        // Libérer les ressources allouées pour les métadonnées et le code SPIR-V
        if (metadata != NULL) 
        {
            RC2D_free(metadata);
            metadata = NULL;
        }
        if (spirvBytecode != NULL) 
        {
            RC2D_free(spirvBytecode);
            spirvBytecode = NULL;
        }

        if (newShader) 
        {
            /**
             * Soumettre le buffer de commandes actuel et attendre que le GPU ait fini de traiter
             * On utilise SDL_SubmitGPUCommandBufferAndAcquireFence pour soumettre le buffer de commandes
             * et acquérir une fence pour synchroniser l'attente.
             */
            SDL_GPUFence* fence = NULL;
            if (rc2d_engine_state.gpu_current_command_buffer) 
            {
                // Soumettre le buffer de commandes actuel et acquérir une fence
                fence = SDL_SubmitGPUCommandBufferAndAcquireFence(rc2d_engine_state.gpu_current_command_buffer);
                if (!fence) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "Failed to submit command buffer and acquire fence : %s", SDL_GetError());
                    SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), newShader);
                    continue;
                }

                // Attendre que le GPU ait fini de traiter le buffer
                if (!SDL_WaitForGPUFences(rc2d_gpu_getDevice(), true, &fence, 1)) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "Failed to wait for GPU fence : %s", SDL_GetError());
                    SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), newShader);
                    continue;
                }

                // Libérer la fence après utilisation
                SDL_ReleaseGPUFence(rc2d_gpu_getDevice(), fence);
            }

            // Acquérir un nouveau command buffer
            rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_gpu_getDevice());
            if (!rc2d_engine_state.gpu_current_command_buffer) 
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to acquire new command buffer after shader reload : %s", SDL_GetError());
                SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), newShader);
                continue;
            }

            // Libérer l'ancien compute shader
            if (entry->shader) 
            {
                SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), entry->shader);
            }

            // Remplacer l'ancien compute shader par le nouveau compute shader, dans le cache de RC2D
            entry->shader = newShader;

            // Mettre à jour le timestamp de dernière modification, c'est le moment où le shader a été rechargé (donc le timestamp actuel)
            entry->lastModified = currentModified;

            RC2D_log(RC2D_LOG_INFO, "Successfully reloaded compute shader %s in %.2f ms", entry->filename, compileTimeMs);
        } 
        else 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to reload compute shader: %s", entry->filename);
        }
    }

    /**
     * Unlock le mutex pour permettre l'accès concurrent aux compute shaders
     * après la mise à jour des shaders de calcul.
     */
    SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
#endif
}

bool rc2d_gpu_createGraphicsPipeline(RC2D_GPUGraphicsPipeline* pipeline, bool addToCache)
{
    // Vérification des paramètres d'entrée
    RC2D_assert_release(pipeline != NULL, RC2D_LOG_CRITICAL, "pipeline is NULL");

    // Créer props pour le nom de débogage si nécessaire
    SDL_PropertiesID props = 0;
    if (pipeline->debug_name) 
    {
        props = SDL_CreateProperties();
        SDL_SetStringProperty(props, SDL_PROP_GPU_GRAPHICSPIPELINE_CREATE_NAME_STRING, pipeline->debug_name);
    }

    // Copie la structure pour injection, en modifiant uniquement props
    RC2D_GPUGraphicsPipelineCreateInfo info = pipeline->create_info;
    info.props = props;

    // Créer le pipeline graphique
    pipeline->pipeline = SDL_CreateGPUGraphicsPipeline(rc2d_gpu_getDevice(), &info);
    SDL_DestroyProperties(props);
    if (pipeline->pipeline == NULL) 
    {
        // Si la création du pipeline échoue, on log l'erreur
        RC2D_log(RC2D_LOG_ERROR, "Failed to create graphics pipeline: %s", SDL_GetError());
        return false;
    }

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    if (addToCache)
    {
        // Ajouter dans le cache des pipelines
        SDL_LockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);

        RC2D_GraphicsPipelineEntry* newPipelines = RC2D_realloc(
            rc2d_engine_state.gpu_graphics_pipelines_cache,
            (rc2d_engine_state.gpu_graphics_pipeline_count + 1) * sizeof(RC2D_GraphicsPipelineEntry)
        );
        RC2D_assert_release(newPipelines != NULL, RC2D_LOG_CRITICAL, "Failed to realloc pipeline cache");
        rc2d_engine_state.gpu_graphics_pipelines_cache = newPipelines;

        RC2D_GraphicsPipelineEntry* entry = &rc2d_engine_state.gpu_graphics_pipelines_cache[rc2d_engine_state.gpu_graphics_pipeline_count++];
        entry->graphicsPipeline = pipeline;
        entry->vertex_shader_filename = RC2D_strdup(pipeline->vertex_shader_filename);
        entry->fragment_shader_filename = RC2D_strdup(pipeline->fragment_shader_filename);

        SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);
    }
#endif

    return true;
}

void rc2d_gpu_bindGraphicsPipeline(RC2D_GPUGraphicsPipeline* graphicsPipeline)
{
    RC2D_assert_release(graphicsPipeline != NULL, RC2D_LOG_CRITICAL, "graphicsPipeline is NULL");
    RC2D_assert_release(graphicsPipeline->pipeline != NULL, RC2D_LOG_CRITICAL, "Attempted to bind NULL graphics pipeline");

    // Bind le pipeline graphique
    SDL_BindGPUGraphicsPipeline(rc2d_engine_state.gpu_current_render_pass, graphicsPipeline->pipeline);
}

// TODO: Disponible à partir de SDL 3.4.0 (pour la fonction : SDL_GetGPUShaderFormats)
/*void rc2d_gpu_getInfo(RC2D_GPUInfo* gpuInfo) 
{
    // Vérification des paramètres d'entrée
    RC2D_assert_release(gpuInfo != NULL, RC2D_LOG_CRITICAL, "GPU info is NULL.");

    // Récupération des propriétés du GPU
    SDL_PropertiesID propsGPU = SDL_GetGPUDeviceProperties(rc2d_gpu_getDevice());

    // Vérification de la récupération des propriétés
    RC2D_assert_release(propsGPU != 0, RC2D_LOG_CRITICAL, "Failed to get GPU properties : %s", SDL_GetError());

    // Récupération des informations sur le GPU
    gpuInfo->gpu_device_name = SDL_GetStringProperty(propsGPU, "SDL_PROP_GPU_DEVICE_NAME_STRING", NULL);
    gpuInfo->gpu_device_driver_name = SDL_GetStringProperty(propsGPU, "SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING", NULL);
    gpuInfo->gpu_device_driver_version = SDL_GetStringProperty(propsGPU, "SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING", NULL);
    gpuInfo->gpu_device_driver_info = SDL_GetStringProperty(propsGPU, "SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING", NULL);

    // Destruction des propriétés du GPU
    SDL_DestroyProperties(propsGPU);
}*/

RC2D_GPUShaderFormat rc2d_gpu_getSupportedShaderFormats()
{
    // Récupération des formats de shaders supportés par le GPU via SDL_GPU
    SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(rc2d_gpu_getDevice());

    /**
     * Par défault, mis à RC2D_GPU_SHADERFORMAT_NONE.
     * 
     * On vérifie chaque format de shader supporté et on l'ajoute au résultat.
     * Peut être combiné avec un bitmask.
     */
    RC2D_GPUShaderFormat result = RC2D_GPU_SHADERFORMAT_NONE;
    if (formats & SDL_GPU_SHADERFORMAT_PRIVATE)    result |= RC2D_GPU_SHADERFORMAT_PRIVATE;
    if (formats & SDL_GPU_SHADERFORMAT_SPIRV)      result |= RC2D_GPU_SHADERFORMAT_SPIRV;
    if (formats & SDL_GPU_SHADERFORMAT_DXBC)       result |= RC2D_GPU_SHADERFORMAT_DXBC;
    if (formats & SDL_GPU_SHADERFORMAT_DXIL)       result |= RC2D_GPU_SHADERFORMAT_DXIL;
    if (formats & SDL_GPU_SHADERFORMAT_MSL)        result |= RC2D_GPU_SHADERFORMAT_MSL;
    if (formats & SDL_GPU_SHADERFORMAT_METALLIB)   result |= RC2D_GPU_SHADERFORMAT_METALLIB;

    // Retourne le résultat
    return result;
}

RC2D_GPUDevice* rc2d_gpu_getDevice(void)
{
    RC2D_assert_release(rc2d_engine_state.gpu_device != NULL, RC2D_LOG_CRITICAL, "GPU device is NULL.");
    return rc2d_engine_state.gpu_device;
}

void rc2d_gpu_clear(void)
{
    /**
     * \brief Étape 1 : Acquisition d’un GPUCommandBuffer
     *
     * Le GPUCommandBuffer est une structure temporaire qui enregistre toutes les commandes de rendu
     * (comme le début de render pass, les draw calls, les changements de pipeline, etc.) pour cette frame.
     * On ne peut soumettre les commandes au GPU qu’après avoir fini d’enregistrer dans ce buffer.
     * SDL nous fournit ce buffer via SDL_AcquireGPUCommandBuffer(), qui doit être appelé avant toute commande GPU.
     */
    rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_gpu_getDevice());
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

    /**
     * \brief Étape 3 : Vérification de la texture de swapchain
     * 
     * Si la texture de swapchain est NULL, cela signifie que la fenêtre est probablement minimisée
     * ou que le GPU n’a pas pu fournir une texture valide pour cette frame.
     * 
     * Dans ce cas, on ne peut pas continuer le rendu de la frame.
     * On marque le rendu comme étant sauté pour cette frame et on soumet le command buffer.
     */
    if (rc2d_engine_state.gpu_current_swapchain_texture == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Swapchain texture is NULL (window may be minimized). Skipping frame rendering.");
        rc2d_engine_state.skip_rendering = true;
        // On soumet le command buffer même s'il n'y a pas de swapchain texture, pour éviter les fuites de mémoire.
        SDL_SubmitGPUCommandBuffer(rc2d_engine_state.gpu_current_command_buffer);
        return;
    }
    else
    {
        // Si nous sommes ici, le rendu peut continuer
        rc2d_engine_state.skip_rendering = false;
    }

    /**
     * \brief Étape 4 : Création du ColorTargetInfo pour le render pass
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
     * - cycle : true si la texture est cyclée (c'est-à-dire que le GPU peut la réutiliser pour le rendu suivant).
     * - cycle_resolve_texture : true si la texture de résolution est cyclée (c'est-à-dire que le GPU peut la réutiliser pour le rendu suivant).
     * - padding1 et padding2 : Champs de remplissage pour l'alignement de la structure.
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
     * \brief Étape 5 : Début d’un Render Pass
     *
     * Le render pass est une section logique dans le GPUCommandBuffer où toutes les opérations de rendu sont encodées.
     * On passe ici le color_target pour spécifier que l’on souhaite dessiner sur la texture de swapchain,
     * avec les réglages définis ci-dessus.
     */
    rc2d_engine_state.gpu_current_render_pass = SDL_BeginGPURenderPass(
        rc2d_engine_state.gpu_current_command_buffer,
        &colorTargetInfo, 
        1, // Nombre de cibles de couleur, ici 1 car on n'utilise que la swapchain texture.
        NULL // Pas besoin de depth_stencil_target_info c'est seulement utiles dans le contexte des applications 3D.
    );
    RC2D_assert_release(rc2d_engine_state.gpu_current_render_pass != NULL, RC2D_LOG_CRITICAL, "Failed to begin GPU render pass");

    /**
     * \brief Étape 6 : Définir le viewport
     *
     * Le viewport est une zone rectangulaire de la cible de rendu où le dessin est autorisé.
     * SDL ne définit pas de viewport par défaut dans le render pass, donc tu dois toujours
     * l’assigner manuellement. Il est impératif que `gpu_current_viewport` ait été
     * correctement calculé avant l’appel à cette fonction.
     */
    RC2D_assert_release(rc2d_engine_state.gpu_current_viewport != NULL, RC2D_LOG_CRITICAL, "No viewport set in rc2d_engine_state");
    SDL_SetGPUViewport(rc2d_engine_state.gpu_current_render_pass, rc2d_engine_state.gpu_current_viewport);
}

void rc2d_gpu_present(void)
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

    /**
     * \brief Étape 3 : Soumettre le command buffer
     *
     * C’est ici que le GPU exécute réellement toutes les commandes encodées pendant cette frame.
     * SDL_SubmitGPUCommandBuffer() envoie le tout pour traitement asynchrone.
     */
    if (rc2d_engine_state.gpu_current_command_buffer && !rc2d_engine_state.skip_rendering)
    {
        SDL_SubmitGPUCommandBuffer(rc2d_engine_state.gpu_current_command_buffer);
    }

    /**
     * \brief Étape 4 : Nettoyage de la swapchain texture, command buffer courant et render pass courant
     *
     * Ces pointeurs sont remis à NULL pour indiquer qu’ils ne sont plus valides.
     * Cela permet de s’assurer que les prochaines commandes GPU ne seront pas envoyées
     * avant que de nouvelles textures, buffers ou passes de rendu ne soient acquises.
     */
    rc2d_engine_state.gpu_current_swapchain_texture = NULL;
    rc2d_engine_state.gpu_current_command_buffer = NULL;
    rc2d_engine_state.gpu_current_render_pass = NULL;
}