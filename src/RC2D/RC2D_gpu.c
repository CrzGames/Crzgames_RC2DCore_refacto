#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_internal.h>

#include <SDL3/SDL_properties.h>

void rc2d_gpu_getInfo(RC2D_GPUDevice* gpuDevice, RC2D_GPUInfo* gpuInfo) {
    RC2D_assert_release(gpuDevice != NULL, "gpuDevice ne doit pas être NULL dans rc2d_gpu_getInfo");
    RC2D_assert_release(gpuInfo != NULL, "gpuInfo ne doit pas être NULL dans rc2d_gpu_getInfo");

    SDL_PropertiesID propsGPU = SDL_GetGPUDeviceProperties(gpuDevice);
    RC2D_assert_release(propsGPU != 0, "SDL_GetGPUDeviceProperties a échoué dans rc2d_gpu_getInfo");

    gpuInfo->gpu_device_name = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_NAME_STRING);
    gpuInfo->gpu_device_driver_name = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING);
    gpuInfo->gpu_device_driver_version = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING);
    gpuInfo->gpu_device_driver_info = SDL_GetStringProperty(propsGPU, SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING);

    SDL_DestroyProperties(propsGPU);
}

RC2D_GPUDevice* rc2d_gpu_getDevice(void)
{
    return rc2d_gpu_device;
}