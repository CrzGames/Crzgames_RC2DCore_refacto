#if RC2D_ONNX_MODULE_ENABLED

#include <RC2D/RC2D_onnx.h>
#include <RC2D/RC2D_logger.h>

// Variables globales pour l’environnement ONNX Runtime et les options de session
static OrtEnv* g_ort_env = NULL;
static OrtSessionOptions* g_session_options = NULL;

bool rc2d_onnx_init(void) 
{
    OrtStatus* status;

    // Récupère l’API ONNX Runtime (interface principale vers les fonctions)
    const OrtApi* ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);

    // Crée un environnement ONNX Runtime avec un niveau de log "warning"
    status = ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "rc2d", &g_ort_env);
    if (status != NULL) 
    {
        // Si la création échoue, log l’erreur et retourne false
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to create ONNX Runtime environment");
        return false;
    }

    // Crée les options de session (permet de configurer les EPs, optimisations, etc.)
    status = ort->CreateSessionOptions(&g_session_options);
    if (status != NULL) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to create session options");

        // Libère l’environnement ONNX Runtime si la création des options échoue
        ort->ReleaseEnv(g_ort_env);
        g_ort_env = NULL;

        return false;
    }

    /**
     * Active la gestion de la mémoire CPU pour améliorer les performances
     * 
     * Au lieu de faire plein de petits malloc()/free() pendant l’exécution d’un modèle, 
     * ONNX Runtime utilise une "arena allocator" :
     * 
     * Une grande zone mémoire préallouée (l'arène) qui est réutilisée efficacement pendant l’inférence.
     */
    ort->EnableCpuMemArena(g_session_options);

    // Définit le niveau d’optimisation graphique au maximum (fusion, simplification, etc.)
    ort->SetSessionGraphOptimizationLevel(g_session_options, ORT_ENABLE_ALL);

    /**
     * Sélection automatique des Execution Providers (EPs) disponibles, 
     * du meilleur au pire donc : GPU -> NPU -> CPU
     */
    status = ort->SessionOptionsSetEpSelectionPolicy(g_session_options, OrtExecutionProviderDevicePolicy_MAX_PERFORMANCE);
    if (status != NULL) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to set EP selection policy");

        // Libère les options de session et l’environnement ONNX Runtime
        ort->ReleaseSessionOptions(g_session_options);
        g_session_options = NULL;
        ort->ReleaseEnv(g_ort_env);
        g_ort_env = NULL;

        return false;
    }

    // Liste les Execution Providers disponibles et les log
    const char** available_providers = NULL;
    int num_providers = 0;
    status = ort->GetAvailableProviders(&available_providers, &num_providers);
    if (status == NULL) 
    {
        RC2D_log(RC2D_LOG_INFO, "Available ONNX Execution Providers:");

        for (int i = 0; i < num_providers; ++i) 
        {
            RC2D_log(RC2D_LOG_INFO, "  - %s", available_providers[i]);
        }

        ort->ReleaseAvailableProviders(available_providers, num_providers);
    } 
    else 
    {
        RC2D_log(RC2D_LOG_WARN, "Failed to get available EPs");
    }

    // Tout s'est bien passé
    return true;
}

void rc2d_onnx_cleanup(void) 
{
    // Récupère l’API ONNX Runtime
    const OrtApi* ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);

    // Libère les options de session si elles ont été allouées
    if (g_session_options) {
        ort->ReleaseSessionOptions(g_session_options);
        g_session_options = NULL;
    }

    // Libère l’environnement ONNX Runtime si alloué
    if (g_ort_env) 
    {
        ort->ReleaseEnv(g_ort_env);
        g_ort_env = NULL;
    }
}

#endif // RC2D_ONNX_MODULE_ENABLED