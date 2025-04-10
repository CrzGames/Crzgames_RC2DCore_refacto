# Guide de préparation pour un nouveau jeu Android

## Étapes à faire si c'est notre deuxième jeu ou plus
### 1. Ajouter dans GooglePlayConsole le nouveau jeu pour le compte "Account Service" (pour les pipelines CI / CD)
- Ouvrez https://play.google.com/console et choisissez votre compte développeur
- Dans le menu de gauche cliquer sur `Utilisateurs et autorisations`.
- Cliquer sur le compte de service ajouté. Puis dans `Autorisations de l'application` il y a un boutton `Ajouter une application` cliquer dessus et ajouter l'application créer sur le PlayStore. Puis cliquer sur `Enregistrer les modifications`.

<br /><br />

## Étapes de modification du projet Android

### 1. Modification du package/dossier Java

- **Chemin du package**: Remplacez le dossier/package existant `./android-project/app/src/main/java/com.crzgames.testexe` par le nouveau nom de votre choix `./android-project/app/src/main/java/*`.

<br />

### 2. Mise à jour du fichier `MyGame.java`

- **Fichier concerné**: `MyGame.java` situé dans `./android-project/app/src/main/java/*/MyGame.java`.
- **Modification**: Changez la ligne `package com.crzgames.testexe;` pour refléter le nouveau nom du package Java.

<br />

### 3. Ajustement dans `AndroidManifest.xml`

- **Localisation**: `./android-project/app/src/main/AndroidManifest.xml`.
- **Directive**: Modifiez `android:name="com.crzgames.testexe.MyGame"` avec le nouveau nom du package tout en conservant `MyGame`.

::: warning IMPORTANT
Gardez `MyGame` dans le nom du package dans `AndroidManifest.xml` pour assurer le lien avec l'activité Android du projet.
:::

<br />

### 4. Modification dans `build.gradle`

- **Fichier**: `./android-project/app/build.gradle`.
- **Changements**: 
    - Modifiez `namespace` avec le nouveau nom du package Java.
    - Modifiez `applicationId` avec le nouveau nom du package Java.
    - Changez `"com.crzgames.testexe.aar"` par le nouveau nom de package : `"*.aar"`.

<br />

### 5. Personnalisation du nom de l'application

- **Fichier**: `./android-project/app/src/main/res/values/strings.xml`.
- **Action**: Remplacez la valeur de `MyGame` avec le nom de votre jeu.

<br />

### 6. Personnalisation de l'icône de l'application

- **Dossier**: `./android-project/app/src/main/res`.
- **Instruction**: Remplacez tous les fichiers `ic_launcher.png` par l'icône de votre nouveau jeu dans les dossiers `mipmap`.

<br />

### 7. Mise à jour de la CI

- **Fichier CI**: `./.github/workflows/build_deploy_staging.yml` et `./.github/workflows/build_deploy_production.yml`.
- **Step concernée**: `Deploy AAB to Play Store - Android`.
- **Modification**: Changez la propriété `packageName` de `com.crzgames.testexe` à votre nouveau package Java.
- **Modification(2)**: Changez la valeur de la propriété `track` pour le cannal de distribution de votre choix.

<br />

### 8. Modification de la valeur de la propriété `project` dans `CMakeLists.txt`

::: tip Conseil
Lors du changement de la valeur de la propriété `project` dans `android-project/app/jni/CMakeLists.txt`, ajustez le nom de la librairie dynamiques dans le wrapper SDL en Java.
:::

- **Fichier**: `./android-project/app/src/main/org/libsdl/app/SDLActivity.java`.
- **Lignes concernées**: 276 et 247.

Donc si la valeur de la propriété `project` est `mygame-test`, alors le nom de la librairie dynamique sera `libmygame-test.so` pour la ligne 247. <br />
Sinon pour la ligne 276, le nom de la librairie dynamique sera `mygame-test`.

<br />

### 9. Ajout ou modification de librairies dynamiques.

::: tip Conseil
Lors de l'ajout ou modification d'une librairie dynamique `.so`, ajustez le chargement des librairies dynamiques dans le wrapper SDL en Java.
:::

- **Fichier**: `./android-project/app/src/main/org/libsdl/app/SDLActivity.java`.
- **Lignes concernées**: de 270 à 276.
- **Directive**: Ajoutez ou modifiez le chargement des librairies dynamiques dans la méthode `getLibraries()`.

<br />

### 10. Changement de version SDL2 (quand on doit up la version de SDL2)

- **Fichier**: `./android-project/app/src/main/org/libsdl/app/SDLActivity.java`.
- **Ligne**: 60, 61 et 62.
- **Directive**: Modifiez les valeurs de `SDL_MAJOR_VERSION`, `SDL_MINOR_VERSION`, et `SDL_MICRO_VERSION` en fonction de la nouvelle version de SDL2.

- **Dossier**: `./android-project/app/jni/SDL`.
- **Directive**: Récupérez la nouvelle version de SDL2 et remplacez le contenu du dossier `SDL` par celui de la nouvelle version. Il faut intégrer les `sources complète` du repository SDL2. Bien laisser le nom `SDL` pour le dossier.

<br />

### 11. Modification du script de developpement pour le projet

- **Fichier**: `./build-scripts/run-project-development/android.sh`.
- **Lignes concernées**: 65 et 73.
- **Modification**: Modifiez `com.crzgames.testexe` par le nouveau nom du package Java.