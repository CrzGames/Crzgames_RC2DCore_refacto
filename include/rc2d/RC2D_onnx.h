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
 * \note Cette session est persistante et permet de faire plusieurs inférences sans recharger le modèle.
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
     * Session ONNX Runtime créée à partir du modèle.
     * Cette session peut être utilisée plusieurs fois pour effectuer des inférences sans avoir
     * à recharger le modèle depuis le disque.
     * 
     * \note La session sera remplie par la fonction rc2d_onnx_loadModel().
     */
    OrtSession* session;
} RC2D_OnnxModel;

/**
 * \brief Charge un modèle ONNX et initialise une session à partir d’un chemin.
 *
 * \param {RC2D_OnnxModel*} model - Pointeur vers la structure à remplir.
 * \return true en cas de succès, false sinon.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_onnx_loadModel(RC2D_OnnxModel* model);

#ifdef __cplusplus
}
#endif

#endif // RC2D_ONNX_MODULE_ENABLED
#endif // RC2D_ONNX_H