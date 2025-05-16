#ifndef RC2D_ONNX_H
#define RC2D_ONNX_H

#if RC2D_ONNX_MODULE_ENABLED

#include <onnxruntime_c_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant un modèle ONNX chargé en mémoire.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_OnnxModel {
    /**
     * Chemin relatif vers le fichier ONNX à charger, par exemple : "models-onnx/my_model.onnx"
     * 
     * \note Le path doit être rempli par l'utilisateur avant d'appeler rc2d_onnx_loadModel().
     */
    const char* path;

    /**
     * Session ONNX Runtime créée à partir du modèle ONNX, qui à été spécifiée par la propriété path.
     * 
     * Cette session peut être utilisée plusieurs fois pour effectuer des inférences sans avoir
     * à recharger le modèle depuis le disque.
     * 
     * \note La session sera remplie par la fonction rc2d_onnx_loadModel().
     */
    OrtSession* session;
} RC2D_OnnxModel;

/**
 * \brief Représente un tenseur d’entrée ou de sortie utilisé dans une inférence ONNX.
 *
 * Cette structure est utilisée à la fois pour :
 *   - fournir des **entrées** au modèle (inputs),
 *   - récupérer les **résultats** d’inférence (outputs).
 *
 * === Cas des ENTRÉES (inputs) ===
 * --------------------------------
 * Tous les champs de la structure doivent être renseignés **manuellement** :
 *
 * - `name`  : Nom exact de l’entrée attendue par le modèle ONNX (sensible à la casse).
 *             Il doit correspondre exactement à ce que le modèle a défini.
 *
 * - `data`  : Pointeur vers les données brutes à fournir au modèle.
 *             Cela peut être un tableau de `float`, `int32_t`, `bool`, ou un tableau de `char*` pour les chaînes.
 *
 * - `type`  : Type des éléments ONNX (ex : `ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT`, `..._STRING`, etc.).
 *
 * - `shape` : Tableau d'entiers 64 bits définissant la forme du tenseur (ex : `{1, 3}` pour un `float[1][3]`).
 *
 * - `dims`  : Nombre de dimensions (ex : `2` pour `{1, 3}`).
 *
 * ❗ En cas d’erreur de type, de taille, ou de shape, le modèle peut échouer à l'exécution.
 *
 *
 * === Cas des SORTIES (outputs) ===
 * ---------------------------------
 * Seuls les champs suivants doivent être spécifiés par l'utilisateur :
 *
 * - `name`  : **Nom logique** de la sortie, utilisé uniquement pour vous repérer dans votre tableau `outputs[]`.
 *             Il n’a pas besoin de correspondre exactement au nom du modèle, car le runtime utilise l’ordre.
 *             Il permet simplement d’identifier les éléments plus facilement dans le code (`outputs[0]`, `outputs[1]`, etc.).
 *
 * - `data`  : 
 *   - Si le type est scalaire (`FLOAT`, `INT32`, `BOOL`, etc.) :
 *     le buffer pointé par `data` doit être **pré-alloué**.
 *   - Si le type est `STRING`, `data` doit être un tableau de `char*` initialisé,
 *     et chaque `char*` sera automatiquement alloué après l’inférence.
 *
 * **Les champs suivants sont automatiquement remplis après `rc2d_onnx_run()` :**
 *
 * - `type`  : Détecté dynamiquement à partir du modèle.
 * - `shape` : Déterminée automatiquement via `GetDimensions()`.
 * - `dims`  : Rempli à partir du nombre de dimensions du modèle.
 *
 *
 * === Exemples ===
 *
 * ➤ **Entrée float :**  
 * ```c
 * float my_input[3] = {1.0f, 2.0f, 3.0f};
 * RC2D_OnnxTensor input = {
 *     .name = "input_tensor",
 *     .data = my_input,
 *     .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
 *     .shape = {1, 3},
 *     .dims = 2
 * };
 * ```
 * 
 * ➤ **Entrée string :**
 * ```c
 * const char* my_strings[] = {"hello", "world"};
 * RC2D_OnnxTensor input = {
 *     .name = "input_tensor_string",
 *     .data = (void*)my_strings,
 *     .type = ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING,
 *     .shape = {1, 2},
 *     .dims = 2
 * };
 * ```
 * 
 * ➤ **Sortie float :**  
 * ```c
 * float scores[10];
 * RC2D_OnnxTensor output = {
 *     .name = "output_scores",
 *     .data = scores // Doit être alloué,
 *     // type / shape / dims seront remplis par rc2d_onnx_run()
 * };
 * ```
 *
 * ➤ **Sortie string :**  
 * ```c
 * char* labels[2]; // Non alloués, mais initialisés (pointeurs nuls ok)
 * RC2D_OnnxTensor output = {
 *     .name = "output_labels",
 *     .data = labels
 *     // type / shape / dims seront remplis par rc2d_onnx_run()
 * };
 * // Après l’appel, chaque labels[i] est alloué avec SDL_malloc
 * ```
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_OnnxTensor {
    const char* name;                         ///< Nom de l’entrée ou de la sortie (obligatoire pour s’y retrouver dans le code)
    void* data;                               ///< Pointeur vers les données (entrée ou sortie)
    ONNXTensorElementDataType type;           ///< Type ONNX (FLOAT, INT32, BOOL, STRING, etc.)
    int64_t shape[8];                         ///< Shape du tensor (calculé automatiquement pour les sorties)
    size_t dims;                              ///< Nombre de dimensions (détecté automatiquement pour les sorties)
} RC2D_OnnxTensor;


/**
 * \brief Charge un modèle ONNX et initialise une session à partir d’un chemin.
 *
 * \param {RC2D_OnnxModel*} model - Pointeur vers la structure à remplir.
 * \return true en cas de succès, false sinon.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_onnx_loadModel(RC2D_OnnxModel* model);

/**
 * \brief Libère la session ONNX associée à un modèle précédemment chargé.
 *
 * \param model Le modèle dont la session doit être libérée.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_onnx_unloadModel(RC2D_OnnxModel* model);

/**
 * \brief Lance une inférence avec un modèle ONNX et un tableau d’entrées/sorties dynamiques.
 *
 * \param {RC2D_OnnxModel*} model - Modèle ONNX chargé
 * \param {RC2D_OnnxTensor*} inputs - Tableau d’entrées
 * \param {RC2D_OnnxTensor*} outputs - Tableau de sorties
 *
 * \return true si l’inférence a réussi, false sinon
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_onnx_run(RC2D_OnnxModel* model, RC2D_OnnxTensor* inputs, RC2D_OnnxTensor* outputs);

/**
 * \brief Libère les ressources associées à un ou plusieurs tensors ONNX.
 *
 * Cette fonction permet de libérer proprement les données associées à une ou plusieurs
 * structures `RC2D_OnnxTensor`, en particulier lorsqu’il s’agit de sorties de type `STRING`.
 *
 * Elle s’utilise typiquement après un appel à `rc2d_onnx_run()` afin de nettoyer les
 * chaînes allouées dynamiquement par le moteur ONNX Runtime dans les sorties de type string.
 *
 * ⚠️ Pour les types scalaires (FLOAT, INT, BOOL...), le buffer pointé par `tensor->data`
 * n’est **pas** libéré (il appartient à l’utilisateur).  
 * En revanche, pour les types `STRING`, chaque `char*` est libéré via `SDL_free()`.
 *
 * Tous les champs de la structure sont ensuite réinitialisés à zéro.
 *
 * \param tensors Tableau de `RC2D_OnnxTensor` à libérer (ou pointeur vers un seul tensor)
 * \param count Nombre de tensors à libérer (1 pour une structure unique)
 *
 * === Exemple : sortie scalaire ===
 * \code
 * float result[10];
 * RC2D_OnnxTensor out = {
 *     .name = "output",
 *     .data = result
 * };
 * rc2d_onnx_run(&model, inputs, &out);
 * rc2d_onnx_freeTensors(&out, 1); // OK, ne libère pas result
 * \endcode
 *
 * === Exemple : sortie string ===
 * \code
 * char* labels[2];
 * RC2D_OnnxTensor out = {
 *     .name = "labels",
 *     .data = labels
 * };
 * rc2d_onnx_run(&model, inputs, &out);
 * rc2d_onnx_freeTensors(&out, 1); // Libère labels[0], labels[1], puis nettoie la structure
 * \endcode
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_onnx_freeTensors(RC2D_OnnxTensor* tensors, size_t count);

size_t rc2d_onnx_computeElementCount(const int64_t* shape, size_t dims);

#ifdef __cplusplus
}
#endif

#endif // RC2D_ONNX_MODULE_ENABLED
#endif // RC2D_ONNX_H