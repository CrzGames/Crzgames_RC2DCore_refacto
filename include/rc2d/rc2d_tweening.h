#ifndef RC2D_TWEENING_H
#define RC2D_TWEENING_H

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

RC2D_TweenContext rc2d_tweening_createTweenContext(double duration, double startValue, double endValue, double (*tweenFunction)(double));
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
double rc2d_tweening_decay(const double x);

double rc2d_tweening_parabolicJump(const double x);

double rc2d_tweening_smoothStart(const double x); 
double rc2d_tweening_smoothStop(const double x);
double rc2d_tweening_smoothStep(const double x);

double rc2d_tweening_easeInOvershoot(const double x);
double rc2d_tweening_easeOutOvershoot(double x);
double rc2d_tweening_easeInOutOvershoot(double x);

double rc2d_tweening_easeInSpring(const double x);
double rc2d_tweening_easeOutSpring(const double x);
double rc2d_tweening_easeInOutSpring(const double x);

double rc2d_tweening_easeInCos(const double t);
double rc2d_tweening_easeOutCos(const double t);
double rc2d_tweening_easeInOutCos(const double t);

double rc2d_tweening_easeInSine(const double x);
double rc2d_tweening_easeOutSine(const double x);
double rc2d_tweening_easeInOutSine(const double x);

double rc2d_tweening_easeInCubic(const double x);
double rc2d_tweening_easeOutCubic(const double x);
double rc2d_tweening_easeInOutCubic(const double x);

double rc2d_tweening_easeInElastic(const double x);
double rc2d_tweening_easeOutElastic(const double x);
double rc2d_tweening_easeInOutElastic(const double x);

double rc2d_tweening_easeInQuad(const double x);
double rc2d_tweening_easeOutQuad(const double x);
double rc2d_tweening_easeInOutQuad(const double x);

double rc2d_tweening_easeInQuart(const double x);
double rc2d_tweening_easeOutQuart(const double x);
double rc2d_tweening_easeInOutQuart(const double x);

double rc2d_tweening_easeInQuint(const double x);
double rc2d_tweening_easeOutQuint(const double x);
double rc2d_tweening_easeInOutQuint(const double x);

double rc2d_tweening_easeInCirc(const double x);
double rc2d_tweening_easeOutCirc(const double x);
double rc2d_tweening_easeInOutCirc(const double x);

double rc2d_tweening_easeInExpo(const double x);
double rc2d_tweening_easeOutExpo(const double x);
double rc2d_tweening_easeInOutExpo(const double x);

double rc2d_tweening_easeInBack(const double x);
double rc2d_tweening_easeOutBack(const double x);
double rc2d_tweening_easeInOutBack(const double x);

double rc2d_tweening_easeInBounce(const double x);
double rc2d_tweening_easeOutBounce(double x);
double rc2d_tweening_easeInOutBounce(const double x);

#ifdef __cplusplus
}
#endif

#endif // RC2D_TWEENING_H