#ifndef RC2D_GPU_H
#define RC2D_GPU_H

#include <SDL3/SDL_gpu.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Mode de dessin pour les formes graphiques.
 *
 * Définit si une forme est dessinée en mode rempli ou en contour.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_DrawMode {
    /**
     * \brief Mode rempli : la forme est dessinée avec une couleur de remplissage.
     */
    RC2D_DRAWMODE_FILL,

    /**
     * \brief Mode contour : la forme est dessinée uniquement avec un contour.
     */
    RC2D_DRAWMODE_LINE
} RC2D_DrawMode;

/**
 * \brief Structure représentant une couleur RGBA.
 * 
 * Cette structure est utilisée pour définir une couleur en termes de rouge, vert, bleu et alpha.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Color {
    /**
     * \brief Composante rouge de la couleur.
     */
	Uint8 r;

    /**
     * \brief Composante verte de la couleur.
     */
	Uint8 g;

    /**
     * \brief Composante bleue de la couleur.
     */
	Uint8 b;

    /**
     * \brief Composante alpha de la couleur (opacité).
     * 
     * Une valeur de 0 signifie complètement transparent, tandis qu'une valeur de 255 signifie complètement opaque.
     */
	Uint8 a;
} RC2D_Color;

/**
 * \brief Contexte GPU utilisé par RC2D.
 *
 * Ce type wrappe SDL_GPUDevice, le contexte GPU principal fourni par SDL3.
 *
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_GPUDevice RC2D_GPUDevice;

/**
 * @brief Informations sur le GPU utilisé par l'application.
 */
typedef struct RC2D_GPUInfo {
    /**
     * \brief Contient le nom du périphérique sous-jacent tel qu'indiqué par le pilote système. 
     * 
     * Cette chaîne n'a pas de format standardisé, présente de fortes incohérences entre les 
     * périphériques matériels et les pilotes, et peut être modifiée à tout moment. 
     * 
     * \note N'essayez pas d'analyser cette chaîne, car elle risque d'échouer à l'avenir lors de la mise à jour 
     * des pilotes système et de l'introduction de nouveaux périphériques matériels.
     * 
     * Les valeurs qui ont été observées dans le passé incluent :
     * - GTX 970
     * - GeForce GTX 970
     * - NVIDIA GeForce GTX 970
     * - Microsoft Direct3D12 (NVIDIA GeForce GTX 970)
     * - ...
     * 
     * La liste ci-dessus montre que le même appareil peut avoir des formats différents, le nom du fournisseur 
     * peut apparaître ou non dans la chaîne, le nom du fournisseur inclus peut ne pas être le fournisseur du chipset 
     * sur l'appareil, certains fabricants incluent des marques pseudo-légales tandis que d'autres ne le font pas, 
     * certains appareils peuvent ne pas utiliser de nom marketing dans la chaîne, la chaîne de l'appareil peut être 
     * enveloppée par le nom d'une interface de traduction, l'appareil peut être émulé dans un logiciel ou la chaîne 
     * peut contenir du texte générique qui n'identifie pas du tout l'appareil.
     */
    const char* gpu_device_name;

    /**
     * \brief Contient le nom auto-déclaré du pilote du système sous-jacent.
     * 
     * Les valeurs qui ont été observées dans le passé incluent :
     * - MoltenVK
     * - Intel open-source Mesa driver
     * - venus
     * - ...
     */
    const char* gpu_device_driver_name;

    /**
     * \brief Contient la version auto-déclarée du pilote système sous-jacent. Il s'agit d'une 
     * chaîne de version relativement courte, au format non spécifié.
     *
     * \note Préférez `gpu_device_driver_info` si elle est disponible, car elle contient généralement 
     * les mêmes informations, mais dans un format plus lisible.
     * 
     * Les valeurs qui ont été observées dans le passé incluent :
     * - 53.0.0
     * - 0.405.2463
     * - 32.0.15.6614
     */
    const char* gpu_device_driver_version;

    /**
     * \brief Contient les informations détaillées sur la version du pilote système sous-jacent, 
     * telles qu'indiquées par le pilote.
     * 
     * \note Si disponible, cette chaîne est préférable à `gpu_device_driver_version`.
     * 
     * Les valeurs qui ont été observées dans le passé incluent :
     * - Mesa 21.2.2 (LLVM 12.0.1)
     * - Mesa 22.2.0-devel (git-f226222 2022-04-14 impish-oibaf-ppa)
     * - 1.2.11
     * - 101.6559
     */
    const char* gpu_device_driver_info;
} RC2D_GPUInfo;

/**
 * \brief Structure pour une texture GPU.
 * 
 * Contient une texture SDL3 GPU et ses dimensions.
 * 
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GPUTexture {
    SDL_GPUTexture* gpu_texture;
    int width;
    int height;
} RC2D_GPUTexture;

/**
 * \brief Nombre maximal d’images autorisées en vol sur le GPU.
 *
 * Permet de choisir le comportement de la file d’attente GPU en ajustant le 
 * nombre maximal d’images « en vol » (c’est-à-dire que le CPU peut soumettre 
 * avant que le GPU ait fini de les afficher).
 * 
 * \note Cela influence directement la **latence visuelle**, le **débit de rendu** et la **compatibilité 
 * avec les petites configurations**. RC2D_GPU_FRAMES_BALANCED est le choix par défaut.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_GPUFramesInFlight {
    /**
     * 1 image en vol : latence minimale, mais risque de baisse de framerate sur GPU lents.
     * - Idéal pour les jeux pixel art ou à forte précision de contrôle.
     * - Réduit la latence visuelle au maximum (le rendu est très réactif).
     * - Recommandé pour les petites configurations (PCs peu puissants, intégrés, etc.).
     * - Peut réduire le framerate si le GPU est souvent à la traîne.
     */
    RC2D_GPU_FRAMES_LOW_LATENCY = 1,

    /**
     * 2 images en vol : bon équilibre entre réactivité et performance.
     * - Bon équilibre entre réactivité et performance.
     * - Convient à la plupart des machines modernes.
     * - Recommandé pour la majorité des jeux 2D.
     */
    RC2D_GPU_FRAMES_BALANCED = 2,

    /**
     * 3 images en vol : latence légèrement augmentée, mais débit GPU maximisé.
     * - Maximise le débit GPU, utile pour les animations lourdes ou les effets complexes.
     * - Légère augmentation de la latence visuelle.
     * - Recommandé pour des jeux fluides, cinématiques, ou avec beaucoup de post-traitement.
     */
    RC2D_GPU_FRAMES_HIGH_THROUGHPUT = 3
} RC2D_GPUFramesInFlight;

/**
 * \brief Pilotes GPU disponibles pour RC2D.
 *
 * Ce type permet à l'utilisateur de forcer un backend GPU spécifique à l'initialisation.
 * RC2D se base sur les backends supportés par SDL3_GPU : `vulkan`, `metal`, `direct3d12`.
 * 
 * \note Concernant RC2D_GPU_DRIVER_PRIVATE, il s'agit d'un backend propriétaire par rapport à la plateforme de développement.
 * Tels que pour la Playstation 5, la Xbox Series X/S, Nintendo Switch 1/2, etc.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_GPUDriver {
    /**
     * Laisser RC2D choisir automatiquement le meilleur backend GPU.
     * 
     * Cela permet sous le capot de RC2D à SDL de sélectionner le backend le plus adapté à la plateforme,
     * en tenant compte des capacités du matériel et des préférences de l'utilisateur.
     */
    RC2D_GPU_DRIVER_DEFAULT = 0,

    /**
     * Forcer l'utilisation de Vulkan comme backend GPU.
     * 
     * Cela peut être utile pour les développeurs qui souhaitent tirer parti des
     * fonctionnalités avancées de Vulkan ou qui ont besoin d'une compatibilité spécifique.
     */
    RC2D_GPU_DRIVER_VULKAN,

    /**
     * Forcer l'utilisation de Metal comme backend GPU.
     * 
     * Cela est particulièrement pertinent pour les applications macOS et iOS,
     * où Metal est le backend graphique natif recommandé.
     */
    RC2D_GPU_DRIVER_METAL,

    /**
     * Forcer l'utilisation de Direct3D12 comme backend GPU.
     * 
     * Cela est pertinent pour les applications Windows qui souhaitent tirer parti
     * des fonctionnalités avancées de Direct3D12.
     */
    RC2D_GPU_DRIVER_DIRECT3D12,

    /**
     * Forcer l'utilisation d'un backend GPU privée sous NDA (Non Disclosure Agreement).
     * 
     * Par rapport à la plateforme de développement, cela peut être un backend propriétaire ou un
     * backend expérimental qui n'est pas encore public.
     */
    RC2D_GPU_DRIVER_PRIVATE
} RC2D_GPUDriver;

/**
 * \brief Options avancées pour la création du contexte GPU.
 * 
 * Permet de configurer certains comportements internes lors de l’initialisation
 * du GPU via SDL3. Ces paramètres influencent la manière dont SDL configure
 * le périphérique graphique sous-jacent (Vulkan, Metal et Direct3D12).
 * 
 * \note RC2D active automatiquement tous les formats de shaders supportés par la plateforme cible
 * (SPIR-V, DXBC, DXIL, MSL, etc.), ce qui permet à l’utilisateur de ne fournir
 * que les formats dont il a besoin sans configuration supplémentaire.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GPUAdvancedOptions {
    /**
     * Active le mode debug GPU.
     * 
     * Cela permet à SDL d’activer des couches de validation supplémentaires 
     * pour capturer les erreurs GPU pendant le développement.
     * 
     * \note True par défaut dans RC2D.
     */
    bool debugMode;

    /**
     * Active les logs détaillés pendant la création du contexte GPU.
     * 
     * Cela permet de voir les capacités GPU détectées, les formats supportés, etc.
     * 
     * \note True par défaut dans RC2D.
     */
    bool verbose;

    /**
     * Demande à SDL de privilégier un GPU à faible consommation si disponible.
     * 
     * Cela peut être utile pour les appareils mobiles ou les ordinateurs portables
     * où l'économie d'énergie est une priorité.
     * 
     * \note False par défaut dans RC2D.
     */
    bool preferLowPower;

    /**
     * Permet de forcer un backend graphique spécifique (Vulkan, Metal, Direct3D12 ou un backend privé).
     * 
     * \note RC2D_GPU_DRIVER_DEFAULT par défaut dans RC2D, ce qui laisse SDL choisir automatiquement.
     */
    RC2D_GPUDriver driver;
} RC2D_GPUAdvancedOptions;

/**
 * \brief Formats de shaders supportés par le GPU.
 *
 * Utilisé pour indiquer quels formats de shaders sont disponibles selon le backend.
 *
 * Formats de shaders supportés :
 * - RC2D_GPU_SHADERFORMAT_SPIRV : SPIR-V (Vulkan)
 * - RC2D_GPU_SHADERFORMAT_DXBC : DXBC (Direct3D 11)
 * - RC2D_GPU_SHADERFORMAT_DXIL : DXIL (Direct3D 12)
 * - RC2D_GPU_SHADERFORMAT_MSL : MSL (Metal)
 * - RC2D_GPU_SHADERFORMAT_METALLIB : MetalLib (Metal)
 * - RC2D_GPU_SHADERFORMAT_PRIVATE : Private (Propriétaire)
 * 
 * \note Peut être combiné avec un bitmask.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_GPUShaderFormat {
    RC2D_GPU_SHADERFORMAT_NONE       = 0,
    RC2D_GPU_SHADERFORMAT_PRIVATE    = 1 << 0,
    RC2D_GPU_SHADERFORMAT_SPIRV      = 1 << 1,
    RC2D_GPU_SHADERFORMAT_DXBC       = 1 << 2,
    RC2D_GPU_SHADERFORMAT_DXIL       = 1 << 3,
    RC2D_GPU_SHADERFORMAT_MSL        = 1 << 4,
    RC2D_GPU_SHADERFORMAT_METALLIB   = 1 << 5
} RC2D_GPUShaderFormat;

/**
 * \brief Shader graphique utilisé par RC2D.
 * 
 * Ce type est un wrapper autour de SDL_GPUShader, utilisé pour les shaders graphiques.
 * 
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_GPUShader RC2D_GPUShader;

/**
 * \brief Shader de calcul utilisé par RC2D.
 * 
 * Ce type est un wrapper autour de SDL_GPUComputePipeline, utilisé pour les shaders de calcul.
 * 
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_GPUComputePipeline RC2D_GPUComputePipeline;

/**
 * \brief Pipeline graphique utilisé par RC2D.
 * 
 * Ce type est un wrapper autour de SDL_GPUGraphicsPipeline, utilisé pour les pipelines graphiques (vertex/fragment shaders).
 * 
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_GPUGraphicsPipeline RC2D_GPUGraphicsPipelineHandle; 

/**
 * \brief Informations de création pour un pipeline graphique.
 * 
 * Cette structure contient les informations nécessaires pour créer un pipeline graphique,
 * telles que les shaders, l'état d'entrée des vertex, le type de primitive, etc.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_GPUGraphicsPipelineCreateInfo RC2D_GPUGraphicsPipelineCreateInfo;

/**
 * \brief Structure pour un pipeline graphique utilisé par RC2D.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GPUGraphicsPipeline {
    RC2D_GPUGraphicsPipelineHandle* pipeline;
    RC2D_GPUGraphicsPipelineCreateInfo create_info;
    const char* debug_name;
    char* vertex_shader_filename;
    char* fragment_shader_filename;
} RC2D_GPUGraphicsPipeline;

/**
 * \brief Remplit une structure RC2D_GPUInfo avec les métadonnées du GPU utilisé.
 * 
 * \param {RC2D_GPUDevice*} gpuDevice - Le périphérique GPU SDL initialisé par RC2D.
 * \param {RC2D_GPUInfo*} gpuInfo - Pointeur vers la structure à remplir.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
//void rc2d_gpu_getInfo(RC2D_GPUInfo* gpuInfo);

/**
 * \brief Récupère le périphérique GPU utilisé par RC2D.
 * 
 * \return {RC2D_GPUDevice*} Pointeur vers le périphérique GPU SDL utilisé par RC2D.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_GPUDevice* rc2d_gpu_getDevice(void);

/**
 * \brief Récupère les formats de shaders supportés par le GPU actuel.
 *
 * \return {RC2D_GPUShaderFormat} Bitmask RC2D_GPUShaderFormat contenant les formats supportés.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
RC2D_GPUShaderFormat rc2d_gpu_getSupportedShaderFormats(void);

/**
 * \brief Charge un shader de calcul à partir d'un fichier source HLSL ou d'un fichier binaire précompilé.
 * 
 * Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est défini à 1, cela compile le shader à la volée à
 * partir du fichier source HLSL. Sinon, cela charge le fichier binaire déjà précompilé.
 * 
 * \param {const char*} filename - Nom du fichier shader à charger.
 * 
 * \return {RC2D_GPUComputePipeline*} Pointeur vers le shader de calcul chargé, ou NULL en cas d'erreur.
 * 
 * \warning Le pointeur retourné doit être libéré par l'appelant avec SDL_ReleaseGPUComputePipeline lorsque le shader n'est plus nécessaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_GPUComputePipeline* rc2d_gpu_loadComputeShader(const char* filename);

/**
 * \brief Charge un shader graphique à partir d'un fichier source HLSL ou d'un fichier binaire précompilé.
 * 
 * Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est défini à 1, cela compile le shader à la volée à
 * partir du fichier source HLSL. Sinon, cela charge le fichier binaire déjà précompilé.
 * 
 * \param {const char*} filename - Nom du fichier shader à charger.
 * 
 * \return {SDL_GPUShader*} Pointeur vers le shader chargé, ou NULL en cas d'erreur.
 * 
 * \warning Le pointeur retourné doit être libéré par l'appelant avec SDL_ReleaseGPUShader lorsque le shader n'est plus nécessaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
SDL_GPUShader* rc2d_gpu_loadGraphicsShader(const char* filename);

/**
 * \brief Crée un pipeline graphique à partir des informations fournies.
 * 
 * \param {RC2D_GPUGraphicsPipeline*} pipeline - Pointeur vers la structure de pipeline à créer.
 * \param {bool} addToCache - Indique si le pipeline doit être ajouté au cache pour le hot-reload.
 * 
 * \return {bool} True si la création du pipeline a réussi, false sinon.
 * 
 * \warning Le champ pipeline de la structure RC2D_GPUGraphicsPipeline doit être libéré par l'appelant avec 
 * SDL_ReleaseGPUGraphicsPipeline lorsque le pipeline n'est plus nécessaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_gpu_createGraphicsPipeline(RC2D_GPUGraphicsPipeline* pipeline, bool addToCache);

/**
 * \brief Charge un shader de calcul à partir d'un fichier source HLSL ou d'un fichier binaire précompilé.
 * 
 * Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est défini à 1, cela compile le shader à la volée à
 * partir du fichier source HLSL. Sinon, cela charge le fichier binaire déjà précompilé.
 * 
 * \param {const char*} filename - Nom du fichier shader à charger.
 * 
 * \return {SDL_GPUShader*} Pointeur vers le shader chargé, ou NULL en cas d'erreur.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_gpu_bindGraphicsPipeline(RC2D_GPUGraphicsPipeline* graphicsPipeline);

/**
 * \brief Efface l'écran pour préparer le rendu d'une nouvelle frame.
 *
 * Cette fonction acquiert un command buffer, la texture de swapchain, et commence une passe de rendu.
 * Elle doit être appelée avant toute opération de rendu dans une frame.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_gpu_clear(void);

/**
 * \brief Présente le rendu à l'écran.
 *
 * Cette fonction termine la passe de rendu, soumet le command buffer au GPU, et présente la frame.
 * Elle doit être appelée après toutes les opérations de rendu dans une frame.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_gpu_present(void);

/**
 * \brief Définit la couleur globale utilisée pour les opérations de dessin.
 *
 * Cette fonction règle la couleur (R, G, B, A) utilisée pour dessiner les formes, comme les rectangles.
 * Les valeurs sont des entiers non signés de 8 bits (0 à 255).
 *
 * \param color Couleur RGBA (composantes r, g, b, a entre 0 et 255).
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_gpu_setColor(RC2D_Color color);

/**
 * \brief Dessine un rectangle à l'écran.
 *
 * Cette fonction dessine un rectangle à la position (x, y) avec la largeur et la hauteur spécifiées.
 * Le mode de dessin peut être RC2D_DRAWMODE_FILL (rempli) ou RC2D_DRAWMODE_LINE (contour).
 *
 * \param mode Mode de dessin (RC2D_DRAWMODE_FILL ou RC2D_DRAWMODE_LINE).
 * \param x Position x en pixels logiques.
 * \param y Position y en pixels logiques.
 * \param width Largeur en pixels logiques.
 * \param height Hauteur en pixels logiques.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_gpu_rectangle(RC2D_DrawMode mode, float x, float y, float width, float height);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_GPU_H