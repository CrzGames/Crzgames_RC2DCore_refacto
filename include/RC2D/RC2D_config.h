#ifndef RC2D_CONFIG_H
#define RC2D_CONFIG_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est défini à 1, le support des shaders en ligne est activé.
 * 
 * Shaders en ligne (1) : à chaud, pendant l'exécution du jeu, avec hot reload.
 * Shader hors ligne (0) : pré-compilation avant l'exécution du jeu, sans hot reload.
 * 
 * Cette option permet d'activer le rechargement à chaud des shaders, ce qui signifie que les shaders peuvent être 
 * rechargés sans avoir à redémarrer l'application. Permet lors de la modification des source d'un shader HLSL de
 * voir le résultat immédiatement en cours d'exécution dans l'application.
 * 
 * Le rechargement à chaud des shaders est disponible que pour les plateformes de bureau (Windows, macOS, Linux).
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

/**
 * \brief Si RC2D_NET_MODULE_ENABLED est défini à 1, le support du réseau (UDP) est activé.
 * 
 * Cela utilise la bibliothèque RCENet pour la gestion des sockets et des connexions réseau.
 * RCENet est une bibliothèque de réseau multiplateforme qui permet de créer des applications réseau
 * en C/C++. Elle fournit une API simple et efficace pour la gestion des connexions réseau, 
 * la communication entre les clients et les serveurs, ainsi que la gestion des événements réseau.
 * 
 * \note RCENet est un fork de ENet, qui est une bibliothèque de réseau populaire pour les jeux vidéo.
 */
#ifndef RC2D_NET_MODULE_ENABLED
#define RC2D_NET_MODULE_ENABLED 1
#endif

/**
 * \brief Si RC2D_DATA_MODULE_ENABLED est défini à 1, la compression / chiffrement / hachage est activé.
 * 
 * Cela utilise la bibliothèque OpenSSL pour la gestion des algorithmes de chiffrement et de hachage.
 * OpenSSL est une bibliothèque open-source qui fournit des outils pour la sécurité des communications
 * sur les réseaux informatiques. Elle est largement utilisée pour le chiffrement, la signature numérique,
 * la gestion des certificats et d'autres tâches liées à la sécurité.
 */
#ifndef RC2D_DATA_MODULE_ENABLED
#define RC2D_DATA_MODULE_ENABLED 1
#endif

/**
 * \brief Si RC2D_ONNX_MODULE_ENABLED est défini à 1, le support de ONNX est activé.
 * 
 * Cela utilise la bibliothèque ONNX Runtime pour la gestion des modèles ONNX.
 * ONNX Runtime est un moteur d'exécution haute performance pour les modèles de machine learning
 * au format Open Neural Network Exchange (ONNX). Il permet d'exécuter des modèles ONNX sur différentes
 * plateformes et matériels, optimisant ainsi les performances et la portabilité des modèles de machine learning.
 */
#ifndef RC2D_ONNX_MODULE_ENABLED
#define RC2D_ONNX_MODULE_ENABLED 1
#endif

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_CONFIG_H
