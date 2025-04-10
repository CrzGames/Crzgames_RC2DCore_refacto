#include "rc2d/rc2d_tweening.h"
#include "rc2d/rc2d_timer.h"
#include "rc2d/rc2d_logger.h"

#include <math.h>

// Define value of PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define value of Euler's number if not already defined
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

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
RC2D_TweenContext rc2d_tweening_createTweenContext(double duration, double startValue, double endValue, double (*tweenFunction)(double))
{
	if (tweenFunction == NULL) 
	{
		rc2d_log(RC2D_LOG_ERROR, "Impossible de créer un état d'animation avec une fonction de tweening null dans rc2d_tweening_createTweenContext.\n");
		return (RC2D_TweenContext){0};
	}

	RC2D_TweenContext tweenContext;
	tweenContext.duration = duration;
	tweenContext.startValue = startValue;
	tweenContext.endValue = endValue;
	tweenContext.tweenFunction = tweenFunction;
	tweenContext.elapsedTime = 0.0;

	return tweenContext;
}

/**
 * Calcule la valeur interpolée pour l'état d'animation actuel en fonction du temps écoulé.
 * Utilise la fonction de tweening spécifiée dans la structure d'état de l'animation pour
 * déterminer le facteur d'interpolation.
 *
 * @param animationState Un pointeur vers une instance de RC2D_TweenContext qui contient
 *                       les informations de l'animation, y compris la fonction de tweening.
 * @return La valeur interpolée entre 0 et 1, calculée en utilisant la fonction de tweening de l'état d'animation.
 */
double rc2d_tweening_interpolate(RC2D_TweenContext* tweenContext) 
{
    if (tweenContext == NULL || tweenContext->tweenFunction == NULL)
	{
		rc2d_log(RC2D_LOG_ERROR, "Impossible d'interpoler une animation avec un état d'animation null ou une fonction de tweening null dans rc2d_tweening_interpolate.\n");
		return 0.0;
	}

    // Mettre à jour le temps écoulé
	tweenContext->elapsedTime += rc2d_timer_getDelta();
    if (tweenContext->elapsedTime > tweenContext->duration) 
	{
        tweenContext->elapsedTime = tweenContext->duration;
    }

    // Calculer la progression de l'animation
    double progress = tweenContext->elapsedTime / tweenContext->duration;
    progress = fmax(0.0, fmin(progress, 1.0)); // Clamper la progression entre 0 et 1

    // Appliquer la fonction de tweening à la progression
    double interpolationFactor = tweenContext->tweenFunction(progress);

    // Calculer et retourner la valeur interpolée
    return tweenContext->startValue + (tweenContext->endValue - tweenContext->startValue) * interpolationFactor;
}

/**
 * Smooth Start : Commence doucement et accélère progressivement vers la fin.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un démarrage en douceur.
 */
double rc2d_tweening_smoothStart(const double x) 
{
    return pow(x, 2); // Exemple simple d'accélération douce
}

/**
 * Smooth Stop : Commence à vitesse normale et ralentit doucement vers la fin.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un arrêt en douceur.
 */
double rc2d_tweening_smoothStop(const double x) 
{
    return 1 - pow(1 - x, 2); // Inverse de Smooth Start pour un arrêt doux
}

/**
 * Smooth Step : Combine Smooth Start et Smooth Stop pour un démarrage et un arrêt en douceur.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un démarrage et un arrêt en douceur.
 */
double rc2d_tweening_smoothStep(const double x) 
{
    return x < 0.5 ? 2 * pow(x, 2) : 1 - 2 * pow(1 - x, 2); // Combinaison de Smooth Start et Smooth Stop
}

/**
 * Applique un effet de décroissance exponentielle à l'animation, simulant un amortissement naturel.
 * 
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * @return Le facteur d'interpolation calculé pour simuler un effet de décroissance.
 */
double rc2d_tweening_decay(double x) 
{
    return 1 - exp(-6 * x);
}

/**
 * Simule le mouvement parabolique d'un objet soumis à la gravité, idéal pour des animations imitant un saut.
 * 
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * @return Le facteur d'interpolation calculé pour simuler un saut parabolique.
 */
double rc2d_tweening_parabolicJump(const double x) 
{
    return -4 * x * (x - 1);
}

/**
 * Applique une interpolation avec dépassement pour un démarrage accéléré.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un effet de dépassement au démarrage.
 */
double rc2d_tweening_easeInOvershoot(const double x) 
{
    const double tension = 2.5;
    return x * x * ((tension + 1) * x - tension);
}

/**
 * Applique une interpolation avec dépassement pour une fin accélérée.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un effet de dépassement à la fin.
 */
double rc2d_tweening_easeOutOvershoot(double x) 
{
    const double tension = 2.5;
    x -= 1;

    return (x * x * ((tension + 1) * x + tension)) + 1;
}

/**
 * Applique une interpolation avec dépassement pour un démarrage et une fin accélérés.
 *
 * @param x La progression de l'animation, entre 0 (début) et 1 (fin).
 * @return La valeur interpolée avec un effet de dépassement au démarrage et à la fin.
 */
double rc2d_tweening_easeInOutOvershoot(double x) 
{
    const double tension = 2.5;

    if (x < 0.5) 
	{
        return 0.5 * (x * x * ((tension + 1) * 2 * x - tension));
    } 
	else 
	{
        x = x * 2 - 2;
        return 0.5 * (x * x * ((tension + 1) * x + tension)) + 2;
    }
}

/**
 * Applique une fonction d'accélération de type ressort pour l'entrée x, simulant un démarrage avec une tension initiale qui se relâche progressivement.
 *
 * @param x La progression actuelle de l'animation, entre 0 et 1.
 * @return La valeur d'interpolation calculée, simulant un comportement de type ressort au début de l'animation.
 */
double rc2d_tweening_easeInSpring(const double x) 
{
    return 1 - pow(M_E, -6 * x) * cos(12 * x);
}

/**
 * Applique une fonction de décélération de type ressort à l'entrée x, simulant un relâchement avec un rebond vers la fin.
 *
 * @param x La progression actuelle de l'animation, entre 0 et 1.
 * @return La valeur d'interpolation calculée, simulant un comportement de type ressort à la fin de l'animation.
 */
double rc2d_tweening_easeOutSpring(const double x) 
{
    return pow(M_E, -6 * (1 - x)) * cos(12 * (1 - x));
}

/**
 * Combine les comportements d'accélération et de décélération de type ressort, pour une transition en douceur qui simule un ressort tendu au début et à la fin, avec une phase de mouvement plus libre au milieu.
 *
 * @param x La progression actuelle de l'animation, entre 0 et 1.
 * @return La valeur d'interpolation calculée, simulant un comportement de type ressort du début à la fin de l'animation.
 */
double rc2d_tweening_easeInOutSpring(const double x) 
{
    if (x < 0.5) 
	{
        return (1 - pow(M_E, -12 * x) * cos(24 * x)) / 2;
    }

    return (1 + pow(M_E, -12 * (1 - x)) * cos(24 * (1 - x))) / 2;
}

/**
 * Applique une interpolation cosinusique pour une accélération douce au début.
 * La progression démarre lentement et accélère graduellement jusqu'à mi-chemin.
 *
 * @param t La valeur d'entrée, typiquement dans l'intervalle [0, 1], représentant la progression de l'animation.
 * @return Le facteur d'interpolation cosinusique calculé pour l'accélération.
 */
double rc2d_tweening_easeInCos(const double x) 
{
    return 1 - cos((1 - x) * M_PI / 2);
}
/**
 * Applique une interpolation cosinusique pour une décélération douce vers la fin.
 * La progression est rapide au début et ralentit graduellement pour arriver doucement à la fin.
 *
 * @param t La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return Le facteur d'interpolation cosinusique calculé pour la décélération.
 */
double rc2d_tweening_easeOutCos(const double x) 
{
    return cos(x * M_PI / 2);
}

/**
 * Combine les effets d'accélération et de décélération cosinusiques, pour une transition en douceur du début à la fin.
 * La progression démarre et se termine lentement, avec une accélération plus rapide au milieu.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return Le facteur d'interpolation cosinusique calculé pour une transition en douceur.
 */
double rc2d_tweening_easeInOutCos(const double x) 
{
    return (1 - cos(x * M_PI)) / 2;
}

/**
 * Applique une fonction d'accélération sinusoidale à l'entrée x, où l'accélération augmente au début.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération.
 */
double rc2d_tweening_easeInSine(const double x)
{
	return 1 - cos((x * M_PI) / 2);
}

/**
 * Applique une fonction de décélération sinusoidale à l'entrée x, où la décélération se produit à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération.
 */
double rc2d_tweening_easeOutSine(const double x)
{
	return sin((x * M_PI) / 2);
}

/**
 * Combine les fonctions d'accélération et de décélération sinusoidales, accélérant au début et décélérant à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération et de la décélération.
 */
double rc2d_tweening_easeInOutSine(const double x)
{
	return -(cos(M_PI * x) - 1) / 2;
}

/**
 * Applique une fonction d'accélération cubique à l'entrée x, où l'accélération augmente plus rapidement.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération cubique.
 */
double rc2d_tweening_easeInOutCubic(const double x)
{
	return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}

/**
 * Applique une fonction de décélération cubique à l'entrée x, où la décélération est plus marquée à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération cubique.
 */
double rc2d_tweening_easeOutCubic(const double x)
{
	return 1 - pow(1 - x, 3);
}

/**
 * Combine les fonctions d'accélération et de décélération cubiques, accélérant au début et décélérant à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des fonctions cubiques.
 */
double rc2d_tweening_easeInCubic(const double x)
{
	return x * x * x;
}

/**
 * Applique une fonction d'accélération quintique à l'entrée x, où l'accélération augmente de manière encore plus marquée.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération quintique.
 */
double rc2d_tweening_easeInQuint(const double x)
{
	return x * x * x * x * x;
}

/**
 * Applique une fonction de décélération quintique à l'entrée x, offrant une décélération très marquée à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération quintique.
 */
double rc2d_tweening_easeOutQuint(const double x)
{
	return 1 - pow(1 - x, 5);
}

/**
 * Combine les fonctions d'accélération et de décélération quintiques, pour une transition très dynamique de début à fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des fonctions quintiques.
 */
double rc2d_tweening_easeInOutQuint(const double x)
{
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
}

/**
 * Applique une fonction d'accélération circulaire à l'entrée x, débutant lentement et accélérant en suivant une courbe circulaire.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération circulaire.
 */
double rc2d_tweening_easeInCirc(const double x)
{
	return 1 - sqrt(1 - pow(x, 2));
}

/**
 * Applique une fonction de décélération circulaire à l'entrée x, décélérant suivant une courbe circulaire vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération circulaire.
 */
double rc2d_tweening_easeOutCirc(const double x)
{
	return sqrt(1 - pow(x - 1, 2));
}

/**
 * Combine les fonctions d'accélération et de décélération circulaires, offrant une transition douce et arrondie du début à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des fonctions circulaires.
 */
double rc2d_tweening_easeInOutCirc(const double x)
{
	return x < 0.5
		? (1 - sqrt(1 - pow(2 * x, 2))) / 2
		: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
}

/**
 * Applique une fonction d'accélération élastique à l'entrée x, simulant une élasticité au début de l'animation.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération élastique.
 */
double rc2d_tweening_easeInElastic(const double x)
{
	const double c4 = (2 * M_PI) / 3;

	return x == 0
		? 0
		: x == 1
		? 1
		: -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
}

/**
 * Applique une fonction de décélération élastique à l'entrée x, simulant une fin d'animation élastique et rebondissante.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération élastique.
 */
double rc2d_tweening_easeOutElastic(const double x)
{
	const double c4 = (2 * M_PI) / 3;

	return x == 0
		? 0
		: x == 1
		? 1
		: pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
}

/**
 * Combine les fonctions d'accélération et de décélération élastiques, offrant une transition élastique du début à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des fonctions élastiques.
 */
double rc2d_tweening_easeInOutElastic(const double x)
{
	const double c5 = (2 * M_PI) / 4.5;

	return x == 0
		? 0
		: x == 1
		? 1
		: x < 0.5
		? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
		: (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
}

/**
 * Applique une accélération quadratique à l'entrée x, créant un effet d'accélération douce au début.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération quadratique.
 */
double rc2d_tweening_easeInQuad(const double x)
{
	return x * x;
}

/**
 * Applique une décélération quadratique à l'entrée x, créant un effet de décélération douce vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération quadratique.
 */
double rc2d_tweening_easeOutQuad(const double x)
{
	return 1 - (1 - x) * (1 - x);
}

/**
 * Combine les effets d'accélération et de décélération quadratiques, pour un mouvement doux au début et à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des effets combinés.
 */
double rc2d_tweening_easeInOutQuad(const double x)
{
	return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}

/**
 * Applique une accélération quartique à l'entrée x, offrant un effet d'accélération plus marqué qu'avec une fonction cubique.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération quartique.
 */
double rc2d_tweening_easeInQuart(const double x)
{
	return x * x * x * x;
}

/**
 * Applique une décélération quartique à l'entrée x, offrant un effet de décélération plus intense vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération quartique.
 */
double rc2d_tweening_easeOutQuart(const double x)
{
	return 1 - pow(1 - x, 4);
}

/**
 * Combine les effets d'accélération et de décélération quartiques, pour un mouvement très dynamique au début et à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des effets combinés.
 */
double rc2d_tweening_easeInOutQuart(const double x)
{
	return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}

/**
 * Applique une accélération exponentielle à l'entrée x, offrant un démarrage lent suivi d'une accélération rapide.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération exponentielle.
 */
double rc2d_tweening_easeInExpo(const double x)
{
	return x == 0 ? 0 : pow(2, 10 * x - 10);
}

/**
 * Applique une décélération exponentielle à l'entrée x, permettant un ralentissement rapide vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération exponentielle.
 */
double rc2d_tweening_easeOutExpo(const double x)
{
	return x == 1 ? 1 : 1 - pow(2, -10 * x);
}

/**
 * Combine les effets d'accélération et de décélération exponentielles, pour une transition rapide au milieu.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des effets combinés.
 */
double rc2d_tweening_easeInOutExpo(const double x)
{
	return x == 0
		? 0
		: x == 1
		? 1
		: x < 0.5 ? pow(2, 20 * x - 10) / 2
		: (2 - pow(2, -20 * x + 10)) / 2;
}

/**
 * Applique une accélération "back" à l'entrée x, offrant un léger recul au début pour un effet dynamique.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération "back".
 */
double rc2d_tweening_easeInBack(const double x)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return c3 * x * x * x - c1 * x * x;
}

/**
 * Applique une décélération "back" à l'entrée x, créant un effet de rebondissement vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération "back".
 */
double rc2d_tweening_easeOutBack(const double x)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

/**
 * Combine les effets d'accélération et de décélération "back", pour un mouvement dynamique au début et à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des effets combinés.
 */
double rc2d_tweening_easeInOutBack(const double x)
{
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

/**
 * Applique une fonction d'accélération "bounce" à l'entrée x, simulant un effet de rebondissement au début.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de l'accélération "bounce".
 */
double rc2d_tweening_easeInBounce(const double x)
{
	return 1 - rc2d_tweening_easeOutBounce(1 - x);
}

/**
 * Applique une fonction de décélération "bounce" à l'entrée x, simulant un effet de rebondissement vers la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application de la décélération "bounce".
 */
double rc2d_tweening_easeOutBounce(double x)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;

	if (x < 1 / d1) {
		return n1 * x * x;
	}
	else if (x < 2 / d1) {
		return n1 * (x -= 1.5 / d1) * x + 0.75;
	}
	else if (x < 2.5 / d1) {
		return n1 * (x -= 2.25 / d1) * x + 0.9375;
	}
	else {
		return n1 * (x -= 2.625 / d1) * x + 0.984375;
	}
}

/**
 * Combine les fonctions d'accélération et de décélération "bounce", pour un effet de rebondissement du début à la fin.
 *
 * @param x La valeur d'entrée, typiquement dans l'intervalle [0, 1].
 * @return La valeur après application des fonctions de rebondissement.
 */
double rc2d_tweening_easeInOutBounce(const double x)
{
	return x < 0.5
		? (1 - rc2d_tweening_easeOutBounce(1 - 2 * x)) / 2
		: (1 + rc2d_tweening_easeOutBounce(2 * x - 1)) / 2;
}