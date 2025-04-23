#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_internal.h>

#include <SDL3/SDL_properties.h>

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