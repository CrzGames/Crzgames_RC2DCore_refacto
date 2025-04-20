#ifndef RC2D_TWEENING_H
#define RC2D_TWEENING_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant l'état d'une animation. 
 * Elle contient toutes les informations nécessaires pour calculer l'interpolation des valeurs au cours du temps,
 * en fonction d'une fonction de tweening spécifiée.
 *
 * @param elapsedTime Le temps écoulé depuis le début de l'animation. Cela permet de suivre la progression
 *                    de l'animation à travers le temps.
 * @param duration    La durée totale de l'animation, qui est la période sur laquelle l'animation doit se dérouler
 *                    de sa valeur de départ à sa valeur de fin.
 * @param startValue  La valeur de départ de l'animation. C'est la valeur initiale à partir de laquelle l'animation commence.
 * @param endValue    La valeur de fin de l'animation. C'est la valeur cible que l'animation vise à atteindre à la fin
 *                    de la période définie par 'duration'.
 */
typedef struct RC2D_TweenContext {
    double elapsedTime;
    double duration;
    double startValue;
    double endValue;
    double (*tweenFunction)(double);
} RC2D_TweenContext; 

/**
 * Crée et initialise une nouvelle instance de RC2D_AnimationState.
 * Cette fonction configure l'état d'animation avec des valeurs spécifiques pour la durée, les valeurs de départ et de fin,
 * ainsi qu'avec la fonction de tweening qui sera utilisée pour calculer l'interpolation des valeurs au cours de l'animation.
 *
 * @param duration       La durée totale de l'animation, exprimée en unités de temps (par exemple, secondes).
 * @param startValue     La valeur initiale à partir de laquelle l'animation commence.
 * @param endValue       La valeur cible que l'animation vise à atteindre à la fin de la durée spécifiée.
 * @param tweenFunction  Un pointeur vers la fonction de tweening utilisée pour l'interpolation des valeurs.
 *                       Cette fonction doit prendre un seul paramètre de type double (la progression de l'animation)
 *                       et retourner un double (le facteur d'interpolation calculé).
 *
 * @return Un RC2D_TweenContext nouvellement créé et initialisé avec les valeurs spécifiées.
 */
RC2D_TweenContext rc2d_tweening_createTweenContext(double duration, double startValue, double endValue, double (*tweenFunction)(double));

/**
 * Calcule la valeur interpolée pour l'état d'animation actuel en fonction du temps écoulé.
 * Utilise la fonction de tweening spécifiée dans la structure d'état de l'animation pour
 * déterminer le facteur d'interpolation.
 *
 * @param animationState Un pointeur vers une instance de RC2D_TweenContext qui contient
 *                       les informations de l'animation, y compris la fonction de tweening.
 * @return La valeur interpolée entre 0 et 1, calculée en utilisant la fonction de tweening de l'état d'animation.
 */
double rc2d_tweening_interpolate(RC2D_TweenContext* tweenContext);

/*
Pour voir les courbes : https://easings.net/

Specific tweening functions:
- Decay : Simule une décroissance ou un amortissement naturel jusqu'à l'arrêt.
- Parabolic Jump : Imitation d'un mouvement parabolique, idéal pour simuler des sauts ou des arcs.
- Smooth Start : Une transition douce qui commence lentement et accélère progressivement.
- Smooth Stop : Une transition douce qui ralentit progressivement avant de s'arrêter.
- Smooth Step : Une transition douce qui commence lentement, accélère, puis ralentit avant de s'arrêter.

Standard tweening functions:
- Spring : Les animations qui simulent un comportement de ressort, avec des oscillations avant de se stabiliser à la valeur cible.
- Overshoot : Les animations qui dépassent la valeur cible puis reviennent, pour un effet dynamique.
- Cos : Les animations qui utilisent les propriétés du cosinus pour une transition en douceur.
- Sine : Les animations de démarrage et d'arrêt doux utilisant les propriétés du sinus.
- Cubic : Changements plus prononcés que pour les animations quadratiques, utiles pour des transitions plus dynamiques.
- Quint : Changements encore plus prononcés, pour des animations qui commencent lentement et finissent rapidement (ou l'inverse).
- Circ : Des transitions qui suivent une courbe circulaire, donnant une sensation de mouvement plus naturel.
- Elastic : Des animations qui dépassent la valeur cible avant de s'y installer, imitant un comportement élastique.
- Quad, Quart, et Expo : D'autres formes de changements progressifs avec différents niveaux d'accélération et de décélération.
- Back : Des animations qui reculent légèrement avant de progresser vers l'objectif, créant un effet de rebond.
- Bounce : Imitent un objet rebondissant contre une surface avant de s'arrêter.
*/

/**
 * \brief Applique un effet de décroissance exponentielle à l'animation, simulant un amortissement naturel.
 * 
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} Le facteur d'interpolation calculé pour la décroissance.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_tweening_decay(const double x);

/**
 * \brief Applique le mouvement parabolique d'un objet soumis à la gravité, idéal pour des animations imitant un saut.
 * 
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un effet de saut parabolique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_tweening_parabolicJump(const double x);

/**
 * \brief Applique une vitesse doucement et accélère progressivement vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un démarrage en douceur.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_smoothStop
 * \see rc2d_tweening_smoothStep
 */
double rc2d_tweening_smoothStart(const double x); 

/**
 * \brief Applique une vitesse normale et ralentit doucement vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un arrêt en douceur.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_smoothStart
 * \see rc2d_tweening_smoothStep
 */
double rc2d_tweening_smoothStop(const double x);

/**
 * \brief Applique Smooth Start et Smooth Stop pour un démarrage et un arrêt en douceur.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un démarrage et un arrêt en douceur.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_smoothStart
 * \see rc2d_tweening_smoothStop
 */
double rc2d_tweening_smoothStep(const double x);

/**
 * \brief Applique une interpolation avec dépassement pour un démarrage accéléré.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un effet de dépassement au démarrage.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutOvershoot
 * \see rc2d_tweening_easeInOutOvershoot
 */
double rc2d_tweening_easeInOvershoot(const double x);

/**
 * \brief Applique une interpolation avec dépassement pour une fin accélérée.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un effet de dépassement à la fin.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInOvershoot
 * \see rc2d_tweening_easeInOutOvershoot
 */
double rc2d_tweening_easeOutOvershoot(double x);

/**
 * \brief Applique une interpolation avec dépassement pour un démarrage et une fin accélérés.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur interpolée avec un effet de dépassement au démarrage et à la fin.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInOvershoot
 * \see rc2d_tweening_easeOutOvershoot
 */
double rc2d_tweening_easeInOutOvershoot(double x);

/**
 * \brief Applique une fonction d'accélération de type ressort pour l'entrée x, 
 * simulant un démarrage avec une tension initiale qui se relâche progressivement.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée, simulant un comportement de type ressort au début de l'animation.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutSpring
 * \see rc2d_tweening_easeInOutSpring
 */
double rc2d_tweening_easeInSpring(const double x);

/**
 * \brief Applique une fonction de décélération de type ressort à l'entrée x, 
 * simulant un relâchement avec un rebond vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée, simulant un comportement de type ressort à la fin de l'animation.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInSpring
 * \see rc2d_tweening_easeInOutSpring
 */
double rc2d_tweening_easeOutSpring(const double x);

/**
 * \brief Combine les comportements d'accélération et de décélération de type ressort, 
 * pour une transition en douceur qui simule un ressort tendu au début et à la fin, 
 * avec une phase de mouvement plus libre au milieu.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée, simulant un comportement de type ressort sur toute la durée de l'animation.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInSpring
 * \see rc2d_tweening_easeOutSpring
 */
double rc2d_tweening_easeInOutSpring(const double x);

/**
 * \brief Applique une interpolation cosinusique pour une accélération douce au début.
 * La progression démarre lentement et accélère graduellement jusqu'à mi-chemin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutCos
 * \see rc2d_tweening_easeInOutCos
 */
double rc2d_tweening_easeInCos(const double x);

/**
 * \brief Applique une interpolation cosinusique pour une décélération douce vers la fin.
 * La progression est rapide au début et ralentit graduellement pour arriver doucement à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour la décélération.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCos
 * \see rc2d_tweening_easeInOutCos
 */
double rc2d_tweening_easeOutCos(const double x);

/**
 * \brief Combine les effets d'accélération et de décélération cosinusiques, pour une transition en douceur du début à la fin.
 * La progression démarre et se termine lentement, avec une accélération plus rapide au milieu.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération et la décélération.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCos
 * \see rc2d_tweening_easeOutCos
 */
double rc2d_tweening_easeInOutCos(const double x);

/**
 * \brief Applique une fonction d'accélération sinusoidale à l'entrée x, où l'accélération augmente au début.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération sinusoidale.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutSine
 * \see rc2d_tweening_easeInOutSine
 */
double rc2d_tweening_easeInSine(const double x);

/**
 * \brief Applique une fonction de décélération sinusoidale à l'entrée x, où la décélération se produit à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour la décélération sinusoidale.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInSine
 * \see rc2d_tweening_easeInOutSine
 */
double rc2d_tweening_easeOutSine(const double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération sinusoidales, accélérant au début et décélérant à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération et la décélération sinusoidales.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInSine
 * \see rc2d_tweening_easeOutSine
 */
double rc2d_tweening_easeInOutSine(const double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération cubiques, accélérant au début et décélérant à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération et la décélération cubiques.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCubic
 * \see rc2d_tweening_easeOutCubic
 */
double rc2d_tweening_easeInCubic(const double x);

/**
 * \brief Applique une fonction de décélération cubique à l'entrée x, où la décélération est plus marquée à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour la décélération cubique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCubic
 * \see rc2d_tweening_easeInOutCubic
 */
double rc2d_tweening_easeOutCubic(const double x);

/**
 * \brief Applique une fonction d'accélération cubique à l'entrée x, où l'accélération augmente plus rapidement.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération cubique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCubic
 * \see rc2d_tweening_easeOutCubic
 */
double rc2d_tweening_easeInOutCubic(const double x);

/**
 * \brief Applique une fonction d'accélération élastique à l'entrée x, simulant une élasticité au début de l'animation.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération élastique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutElastic
 * \see rc2d_tweening_easeInOutElastic
 */
double rc2d_tweening_easeInElastic(const double x);

/**
 * \brief Applique une fonction de décélération élastique à l'entrée x, simulant une fin d'animation élastique et rebondissante.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour la décélération élastique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInElastic
 * \see rc2d_tweening_easeInOutElastic
 */
double rc2d_tweening_easeOutElastic(const double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération élastiques, offrant une transition élastique du début à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur d'interpolation calculée pour l'accélération et la décélération élastiques.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInElastic
 * \see rc2d_tweening_easeOutElastic
 */
double rc2d_tweening_easeInOutElastic(const double x);

/**
 * \brief Applique une accélération quadratique à l'entrée x, créant un effet d'accélération douce au début.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération quadratique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutQuad
 * \see rc2d_tweening_easeInOutQuad
 */
double rc2d_tweening_easeInQuad(const double x);

/**
 * \brief Applique une décélération quadratique à l'entrée x, créant un effet de décélération douce vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération quadratique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuad
 * \see rc2d_tweening_easeInOutQuad
 */
double rc2d_tweening_easeOutQuad(const double x);

/**
 * \brief Applique les effets d'accélération et de décélération quadratiques, pour un mouvement doux au début et à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuad
 * \see rc2d_tweening_easeOutQuad
 */
double rc2d_tweening_easeInOutQuad(const double x);

/**
 * \brief Applique une accélération quartique à l'entrée x, offrant un effet d'accélération plus marqué qu'avec une fonction cubique.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération quartique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutQuart
 * \see rc2d_tweening_easeInOutQuart
 */
double rc2d_tweening_easeInQuart(const double x);

/**
 * \brief Applique une décélération quartique à l'entrée x, offrant un effet de décélération plus intense vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération quartique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuart
 * \see rc2d_tweening_easeInOutQuart
 */
double rc2d_tweening_easeOutQuart(const double x);

/**
 * \brief Applique les effets d'accélération et de décélération quartiques, pour un mouvement très dynamique au début et à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuart
 * \see rc2d_tweening_easeOutQuart
 */
double rc2d_tweening_easeInOutQuart(const double x);

/**
 * \brief Applique une fonction d'accélération quintique à l'entrée x, où l'accélération augmente de manière encore plus marquée.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération quintique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutQuint
 * \see rc2d_tweening_easeInOutQuint
 */
double rc2d_tweening_easeInQuint(const double x);

/**
 * \brief Applique une fonction de décélération quintique à l'entrée x, offrant une décélération très marquée à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération quintique.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuint
 * \see rc2d_tweening_easeInOutQuint
 */
double rc2d_tweening_easeOutQuint(const double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération quintiques, pour une transition très dynamique de début à fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInQuint
 * \see rc2d_tweening_easeOutQuint
 */
double rc2d_tweening_easeInOutQuint(const double x);

/**
 * \brief Applique une fonction d'accélération circulaire à l'entrée x, débutant lentement et accélérant en suivant une courbe circulaire.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération circulaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutCirc
 * \see rc2d_tweening_easeInOutCirc
 */
double rc2d_tweening_easeInCirc(const double x);

/**
 * \brief Applique une fonction de décélération circulaire à l'entrée x, décélérant suivant une courbe circulaire vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération circulaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCirc
 * \see rc2d_tweening_easeInOutCirc
 */
double rc2d_tweening_easeOutCirc(const double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération circulaires, offrant une transition douce et 
 * arrondie du début à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInCirc
 * \see rc2d_tweening_easeOutCirc
 */
double rc2d_tweening_easeInOutCirc(const double x);

/**
 * \brief Applique une accélération exponentielle à l'entrée x, offrant un démarrage lent suivi d'une accélération rapide.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération exponentielle.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutExpo
 * \see rc2d_tweening_easeInOutExpo
 */
double rc2d_tweening_easeInExpo(const double x);

/**
 * \brief Applique une décélération exponentielle à l'entrée x, permettant un ralentissement rapide vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération exponentielle.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInExpo
 * \see rc2d_tweening_easeInOutExpo
 */
double rc2d_tweening_easeOutExpo(const double x);

/**
 * \brief Applique les effets d'accélération et de décélération exponentielles, pour une transition rapide au milieu.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInExpo
 * \see rc2d_tweening_easeOutExpo
 */
double rc2d_tweening_easeInOutExpo(const double x);

/**
 * \brief Applique une accélération "back" à l'entrée x, offrant un léger recul au début pour un effet dynamique.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération "back".
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutBack
 * \see rc2d_tweening_easeInOutBack
 */
double rc2d_tweening_easeInBack(const double x);

/**
 * \brief Applique une décélération "back" à l'entrée x, créant un effet de rebondissement vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération "back".
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInBack
 * \see rc2d_tweening_easeInOutBack
 */
double rc2d_tweening_easeOutBack(const double x);

/**
 * \brief Applique les effets d'accélération et de décélération "back", pour un mouvement dynamique au début et à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInBack
 * \see rc2d_tweening_easeOutBack
 */
double rc2d_tweening_easeInOutBack(const double x);

/**
 * \brief Applique une fonction d'accélération "bounce" à l'entrée x, simulant un effet de rebondissement au début.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de l'accélération "bounce".
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeOutBounce
 * \see rc2d_tweening_easeInOutBounce
 */
double rc2d_tweening_easeInBounce(const double x);

/**
 * \brief Applique une fonction de décélération "bounce" à l'entrée x, simulant un effet de rebondissement vers la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application de la décélération "bounce".
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInBounce
 * \see rc2d_tweening_easeInOutBounce
 */
double rc2d_tweening_easeOutBounce(double x);

/**
 * \brief Applique les fonctions d'accélération et de décélération "bounce", pour un effet de rebondissement du début à la fin.
 *
 * \param {double} x - La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * \return {double} La valeur après application des effets combinés.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_tweening_easeInBounce
 * \see rc2d_tweening_easeOutBounce
 */
double rc2d_tweening_easeInOutBounce(const double x);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_TWEENING_H