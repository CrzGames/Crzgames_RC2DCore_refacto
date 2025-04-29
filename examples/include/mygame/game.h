#ifndef GAME_H
#define GAME_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

void rc2d_unload(void);
void rc2d_load(void);
void rc2d_update(double dt);
void rc2d_draw(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // GAME_H