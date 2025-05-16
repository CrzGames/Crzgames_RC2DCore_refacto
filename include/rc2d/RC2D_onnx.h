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
 * \brief Représente une entrée ou une sortie pour une inférence ONNX.
 *
 * \note Le type, les dimensions et le buffer doivent être correctement renseignés par l’appelant.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_OnnxTensor {
    const char* name;                         ///< Nom de l’entrée ou de la sortie (doit correspondre à celui du modèle)
    void* data;                               ///< Pointeur vers les données
    ONNXTensorElementDataType type;           ///< Type ONNX (ex : FLOAT, INT32, BOOL, STRING…)
    int64_t shape[8];                         ///< Shape du tensor (max 8 dimensions supportées ici)
    size_t dims;                              ///< Nombre de dimensions
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

#ifdef __cplusplus
}
#endif

#endif // RC2D_ONNX_MODULE_ENABLED
#endif // RC2D_ONNX_H