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
 * 
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
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
 * 
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
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
 * 
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
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
 * 
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
 */
#ifndef RC2D_ONNX_MODULE_ENABLED
#define RC2D_ONNX_MODULE_ENABLED 1
#endif

/**
 * \brief Si RC2D_MEMORY_DEBUG_ENABLED est défini à 1, le suivi des allocations mémoire est activé.
 *
 * Lorsque cette option est activée, toutes les allocations dynamiques effectuées via `RC2D_malloc`,
 * `RC2D_calloc`, `RC2D_realloc`, `RC2D_free`, `RC2D_strdup`, ou `RC2D_strndup` sont enregistrées
 * avec des métadonnées supplémentaires, telles que le fichier source, la ligne, et la fonction appelante.
 *
 * Cela permet de générer un rapport complet des fuites mémoire (pointeurs non libérés) à la fin de l'exécution
 * du programme ou à la demande via la fonction `rc2d_memory_report()`. Le rapport inclut des informations
 * détaillées sur chaque allocation non libérée, facilitant le débogage des problèmes de gestion de mémoire.
 *
 * \note Cette fonctionnalité est destinée exclusivement au développement ou au débogage, car elle entraîne
 * un surcoût en termes de mémoire et de performances. En production, il est fortement recommandé de désactiver
 * cette option (en définissant `RC2D_MEMORY_DEBUG_ENABLED` à 0) pour éviter ces surcoûts et optimiser l'exécution.
 *
 * Pour utiliser cette fonctionnalité :
 * - Remplacez les appels aux fonctions SDL standard (`SDL_malloc`, `SDL_free`, etc.) ou C standard par leurs équivalents
 *   RC2D (`RC2D_malloc`, `RC2D_free`, etc.).
 * - Activez la macro en définissant `RC2D_MEMORY_DEBUG_ENABLED` à 1 dans votre configuration.
 * - À la fin de l'exécution du programme, un rapport des fuites mémoire est automatiquement affiché
 *   (via un appel à `rc2d_memory_report()` enregistré avec `atexit`), ou vous pouvez appeler manuellement
 *   `rc2d_memory_report()` pour un rapport à un moment spécifique.
 *
 * \warning L'activation de cette fonctionnalité augmente la consommation de mémoire, car chaque allocation
 * est accompagnée d'une structure de suivi. De plus, les performances peuvent être légèrement dégradées
 * en raison de la gestion de la liste des allocations.
 *
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
 */
#ifndef RC2D_MEMORY_DEBUG_ENABLED
#define RC2D_MEMORY_DEBUG_ENABLED 0
#endif

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_CONFIG_H
