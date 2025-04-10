
# 🔄 Development Cycle

This section details the steps involved in the development cycle for projects using the RC2D Game Engine, from generating your project to running it across different platforms.

## 🔧 Generate Project

::: danger IMPORTANT
Ensure your development environment is properly set up for each platform before running these scripts. This includes installing any required SDKs or tools specific to the platform you are developing for. <br />

Voir : https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html
:::

Before you can start developing your game, you need to generate the project files specific to your development platform. Use the following commands based on your operating system:


### 🐧 Linux
::: details Click me to view
```bash
./build-scripts/generate-project-development/linux.sh
```
Ce script est spécifique à Linux, en générant un environnement de build pour l'architecture x86_64, en préparant les assets et en configurant les environnements de build pour Debug et Release.
:::

<br />

### 🍏 macOS
::: details Click me to view
```bash
./build-scripts/generate-project-development/macos.sh
```
Ce script est spécifique à macOS, en générant un environnement de build pour l'architecture Intel x64 et Apple Silicon arm64 (fatlib universal), en préparant les assets et en configurant les environnements de build pour Debug et Release.
:::

<br />

### 🖥 Windows
::: details Click me to view
```bash
sh .\build-scripts\generate-project-development\windows.sh
```
Ce script est spécifique à Windows, en générant un environnement de build pour l'architecture x64 et x86 (Win32), en préparant les assets et en configurant les environnements de build pour Debug et Release.
:::

<br />
 
### 📱 iOS (only macOS)
::: details Click me to view
```bash
./build-scripts/generate-project-development/ios.sh
```
Ce script est spécifique à iOS et nécessite macOS pour être exécuté. Il prépare le projet pour les architectures arm64 (iphoneos) et arm64-x86_64 (iphonesimulator), en préparant les assets et en configurant les environnements de build pour Debug et Release.
:::

<br />

### 🤖 Android
::: details Click me to view
The project is already generated, the folder is called `android-project`, it is fully versioned unlike the rest of the platforms
:::

<br />

:::tip IMPORTANT
Chaque script de génération de projet effectue les opérations suivantes :
- Création des dossiers nécessaires pour les builds, situé à la racine du repository : `./dist/`
- Compilation des assets original en format `.rres`.
- Configuration des environnements de build spécifiques à chaque plateforme, pour les modes `Debug` et `Release`.
- Nettoyage des builds précédents avant de commencer une nouvelle compilation.
:::

Nous vous encourageons à suivre ces étapes pour assurer un développement fluide et efficace de votre jeu sur la plateforme choisie.

<br /><br /><br /><br />


## ▶️ Run Project
:::danger IMPORTANT
Pour pouvoir run le projet via les scripts, il faut avoir OBLIGATOIREMENT générer le projet pour le système souhaiter via les explications ci-dessus.
:::

Once the project has been generated, you can run it using the commands provided below. These commands launch your project in a development environment, allowing you to test and debug your game.

### 🐧 Linux
::: details Click me to view
```bash
./build-scripts/run-project-development/linux.sh
```
Ce script lance votre projet Linux, en préparant l'environnement pour une `exécution` en mode `Debug`. Il nettoie et reconstruit le projet, génère les assets en `.rres` et `exécute l'application`.
:::

<br />

### 🍏 macOS
::: details Click me to view
```bash
./build-scripts/run-project-development/macos.sh
```
Ce script lance votre projet macOS, en préparant l'environnement pour une `exécution` en mode `Debug`. Il nettoie et reconstruit le projet, génère les assets en `.rres` et `exécute l'application`.
:::

<br />

### 🖥 Windows
::: details Click me to view
```bash
sh .\build-scripts\run-project-development\windows.sh
```
Ce script lance votre projet Windows, en préparant l'environnement pour une `exécution` en mode `Debug`. Il nettoie et reconstruit le projet, génère les assets en `.rres` et `exécute l'application`.
:::

<br />

### 🤖 Android (Windows/Unix)
::: details Click me to view
```bash
# Windows
sh .\build-scripts\run-project-development\android.sh

# Unix
./build-scripts/run-project-development/android.sh
```
Ce script lance votre projet Android, le script vérifie les prérequis comme : ADB, génère les assets en `.rres`, si il ya bien un device connecté en USB à la machine hôte, puis nettoie le projet, désinstalle l'APK existant sur l'appareil actuellement connecté, reinstalle la nouveau apk sur le device connecté et lance la nouvelle version de l'application automatiquement. <br />

:::tip
Pour la récupération des logs sous Android, `logcat` est utilisé pour afficher les logs en temps réel avec une coloration pour une clarté accrue. <br />

Exemple : <br />

![An image](/images/run-project-android.png)
:::

:::warning IMPORTANT
Pour exécuter sur Android, assurez-vous que :
- ADB est installé et ajouté au PATH.
- Un appareil Android connecté en USB.
- Un appareil Android en mode developpeur activé + USB Debugging activé.
- Un appareil Android 6.0 ou supérieure est requis.
:::

<br />

### 📱 iOS (uniquement macOS)
::: details Click me to view
```bash
./build-scripts/run-project-development/ios.sh
```
Ce script lance votre projet iOS, le script vérifie les prérequis comme ios-deploy, génère les assets en `.rres`, si il ya bien un device connecté en USB à la machine hôte, puis nettoie le projet, désinstalle le .ipa existant sur l'appareil actuellement connecté, reinstalle la nouveau .ipa sur le device connecté. <br />

:::tip
Pour la récupération des logs sous iOS, `idevicesyslog` est utilisé pour afficher les logs en temps réel avec une coloration pour une clarté accrue.
:::

:::warning IMPORTANT
Pour exécuter sur iOS, assurez-vous que :
- ios-deploy est installé.
- Un appareil iOS connecté en USB.
- Un appareil iOS en mode developpeur activé.
- Un appareil iOS 12.0 ou supérieure est requis.
:::