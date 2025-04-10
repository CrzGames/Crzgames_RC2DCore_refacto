# Project Android
## Modification du projet Android de base fournie (quand c'est un nouveau jeu qu'ont commence)
1. Modifier le dossier/package pour la nouvelle application : app/src/main/java/com.crzgames.rc2d le remplacer par -> app/src/main/java/com.crzgames.*
2. Modifier dans le fichier 'MyGame.java' depuis 'app/src/main/java/com.crzgames.*' : il faudra modifier la ligne 1 'package com.crzgames.rc2d' par le nouveau nom du package java.
3. Dans le fichier app/src/main/java/AndroidManifest.xml à la ligne : 'android:name="com.crzgames.rc2d.MyGame"', modifier le nom du package par le nouveau (CONSERVER 'MyGame' IMPORTANT).
4. Dans le fichier app/src/build.gradle modifier la valeur du namespace, applicationId par le nouveau nom du package java : com.crzgames.* 
5. Dans le fichier app/src/build.gradle modifier la valeur "com.crzgames.rc2d.aar" par le nouveau nom du package java "com.crzgames.*.aar"
6. Personnaliser le nom de l'application depuis le fichier app/src/main/res/values/strings.xml modifier la valeur de "MyGame" avec le nom du jeu.
7. Personnalisation de l'icône de votre application : Dans le dossier app/src/main/res, modifier tout les fichiers "ic_launcher.png" par le fichier .png de votre logo pour le nouveau jeu dans les dossiers 'mipmap' (il existe plusieurs répertoires pour différentes tailles d'écran).
8. Modifier la CI (build-sign_deploy-playstore.yml) dans le step "Deploy AAB to Play Store", la propriété "packageName" de "com.crzgames.rc2d" à "com.crzgames.*"
9. Quand on ajoute une librairie ou on change le 'project-name' du CMakeLists.txt il faudra changer concernant la récupération des librairie .so/.a via le path : <br />
   Fichier : app/src/main/java/jorg/jlibsdl/japp/jSDLActivity.java <br />
   Ligne : 268 et 247
10. Il faudra modifier dans les projets android : app/src/main/java/SDLActivity.java, ligne : 60. Il faudra modifier SDL_MAJOR_VERSION, SDL_MINOR_VERSION et SDL_MICRO_VERSION par rapport à la version qu'ont n'as récupérer
    
<br /><br />


## Guides (new account google developer and new app android to PlayStore)
1. Create account Google.
2. Connect account Google.
3. Paid 25$ lifetime for account developer
4. Vérifier sont compte Google Developer via ça carte d'identité, permis ou passeport.
5. Créer une clé de signature pour signé les applications Android via gradle :
    - Download and Install Java OpenJDK v20.x and add in PATH ENVIRONMENT, exemple : \..\open-jdk\jdk-20\bin (pour le binaire 'keytool')
    - Utilisez la commande keytool qui est incluse avec OpenJDK 20.x (ne pas utiliser Oracle pour cette commande !) pour générer un nouveau keystore : <br />
      ```bash
      # Cette commande vous invitera à entrer des informations.
      # IMPORTANT CONSERVER LES : KEY_PASSWORD (identique au KEYSTORE_PASSWORD), KEYSTORE_PASSWORD, le fichier générer .keystore, KEYSTORE_ALIAS_NAME
      # Ajouter au SECRETS GITHUB
      keytool -genkeypair -v -keystore my-release-key.keystore -keyalg RSA -keysize 2048 -validity 10000 -alias keystore_alias_name      
      ```
    - Encoder le fichier .keystore en base64 :
      ```bash
      # IMPORTANT : Conserver le fichier .json générer
      # Ajouter au SECRET GITHUB : KEYSTORE_BASE64 (la valeur du .json)
      base64 my-release-key.keystore > keystore_base64.txt 
      ```
6. Aller sur le site de Google Play Console.
7. Cliquer sur le menu de gauche 'Toute les applications'.
8. Cliquer sur le boutton en haut à droite de le page "Créer une application"
9. Remplisser les informations de base pour la nouvelle application, puis cliquer en bas a droite "Créer une application".
10. Revenir sur la page "Toute les applications", il y aura l'application créer en mode "brouillon" / "draft".
11. Cliquer sur la fleche sur la ligne ou il y a la nouvelle application qu'ont viens de créer.
12. On arrive sur le "Tableau de bord", il faudra faire les étapes suivantes (obligatoire avant de pouvoir publier une release sur le canal de production du PlayStore) :
    - Première partie obligatoire (Démarrer les tests maintenant), commencer par "Selectionner les testeurs", ajouter des testeurs. Aller ensuite sur "Créer une release", cliquer sur le boutton "Selectionner une clé de signature" puis cliquer sur "Utiliser une clé de signature par Google", puis ajouté le fichier AAB SIGNER (il faudra build/signé le projet manuellement juste pour cette fois pour la premiere version de l'app), puis ajouter un nom à la release par exemple "0.0.1" faite "SUIVANT" en bas à droite puis faire "Prévisualiser et confirmer la version".
    - Deuxième partie obligatoire (Configurer votre application) il faudra remplir pour les informations pour : Définir les règles de confidentialité, Accès aux applications..etc. Configurer la fiche du PlayStore (il faudra lancer l'application via Android Studio est lancer via les devices emulator pour faire les diffèrent screenshots pour tablette 7", 10" et pour téléphone.
13. L'application doit avoir une URL vers la politique de confidentialité dans Google Play Console ET à l'intérieur même de l'application, par exemple : On lance l'application, on arrive dans le menu, on "clique" sur "A propos" puis on "clique" sur "Politique de confidentialité" cela ouvre dans le navigateur vers l'URL requis qui décrit la politique de confidentialité.
14. Ajouter a la page Politique de confidentialité existante si c'est un nouveau jeu concernant le paragraphe (remplacer le nom RC2D par l'app) : "This policy specifically applies to the Android application "RC2D" developed by Corentin Recanzone for studio CrzGames (DEV-ALL) and available on Google Play."
15. Déployer la première version dans le canal de production pour le PlayStore, aller dans le "Tableau de bord", faire la partie "Créer et publier une version" puis les sections "Sélectionner les pays et les régions", "Créer une release", "Prévisualiser et confirmer la version" et pour finir "Envoyer la version à Google pour examen".
16. A ce stade, l'application sera "En attente d'examen", dès que l'application est approuver par Google, le déploiement des futures mise à jour de l'application sera automatiser via la CI lorsque c'est push sur la branche de production (FAIRE l'étape ci-dessous pour cela).
17. Créer un compte "Account Service" pour Google Cloud Plateform (qui permettra a la CI/CD de déployer auto sur le PlayStore via l'API Android) :
    - Click 'Play Google Console' -> 'Configuration' -> 'Accès à l'API'
    - Cliquer sur le boutton 'Découvrez comment créer des compte de service' suivez l'étape 1 en cliquant sur le boutton 'Accédez à Google Cloud Platform' on sera rediriger sur : https://console.cloud.google.com/
    - Cliquer sur le boutton 'CREER UN COMPTE DE SERVICE' suivez les étapes, puis revenir en arriere sur la page
    - Cliquer sur les '...' dans la colonne 'Actions' dans la ligne ou le compte à était créer, puis cliquer sur 'Gérer les clés', cliquer sur clé de type "JSON" il vas télécharger un fichier .JSON conserver le pour l'ajouter au SECRET GITHUB : GOOGLE_CLOUD_PLATEFORM_SERVICE_JSON.
    - Puis revenir sur la page 'Google Play Console' -> 'Configuration' -> 'Accès à l'API' -> puis cliquer sur 'Actualiser' le compte "Account Service" créer précédemment sur Google Cloud Plateform devrait apparaitre à présent.
18. Dans le menu de gauche cliquer sur "Utilisateurs et autorisations", ajouter la nouvelle application créer au compte "Account Service".
19. Désactiver le "Jouer pendant le téléchargement", dans le menu de gauche de Google Play Store -> 'Configuration' -> 'Paramètres avancés' -> Sur la page cliquer sur la section 'Jouer pendant le téléchargement' et cliquer sur 'Jouer pendant le téléchargement désactivé'

<br />

## Guides (new app android to PlayStore)
1. Aller sur le site de Google Play Console.
2. Cliquer sur le menu de gauche 'Toute les applications'.
3. Cliquer sur le boutton en haut à droite de le page "Créer une application"
4. Remplisser les informations de base pour la nouvelle application, puis cliquer en bas a droite "Créer une application".
5. Revenir sur la page "Toute les applications", il y aura l'application créer en mode "brouillon" / "draft".
6. Cliquer sur la fleche sur la ligne ou il y a la nouvelle application qu'ont viens de créer.
7. On arrive sur le "Tableau de bord", il faudra faire les étapes suivantes (obligatoire avant de pouvoir publier une release sur le canal de production du PlayStore) :
    - Première partie obligatoire (Démarrer les tests maintenant), commencer par "Selectionner les testeurs", ajouter des testeurs. Aller ensuite sur "Créer une release", cliquer sur le boutton "Selectionner une clé de signature" puis cliquer sur "Utiliser une clé de signature par Google", puis ajouté le fichier AAB SIGNER (il faudra build/signé le projet manuellement juste pour cette fois pour la premiere version de l'app), puis ajouter un nom à la release par exemple "0.0.1" faite "SUIVANT" en bas à droite puis faire "Prévisualiser et confirmer la version".
    - Deuxième partie obligatoire (Configurer votre application) il faudra remplir pour les informations pour : Définir les règles de confidentialité, Accès aux applications..etc. Configurer la fiche du PlayStore (il faudra lancer l'application via Android Studio est lancer via les devices emulator pour faire les diffèrent screenshots pour tablette 7", 10" et pour téléphone.
8. L'application doit avoir une URL vers la politique de confidentialité dans Google Play Console ET à l'intérieur même de l'application, par exemple : On lance l'application, on arrive dans le menu, on "clique" sur "A propos" puis on "clique" sur "Politique de confidentialité" cela ouvre dans le navigateur vers l'URL requis qui décrit la politique de confidentialité.
9. Ajouter a la page Politique de confidentialité existante si c'est un nouveau jeu concernant le paragraphe (remplacer le nom RC2D par l'app) : "This policy specifically applies to the Android application "RC2D" developed by Corentin Recanzone for studio CrzGames (DEV-ALL) and available on Google Play."
10. Déployer la première version dans le canal de production pour le PlayStore, aller dans le "Tableau de bord", faire la partie "Créer et publier une version" puis les sections "Sélectionner les pays et les régions", "Créer une release", "Prévisualiser et confirmer la version" et pour finir "Envoyer la version à Google pour examen".
11. A ce stade, l'application sera "En attente d'examen", dès que l'application est approuver par Google, le déploiement des futures mise à jour de l'application sera automatiser via la CI lorsque c'est push sur la branche de production (main).
12. Dans le menu de gauche cliquer sur "Utilisateurs et autorisations", ajouter la nouvelle application créer au compte "Account Service".
13. Désactiver le "Jouer pendant le téléchargement", dans le menu de gauche de Google Play Store -> 'Configuration' -> 'Paramètres avancés' -> Sur la page cliquer sur la section 'Jouer pendant le téléchargement' et cliquer sur 'Jouer pendant le téléchargement désactivé'

<br /><br />


## PlayStore - Infos
- Première fois qu'ont déploie une nouvelle application compter entre : 4 à 7 jours
- Pour une mise à jour d'une application déjà déployer compter entre : quelques heures à 2 jours
- Restriction du Play Store pour les nouvelles applications Android : Build les APK / AAB pour Android SDK/API version >= 33 (30 aout 2023)
  
<br /><br />


## New Release Android (après le premier déploiement de l'application après examen)
1. Pour chaque nouvelle mise à jour de l'application Android, on peux mettre à jour le changelog du jeu pour le PlayStore. Ajouter les changelogs pour les diffèrentes langues, depuis le dossier racine : distribution/whatsnew <br />
Choses important respecter bien les noms des fichiers pour chaque langue. <br />
Respecter la norme : https://www.rfc-editor.org/info/bcp47 (Doc si besoin : https://github.com/r0adkll/upload-google-play)
2. Concernant les numéro de versions des mise à jours d'une application Android, il y a "codeName" et "codeVersion" (via AndroidManifest.xml ou build.gradle) : Le "codeVersion" est utilisé par le PlayStore de Google pour une nouvelle mise à jour et le "codeName" est affiché à l'utilisateur. <br /> Le système de versionning de l'application Android pour chaque mise à jour de l'application est automatiser via la CI avec un script .sh : update-android-version.sh, qui modifie dynamiquement le "codeVersion" et "codeName", et utilise "release-please" de Google en tant que SemVer.
3. Pousser la nouvelle version : 
    - Go in branch 'develop' :
    ```bash
    git checkout develop
    git pull origin develop
    ```
    - Fusionnez la branche develop dans la branche de pré-production (staging) :
    ```bash
    git checkout staging
    git merge develop
    git push origin staging
    ```
    - Effectuez les tests et les validations nécessaires dans l'environnement de staging.
    - Lorsque la version est validée, fusionnez la branche de development (develop) dans la branche de production (main) pour publier la version finale :
    ```bash
    git checkout main
    git merge develop
    git push origin main
    
    # Lance la CI / CD auto lorsque c'est push sur main
    ```
<br /><br />


## Loading Assets
Tous les fichiers images, videos, data..etc doit être placer dans le répertoire "app/src/main/assets" de votre répertoire de projet seront regroupés dans le package d'application et vous pourrez les charger à l'aide des fonctions standard de SDL_rwops.h

Il existe également quelques fonctions spécifiques à Android qui vous permettent d'obtenir d'autres chemins utiles pour enregistrer et charger des données :
- SDL_AndroidGetInternalStoragePath()
- SDL_AndroidGetExternalStorageState()
- SDL_AndroidGetExternalStoragePath()

<br /><br />


## Life cycle - Quitter l'application proprement
IMPORTANT : N'appelez pas exit() car cela arrête gravement l'activité Java.

<br />

Les deux manières légitimes : 
1. Via le "return" de la fonction main(argc, char* argv[]). Le côté Java mettra automatiquement fin à l'activité en appelant Activity.finish().
2. Le système d'exploitation Android peut décider de mettre fin à votre application en appelant onDestroy(). Votre application recevra un événement "SDL_QUIT" que vous pourrez gérer pour sauvegarder des éléments ou autres et quitter.

<br /><br />


## Evenements Android
### Gestion du TACTILE 
- SDL_FINGERDOWN, reçu lorsqu'un doigt est posé sur l'écran.
- SDL_FINGERMOTION, reçu lorsque ce doigt bouge.
- SDL_FINGERUP, reçu lorsque le doigt quitte l'écran.

### Informations IMPORTANT :
C'est en soi assez similaire à la gestion de la souris qui a 3 événements équivalents (SDL_MOUSEBUTTONDOWN, SDL_MOUSEMOTION et SDL_MOUSEBUTTONUP), avec une petite subtilité tout de même.

Les événements de la souris sont donnés avec des coordonnées ENTIERES correspondant aux nombres de pixels parcourus depuis l'origine en haut à gauche de la fenêtre. Si je clique au milieu de fenêtre HD 1920x1080, alors mon programme reçoit un événement ev de type SDL_MOUSEBUTTONDOWN avec les valeurs ev.button.x == 960 et ev.button.y == 540.

Les événements tactiles donnent eux des coordonnées FLOTTANTES correspondant à la FRACTION de la fenêtre parcoure depuis l'origine en haut à gauche. Ainsi, si je clique au milieu de ma fenêtre, mon programme reçoit un événement ev de type SDL_FINGERDOWN avec les valeurs ev.button.x == 0.5 et ev.button.y == 0.5. 
Il suffit donc de MULTIPLIER ces valeurs respectivement par la LARGEUR et la HAUTEUR de votre fenêtre pour retomber sur les mêmes valeurs qu'avec un clic de souris. 

<br />

### Autres Evenements Android
- Le bouton "Retour" peut être géré comme un événement SDL_KEYDOWN/UP, avec le Keycode "SDLK_AC_BACK" ou via "ANDROID_BACK".

- SDL_APP_WILLENTERBACKGROUND est générée lorsque l'application est sur le point de passer en arrièr-plan (par exemple, si la touche pour revenir à l'accueil d'Android a été pressée) : dans mon cas, ça me permet de mettre le jeu en pause. On a aussi SDL_APPDIDENTERBACKGROUND qui est générée lorsque l'application est effectivement passée en arrière-plan (la différence est subtile).
  
- SDL_APP_WILLENTERFOREGROUND et SDL_APP_DIDENTERFOREGROUND pour vous prévenir que l'app va revenir (ou est revenue) au premier plan : dans mon cas, c'est la fin de la pause, le jeu reprend normalement.
  
- SDL_APP_TERMINATING qui vous prévient que l'application s'arrête, et vous laisse donc sauvegarder ce qu'il faut et quitter proprement l'appli (on peut voir ça comme l'équivalent de l'appui sur la croix dans la version bureau, soit l'événement SDL_QUIT).

<br /><br />


## Divers
1. Pour certains appareils, il semble fonctionner mieux de définir explicitement les attributs GL avant de créer une fenêtre, exemple : <br />
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5); SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6); SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
2. Événements souris/tactile : Dans certains cas, SDL génère des événements de souris synthétiques (resp. tactiles) pour les appareils tactiles (resp. souris). Pour activer/désactiver ce comportement, consultez SDL_hints.h : SDL_HINT_TOUCH_MOUSE_EVENTS, SDL_HINT_MOUSE_TOUCH_EVENTS

<br /><br />


## Setup Environment
1. Download and Install Java JDK LTS v17.0.10 to Oracle for Android version >= 14 : https://www.oracle.com/java/technologies/downloads/#jdk17-windows
2. Download and Install Android Studio >= 2023.1.1 : https://developer.android.com/studio
3. Open Android Studio -> Click 'SDK Manager' -> SDK Plateforms installs : <br />
   - Android API 34 :
       - Android SDK Plateform 34
       - Sources for Android 34
       - Google APIs Intel x86_54 Atom System Image,
       - Google APIs Play Intel x86_54 Atom System Image
4. Open Android Studio -> Click 'SDK Manager' -> SDK Tools installs : <br />
   - Android SDK Build-Tools : 34.0.0  <br />
   - NDK (Side by side) : 26.1.10909125 <br />
   - Android SDK Command-line Tools : Android SDK Commande Line Tools 11.0 <br />
   - CMake : 3.22.1 <br />
   - Android Emulator and Android Emulator Hypervisor driver <br />
   - Google USB Driver
5. Add variable environnement "ANDROID_HOME", open Android Studio -> Click 'SDK Manager' -> Copier le path qui contient le chemin du SDK et l'ajouté au variable d'environnement
6. Download plateform-tools latest for ADB Android : https://developer.android.com/studio/releases/platform-tools?hl=fr and add in PATH ENVIRONNEMENT
7. Open project Android Studio -> Device Manager -> Create devices emulator for Telephone and Tablette to Android API 34 OR use device real.
8. Open project Android Studio -> Click icon "Synchronize Gradle", il vas télécharger les packages et dépendences par rapport au projet des fichiers .gradle
    
<br /><br /><br /><br />


## Cycle Development
1. Open project Android Studio.
2. (optional) Click icon "Synchronize Gradle", il vas télécharger les packages et dépendences par rapport au projet des fichiers .gradle
3. Select device emulator or real device.
4. Run project (icon avec la fleche verte), cela vas lancer la construction du projet via CMake avec le premier fichier CMakelist.txt du projet Android Studio.
5. Debugger l'application Android via l'onglet "Logcat" dans Android Studio pour voir les traces des logs.
   
<br /><br /><br /><br />


## Production
### Automatic - Pipeline CI / CD
#### Si c'est un nouveau projet suivez les instructions : 
1. Ajoutées les SECRETS_GITHUB pour :
   - GOOGLE_CLOUD_PLATEFORM_SERVICE_JSON
   - KEYSTORE_BASE64
   - ALIAS_NAME
   - KEYSTORE_PASSWORD
   - KEY_PASSWORD
   - PAT (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))

<br />


### Manual
#### Build AAB :
```bash
# Gradle Wrapper
# Debug
gradlew bundleDebug

# Release
# OBLIGATOIRE : Add 3 VARIABLE ENVIRONMENT -> KEYSTORE_PASSWORD, KEY_PASSWORD, ALIAS_NAME
# OBLIGATOIRE : Add file 'my-release-key.keystore' à la racine de ce repository. 
gradlew bundleRelease



# Gradle
# Debug
gradle bundleDebug

# Release
# OBLIGATOIRE : Add 3 VARIABLE ENVIRONMENT -> KEYSTORE_PASSWORD, KEY_PASSWORD, ALIAS_NAME
# OBLIGATOIRE : Add file 'my-release-key.keystore' à la racine de ce repository. 
gradle bundleRelease
```

#### Build APK :
```bash
# Gradle Wrapper
# Debug
gradlew assembleDebug

# Release
# OBLIGATOIRE : Add 3 VARIABLE ENVIRONMENT -> KEYSTORE_PASSWORD, KEY_PASSWORD, ALIAS_NAME
# OBLIGATOIRE : Add file 'my-release-key.keystore' à la racine de ce repository. 
gradlew assembleRelease



# Gradle
# Debug
gradle assembleDebug

# Release
# OBLIGATOIRE : Add 3 VARIABLE ENVIRONMENT -> KEYSTORE_PASSWORD, KEY_PASSWORD, ALIAS_NAME
# OBLIGATOIRE : Add file 'my-release-key.keystore' à la racine de ce repository. 
gradle assembleRelease
```
