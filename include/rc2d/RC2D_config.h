#ifndef RC2D_CONFIG_H
#define RC2D_CONFIG_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Si RC2D_SHADER_HOT_RELOAD_ENABLED est défini à 1, le support des shaders en ligne est activé.
 * 
 * Shaders en ligne (1) : à chaud, pendant l'exécution du jeu, avec hot reload.
 * Shader hors ligne (0) : pré-compilation avant l'exécution du jeu, sans hot reload.
 * 
 * Cette option permet d'activer le rechargement à chaud des shaders, ce qui signifie que les shaders peuvent être 
 * rechargés sans avoir à redémarrer l'application. Permet lors de la modification des source d'un shader HLSL de
 * voir le résultat immédiatement en cours d'exécution dans l'application.
 * 
 * \note Cette fonctionnalité est utile pour le développement, mais cela engendre une surcharge à l'exécution.
 * Il est donc fortement recommandé de la désactiver pour les versions de production et d'utiliser les shaders hors ligne.
 * Ce qui évitera également d'embarquer les bibliothèques tiers dans l'application.
 * 
 * Pour Windows/macOS/Linux, il faudra linker/fournir les bibliothèques suivantes :
 * - dxcompiler : Traduit HLSL en DXIL (Direct3D12) ou SPIR-V (Vulkan).
 * - dxil : Contient les définitions de base et le validateur DXIL officiel de Microsoft.
 * - libspirv-cross-c-shared : Traduit SPIR-V en MSL (Metal) + Permet la réflexion automatique des ressources shaders.
 * - SDL3_shadercross : Permet la compilation en ligne de shaders.
 */
#ifndef RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
#define RC2D_GPU_SHADER_HOT_RELOAD_ENABLED 0
#endif

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_CONFIG_H
