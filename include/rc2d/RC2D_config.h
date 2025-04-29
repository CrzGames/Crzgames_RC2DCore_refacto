#ifndef RC2D_CONFIG_H
#define RC2D_CONFIG_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Si RC2D_SHADER_HOT_RELOAD_ENABLED est défini à 1, le support des shaders en ligne est activé.
 * 
 * Cette option permet d'activer le rechargement à chaud des shaders, ce qui signifie que les shaders peuvent être 
 * rechargés sans avoir à redémarrer l'application. Permet lors de la modification des source d'un shader HLSL de
 * voir le résultat immédiatement en cours d'exécution dans l'application.
 * 
 * \note Cette fonctionnalité est utile pour le développement, mais peut avoir un impact sur les performances.
 * Il est donc fortement recommandé de la désactiver pour les versions de production et d'utiliser les shaders hors ligne (précompilés).
 * Ce qui évitera également d'embarquer les bibliothèques tiers dans l'application.
 * 
 * Pour Windows, il faudra linker/fournir les bibliothèques suivantes :
 * - dxcompiler.dll : Traduit HLSL en DXIL (Direct3D12) ou SPIR-V (Vulkan).
 * - dxil.dll : Contient les définitions de base et le validateur DXIL officiel de Microsoft.
 * - spirv-cross-c-shared.dll : Permet les réflexions automatiques sur les ressources shaders.
 * - SDL3_shadercross.dll + SDL3_shadercross.lib OU SDL3_shadercross_static.lib : Permet la compilation en ligne de shaders.
 * 
 * \note Pour Windows, SPIRV-Cross ne permet pas de traduire les shaders SPIR-V en MSL (Metal), mais permet quand même d'avoir 
 * les réflexions automatiques sur les ressources shaders.
 * 
 * Pour Linux, il faudra linker/fournir les bibliothèques suivantes :
 * - libdxcompiler.so : Traduit HLSL en DXIL (Direct3D12) ou SPIR-V (Vulkan).
 * - libdxil.so : Contient les définitions de base et la validation du format DXIL.
 * - libspirv-cross-c-shared.so : Traduit SPIR-V en MSL (Metal) + Permet la réflexion automatique des ressources shaders.
 * - libSDL3_shadercross.so : Permet la compilation en ligne de shaders.
 * - libvkd3d.so : ?
 * - libvkd3d-shader.so : ?
 * - libvkd3d-utils.so : Bibliothèque utilitaire pour VKD3D.
 * 
 * Pour macOS, il faudra linker/fournir les bibliothèques suivantes :
 * - libdxcompiler.dylib : Traduit HLSL en DXIL (Direct3D12) ou SPIR-V (Vulkan).
 * - libdxil.dylib : Contient les définitions de base et la validation du format DXIL.
 * - libspirv-cross-c-shared.dylib : Traduit SPIR-V en MSL (Metal) + Permet la réflexion automatique des ressources shaders.
 * - libSDL3_shadercross.dylib : Permet la compilation en ligne de shaders.
 * - libvkd3d.dylib : ?
 * - libvkd3d-shader.dylib : ?
 * - libvkd3d-utils.dylib : Bibliothèque utilitaire pour VKD3D.
 */
#ifndef RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
#define RC2D_GPU_SHADER_HOT_RELOAD_ENABLED 0
#endif

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_CONFIG_H
