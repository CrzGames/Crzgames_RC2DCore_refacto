#ifndef RC2D_TOUCH_H
#define RC2D_TOUCH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant l'état des pressions tactiles.
 * @typedef {object} RC2D_TouchState
 * @property {number[]} touches - Tableau des identifiants des pressions tactiles.
 * @property {number[]} pressures - Tableau des valeurs de pression pour chaque pression tactile.
 * @property {number[]} x - Tableau des coordonnées x pour chaque pression tactile.
 * @property {number[]} y - Tableau des coordonnées y pour chaque pression tactile.
 * @property {number} numTouches - Nombre total de pressions tactiles enregistrées.
 */
typedef struct RC2D_TouchState {
    int* touches;
    float* pressures;
    float* x;
    float* y;
    int numTouches;
} RC2D_TouchState;

void rc2d_touch_updateState(const int fingerId, const int eventType, const float pressure, const float x, const float y); 
void rc2d_touch_freeTouchState(void);

void rc2d_touch_getPosition(const int fingerId, float* x, float* y);
float rc2d_touch_getPressure(const int touchId);

int* rc2d_touch_getTouches(void);
void rc2d_touch_freeTouches(int* touches);

#ifdef __cplusplus
};
#endif

#endif // RC2D_TOUCH_H