#include <RC2D/RC2D_tweening.h>
#include <RC2D/RC2D_timer.h>
#include <RC2D/RC2D_logger.h>

#include <math.h>

// Define value of PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define value of Euler's number if not already defined
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

RC2D_TweenContext rc2d_tweening_createTweenContext(double duration, double startValue, double endValue, double (*tweenFunction)(double))
{
	// Vérifier si la durée est valide
	if (tweenFunction == NULL) 
	{
		RC2D_log(RC2D_LOG_ERROR, "Impossible de créer un état d'animation avec une fonction de tweening null dans rc2d_tweening_createTweenContext.\n");
		return (RC2D_TweenContext){0};
	}

	// Créer et initialiser le contexte de tweening avec les valeurs fournies
	RC2D_TweenContext tweenContext;
	tweenContext.duration = duration;
	tweenContext.startValue = startValue;
	tweenContext.endValue = endValue;
	tweenContext.tweenFunction = tweenFunction;
	tweenContext.elapsedTime = 0.0;

	// Renvoie le contexte de tweening
	return tweenContext;
}

double rc2d_tweening_interpolate(RC2D_TweenContext* tweenContext) 
{
	// Vérifier si le contexte de tweening est valide
    if (tweenContext == NULL || tweenContext->tweenFunction == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "Impossible d'interpoler une animation avec un état d'animation null ou une fonction de tweening null dans rc2d_tweening_interpolate.\n");
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

double rc2d_tweening_decay(double x) 
{
    return 1 - exp(-6 * x);
}

double rc2d_tweening_parabolicJump(const double x) 
{
    return -4 * x * (x - 1);
}

double rc2d_tweening_smoothStart(const double x) 
{
    return pow(x, 2);
}

double rc2d_tweening_smoothStop(const double x) 
{
    return 1 - pow(1 - x, 2);
}

double rc2d_tweening_smoothStep(const double x) 
{
    return x < 0.5 ? 2 * pow(x, 2) : 1 - 2 * pow(1 - x, 2);
}

double rc2d_tweening_easeInOvershoot(const double x) 
{
    const double tension = 2.5;
    return x * x * ((tension + 1) * x - tension);
}

double rc2d_tweening_easeOutOvershoot(double x) 
{
    const double tension = 2.5;
    x -= 1;

    return (x * x * ((tension + 1) * x + tension)) + 1;
}

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

double rc2d_tweening_easeInSpring(const double x) 
{
    return 1 - pow(M_E, -6 * x) * cos(12 * x);
}

double rc2d_tweening_easeOutSpring(const double x) 
{
    return pow(M_E, -6 * (1 - x)) * cos(12 * (1 - x));
}

double rc2d_tweening_easeInOutSpring(const double x) 
{
    if (x < 0.5) 
	{
        return (1 - pow(M_E, -12 * x) * cos(24 * x)) / 2;
    }

    return (1 + pow(M_E, -12 * (1 - x)) * cos(24 * (1 - x))) / 2;
}

double rc2d_tweening_easeInCos(const double x) 
{
    return 1 - cos((1 - x) * M_PI / 2);
}

double rc2d_tweening_easeOutCos(const double x) 
{
    return cos(x * M_PI / 2);
}

double rc2d_tweening_easeInOutCos(const double x) 
{
    return (1 - cos(x * M_PI)) / 2;
}

double rc2d_tweening_easeInSine(const double x)
{
	return 1 - cos((x * M_PI) / 2);
}

double rc2d_tweening_easeOutSine(const double x)
{
	return sin((x * M_PI) / 2);
}

double rc2d_tweening_easeInOutSine(const double x)
{
	return -(cos(M_PI * x) - 1) / 2;
}

double rc2d_tweening_easeInCubic(const double x)
{
	return x * x * x;
}

double rc2d_tweening_easeOutCubic(const double x)
{
	return 1 - pow(1 - x, 3);
}

double rc2d_tweening_easeInOutCubic(const double x)
{
	return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}

double rc2d_tweening_easeInElastic(const double x)
{
	const double c4 = (2 * M_PI) / 3;

	return x == 0
		? 0
		: x == 1
		? 1
		: -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
}

double rc2d_tweening_easeOutElastic(const double x)
{
	const double c4 = (2 * M_PI) / 3;

	return x == 0
		? 0
		: x == 1
		? 1
		: pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
}

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

double rc2d_tweening_easeInQuad(const double x)
{
	return x * x;
}

double rc2d_tweening_easeOutQuad(const double x)
{
	return 1 - (1 - x) * (1 - x);
}

double rc2d_tweening_easeInOutQuad(const double x)
{
	return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}

double rc2d_tweening_easeInQuart(const double x)
{
	return x * x * x * x;
}

double rc2d_tweening_easeOutQuart(const double x)
{
	return 1 - pow(1 - x, 4);
}

double rc2d_tweening_easeInOutQuart(const double x)
{
	return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}

double rc2d_tweening_easeInQuint(const double x)
{
	return x * x * x * x * x;
}

double rc2d_tweening_easeOutQuint(const double x)
{
	return 1 - pow(1 - x, 5);
}

double rc2d_tweening_easeInOutQuint(const double x)
{
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
}

double rc2d_tweening_easeInCirc(const double x)
{
	return 1 - sqrt(1 - pow(x, 2));
}

double rc2d_tweening_easeOutCirc(const double x)
{
	return sqrt(1 - pow(x - 1, 2));
}

double rc2d_tweening_easeInOutCirc(const double x)
{
	return x < 0.5
		? (1 - sqrt(1 - pow(2 * x, 2))) / 2
		: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
}

double rc2d_tweening_easeInExpo(const double x)
{
	return x == 0 ? 0 : pow(2, 10 * x - 10);
}

double rc2d_tweening_easeOutExpo(const double x)
{
	return x == 1 ? 1 : 1 - pow(2, -10 * x);
}

double rc2d_tweening_easeInOutExpo(const double x)
{
	return x == 0
		? 0
		: x == 1
		? 1
		: x < 0.5 ? pow(2, 20 * x - 10) / 2
		: (2 - pow(2, -20 * x + 10)) / 2;
}

double rc2d_tweening_easeInBack(const double x)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return c3 * x * x * x - c1 * x * x;
}

double rc2d_tweening_easeOutBack(const double x)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;

	return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

double rc2d_tweening_easeInOutBack(const double x)
{
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

double rc2d_tweening_easeInBounce(const double x)
{
	return 1 - rc2d_tweening_easeOutBounce(1 - x);
}

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

double rc2d_tweening_easeInOutBounce(const double x)
{
	return x < 0.5
		? (1 - rc2d_tweening_easeOutBounce(1 - 2 * x)) / 2
		: (1 + rc2d_tweening_easeOutBounce(2 * x - 1)) / 2;
}