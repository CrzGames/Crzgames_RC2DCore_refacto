# Project iOS
## Retina / High-DPI et tailles de fenêtre : 
Les tailles de fenêtre et de mode d'affichage dans SDL sont en « coordonnées d'écran » (ou « points », dans la terminologie d'Apple) plutôt qu'en pixels. Sur iOS, cela signifie qu'une fenêtre créée sur un iPhone 6 aura une taille en coordonnées d'écran de 375 x 667, plutôt qu'une taille en pixels de 750 x 1334. Toutes les applications iOS sont censées dimensionner leur contenu en fonction des coordonnées/points d'écran. plutôt que des pixels, car cela permet à différents appareils iOS d'avoir des densités de pixels différentes (écrans Retina ou non Retina, etc.) sans que les applications s'en soucient trop. <br />

IMPORTANT : Par défaut, SDL n'utilisera pas toute la densité de pixels de l'écran sur les appareils compatibles Retina/haute résolution. Utilisez l'indicateur SDL_WINDOW_ALLOW_HIGHDPI lors de la création de votre fenêtre pour activer la prise en charge haute résolution. <br />

Lorsque la prise en charge des hautes résolutions est activée, SDL_GetWindowSize() et les tailles du mode d'affichage seront toujours en "coordonnées d'écran" plutôt qu'en pixels, mais la fenêtre aura une densité de pixels beaucoup plus grande lorsque l'appareil le prend en charge, et SDL_GL_GetDrawableSize() ou SDL_GetRendererOutputSize () (selon que l'OpenGL brut ou l'API SDL_Render est utilisé) peuvent être interrogées pour déterminer la taille en pixels du framebuffer d'écran pouvant être dessiné. <br />

Certaines fonctions OpenGL ES telles que glViewport attendent des tailles en pixels plutôt que des tailles en coordonnées d'écran. Lors du rendu 2D avec OpenGL ES, une matrice de projection orthographique utilisant la taille en coordonnées d'écran (SDL_GetWindowSize()) peut être utilisée afin d'afficher le contenu à la même échelle, peu importe si un appareil Retina est utilisé ou non.

<br /><br />


## Event de l'application : 
Sur iOS, l'application traverse un cycle de vie fixe et vous recevrez des notifications de changements d'état via les événements de l'application. Lorsque ces événements sont livrés, vous devez les gérer dans un rappel d'événement, car le système d'exploitation peut ne vous accorder aucun temps de traitement après la livraison des événements. <br />
Exemple :
```bash
int HandleAppEvents(void *userdata, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_APP_TERMINATING:
        /* Terminate the app.
           Shut everything down before returning from this function.
        */
        return 0;
    case SDL_APP_LOWMEMORY:
        /* You will get this when your app is paused and iOS wants more memory.
           Release as much memory as possible.
        */
        return 0;
    case SDL_APP_WILLENTERBACKGROUND:
        /* Prepare your app to go into the background.  Stop loops, etc.
           This gets called when the user hits the home button, or gets a call.
        */
        return 0;
    case SDL_APP_DIDENTERBACKGROUND:
        /* This will get called if the user accepted whatever sent your app to the background.
           If the user got a phone call and canceled it, you'll instead get an SDL_APP_DIDENTERFOREGROUND event and restart your loops.
           When you get this, you have 5 seconds to save all your state or the app will be terminated.
           Your app is NOT active at this point.
        */
        return 0;
    case SDL_APP_WILLENTERFOREGROUND:
        /* This call happens when your app is coming back to the foreground.
           Restore all your state here.
        */
        return 0;
    case SDL_APP_DIDENTERFOREGROUND:
        /* Restart your loops here.
           Your app is interactive and getting CPU again.
        */
        return 0;
    default:
        /* No special processing, add it to the event queue */
        return 1;
    }
}

int main(int argc, char *argv[])
{
    SDL_SetEventFilter(HandleAppEvents, NULL);

    ... run your main loop

    return 0;
}
```

<br /><br />


## Joystick 
SDL pour iPhone prend en charge l'interrogation de l'accéléromètre intégré en tant que joystick. Pour un exemple sur la façon de procéder, consultez accelerometer.c dans le répertoire demos.

La principale chose à noter lors de l'utilisation de l'accéléromètre avec SDL est que, alors que l'iPhone rapporte nativement l'accéléromètre sous forme de valeurs à virgule flottante en unités de force g, SDL_JoystickGetAxis() rapporte les valeurs du joystick sous forme d'entiers signés. Par conséquent, afin de convertir entre les deux, un certain serrage et une mise à l'échelle sont nécessaires de la part du pilote du joystick SDL de l'iPhone. Pour convertir les valeurs signalées par SDL_JoystickGetAxis() BACK en unités de force g, multipliez simplement les valeurs par SDL_IPHONE_MAX_GFORCE / 0x7FFF.

<br /><br />


## Clavier
void SDL_StartTextInput() -- active les événements de texte et révèle le clavier à l'écran. <br />
void SDL_StopTextInput() -- désactive les événements de texte et masque le clavier à l'écran. <br />
SDL_bool SDL_IsTextInputActive() -- renvoie si les événements de texte sont activés ou non (et le clavier à l'écran est visible)

<br /><br />


## Souris
iOS prend désormais en charge les souris Bluetooth sur iPad, mais par défaut, l'entrée de la souris sera tactile. <br />
Pour que SDL puisse voir les événements réels de la souris, vous devez définir la clé UIApplicationSupportsIndirectInputEvents sur true dans votre Info.plist.

<br /><br />


## Bluetooth
SDL_JOYSTICK_HIDAPI est désactivé par défaut. Il peut vous donner accès à beaucoup plus de contrôleurs de jeu, mais il nécessite l'autorisation de l'utilisateur avant que votre application puisse communiquer avec le matériel Bluetooth. Les contrôleurs de marque « Made For iOS » n'en ont pas besoin car nous n'avons pas besoin de leur parler directement avec le Bluetooth brut, donc de nombreuses applications peuvent s'en passer. <br />

Vous devrez créer un lien avec CoreBluetooth.framework et ajouter quelque chose comme ceci à votre Info.plist : <br />

NSBluetoothPeripheralUsageDescription MyApp souhaite rester connecté aux contrôleurs de jeu et aux manettes de jeu Bluetooth à proximité, même lorsque vous n'utilisez pas l'application.

<br /><br />


## OpenGL ES
Si votre application n'utilise pas le tampon de profondeur d'OpenGL, vous pouvez constater une amélioration significative des performances en définissant SDL_GL_DEPTH_SIZE sur 0.