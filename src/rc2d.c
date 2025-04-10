#include "rc2d/RC2D.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <rcenet/enet.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#endif

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

// Window
SDL_Window* rc2d_sdl_window = NULL;
int rc2d_sdl_window_width = 800; // Default
int rc2d_sdl_window_height = 600; // Default

// Renderer
SDL_Renderer* rc2d_sdl_renderer = NULL;
int rc2d_sdl_renderer_width = 800; // Default
int rc2d_sdl_renderer_height = 600; // Default
#ifdef __EMSCRIPTEN__
double rc2d_devicePixelRatioCanvas = 1.0;
#endif

// Aspect Ratio and Renderer Scaling Mode (Letterbox, Overscan, None)
RC2D_RenderLogicalMode rc2d_renderLogicalSizeMode = RC2D_RENDER_LOGICAL_SIZE_MODE_NONE;
SDL_Texture* rc2d_renderTarget;
SDL_Texture* rc2d_letterBoxTexture;
double rc2d_offsetXOverscan = 0.0;
double rc2d_offsetYOverscan = 0.0;
double rc2d_aspectRatioScaleX = 0.0;
double rc2d_aspectRatioScaleY = 0.0;

// GameLoop
bool gameIsRunning = true;

// DeltaTime / FrameRates
double rc2d_deltaTime = 0;
Uint64 rc2d_lastFrameTime = 0;
int FPS = 60; // Default if currentDisplay.refresh_rate == 0 or not found

// Event GameLoop
SDL_Event rc2d_event;

// Callbacks API Engine
RC2D_Callbacks callbacksEngine = { 
    NULL, // rc2d_unload
    NULL, // rc2d_load
    NULL, // rc2d_update
    NULL, // rc2d_draw

    NULL, // rc2d_keypressed
    NULL, // rc2d_keyreleased

    NULL, // rc2d_mousemoved
    NULL, // rc2d_mousepressed
    NULL, // rc2d_mousereleased
    NULL, // rc2d_wheelmoved

    NULL, // rc2d_touchmoved
    NULL, // rc2d_touchpressed
    NULL, // rc2d_touchreleased

    NULL, // rc2d_gamepadpressed
    NULL, // rc2d_gamepadreleased
    NULL, // rc2d_joystickpressed
    NULL, // rc2d_joystickreleased
    NULL, // rc2d_joystickadded
    NULL, // rc2d_joystickremoved
    NULL, // rc2d_joystickaxis
    NULL, // rc2d_joystickhat
    NULL, // rc2d_gamepadaxis

    NULL, // rc2d_dropfile
    NULL, // rc2d_droptext
    NULL, // rc2d_dropbegin
    NULL,  // rc2d_dropcomplete

    NULL, // rc2d_windowresized
    NULL, // rc2d_windowmoved
    NULL, // rc2d_windowsizedchanged
    NULL, // rc2d_windowexposed
    NULL, // rc2d_windowminimized
    NULL, // rc2d_windowmaximized
    NULL, // rc2d_windowrestored
    NULL, // rc2d_mouseenteredwindow
    NULL, // rc2d_mouseleftwindow
    NULL, // rc2d_keyboardfocusgained
    NULL, // rc2d_keyboardfocuslost
    NULL, // rc2d_windowclosed
    NULL, // rc2d_windowtakefocus
    NULL, // rc2d_windowhittest

    NULL, // rc2d_overscanadjusted
};


void rc2d_drawTileTextureInRect(SDL_Rect targetRect) 
{
    int textureWidth, textureHeight;
    SDL_QueryTexture(rc2d_letterBoxTexture, NULL, NULL, &textureWidth, &textureHeight);

    // Calculer le nombre de tuiles nécessaires pour couvrir la zone, en ajustant pour la dernière tuile
    int tilesX = (targetRect.w + textureWidth - 1) / textureWidth; // Arrondi supérieur pour les tuiles en largeur
    int tilesY = (targetRect.h + textureHeight - 1) / textureHeight; // Arrondi supérieur pour les tuiles en hauteur

    for (int y = 0; y < tilesY; y++) 
    {
        for (int x = 0; x < tilesX; x++) 
        {
            SDL_Rect srcRect = {
                0, 
                0, 
                textureWidth, 
                textureHeight
            };
            SDL_Rect destRect = {
                targetRect.x + x * textureWidth,
                targetRect.y + y * textureHeight,
                textureWidth,
                textureHeight
            };

            // Ajuster la dernière tuile en largeur si nécessaire
            if (destRect.x + textureWidth > targetRect.x + targetRect.w) 
            {
                srcRect.w = (targetRect.x + targetRect.w) - destRect.x;
                destRect.w = srcRect.w;
            }

            // Ajuster la dernière tuile en hauteur si nécessaire
            if (destRect.y + textureHeight > targetRect.y + targetRect.h) 
            {
                srcRect.h = (targetRect.y + targetRect.h) - destRect.y;
                destRect.h = srcRect.h;
            }

            SDL_RenderCopy(rc2d_sdl_renderer, rc2d_letterBoxTexture, &srcRect, &destRect);
        }
    }
}

// get render logical size mode (letterbox, overscan, none)
RC2D_RenderLogicalMode rc2d_getRenderLogicalSizeMode(void) 
{
    return rc2d_renderLogicalSizeMode;
}

// 
void rc2d_getRendererVirtualSize(int* width, int* height) 
{
    if (width != NULL) 
    {
        *width = rc2d_sdl_renderer_width;
    }
    if (height != NULL) 
    {
        *height = rc2d_sdl_renderer_height;
    }
}

// Fonction pour récupérer les décalages pour l'overscan
void rc2d_getOverscanOffsets(double* offsetX, double* offsetY) {
    if (offsetX != NULL) {
        *offsetX = rc2d_offsetXOverscan;
    }
    if (offsetY != NULL) {
        *offsetY = rc2d_offsetYOverscan;
    }
}

// Fonction pour récupérer les facteurs de mise à l'échelle
void rc2d_getAspectRatioScales(double* scaleX, double* scaleY) {
    if (scaleX != NULL) {
        *scaleX = rc2d_aspectRatioScaleX;
    }
    if (scaleY != NULL) {
        *scaleY = rc2d_aspectRatioScaleY;
    }
}

void rc2d_manageAspectRatioFitting(SDL_Rect* destRect)
{
    // Obtenez les dimensions de sortie du rendu (resolution reelle), au lieu de la taille de la fenetre (qui comprends aussi le HIGH DPI)
    int outputWidth, outputHeight;
    rc2d_graphics_getRendererOutputSize(&outputWidth, &outputHeight);

    // Dimensions de la texture de rendu (resolution virtuelle du jeu)
    int textureWidth = rc2d_sdl_renderer_width;
    int textureHeight = rc2d_sdl_renderer_height;

    // Calculer le facteur de mise à l'echelle pour remplir l'ecran
    rc2d_aspectRatioScaleX = (double)outputWidth / (double)textureWidth;
    rc2d_aspectRatioScaleY = (double)outputHeight / (double)textureHeight;
    double scale = 1.0;

    // Utiliser fmin pour le letter boxing et fmax pour l'overscan (selon le besoin)
    if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_LETTERBOX)
    {
        scale = fmin(rc2d_aspectRatioScaleX, rc2d_aspectRatioScaleY);

        // Remplir via une texture les zone noir pour les zones de letterbox
        if (rc2d_letterBoxTexture != NULL)
        {
            // Identifiez les zones letterbox basées sur destRect et la résolution de sortie reel (outputWidth, outputHeight)
            SDL_Rect topLetterbox = {0, 0, outputWidth, destRect->y};
            SDL_Rect bottomLetterbox = {0, destRect->y + destRect->h, outputWidth, outputHeight - (destRect->y + destRect->h)};
            SDL_Rect leftLetterbox = {0, destRect->y, destRect->x, destRect->h};
            SDL_Rect rightLetterbox = {destRect->x + destRect->w, destRect->y, outputWidth - (destRect->x + destRect->w), destRect->h};

            // Pour chaque zone letterbox, vérifiez si elle existe (largeur et hauteur > 0) et dessinez la texture de tuiles
            if (topLetterbox.h > 0) rc2d_drawTileTextureInRect(topLetterbox);
            if (bottomLetterbox.h > 0) rc2d_drawTileTextureInRect(bottomLetterbox);
            if (leftLetterbox.w > 0) rc2d_drawTileTextureInRect(leftLetterbox);
            if (rightLetterbox.w > 0) rc2d_drawTileTextureInRect(rightLetterbox);
        }
    }
    else if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN)
    {
        scale = fmax(rc2d_aspectRatioScaleX, rc2d_aspectRatioScaleY);

        // Calculer le decalage necessaire pour centrer le contenu (notamment la GUI et les HUD) dans la fenetre window
        // Ceci n'ai pas utiliser la, c'est pour les jeux qui veulent utiliser l'overscan pour recentrer les elements comme HUD..etc
        rc2d_offsetXOverscan = (outputWidth - (textureWidth * scale)) / 2.0;
        rc2d_offsetYOverscan = (outputHeight - (textureHeight * scale)) / 2.0;
    }

    if (destRect == NULL) return;
    
    // Calculer les nouvelles dimensions de destination
    destRect->w = textureWidth * scale; // Largeur agrandie
    destRect->h = textureHeight * scale; // Hauteur agrandie
    destRect->x = (outputWidth - destRect->w) / 2; // Centrer horizontalement
    destRect->y = (outputHeight - destRect->h) / 2; // Centrer verticalement
}

void rc2d_prepareScaledRendering(void) 
{
    // Definir la texture comme cible de rendu
    SDL_SetRenderTarget(rc2d_sdl_renderer, rc2d_renderTarget);
}

void rc2d_finalizeScaledRendering(void) 
{
    // Retablir le rendu à l'ecran
    SDL_SetRenderTarget(rc2d_sdl_renderer, NULL);
    
    // Mise a l echelle + letter box or overscan
    SDL_Rect destRect;
    rc2d_manageAspectRatioFitting(&destRect);

    // Dessiner la texture de rendu agrandie à l'ecran
    SDL_RenderCopy(rc2d_sdl_renderer, rc2d_renderTarget, NULL, &destRect);
}

void updateFPSBasedOnCurrentMonitor() 
{
    // Récupére le monitor associé a la fenetre window
    int displayIndex = SDL_GetWindowDisplayIndex(rc2d_sdl_window);
    if (displayIndex < 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Could not get display index for window: %s", SDL_GetError());
        return;
    }

    // Obtient le mode d'affichage actuel pour ce moniteur
    SDL_DisplayMode currentDisplayMode;
    if (SDL_GetCurrentDisplayMode(displayIndex, &currentDisplayMode) != 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Could not get current display mode for display #%d: %s", displayIndex, SDL_GetError());
        return;
    }

    // Met à jour les FPS selon le taux de rafraîchissement du moniteur
    if (currentDisplayMode.refresh_rate != 0) 
    {
        FPS = currentDisplayMode.refresh_rate;
    } 
    else 
    {
        // Si aucun taux de rafraîchissement n'est trouvé, utilise une valeur par défaut
        FPS = 60;
    }
}

void rc2d_deltatimeframerates_start(void)
{
    // Capture le temps au debut de la frame actuelle
    Uint64 now = SDL_GetPerformanceCounter();

    // Calcule le delta time depuis la derniere frame
    rc2d_deltaTime = (double)(now - rc2d_lastFrameTime) / (double)SDL_GetPerformanceFrequency();
    
    // Met a jour 'lastFrameTime' pour la prochaine utilisation
    rc2d_lastFrameTime = now;

    // Set the current background color
    rc2d_graphics_setCurrentBackgroundColorInternal();
}

void rc2d_deltatimeframerates_end(void)
{
    // Capture le temps a la fin de la frame actuelle
    Uint64 frameEnd = SDL_GetPerformanceCounter();

    // Calcule le temps de la frame actuelle en millisecondes
    double frameTimeMs = (double)(frameEnd - rc2d_lastFrameTime) * 1000.0 / (double)SDL_GetPerformanceFrequency();

    // Attendre le temps necessaire pour atteindre le FPS cible
    if (frameTimeMs < 1000.0 / FPS) 
    {
        SDL_Delay((Uint32)((1000.0 / FPS) - frameTimeMs));
    }
}

void rc2d_processevent(void) 
{
    // PollEvent : Quit Program, Event KeyBoard, Event Mouse..
    while (rc2d_event_poll() != 0) {
        // Quit program
        if (rc2d_event.type == SDL_QUIT) 
        {
            rc2d_event_quit();
        }

        // Touch moved
        else if (rc2d_event.type == SDL_FINGERMOTION) 
        {
            if (callbacksEngine.rc2d_touchmoved != NULL) 
            {
                callbacksEngine.rc2d_touchmoved(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                                rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                                rc2d_event.tfinger.dx, rc2d_event.tfinger.dy);
            }

            // Mettre à jour l'état des pressions tactiles
            rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_FINGERMOTION, rc2d_event.tfinger.pressure, rc2d_event.tfinger.x, rc2d_event.tfinger.y);
        }

        // Touch pressed
        else if (rc2d_event.type == SDL_FINGERDOWN) 
        {
            if (callbacksEngine.rc2d_touchpressed != NULL) 
            {
                callbacksEngine.rc2d_touchpressed(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                                rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                                rc2d_event.tfinger.pressure);
            }

            // Mettre à jour l'état des pressions tactiles
            rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_FINGERDOWN, rc2d_event.tfinger.pressure, rc2d_event.tfinger.x, rc2d_event.tfinger.y);
        }

        // Touch released
        else if (rc2d_event.type == SDL_FINGERUP) 
        {
            if (callbacksEngine.rc2d_touchreleased != NULL) 
            {
                callbacksEngine.rc2d_touchreleased(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                                    rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                                    rc2d_event.tfinger.pressure);
            }

            // Mettre à jour l'état des pressions tactiles
            rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_FINGERUP, 0.0f, 0.0f, 0.0f);
        }

        // Window resized
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_RESIZED) 
        {
            rc2d_sdl_window_width = rc2d_event.window.data1; // Nouvelle largeur
            rc2d_sdl_window_height = rc2d_event.window.data2; // Nouvelle hauteur

            if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN && callbacksEngine.rc2d_overscanadjusted != NULL)
            {
                // Calcule les offsets de rendu pour l'overscan et appelle la callback pour ajuster les éléments de jeu
                rc2d_manageAspectRatioFitting(NULL);
                callbacksEngine.rc2d_overscanadjusted(rc2d_offsetXOverscan, rc2d_offsetYOverscan);
            }

            if (callbacksEngine.rc2d_windowresized != NULL) 
                callbacksEngine.rc2d_windowresized(rc2d_event.window.data1, rc2d_event.window.data2);
        }

        // Window moved
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_MOVED) 
        {
            // Check a chaque fois que la fenetre window bouge, pour pouvoir mettre a jour les FPS si besoins si la fenetre window est sur un autre moniteur.
            updateFPSBasedOnCurrentMonitor();

            if (callbacksEngine.rc2d_windowmoved != NULL) 
                callbacksEngine.rc2d_windowmoved(rc2d_event.window.data1, rc2d_event.window.data2);
        }

        // Window size changed
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
        {
            rc2d_sdl_window_width = rc2d_event.window.data1; // Nouvelle largeur
            rc2d_sdl_window_height = rc2d_event.window.data2; // Nouvelle hauteur

            if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN && callbacksEngine.rc2d_overscanadjusted != NULL)
            {
                // Calcule les offsets de rendu pour l'overscan et appelle la callback pour ajuster les éléments de jeu
                rc2d_manageAspectRatioFitting(NULL);
                callbacksEngine.rc2d_overscanadjusted(rc2d_offsetXOverscan, rc2d_offsetYOverscan);
            }

            if (callbacksEngine.rc2d_windowsizedchanged != NULL) 
                callbacksEngine.rc2d_windowsizedchanged(rc2d_event.window.data1, rc2d_event.window.data2);
        }

        // Window exposed
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_EXPOSED) 
        {
            if (callbacksEngine.rc2d_windowexposed != NULL) 
                callbacksEngine.rc2d_windowexposed();
        }

        // Window minimized
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_MINIMIZED) 
        {
            if (callbacksEngine.rc2d_windowminimized != NULL) 
                callbacksEngine.rc2d_windowminimized();
        }

        // Window maximized
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_MAXIMIZED) 
        {
            rc2d_sdl_window_width = rc2d_event.window.data1; // Nouvelle largeur
            rc2d_sdl_window_height = rc2d_event.window.data2; // Nouvelle hauteur
            
            if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN && callbacksEngine.rc2d_overscanadjusted != NULL)
            {
                // Calcule les offsets de rendu pour l'overscan et appelle la callback pour ajuster les éléments de jeu
                rc2d_manageAspectRatioFitting(NULL);
                callbacksEngine.rc2d_overscanadjusted(rc2d_offsetXOverscan, rc2d_offsetYOverscan);
            }

            if (callbacksEngine.rc2d_windowmaximized != NULL) 
                callbacksEngine.rc2d_windowmaximized();
        }

        // Window restored
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_RESTORED) 
        {
            rc2d_sdl_window_width = rc2d_event.window.data1; // Nouvelle largeur
            rc2d_sdl_window_height = rc2d_event.window.data2; // Nouvelle hauteur

            if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN && callbacksEngine.rc2d_overscanadjusted != NULL)
            {
                // Calcule les offsets de rendu pour l'overscan et appelle la callback pour ajuster les éléments de jeu
                rc2d_manageAspectRatioFitting(NULL);
                callbacksEngine.rc2d_overscanadjusted(rc2d_offsetXOverscan, rc2d_offsetYOverscan);
            }

            if (callbacksEngine.rc2d_windowrestored != NULL) 
                callbacksEngine.rc2d_windowrestored();
        }

        // Mouse entered window
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_ENTER) 
        {
            if (callbacksEngine.rc2d_mouseenteredwindow != NULL) 
                callbacksEngine.rc2d_mouseenteredwindow();
        }

        // Mouse left window
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_LEAVE) 
        {
            if (callbacksEngine.rc2d_mouseleftwindow != NULL) 
                callbacksEngine.rc2d_mouseleftwindow();
        }

        // Keyboard focus gained
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) 
        {
            if (callbacksEngine.rc2d_keyboardfocusgained != NULL) 
                callbacksEngine.rc2d_keyboardfocusgained();
        }

        // Keyboard focus lost
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) 
        {
            if (callbacksEngine.rc2d_keyboardfocuslost != NULL) 
                callbacksEngine.rc2d_keyboardfocuslost();
        }

        // Window closed
        else if (rc2d_event.window.event == SDL_WINDOWEVENT_CLOSE) 
        {
            if (callbacksEngine.rc2d_windowclosed != NULL) 
                callbacksEngine.rc2d_windowclosed();
        }
        
        // Mouse Moved
        else if (rc2d_event.type == SDL_MOUSEMOTION) 
        {
            if (callbacksEngine.rc2d_mousemoved != NULL) 
            {
                callbacksEngine.rc2d_mousemoved(rc2d_event.motion.x, rc2d_event.motion.y);
            }
        }

        // Mouse Wheel
        else if (rc2d_event.type == SDL_MOUSEWHEEL) 
        {
            if (callbacksEngine.rc2d_wheelmoved != NULL) 
            {
                const char* stateScroll = "";

                if (rc2d_event.wheel.y > 0) stateScroll = "up"; // scroll up
                else if (rc2d_event.wheel.y < 0) stateScroll = "down"; // scroll down
                else if (rc2d_event.wheel.x > 0) stateScroll = "right"; // scroll right
                else if (rc2d_event.wheel.x < 0) stateScroll = "left"; // scroll left

                callbacksEngine.rc2d_wheelmoved(stateScroll);
            }
        }

        // Mouse pressed
        else if (rc2d_event.type == SDL_MOUSEBUTTONDOWN) 
        {
            if (callbacksEngine.rc2d_mousepressed != NULL) 
            {
                const char* button = "";

                if (rc2d_event.button.button == SDL_BUTTON_LEFT) button = "left"; // Click gauche
                else if (rc2d_event.button.button == SDL_BUTTON_MIDDLE) button = "middle"; // Click roulette
                else if (rc2d_event.button.button == SDL_BUTTON_RIGHT) button = "right"; // Click droit

                callbacksEngine.rc2d_mousepressed(rc2d_event.button.x, rc2d_event.button.y, button, rc2d_event.button.clicks);
            }
        }

        // Mouse released
        else if (rc2d_event.type == SDL_MOUSEBUTTONUP) 
        {
            if (callbacksEngine.rc2d_mousereleased != NULL) 
            {
                const char* button = "";

                if (rc2d_event.button.button == SDL_BUTTON_LEFT) button = "left"; // Click gauche
                else if (rc2d_event.button.button == SDL_BUTTON_MIDDLE) button = "middle"; // Click roulette
                else if (rc2d_event.button.button == SDL_BUTTON_RIGHT) button = "right"; // Click droit

                callbacksEngine.rc2d_mousereleased(rc2d_event.button.x, rc2d_event.button.y, button, rc2d_event.button.clicks);
            }
        }

        // Keyboard pressed
        else if (rc2d_event.type == SDL_KEYDOWN)
        {
            if (callbacksEngine.rc2d_keypressed != NULL) 
            {
                const char* keyCurrent = SDL_GetKeyName(rc2d_event.key.keysym.sym);

                bool keyRepeat = rc2d_event.key.repeat != 0;

                callbacksEngine.rc2d_keypressed(keyCurrent, keyRepeat);
            }
        }

        // Keyboard released
        else if (rc2d_event.type == SDL_KEYUP)
        {
            if (callbacksEngine.rc2d_keyreleased != NULL) 
            {
                const char* keyCurrent = SDL_GetKeyName(rc2d_event.key.keysym.sym);
                callbacksEngine.rc2d_keyreleased(keyCurrent);
            }
        }

        // Gamepad axis moved
        else if (rc2d_event.type == SDL_CONTROLLERAXISMOTION) 
        {
            if (callbacksEngine.rc2d_gamepadaxis != NULL) 
            {
                /*
                En normalisant les valeurs d'axe de cette manière, vous assurez que les callbacks reçoivent une valeur flottante entre -1.0 et 1.0, 
                ce qui est généralement plus utile pour la logique de jeu, car cela indique la direction et l'intensité de l'entrée de manière cohérente, 
                indépendamment de la plateforme ou du contrôleur spécifique.
                */
                float valueNormalized = rc2d_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
                callbacksEngine.rc2d_gamepadaxis(rc2d_event.caxis.which, rc2d_event.caxis.axis, valueNormalized);
            }
        }

        // Gamepad Pressed
        else if (rc2d_event.type == SDL_CONTROLLERBUTTONDOWN) 
        {
            if (callbacksEngine.rc2d_gamepadpressed != NULL) 
            {
                callbacksEngine.rc2d_gamepadpressed(rc2d_event.cbutton.which, rc2d_event.cbutton.button);
            }
        }

        // Gamepad Released
        else if (rc2d_event.type == SDL_CONTROLLERBUTTONUP) 
        {
            if (callbacksEngine.rc2d_gamepadreleased != NULL) 
            {
                callbacksEngine.rc2d_gamepadreleased(rc2d_event.cbutton.which, rc2d_event.cbutton.button);
            }
        }

        // Joystick axis moved
        else if (rc2d_event.type == SDL_JOYAXISMOTION) 
        {
            if (callbacksEngine.rc2d_joystickaxis != NULL) 
            {
                /*
                En normalisant les valeurs d'axe de cette manière, vous assurez que les callbacks reçoivent une valeur flottante entre -1.0 et 1.0, 
                ce qui est généralement plus utile pour la logique de jeu, car cela indique la direction et l'intensité de l'entrée de manière cohérente, 
                indépendamment de la plateforme ou du contrôleur spécifique.
                */
                float valueNormalized = rc2d_event.jaxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
                callbacksEngine.rc2d_joystickaxis(rc2d_event.jaxis.which, rc2d_event.jaxis.axis, valueNormalized);
            }
        }

        // Joystick Pressed
        else if (rc2d_event.type == SDL_JOYBUTTONDOWN) 
        {
            if (callbacksEngine.rc2d_joystickpressed != NULL)
            {
                callbacksEngine.rc2d_joystickpressed(rc2d_event.jbutton.which, rc2d_event.jbutton.button);
            }
        }

        // Joystick Released
        else if (rc2d_event.type == SDL_JOYBUTTONUP)
         {
            if (callbacksEngine.rc2d_joystickreleased != NULL)
            {
                callbacksEngine.rc2d_joystickreleased(rc2d_event.jbutton.which, rc2d_event.jbutton.button);
            }
        }

        // Joystick added
        else if (rc2d_event.type == SDL_JOYDEVICEADDED) 
        {
            if (callbacksEngine.rc2d_joystickadded != NULL)
            {
                callbacksEngine.rc2d_joystickadded(rc2d_event.jdevice.which);
            }
        }

        // Joystick remove
        else if (rc2d_event.type == SDL_JOYDEVICEREMOVED) 
        {
            if (callbacksEngine.rc2d_joystickremoved != NULL) 
            {
                callbacksEngine.rc2d_joystickremoved(rc2d_event.jdevice.which);
            }
        }

        // Joystick hat moved
        else if (rc2d_event.type == SDL_JOYHATMOTION) 
        {
            if (callbacksEngine.rc2d_joystickhat != NULL) 
            {
                callbacksEngine.rc2d_joystickhat(rc2d_event.jhat.which, rc2d_event.jhat.hat, rc2d_event.jhat.value);
            }
        }

        // DROP FILE
        else if (rc2d_event.type == SDL_DROPFILE) 
        {
            if (callbacksEngine.rc2d_dropfile != NULL)
            {
                char* filedir = rc2d_event.drop.file;
                callbacksEngine.rc2d_dropfile(filedir);
                SDL_free(filedir);
            }
        }

        // SDL_DROPTEXT
        else if (rc2d_event.type == SDL_DROPTEXT) 
        {
            if (callbacksEngine.rc2d_droptext != NULL) 
            {
                char* filedir = rc2d_event.drop.file;
                callbacksEngine.rc2d_droptext(filedir);
                SDL_free(filedir);
            }
        }

        // SDL_DROPBEGIN
        else if (rc2d_event.type == SDL_DROPBEGIN) 
        {
            if (callbacksEngine.rc2d_dropbegin != NULL) 
            {
                char* filedir = rc2d_event.drop.file;
                callbacksEngine.rc2d_dropbegin(filedir);
                SDL_free(filedir);
            }
        }

        // SDL_DROPCOMPLETE
        else if (rc2d_event.type == SDL_DROPCOMPLETE) 
        {
            if (callbacksEngine.rc2d_dropcomplete != NULL) 
            {
                char* filedir = rc2d_event.drop.file;
                callbacksEngine.rc2d_dropcomplete(filedir);
                SDL_free(filedir);
            }
        }
    }
}

// Set Callbacks Engine by User
void rc2d_set_callbacks(RC2D_Callbacks* callbacksUser) 
{
    // Game loop callbacks
    if (callbacksUser->rc2d_unload != NULL) callbacksEngine.rc2d_unload = callbacksUser->rc2d_unload;
    if (callbacksUser->rc2d_load != NULL) callbacksEngine.rc2d_load = callbacksUser->rc2d_load;
    if (callbacksUser->rc2d_draw != NULL) callbacksEngine.rc2d_draw = callbacksUser->rc2d_draw;
    if (callbacksUser->rc2d_update != NULL) callbacksEngine.rc2d_update = callbacksUser->rc2d_update;

    // Keyboard callbacks
    if (callbacksUser->rc2d_keypressed != NULL) callbacksEngine.rc2d_keypressed = callbacksUser->rc2d_keypressed;
    if (callbacksUser->rc2d_keyreleased != NULL) callbacksEngine.rc2d_keyreleased = callbacksUser->rc2d_keyreleased;

    // Mouse callbacks
    if (callbacksUser->rc2d_mousemoved != NULL) callbacksEngine.rc2d_mousemoved = callbacksUser->rc2d_mousemoved;
    if (callbacksUser->rc2d_mousepressed != NULL) callbacksEngine.rc2d_mousepressed = callbacksUser->rc2d_mousepressed;
    if (callbacksUser->rc2d_mousereleased != NULL) callbacksEngine.rc2d_mousereleased = callbacksUser->rc2d_mousereleased;
    if (callbacksUser->rc2d_wheelmoved != NULL) callbacksEngine.rc2d_wheelmoved = callbacksUser->rc2d_wheelmoved;

    // Touch callbacks
    if (callbacksUser->rc2d_touchmoved != NULL) callbacksEngine.rc2d_touchmoved = callbacksUser->rc2d_touchmoved;
    if (callbacksUser->rc2d_touchpressed != NULL) callbacksEngine.rc2d_touchpressed = callbacksUser->rc2d_touchpressed;
    if (callbacksUser->rc2d_touchreleased != NULL) callbacksEngine.rc2d_touchreleased = callbacksUser->rc2d_touchreleased;

    // Joystick/Gamepad callbacks
    if (callbacksUser->rc2d_gamepadpressed != NULL) callbacksEngine.rc2d_gamepadpressed = callbacksUser->rc2d_gamepadpressed;
    if (callbacksUser->rc2d_gamepadreleased != NULL) callbacksEngine.rc2d_gamepadreleased = callbacksUser->rc2d_gamepadreleased;
    if (callbacksUser->rc2d_joystickpressed != NULL) callbacksEngine.rc2d_joystickpressed = callbacksUser->rc2d_joystickpressed;
    if (callbacksUser->rc2d_joystickreleased != NULL) callbacksEngine.rc2d_joystickreleased = callbacksUser->rc2d_joystickreleased;
    if (callbacksUser->rc2d_joystickadded != NULL) callbacksEngine.rc2d_joystickadded = callbacksUser->rc2d_joystickadded;
    if (callbacksUser->rc2d_joystickremoved != NULL) callbacksEngine.rc2d_joystickremoved = callbacksUser->rc2d_joystickremoved;
    if (callbacksUser->rc2d_joystickaxis != NULL) callbacksEngine.rc2d_joystickaxis = callbacksUser->rc2d_joystickaxis;
    if (callbacksUser->rc2d_joystickhat != NULL) callbacksEngine.rc2d_joystickhat = callbacksUser->rc2d_joystickhat;
    if (callbacksUser->rc2d_gamepadaxis != NULL) callbacksEngine.rc2d_gamepadaxis = callbacksUser->rc2d_gamepadaxis;

    // File drop callbacks
    if (callbacksUser->rc2d_dropfile != NULL) callbacksEngine.rc2d_dropfile = callbacksUser->rc2d_dropfile;
    if (callbacksUser->rc2d_droptext != NULL) callbacksEngine.rc2d_droptext = callbacksUser->rc2d_droptext;
    if (callbacksUser->rc2d_dropbegin != NULL) callbacksEngine.rc2d_dropbegin = callbacksUser->rc2d_dropbegin;
    if (callbacksUser->rc2d_dropcomplete != NULL) callbacksEngine.rc2d_dropcomplete = callbacksUser->rc2d_dropcomplete;

    // Window callbacks
    if (callbacksUser->rc2d_windowexposed != NULL) callbacksEngine.rc2d_windowexposed = callbacksUser->rc2d_windowexposed;
    if (callbacksUser->rc2d_windowmoved != NULL) callbacksEngine.rc2d_windowmoved = callbacksUser->rc2d_windowmoved;
    if (callbacksUser->rc2d_windowsizedchanged != NULL) callbacksEngine.rc2d_windowsizedchanged = callbacksUser->rc2d_windowsizedchanged;
    if (callbacksUser->rc2d_windowminimized != NULL) callbacksEngine.rc2d_windowminimized = callbacksUser->rc2d_windowminimized;
    if (callbacksUser->rc2d_windowmaximized != NULL) callbacksEngine.rc2d_windowmaximized = callbacksUser->rc2d_windowmaximized;
    if (callbacksUser->rc2d_windowrestored != NULL) callbacksEngine.rc2d_windowrestored = callbacksUser->rc2d_windowrestored;
    if (callbacksUser->rc2d_mouseenteredwindow != NULL) callbacksEngine.rc2d_mouseenteredwindow = callbacksUser->rc2d_mouseenteredwindow;
    if (callbacksUser->rc2d_mouseleftwindow != NULL) callbacksEngine.rc2d_mouseleftwindow = callbacksUser->rc2d_mouseleftwindow;
    if (callbacksUser->rc2d_keyboardfocusgained != NULL) callbacksEngine.rc2d_keyboardfocusgained = callbacksUser->rc2d_keyboardfocusgained;
    if (callbacksUser->rc2d_keyboardfocuslost != NULL) callbacksEngine.rc2d_keyboardfocuslost = callbacksUser->rc2d_keyboardfocuslost;
    if (callbacksUser->rc2d_windowclosed != NULL) callbacksEngine.rc2d_windowclosed = callbacksUser->rc2d_windowclosed;
    if (callbacksUser->rc2d_windowtakefocus != NULL) callbacksEngine.rc2d_windowtakefocus = callbacksUser->rc2d_windowtakefocus;
    if (callbacksUser->rc2d_windowhittest != NULL) callbacksEngine.rc2d_windowhittest = callbacksUser->rc2d_windowhittest;
    if (callbacksUser->rc2d_windowresized != NULL) callbacksEngine.rc2d_windowresized = callbacksUser->rc2d_windowresized;

    // Overscan adjusted
    if (callbacksUser->rc2d_overscanadjusted != NULL) callbacksEngine.rc2d_overscanadjusted = callbacksUser->rc2d_overscanadjusted;
}

int rc2d_engine(void)
{
	// Lib SDL2 Initialize
#ifdef __EMSCRIPTEN__
    // SDL_INIT_HAPTIC = n'ai pas pris en charge via Emscripten retour d'erreur dans la console JS : SDL2 could not init SDL2 Error : SDL not built with haptic (force feedback) support 
    Uint32 flagsSDL = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS | SDL_INIT_NOPARACHUTE; 
#else
    Uint32 flagsSDL = SDL_INIT_EVERYTHING;
#endif
	if (SDL_Init(flagsSDL) != 0)
	{
		rc2d_log(RC2D_LOG_CRITICAL, "SDL2 could not init SDL2 Error : %s \n", SDL_GetError());
		return -1;
	}
	

	// Lib SDL2_image Initialize
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (! (IMG_Init(imgFlags) & imgFlags))
	{
		rc2d_log(RC2D_LOG_CRITICAL, "Could not init SDL2_image : %s \n", IMG_GetError());
		return -1;
	}


	// Lib SDL2_mixer Initialize
    int audioFlags = MIX_INIT_OGG | MIX_INIT_MP3; // MIX_INIT_OGG for Nintendo Switch and other platforms
    if (Mix_Init(audioFlags) == -1) {
        rc2d_log(RC2D_LOG_CRITICAL, "Could not init SDL2_mixer : %s\n", Mix_GetError());
		return -1;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) {
        rc2d_log(RC2D_LOG_CRITICAL, "Could not init Mix_OpenAudio : %s\n", SDL_GetError());
        return -1;
    }


	// Lib SDL2_ttf Initialize
    if (!TTF_WasInit() && TTF_Init() == -1) 
    {
		rc2d_log(RC2D_LOG_CRITICAL, "Could not init SDL_ttf : %s \n", TTF_GetError());
		return -1;
    }


#ifndef __EMSCRIPTEN__
    // Lib ENet Initialize
	if (enet_initialize() != 0)
    {
        rc2d_log(RC2D_LOG_CRITICAL, "An error occurred while initializing ENet.\n");
        return -1;
    }

    // Lib OpenSSL Initialize
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
#endif


	// Check si il y a au moins un adpateur video connectee a l ordinateur.
	// Un adaptateur est en realite un port video auquel peut etre connecte un moniteur.
	if (SDL_GetNumVideoDisplays() < 1)
	{
		rc2d_log(RC2D_LOG_CRITICAL, "Aucun port video connecter : %s \n", SDL_GetError());
		return -1;
	}


	// Create window and set params
	Uint32 flagsWindow = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
	rc2d_sdl_window = SDL_CreateWindow("RC2D",
										SDL_WINDOWPOS_CENTERED,
										SDL_WINDOWPOS_CENTERED,
										rc2d_sdl_window_width,
										rc2d_sdl_window_height,
									    flagsWindow);
	if (rc2d_sdl_window == NULL)
	{
		rc2d_log(RC2D_LOG_CRITICAL, "Error create window : %s", SDL_GetError());
		return -1;
	}

#ifndef __EMSCRIPTEN__
    // Taille minimum de la fenetre window
    SDL_SetWindowMinimumSize(rc2d_sdl_window, 800, 600);
#endif


    // Recupere les donnees du moniteur qui contient la fenetre window pour regarder le nombre de HZ du moniteur et lui set les FPS.
	// Si les hz n'ont pas etait trouve, FPS par default : 60.
    updateFPSBasedOnCurrentMonitor();


    // Tentative de creation d'un renderer avec acceleration materielle (utilisera le GPU si disponible ou en secours juste endessous le cpu via SDL_RENDERER_SOFTWARE).
    Uint32 flagsRenderer = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE;
    rc2d_sdl_renderer = SDL_CreateRenderer(rc2d_sdl_window, -1, flagsRenderer);

    // Verifiez si la creation du renderer a echoue.
    if (rc2d_sdl_renderer == NULL) 
    {
        // La creation du renderer accelere a ehoue, essayez de creer un renderer logiciel a la place.
        flagsRenderer = SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE;
        rc2d_sdl_renderer = SDL_CreateRenderer(rc2d_sdl_window, -1, flagsRenderer);

        // Verifiez si la creation du renderer logiciel a egalement echoue.
        if (rc2d_sdl_renderer == NULL) 
        {
            rc2d_log(RC2D_LOG_CRITICAL, "Error creating software renderer: %s\n", SDL_GetError());
            return -1;
        }
    }


    // Mobile iOS and Android force orientation landscape
#if defined(__ANDROID__) || defined(__IPHONEOS__)
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif


	// Active dans SDL2 : SpriteBatch
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");


	// Active dans SDL2 : Le blending (alpha), on accepte qu'il melanges les couleurs.
	SDL_SetRenderDrawBlendMode(rc2d_sdl_renderer, SDL_BLENDMODE_BLEND);


    // Création d'une texture de rendu pour le rendu cible de rc2d_renderTarget pour gerer la mise a l'echelle de la resolution logique
    if (rc2d_renderLogicalSizeMode != RC2D_RENDER_LOGICAL_SIZE_MODE_NONE)
    {
        rc2d_renderTarget = SDL_CreateTexture(rc2d_sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rc2d_sdl_renderer_width, rc2d_sdl_renderer_height);

        if (rc2d_renderTarget == NULL)
        {
            rc2d_log(RC2D_LOG_CRITICAL, "Error create render target : %s", SDL_GetError());
            return -1;
        }
    }

    
    // Calcule les offsets de rendu pour l'overscan et appelle la callback pour ajuster les éléments de jeu (si necessaire)
    if (rc2d_renderLogicalSizeMode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN && callbacksEngine.rc2d_overscanadjusted != NULL)
    {
        rc2d_manageAspectRatioFitting(NULL);
        callbacksEngine.rc2d_overscanadjusted(rc2d_offsetXOverscan, rc2d_offsetYOverscan);
    }


    // Init Modules RC2D interne 
	//rc2d_keyboard_init();
    rc2d_timer_init();

	return 0;
}

void rc2d_gameloop(void) 
{
#ifdef __EMSCRIPTEN__
    if (!gameIsRunning) 
    {
        emscripten_cancel_main_loop();
    }
#endif

    rc2d_deltatimeframerates_start();
    rc2d_processevent();
    if (callbacksEngine.rc2d_update != NULL) callbacksEngine.rc2d_update(rc2d_deltaTime);
    rc2d_graphics_clear();
    if (rc2d_renderLogicalSizeMode != RC2D_RENDER_LOGICAL_SIZE_MODE_NONE) rc2d_prepareScaledRendering();
    if (callbacksEngine.rc2d_draw != NULL) callbacksEngine.rc2d_draw();
    if (rc2d_renderLogicalSizeMode != RC2D_RENDER_LOGICAL_SIZE_MODE_NONE) rc2d_finalizeScaledRendering();
    rc2d_graphics_present();
    rc2d_deltatimeframerates_end();
}

int rc2d_init(void)
{
	// Init GameEngine house
	if (rc2d_engine() != 0)
    {
		return -1;
    }

	return 0;
}


void rc2d_quit(void)
{
	// Destroy pointeur for prefPath and basePath
	rc2d_filesystem_quit();

#ifndef __EMSCRIPTEN__
    // Lib enet Deinitialize
    enet_deinitialize();

    // Lib OpenSSL Deinitialize
    ERR_free_strings();
    EVP_cleanup();
#endif

    // Lib SDL2_ttf Deinitialize
	TTF_Quit();

    // Lib SDL2_image Deinitialize    
	IMG_Quit();

    // Fermer le systeme audio
    Mix_CloseAudio();

    // Quitter SDL2_mixer
    Mix_Quit();

    // Module touch RC2D internal free
    rc2d_touch_freeTouchState();

    // Libérer la texture de rendu cible, si elle existe
    if (rc2d_renderTarget != NULL) {
        SDL_DestroyTexture(rc2d_renderTarget);
        rc2d_renderTarget = NULL;
    }

    // Libérer la texture utilisée pour les tuiles de letterbox, si elle existe
    if (rc2d_letterBoxTexture != NULL) {
        SDL_DestroyTexture(rc2d_letterBoxTexture);
        rc2d_letterBoxTexture = NULL;
    }
    
    // Destroy renderer
    if (rc2d_sdl_renderer != NULL)
    {
        SDL_DestroyRenderer(rc2d_sdl_renderer);
        rc2d_sdl_renderer = NULL;
    }

    // Destroy window
    if (rc2d_sdl_window != NULL)
    {
        SDL_DestroyWindow(rc2d_sdl_window);
        rc2d_sdl_window = NULL;
    }

    // Quit subsystems
	SDL_Quit();
}

int rc2d_run(RC2D_Callbacks* callbacksUser, int windowWidth, int windowHeight, int rendererWidth, int rendererHeight, RC2D_RenderLogicalMode renderLogicalMode, RC2D_Image* image)
{
    // Set Callbacks User to Engine
    if (callbacksUser != NULL)
    {
        rc2d_set_callbacks(callbacksUser);
    }

    // Set window size
    rc2d_sdl_window_width = windowWidth;
    rc2d_sdl_window_height = windowHeight;

    // Set renderer size
    rc2d_sdl_renderer_width = rendererWidth;
    rc2d_sdl_renderer_height = rendererHeight;
    
    // Set scaling mode
    rc2d_renderLogicalSizeMode = renderLogicalMode;

    // Set letterbox texture
    if (image != NULL && image->sdl_texture != NULL)
    {
        rc2d_letterBoxTexture = image->sdl_texture;
    }

    // Init GameEngine RC2D
	if(rc2d_init() != 0)
    {
		rc2d_quit();
        return -1;
    }

    // First call the callback to load the game loop (load textures, sounds, etc.)
    if (callbacksEngine.rc2d_load != NULL) 
    {
        callbacksEngine.rc2d_load();
    }

    // Pour rc2d_lastFrameTime, nous ne voulons pas que le dt de la premiere image inclue le temps pris par rc2d_load (donc ont le fait ici)
    rc2d_lastFrameTime = SDL_GetPerformanceCounter();

    // GameLoop - Main thread
#ifdef __EMSCRIPTEN__ // Version Emscripten de la boucle de jeu
    emscripten_set_main_loop(rc2d_gameloop, 0, 1);
#else // Version non-Emscripten de la boucle de jeu
    while (gameIsRunning) 
    {
        rc2d_gameloop();
    }
#endif

    // Last call the callback to unload the game loop (free memory, etc.)
    if (callbacksEngine.rc2d_unload != NULL)
    {
        callbacksEngine.rc2d_unload();
    }

    // FreeMemory GameEngine and Quit Program.
    rc2d_quit();

	return 0;
}