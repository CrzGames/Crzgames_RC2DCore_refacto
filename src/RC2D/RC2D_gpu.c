#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_internal.h>

#include <SDL3/SDL_properties.h>
#ifdef RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
#include <SDL3_shadercross/SDL_shadercross.h>
#endif

SDL_GPUShader* rc2d_gpu_loadShader(const char* filename) {
    // Vérification des paramètres d'entrée
    RC2D_assert_release(filename != NULL, RC2D_LOG_CRITICAL, "Shader filename is NULL");

#ifdef RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    // Vérifier si le shader est déjà dans le cache
    SDL_LockMutex(rc2d_engine_state.gpu_shader_mutex);
    for (int i = 0; i < rc2d_engine_state.gpu_shader_count; i++) {
        if (SDL_strcmp(rc2d_engine_state.gpu_shaders[i].filename, filename) == 0) {
            SDL_GPUShader* shader = rc2d_engine_state.gpu_shaders[i].shader;
            SDL_UnlockMutex(rc2d_engine_state.gpu_shader_mutex);
            RC2D_log(RC2D_LOG_INFO, "Shader already loaded from cache: %s", filename);
            return shader;
        }
    }
    SDL_UnlockMutex(rc2d_engine_state.gpu_shader_mutex);
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

#ifndef RC2D_GPU_SHADER_HOT_RELOAD_ENABLED // Compilation hors ligne des shaders HLSL
    /**
     * entrypoint : Point d'entrée du shader (main pour SPIR-V, DXIL et main0 pour MSL).
     */
    const char* entrypoint = NULL;

    // Récupérer les formats supportés par le backend actuel
    SDL_GPUShaderFormat backendFormatsSupported = rc2d_gpu_getSupportedShaderFormats();

    // Le format de shader à utiliser pour la compilation hors ligne
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

    // Générer le chemin d'accès au fichier binaire du shader compilé en fonction du backend
    if (backendFormatsSupported & SDL_GPU_SHADERFORMAT_SPIRV) 
    {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/compiled/spirv/%s.spv", basePath, filename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
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
     * On utilise SDL_sscanf pour extraire les valeurs des champs "samplers", "uniformBuffers", "readOnlyStorageBuffers" et "readOnlyStorageTextures".
     * On ferme le fichier JSON après la lecture.
     */
    Uint32 numSamplers = 0;
    Uint32 numUniformBuffers = 0;
    Uint32 numStorageBuffers = 0;
    Uint32 numStorageTextures = 0;

    void* jsonContent = SDL_LoadFile(jsonPath, NULL);
    if (jsonContent) 
    {
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
    
        SDL_free(jsonContent);
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

    SDL_GPUShader* shader = SDL_CreateGPUShader(rc2d_gpu_getDevice(), &info);
    SDL_free(codeShaderCompiled);

    if (!shader) 
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
    size_t codeHLSLSize = 0;
    char* codeHLSLSource = SDL_LoadFile(fullPath, &codeHLSLSize);
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
        .shader_stage = stage,
        .enable_debug = true,
        .name = filename,
        .props = 0
    };

    SDL_GPUShader* shader = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(
        rc2d_gpu_getDevice(),
        &hlslInfo,
        &metadata
    );
    SDL_free(codeHLSLSource);

    if (!shader) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to create GPU shader from HLSL: %s", filename);
        return NULL;
    }

    // Ajouter le shader au cache
    SDL_LockMutex(rc2d_engine_state.gpu_shader_mutex);
    RC2D_ShaderEntry* newShaders = SDL_realloc(
        rc2d_engine_state.gpu_shaders,
        (rc2d_engine_state.gpu_shader_count + 1) * sizeof(RC2D_ShaderEntry)
    );
    RC2D_assert_release(newShaders != NULL, RC2D_LOG_CRITICAL, "Failed to realloc shader cache");
    rc2d_engine_state.shaders = newShaders;

    RC2D_ShaderEntry* entry = &rc2d_engine_state.gpu_shaders[rc2d_engine_state.gpu_shader_count++];
    entry->filename = SDL_strdup(filename);
    entry->shader = shader;
    entry->lastModified = get_file_modification_time(fullPath);
    SDL_UnlockMutex(rc2d_engine_state.gpu_shader_mutex);

    RC2D_log(RC2D_LOG_INFO, "Shader loaded and cached: %s", filename);
    return shader;
#endif
}

static Sint64 get_file_modification_time(const char* path) 
{
    SDL_IOStream* file = SDL_OpenIO(path, "r");
    if (!file) return 0;
    Sint64 mtime = SDL_GetIOProperties(file, SDL_PROP_IOSTREAM_MODIFICATION_TIME_NUMBER, 0);
    SDL_CloseIO(file);
    return mtime;
}

void rc2d_gpu_checkShaderUpdates(void) 
{
    if (!rc2d_engine_state.gpu_shader_mutex) return;

    const char* basePath = SDL_GetBasePath();
    if (!basePath) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get base path for shader updates");
        return;
    }

    SDL_LockMutex(rc2d_engine_state.gpu_shader_mutex);
    for (int i = 0; i < rc2d_engine_state.gpu_shader_count; i++) 
    {
        RC2D_ShaderEntry* entry = &rc2d_engine_state.gpu_shaders[i];

        // Construire le chemin complet
        char fullPath[512];
        SDL_snprintf(fullPath, sizeof(fullPath), "%sshaders/src/%s.hlsl", basePath, entry->filename);

        // Vérifier le timestamp
        Sint64 currentModified = get_file_modification_time(fullPath);
        if (currentModified > entry->lastModified) 
        {
            RC2D_log(RC2D_LOG_INFO, "Shader file modified: %s", entry->filename);

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

            // Charger le fichier HLSL
            size_t codeHLSLSize = 0;
            char* codeHLSLSource = SDL_LoadFile(fullPath, &codeHLSLSize);
            if (!codeHLSLSource) {
                RC2D_log(RC2D_LOG_ERROR, "Failed to load HLSL shader source: %s", fullPath);
                continue;
            }

            // Recompiler le shader
            SDL_ShaderCross_GraphicsShaderMetadata metadata = {0};
            SDL_ShaderCross_HLSL_Info hlslInfo = {
                .source = codeHLSLSource,
                .entrypoint = "main",
                .include_dir = NULL,
                .defines = NULL,
                .shader_stage = stage,
                .enable_debug = true,
                .name = entry->filename,
                .props = 0
            };

            SDL_GPUShader* newShader = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(
                rc2d_gpu_getDevice(),
                &hlslInfo,
                &metadata
            );
            SDL_free(codeHLSLSource);

            if (newShader) {
                // Attendre que le GPU ait fini d'utiliser l'ancien shader
                SDL_GPUSubmitAndReleaseCommandBuffer(rc2d_engine_state.gpu_current_command_buffer);
                rc2d_engine_state.gpu_current_command_buffer = SDL_AcquireGPUCommandBuffer(rc2d_engine_state.gpu_device);

                // Remplacer l'ancien shader
                SDL_DestroyGPUShader(entry->shader);
                entry->shader = newShader;
                entry->lastModified = currentModified;
                RC2D_log(RC2D_LOG_INFO, "Shader reloaded: %s", entry->filename);
            } else {
                RC2D_log(RC2D_LOG_ERROR, "Failed to reload shader: %s", entry->filename);
            }
        }
    }
    SDL_UnlockMutex(rc2d_engine_state.shader_mutex);
}

void rc2d_gpu_getInfo(RC2D_GPUInfo* gpuInfo) 
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
}

static RC2D_GPUShaderFormat rc2d_gpu_getSupportedShaderFormats()
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