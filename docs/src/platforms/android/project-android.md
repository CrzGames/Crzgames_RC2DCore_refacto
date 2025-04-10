# 🤖 Project Android
## Description
Cette page est pour les développeurs qui souhaitent configurer et développer des applications Android avec Android Studio. <br />
Ce qui permet d'aller plus loin que les scripts de base pour developper des applications Android qui est fournie avec le projet, permet également de debugger plus facilement.

<br /><br />


## ⚙️ Setup Environment Development
1. Voir la page pour setup l'environment pour Android pour votre système : https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html

<br /><br />


## 🛠 Android Studio - Configuration du SDK Android
1. Ouvrir Android Studio.
2. Dans `Android Studio`, cliquez en haut sur `Tools` dans le menu, puis sur `SDK Manager`.
3. Une fenetre s'ouvre, ouvrer l'onglet `Languages & Frameworks` puis cliquer sur `Android SDK`.
4. Cliquer sur `SDK Platforms` et cocher les cases suivantes :
   - **Android API 34** :
       - `Android SDK Platform 34`
       - `Sources for Android 34`
       - `Google APIs Intel x86 Atom System Image`
       - `Google APIs Play Intel x86 Atom System Image`
5. Cliquer ensuite sur `SDK Tools` et cocher les cases suivantes :
    - **Android SDK Build-Tools 35**
        - `Android SDK Build-Tools 34`
    - **NDK (Side by side)**
        - `26.3.11579264`
    - **Android SDK Command-line Tools**
        - `11.0`
    - **CMake**
        - `3.22.1`
    - `Android Emulator`
    - `Android Emulator Hypervisor Driver for AMD Processor`
    - `Android Emulator Hypervisor Driver for Intel Processor`
    - `Google USB Driver`
6. Puis cliquer sur le boutton `Apply` en bas à droite pour installer les packages.

## 🖥️ Android Studio - Configuration des devices Android
1. Ouvrir Android Studio.
2. Dans `Android Studio`, cliquez en haut sur `Tools` dans le menu, puis sur `Device Manager`.
3. Un onglet s'ouvre sur la droite, cliquer sur le boutton `+` pour ajouter un device.
4. Puis cliquer sois sur `Create Virtual Device` pour ajouter un nouvel émulateur ou `Select Remote Device` pour ajouter un appareil physique.

<br /><br />


## 🔄 Cycle de Développement

Suivez ces étapes pour le développement et le débogage de votre application :

1. **Ouvrez votre projet** dans Android Studio à partir du répertoire racine du projet `android-project`.
2. **Laissez le projet s'indexer** complètement à l'ouverture d'Android Studio.
3. **Synchronisez Gradle** pour télécharger les dépendances nécessaires, surtout après des modifications majeures dans vos fichiers Gradle/CMake.
4. **Sélectionnez un appareil** émulateur ou réel.
5. **Exécutez le projet** pour construire le projet via CMake.
6. **Déboguez l'application** en utilisant l'onglet `Logcat` dans Android Studio pour consulter les journaux.

:::warning IMPORTANT
Pour exécuter sur un device Android physique, assurez-vous que :
- ADB est installé et ajouté au PATH.
- Un appareil Android connecté en USB.
- Un appareil Android en mode developpeur activé + USB Debugging activé.
- Un appareil Android 6.0 ou supérieure est requis.
:::
