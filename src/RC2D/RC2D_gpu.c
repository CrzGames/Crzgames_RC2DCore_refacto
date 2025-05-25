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

static SDL_Time get_file_modification_time(const char* path) 
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
        RC2D_log(RC2D_LOG_ERROR, "Failed to get path info for %s: %s", path, SDL_GetError());
        return 0;
    }
}

RC2D_GPUShader* rc2d_gpu_loadGraphicsShader(const char* filename) 
{
    // Vérification des paramètres d'entrée
    RC2D_assert_release(filename != NULL, RC2D_LOG_CRITICAL, "Graphics Shader filename is NULL");

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    // Vérifier si le shader graphics est déjà dans le cache (donc déjà chargé une fois)
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);

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

    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
#endif

    // Récupérer le chemin de base de l'application (où est exécuté l'exécutable)
    const char* basePath = SDL_GetBasePath();
    RC2D_assert_release(basePath != NULL, RC2D_LOG_CRITICAL, "SDL_GetBasePath() failed");

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

#if !RC2D_GPU_SHADER_HOT_RELOAD_ENABLED // Compilation hors ligne des shaders HLSL
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
    if (!codeShaderCompiled) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load compiled shader: %s", fullPath);
        return NULL;
    }

    /**
     * Récupérer les informations de réflexion du shader (nombre de samplers, uniform buffers, etc.)
     * En mode hors ligne, on utilise un fichier JSON généré par le compilateur de shaders.
     * 
     * On génère le chemin d'accès au fichier JSON de réflexion en fonction du nom du shader et de son stage.
     */
    char jsonPath[512];
    SDL_snprintf(jsonPath, sizeof(jsonPath), "%sshaders/reflection/%s.json", basePath, filename);

    /**
     * On ouvre le fichier JSON de réflexion pour récupérer les informations de réflexions sur le shader.
     * On utilise SDL_OpenIO pour ouvrir le fichier JSON et SDL_ReadLineIO pour lire chaque ligne.
     * On utilise SDL_sscanf pour extraire les valeurs des champs : samplers, uniform_buffers, storage_buffers et storage_textures.
     * On ferme le fichier JSON après la lecture.
     */
    Uint32 numSamplers = 0;
    Uint32 numUniformBuffers = 0;
    Uint32 numStorageBuffers = 0;
    Uint32 numStorageTextures = 0;

    /**
     * Charger le fichier JSON de réflexion
     * On utilise SDL_LoadFile pour charger le fichier JSON de réflexion.
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
        RC2D_free(jsonContent);
    }
    else 
    {
        RC2D_log(RC2D_LOG_WARN, "Shader reflection file not found: %s", jsonPath);
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

    SDL_GPUShader* graphicsShader = SDL_CreateGPUShader(rc2d_gpu_getDevice(), &info);
    RC2D_free(codeShaderCompiled);

    if (!graphicsShader) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU shader from file: %s", filename);
        return NULL;
    }

#else // Compilation en ligne des shaders HLSL
    /**
     * On génère le chemin d'accès au fichier HLSL source en fonction du nom du shader et de son stage.
     * On utilise SDL_snprintf pour formater le chemin d'accès au fichier HLSL source.
     */
    SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, filename);

    /**
     * Charger le fichier HLSL source
     * On utilise SDL_LoadFile pour charger le fichier HLSL source.
     */
    char* codeHLSLSource = SDL_LoadFile(fullPath, NULL);
    if (!codeHLSLSource) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source: %s", fullPath);
        return NULL;
    }

    // Réflexion automatique via shadercross
    SDL_ShaderCross_GraphicsShaderMetadata metadata = {0};
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

    SDL_GPUShader* graphicsShader = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(
        rc2d_gpu_getDevice(),
        &hlslInfo,
        &metadata
    );
    RC2D_free(codeHLSLSource);

    if (!graphicsShader) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU graphics shader from HLSL: %s", filename);
        return NULL;
    }

    // Ajouter le shader graphics dans le cache
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);

    RC2D_GraphicsShaderEntry* newShaders = RC2D_realloc(
        rc2d_engine_state.gpu_graphics_shaders_cache,
        (rc2d_engine_state.gpu_graphics_shader_count + 1) * sizeof(RC2D_GraphicsShaderEntry)
    );
    RC2D_assert_release(newShaders != NULL, RC2D_LOG_CRITICAL, "Failed to realloc shader cache");

    rc2d_engine_state.gpu_graphics_shaders_cache = newShaders;
    RC2D_GraphicsShaderEntry* entry = &rc2d_engine_state.gpu_graphics_shaders_cache[rc2d_engine_state.gpu_graphics_shader_count++];
    entry->filename = RC2D_strdup(filename);
    entry->shader = graphicsShader;
    entry->lastModified = get_file_modification_time(fullPath);

    SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
#endif

    RC2D_log(RC2D_LOG_INFO, "Graphics Shader loaded and cached: %s", filename);
    return graphicsShader;
}

RC2D_GPUComputePipeline* rc2d_gpu_loadComputeShader(const char* filename) 
{
    // Vérification des paramètres d'entrée
    RC2D_assert_release(filename != NULL, RC2D_LOG_CRITICAL, "Compute Shader filename is NULL");

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    // Vérifier si le shader est déjà dans le cache
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

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

    SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
#endif

    // Récupérer le chemin de base de l'application (où est exécuté l'exécutable)
    const char* basePath = SDL_GetBasePath();
    RC2D_assert_release(basePath != NULL, RC2D_LOG_CRITICAL, "SDL_GetBasePath() failed");

    /**
     * fullPath : Chemin d'accès complet au fichier binaire du shader compilé ou au fichier HLSL source.
     */
    char fullPath[512];

#if !RC2D_GPU_SHADER_HOT_RELOAD_ENABLED // Compilation hors ligne des shaders HLSL
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
    if (!codeShaderCompiled) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load compiled shader: %s", fullPath);
        return NULL;
    }

    /**
     * Récupérer les informations de réflexion du shader (nombre de samplers, uniform buffers, etc.)
     * En mode hors ligne, on utilise un fichier JSON généré par le compilateur de shaders.
     * 
     * On génère le chemin d'accès au fichier JSON de réflexion en fonction du nom du shader et de son stage.
     */
    char jsonPath[512];
    SDL_snprintf(jsonPath, sizeof(jsonPath), "%sshaders/reflection/%s.json", basePath, filename);

    /**
     * On ouvre le fichier JSON de réflexion pour récupérer les informations de réflexions sur le shader.
     * On utilise SDL_OpenIO pour ouvrir le fichier JSON et SDL_ReadLineIO pour lire chaque ligne.
     * On utilise SDL_sscanf pour extraire les valeurs des champs : "samplers", "uniformBuffers", "readOnlyStorageBuffers", "readOnlyStorageTextures", "readwriteStorageTextures", "readwriteStorageBuffers", "threadCountX", "threadCountY" et "threadCountZ".
     * On ferme le fichier JSON après la lecture.
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

    /**
     * Charger le fichier JSON de réflexion
     * On utilise SDL_LoadFile pour charger le fichier JSON de réflexion.
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
        RC2D_free(jsonContent);
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

    SDL_GPUComputePipeline* computePipelineShader = SDL_CreateGPUComputePipeline(
        rc2d_gpu_getDevice(),
        &info
    );
    RC2D_free(codeShaderCompiled);

    if (!computePipelineShader) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU compute shader from file: %s", filename);
        return NULL;
    }

#else // Compilation en ligne des shaders HLSL
    /**
     * On génère le chemin d'accès au fichier HLSL source en fonction du nom du shader et de son stage.
     * On utilise SDL_snprintf pour formater le chemin d'accès au fichier HLSL source.
     */
    SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, filename);

    /**
     * Charger le fichier HLSL source
     * On utilise SDL_LoadFile pour charger le fichier HLSL source.
     */
    char* codeHLSLSource = SDL_LoadFile(fullPath, NULL);
    if (!codeHLSLSource) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source: %s", fullPath);
        return NULL;
    }

    // Réflexion automatique du compute shader via SDL_shadercross
    SDL_ShaderCross_ComputePipelineMetadata metadata = {0};
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

    SDL_GPUComputePipeline* computePipelineShader = SDL_ShaderCross_CompileComputePipelineFromHLSL(
        rc2d_gpu_getDevice(),
        &hlslInfo,
        &metadata
    );
    RC2D_free(codeHLSLSource);

    if (!computePipelineShader) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU compute shader from HLSL: %s", filename);
        return NULL;
    }

    // Ajouter le shader de calcul dans le cache
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

    RC2D_ComputeShaderEntry* newShaders = RC2D_realloc(
        rc2d_engine_state.gpu_compute_shaders_cache,
        (rc2d_engine_state.gpu_compute_shader_count + 1) * sizeof(RC2D_ComputeShaderEntry)
    );
    RC2D_assert_release(newShaders != NULL, RC2D_LOG_CRITICAL, "Failed to realloc compute shader cache");

    rc2d_engine_state.gpu_compute_shaders_cache = newShaders;
    RC2D_ComputeShaderEntry* entry = &rc2d_engine_state.gpu_compute_shaders_cache[rc2d_engine_state.gpu_compute_shader_count++];
    entry->filename = RC2D_strdup(filename);
    entry->shader = computePipelineShader;
    entry->lastModified = get_file_modification_time(fullPath);

    SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
#endif

    RC2D_log(RC2D_LOG_INFO, "Compute Shader loaded and cached: %s", filename);
    return computePipelineShader;
}

void rc2d_gpu_hotReloadGraphicsShadersAndGraphicsPipeline(void)
{
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    if (!rc2d_engine_state.gpu_graphics_shader_mutex)
        RC2D_log(RC2D_LOG_ERROR, "gpu_graphics_shader_mutex is NULL");

    if (!rc2d_engine_state.gpu_graphics_pipeline_mutex)
        RC2D_log(RC2D_LOG_ERROR, "gpu_graphics_pipeline_mutex is NULL");


    const char* basePath = SDL_GetBasePath();
    if (basePath == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get base path for shader updates");
        return;
    }

    SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
    SDL_LockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);

    for (int i = 0; i < rc2d_engine_state.gpu_graphics_shader_count; i++) 
    {
        RC2D_GraphicsShaderEntry* entry = &rc2d_engine_state.gpu_graphics_shaders_cache[i];

        // Construire le chemin complet
        char fullPath[512];
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, entry->filename);

        // Vérifier le timestamp
        SDL_Time currentModified = get_file_modification_time(fullPath);

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
             * et continue avec le prochain shader
             */
            if (!codeHLSLSource) 
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source after retries: %s", fullPath);
                continue;
            }

            // Données du shader
            SDL_ShaderCross_GraphicsShaderMetadata metadata = {0};
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

            // Temps de début pour le rechargement pour connaitre le temps de compilation
            Uint64 t0 = SDL_GetPerformanceCounter();

            // Compiler le shader HLSL
            SDL_GPUShader* newShader = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(
                rc2d_gpu_getDevice(),
                &hlslInfo,
                &metadata
            );

            // Temps de fin pour le rechargement pour connaitre le temps de compilation
            Uint64 t1 = SDL_GetPerformanceCounter();
            double compileTimeMs = (double)(t1 - t0) * 1000.0 / SDL_GetPerformanceFrequency();

            // Free le code source HLSL
            RC2D_free(codeHLSLSource);

            if (newShader) 
            {
                // Soumettre le buffer de commandes actuel et obtenir une fence
                SDL_GPUFence* fence = NULL;
                if (rc2d_engine_state.gpu_current_command_buffer) 
                {
                    fence = SDL_SubmitGPUCommandBufferAndAcquireFence(rc2d_engine_state.gpu_current_command_buffer);
                    if (!fence) 
                    {
                        RC2D_log(RC2D_LOG_ERROR, "Failed to submit command buffer and acquire fence");
                        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), newShader);
                        continue;
                    }

                    // Attendre que le GPU ait fini de traiter le buffer
                    if (!SDL_WaitForGPUFences(rc2d_engine_state.gpu_device, true, &fence, 1))
                    {
                        RC2D_log(RC2D_LOG_ERROR, "Failed to wait for GPU fence: %s", SDL_GetError());
                        SDL_ReleaseGPUShader(rc2d_gpu_getDevice(), newShader);
                        continue;
                    }
                    SDL_ReleaseGPUFence(rc2d_gpu_getDevice(), fence);
                }

                // Acquérir un nouveau buffer de commandes
                rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_engine_state.gpu_device);
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

                // Remplacer l'ancien shader par le nouveau dans le cache de RC2D
                entry->shader = newShader;
                entry->lastModified = currentModified;

                // Log la réussite du rechargement du shader
                RC2D_log(RC2D_LOG_INFO, "Successfully Shader %s reloaded in %.2f ms", entry->filename, compileTimeMs);

                // Mettre à jour tous les pipelines qui utilisent ce shader
                for (int j = 0; j < rc2d_engine_state.gpu_graphics_pipeline_count; j++) 
                {
                    RC2D_GraphicsPipelineEntry* pipeline = &rc2d_engine_state.gpu_graphics_pipelines_cache[j];

                    if ((stage == SDL_GPU_SHADERSTAGE_VERTEX && SDL_strcmp(pipeline->vertex_shader_filename, entry->filename) == 0) ||
                        (stage == SDL_GPU_SHADERSTAGE_FRAGMENT && SDL_strcmp(pipeline->fragment_shader_filename, entry->filename) == 0)) 
                    {
                        SDL_ReleaseGPUGraphicsPipeline(rc2d_gpu_getDevice(), pipeline->graphicsPipeline->pipeline);
                        pipeline->graphicsPipeline->pipeline = NULL;

                        if (stage == SDL_GPU_SHADERSTAGE_VERTEX) 
                        {
                            pipeline->graphicsPipeline->create_info.vertex_shader = newShader;
                        } 
                        else if (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) 
                        {
                            pipeline->graphicsPipeline->create_info.fragment_shader = newShader;
                        }

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
                RC2D_log(RC2D_LOG_ERROR, "Failed to reload shader: %s", entry->filename);
            }
        }
    }

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
        RC2D_log(RC2D_LOG_ERROR, "gpu_compute_shader_mutex is NULL");
        return;
    }

    // Récupérer le chemin de base de l'application
    const char* basePath = SDL_GetBasePath();
    if (basePath == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get base path for shader updates");
        return;
    }

    // Verrouiller le mutex pour accéder au cache des compute shaders
    SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);

    // Parcourir le cache des compute shaders
    for (int i = 0; i < rc2d_engine_state.gpu_compute_shader_count; i++) 
    {
        RC2D_ComputeShaderEntry* entry = &rc2d_engine_state.gpu_compute_shaders_cache[i];

        // Construire le chemin complet vers le fichier HLSL source
        char fullPath[512];
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, entry->filename);

        // Vérifier le timestamp de dernière modification
        SDL_Time currentModified = get_file_modification_time(fullPath);
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

        if (!codeHLSLSource) 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source after retries: %s", fullPath);
            continue;
        }

        // Préparer les métadonnées et les informations pour la compilation du compute shader
        SDL_ShaderCross_ComputePipelineMetadata metadata = {0};
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

        // Compiler le nouveau compute shader
        SDL_GPUComputePipeline* newShader = SDL_ShaderCross_CompileComputePipelineFromHLSL(
            rc2d_gpu_getDevice(),
            &hlslInfo,
            &metadata
        );

        // Calculer le temps de compilation
        Uint64 t1 = SDL_GetPerformanceCounter();
        double compileTimeMs = (double)(t1 - t0) * 1000.0 / SDL_GetPerformanceFrequency();

        // Libérer le code source HLSL
        RC2D_free(codeHLSLSource);

        if (newShader) 
        {
            // Synchroniser le GPU pour éviter les conflits
            SDL_GPUFence* fence = NULL;
            if (rc2d_engine_state.gpu_current_command_buffer) 
            {
                fence = SDL_SubmitGPUCommandBufferAndAcquireFence(rc2d_engine_state.gpu_current_command_buffer);
                if (!fence) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "Failed to submit command buffer and acquire fence : %s", SDL_GetError());
                    SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), newShader);
                    continue;
                }

                // Attendre que le GPU ait fini de traiter le buffer
                if (!SDL_WaitForGPUFences(rc2d_engine_state.gpu_device, true, &fence, 1)) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "Failed to wait for GPU fence : %s", SDL_GetError());
                    SDL_ReleaseGPUComputePipeline(rc2d_gpu_getDevice(), newShader);
                    continue;
                }
                SDL_ReleaseGPUFence(rc2d_gpu_getDevice(), fence);
            }

            // Acquérir un nouveau command buffer
            rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_engine_state.gpu_device);
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

            // Mettre à jour l'entrée dans le cache
            entry->shader = newShader;
            entry->lastModified = currentModified;

            RC2D_log(RC2D_LOG_INFO, "Successfully reloaded compute shader %s in %.2f ms", entry->filename, compileTimeMs);
        } 
        else 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to reload compute shader: %s", entry->filename);
        }
    }

    // Déverrouiller le mutex
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