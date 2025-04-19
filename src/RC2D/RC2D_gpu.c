#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_internal.h>

#include <SDL3/SDL_properties.h>

void rc2d_gpu_getInfo(RC2D_GPUDevice* gpuDevice, RC2D_GPUInfo* gpuInfo) {
    RC2D_assert_release(gpuDevice != NULL, RC2D_LOG_CRITICAL, "GPU device is NULL.");
    RC2D_assert_release(gpuInfo != NULL, RC2D_LOG_CRITICAL, "GPU info is NULL.");

    SDL_PropertiesID propsGPU = SDL_GetGPUDeviceProperties(gpuDevice);

    RC2D_assert_release(propsGPU != 0, RC2D_LOG_CRITICAL, "Failed to get GPU properties : %s", SDL_GetError());

    gpuInfo->gpu_device_name = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_NAME_STRING);
    gpuInfo->gpu_device_driver_name = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING);
    gpuInfo->gpu_device_driver_version = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING);
    gpuInfo->gpu_device_driver_info = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING);

    SDL_DestroyProperties(propsGPU);
}

RC2D_GPUDevice* rc2d_gpu_getDevice(void)
{
    RC2D_assert_release(rc2d_gpu_device != NULL, RC2D_LOG_CRITICAL, "GPU device is NULL.");
    return rc2d_gpu_device;
}