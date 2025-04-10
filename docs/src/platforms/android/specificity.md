# Spécificités Android

## Cycle de vie - Quitter l'application proprement

::: warning IMPORTANT
N'appelez **pas** `exit()` car cela arrête gravement l'activité Java.
:::

Il y a deux manières légitimes de quitter l'application :
1. **Via le retour de la fonction `main(argc, char* argv[])`** : Le côté Java mettra automatiquement fin à l'activité en appelant `Activity.finish()`.
2. **Le système d'exploitation Android peut décider de mettre fin à votre application** en appelant `onDestroy()`. Votre application recevra un événement `SDL_QUIT` que vous pourrez gérer pour sauvegarder des éléments ou autres et quitter proprement.

<br /><br />


## Événements Android

### 🖐️ Gestion du tactile

- `SDL_FINGERDOWN`: reçu lorsqu'un doigt est posé sur l'écran.
- `SDL_FINGERMOTION`: reçu lorsque ce doigt bouge sur l'écran.
- `SDL_FINGERUP`: reçu lorsque le doigt quitte l'écran.

### Informations IMPORTANTES 🛈

La gestion des événements tactiles est assez similaire à celle de la souris qui comprend trois événements équivalents (`SDL_MOUSEBUTTONDOWN`, `SDL_MOUSEMOTION` et `SDL_MOUSEBUTTONUP`), avec une subtilité notable.

Les événements de la souris sont donnés avec des coordonnées ENTIERES correspondant aux nombres de pixels parcourus depuis l'origine en haut à gauche de la fenêtre. Si je clique au milieu de fenêtre HD 1920x1080, alors mon programme reçoit un événement ev de type SDL_MOUSEBUTTONDOWN avec les valeurs ev.button.x == 960 et ev.button.y == 540.

Les événements tactiles donnent eux des coordonnées FLOTTANTES correspondant à la FRACTION de la fenêtre parcoure depuis l'origine en haut à gauche. Ainsi, si je clique au milieu de ma fenêtre, mon programme reçoit un événement ev de type SDL_FINGERDOWN avec les valeurs ev.button.x == 0.5 et ev.button.y == 0.5. 
Il suffit donc de MULTIPLIER ces valeurs respectivement par la LARGEUR et la HAUTEUR de votre fenêtre pour retomber sur les mêmes valeurs qu'avec un clic de souris. 

### Autres Événements Android 📱

- **Bouton "Retour"**: Peut être géré comme un événement `SDL_KEYDOWN/UP`, avec le keycode `SDLK_AC_BACK` ou via `ANDROID_BACK`.
- `SDL_APP_WILLENTERBACKGROUND`: Généré lorsque l'application est sur le point de passer en arrière-plan, permettant par exemple de mettre le jeu en pause.
- `SDL_APP_DIDENTERBACKGROUND`: Généré lorsque l'application est effectivement passée en arrière-plan.
- `SDL_APP_WILLENTERFOREGROUND` et `SDL_APP_DIDENTERFOREGROUND`: Pour vous prévenir que l'application va revenir ou est revenue au premier plan, utile pour reprendre le jeu.
- `SDL_APP_TERMINATING`: Prévient que l'application s'arrête, permettant de sauvegarder ce qu'il faut et quitter proprement l'application.

<br /><br />


## 🔍 Divers

1. **Configuration des attributs GL**: Pour certains appareils, il peut être bénéfique de définir explicitement les attributs GL avant de créer une fenêtre. Exemple :
   ```cpp
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
2. **Événements souris/tactile**: SDL peut générer des événements de souris synthétiques pour les appareils tactiles et vice-versa. Pour gérer ce comportement, consultez SDL_hints.h : SDL_HINT_TOUCH_MOUSE_EVENTS, SDL_HINT_MOUSE_TOUCH_EVENTS.
