# Crzgames - RC2DCore

## 🛠 Tech Stack
- C (Language)
- A C99 Compiler
- CI / CD (Github Actions)
- CMake (Build script)
- Compiler (GCC, CL, Clang, Ninja, NDK)

<br /><br /><br /><br />


## 📁 Project Structure

```
📦 Crzgames_RC2DCore
├── 📁 .github                        # Configuration GitHub (workflows CI/CD, templates de issues/PRs, etc.)
├── 📁 android-project                # Projet Android contenant les fichiers nécessaires pour packager RC2D sous forme de .aar
├── 📁 build-scripts                  # Scripts de build personnalisés (.sh / .bat), puis les scripts utilise le CMakelists.txt
├── 📁 cmake                          
│   └── 📄 setup_dependencies.cmake   # Script CMake chargé de lire `dependencies.txt` et cloner/configurer les dépendances dans `/dependencies`
├── 📁 dependencies (git ignored)     # Répertoire local contenant les dépendances clonées (ignoré par Git pour ne pas polluer le repo)
│   ├── 📁 Crzgames_Libraries         # Librairies précompilées (OpenSSL, ONNX Runtime et SDL_shadercross) propres à Crzgames
│   ├── 📁 Crzgames_RCENet            # Librairie réseau RCENet (wrapper autour d’ENet)
│   ├── 📁 SDL                        # SDL3 (dépendance principale du moteur)
│   ├── 📁 SDL_image                  # Extension SDL3 pour le support des images (PNG, JPEG, etc.)
│   ├── 📁 SDL_ttf                    # Extension SDL3 pour le rendu de polices TrueType
│   ├── 📁 SDL_mixer                  # Extension SDL3 pour la gestion audio avancée
├── 📁 docs                           # Documentation du moteur (ex: pages Markdown, auto-générées ou manuelles)
├── 📁 example                        # Exemples d’utilisation du moteur RC2D (projets de démo, test de fonctionnalités)
├── 📁 include                        # En-têtes publics exposés aux utilisateurs de la lib (API du moteur)
├── 📁 src                            # Code source interne de la bibliothèque RC2D (implémentations .c)
├── 📁 tests                          # Tests unitaires (ex: avec Criterion) pour vérifier les modules du moteur
├── 📄 .gitignore                     # Fichiers/dossiers à ignorer par Git (ex: /dependencies, artefacts, builds temporaires)
├── 📄 CHANGELOG.md                  # Historique des versions avec les modifications apportées à chaque release
├── 📄 CMakeLists.txt                # Point d’entrée de la configuration CMake (build multiplateforme)
├── 📄 dependencies.txt              # Fichier listant les dépendances à cloner (format : nom=repo:version)
├── 📄 README.md                     # Page d’accueil du dépôt (description, installation, exemples d’usage)
├── 📄 release-please-config.json    # Configuration pour `release-please` (outil Google de génération automatique de releases)
├── 📄 version.txt                   # Contient la version actuelle du moteur (utilisé dans le build ou les releases)

```

<br /><br /><br /><br />


## 📋 Plateformes supportées
| Platform | Architectures | System Version | Compatible |
|----------|---------------|----------------|------------|
| **Windows** | x64 / arm64 | Windows 10+   | ✓          |
| **macOS** | Apple Silicon arm64 | macOS 15.0+ | ✓ |
| **iOS/iPadOS** | arm64 (iphoneos) - not iphonesimulator | iOS/iPadOS 18.0+ | ✓ |
| **Android** | arm64-v8a / armeabi-v7a | Android 9.0+ | ✓ |
| **Linux** | x64 / arm64 | glibc 2.35+ | ✓ |
| **Steam Linux** | x64 / arm64 | Steam Linux Runtime 3.0 (Sniper) | ✓ |
| **Steam Deck** | x64 | Steam Linux Runtime 3.0 (Sniper) | ✓ |
| **Xbox** | x64 | Xbox Série X/S+ |  |
| **Nintendo Switch** | arm64 | Nintendo Switch 1+ |  |
| **Playstation** | x64 | Playstation5+ |  |

<br /><br /><br /><br />


## 📱 Appareils compatibles par plateforme

### **iOS / iPadOS (iOS/iPadOS 18.0+)**

#### iPhones:
- iPhone XR / XS / XS Max
- iPhone SE (2/3ème génération)
- iPhone 11 / 12 / 13 / 14 / 15 / 16 (Normal, Mini, Plus, Pro, Pro Max, E) et plus récent

#### iPads:
- iPad mini (5/6ème génération, A17 Pro) et plus récent
- iPad (7/8/9/10ème génération, A16) et plus récent
- iPad Air (3/4/5ème génération, M2, M3) et plus récent
- iPad Pro (1/2/3/4/5/6ème génération, M4) et plus récent

### **macOS (macOS 15.0+)**
- Tous les modèles macOS Apple Silicon (M1, M2, M3, M4) et plus récent

### **Android (Android 9.0+, API 28+)**
- Samsung Galaxy S9+ (2018) et plus récent
- Google Pixel 3 et plus récent
- OnePlus 6T et plus récent
- Galaxy Tab S4 (2018) et plus récent

### **Linux (glibc 2.35+)**
- Ubuntu 22.04+
- Debian 12+
- Fedora 36+
- Linux Mint 21+
- elementary OS 7+
- CentOS/RHEL 10+

### **Windows (10+)**
- Windows 10 et plus récent.

<br /><br /><br /><br />


## 🎯 Raisons techniques des versions minimales et autres par plateforme

### Windows
- **Version minimale** : Windows 10+
- **Raison** :
  - SDL3 API GPU repose sur Direct3D12 (Level Feature 11_1)
  - Windows ARM64 nécessite également Windows 10+

### macOS
- **Version minimale** : macOS 15.0+
- **Raison** :
  - Requis par ONNX Runtime pour C++20 (macOS 13.4+)
  - Requis par MSL version 3.2.0 (macOS 15.0+)

### iOS/iPadOS
- **Version minimale** : iOS/iPadOS 18.0+
- **Raison** :
  - SDL3 API GPU supporté depuis iOS/iPadOS 13.0
  - CoreML pour ONNX Runtime nécessite iOS/iPadOS 13.0+
  - Requis par MSL version 3.2.0 (iOS/iPadOS 18.0+)
  - Pas de librairie pour iOS/iPadOS simulator parce que SDL3 GPU ne le supporte pas.

### Android
- **Version minimale** : Android 9.0 (API 28+)
- **Raison** :
  - SDL3 GPU utilise Vulkan (introduit à partir d'Android 7.0)
  - ONNX Runtime avec NNAPI demande Android 8.1+ et recommande Android 9.0+
  - Pas d'architecture Android : x86_64 et x86, parce que ONNX Runtime compatible que : arm64-v8a / armeabi-v7a

### Linux
- **Version minimale** : glibc 2.35+
- **Raison** :
  - CI/CD basée sur Ubuntu 22.04 LTS (donc librairie RC2D + dépendences construite sur glibc 2.35)
  - ONNX Runtime nécessite C++20 (glibc 2.31+)

<br /><br /><br /><br />


## 📦 Dépendances principales

| Librairie              | Utilisation principale                                       | Intégration                |
|------------------------|--------------------------------------------------------------|----------------------------|
| **SDL3**               | Moteur principal, gestion entrée/sortie, rendu GPU           | Obligatoire                |
| **SDL3_image**         | Chargement des images                                        | Obligatoire                |
| **SDL3_ttf**           | Rendu de polices TrueType                                    | Obligatoire                |
| **SDL3_mixer**         | Gestion du mixage audio (WAV, MP3, OGG...)                   | Obligatoire                |
| **SDL3_shadercross**   | Transpilation code HLSL → MSL/SPIR-V/DXIL/METALLIB           | Activé par défault mais optionnel. Passé à CMake: RC2D_GPU_SHADER_HOT_RELOAD_ENABLED=OFF/ON. Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est à ON alors SDL3_shadercross sera link avec ces dépendences pour le rechargement à chaud des shaders à l'execution pour le temps du développement, sinon pour la production passé RC2D_GPU_SHADER_HOT_RELOAD_ENABLED à OFF et utilisé SDL3_shadercross en mode CLI pour la compilation hors ligne des shaders |
| **RCENet**             | Fork de ENet (Communication UDP)                             | Activé par défault mais optionnel, mais le module "RC2D_net" ne sera pas utilisable si désactiver. Passé à CMake : RC2D_NET_MODULE_ENABLED=OFF/ON |
| **OpenSSL**            | Hashing, Chiffrement, Compression..etc                       | Activé par défault mais optionnel, mais le module "RC2D_data" ne sera pas utilisable si désactiver. Passé à CMake : RC2D_DATA_MODULE_ENABLED=OFF/ON |
| **ONNX Runtime**       | Exécution de modèles ONNX pour l'inférence                   | Activé par défault mais optionnel, mais le module "RC2D_onnx" ne sera pas utilisable si désactiver. Passé à CMake : RC2D_ONNX_MODULE_ENABLED=OFF/ON |

<br /><br /><br /><br />


## ⚙️ Setup Environment Development
1. Cloner le projet ainsi que les submodules en recursive, penser à clone le projet à la racine du disque dur C:/
   sinon il y a un gros risque pendant la compilation de certaines dépendences de se retrouver avec un probleme de chemin trop long :
  ```bash
  git clone --recurse-submodules git@github.com:CrzGames/Crzgames_RC2DCore.git
  ```
2. (Optional) Download and Install Node.js >= 18.0.0 to run website documentation for GameEngine
3. Steps by Platform :
  ```bash  
  # Windows :
  1. Requirements : Windows >= 10 (x64 or arm64)
  2. Download and Install Visual Studio == 2022 (MSVC >= v143 + Windows SDK >= 10) : https://visualstudio.microsoft.com/fr/downloads/
  3. Download and Install CMake >= 3.25 : https://cmake.org/download/ and add PATH ENVIRONMENT.
  4. Ouvrir PowerShell en admin (pour eviter les erreurs de chemin long possible lors du setup des dependencies) :
   Set-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1
   git config --global core.longpaths true
  5. Télécharger et Installer LuaJIT (pour la dépendence cimgui), puis ajouté au PATH.
  6. Run le scripts .bat via la console "Visual Studio Developer Command" (terminal Visual Studio), contrainte pour cimgui.
  
  # Linux :
  1. Requirements : glibc >= 3.25 (Ubuntu >= 22.04 OR Debian >= 12.0)
  2. Run command (replace debian for name) : sudo usermod -aG sudo debian
  3. Download and Install brew : /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  4. Après l'installation de homebrew il faut importer les variables d'environnement et installer les deux librairies : 
    echo '# Set PATH, MANPATH, etc., for Homebrew.' >> /home/debian/.bashrc && 
    echo 'eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"' >> /home/debian/.bashrc && 
    eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)" &&
    sudo apt-get install -y build-essential &&
    brew install gcc
  5. Download and Install CMake >= 3.28 : brew install cmake
  6. Télécharger et Installer LuaJIT (pour la dépendence cimgui), puis ajouté au PATH.
  7. Télécharger et Installer patchelf (pour la dépendence SDL_shadercross), puis ajouté au PATH.


  # macOS :
  1. Requirements : MacOS X >= 14.0
  2. Download and Install xCode >= 15.2.0
  3. Download and Install Command Line Tools : xcode-select --install
  4. Download and Install CMake >= 3.28 : brew install cmake
  5. Télécharger et Installer LuaJIT (pour la dépendence cimgui), puis ajouté au PATH.


  # Android (run in Windows) :
  1. Download and Install : Android Studio 2023.1.1 or newer
  2. Add environment variable: ANDROID_HOME for path SDK Android (SDK Manager path)
  3. Download and Install CMake >= 3.28 : https://cmake.org/download/ and add PATH ENVIRONMENT.
  4. Download and Install Java JDK LTS (Oracle) == 17.0.10
  5. Télécharger et Installer LuaJIT (pour la dépendence cimgui), puis ajouté au PATH.


  # iOS (only macOS) :
  1. Requirements : MacOS X >= 14.0
  2. Download and Install Command Line Tools : xcode-select --install
  3. Download and Install xCode >= 15.2.0
  4. Download and Install SDK iOS >= 17.0.0
  5. Download and Install brew : /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  6. Download and Install cmake : brew install cmake 
  7. Télécharger et Installer LuaJIT (pour la dépendence cimgui), puis ajouté au PATH.
  ```
  
4. Avant toute compilation, exécute le script suivant :

```bash
cmake -P cmake/setup_dependencies.cmake
```

Ce script va :
- Lire `dependencies.txt`
- Cloner chaque dépôt dans `dependencies/`
- Faire un `git reset --hard` au SHA/tag fourni
- Initialiser les sous-modules si présents

### 📄 Exemple de fichier : `dependencies.txt`

```txt
# Format: library=repository:tag ou library=repository:commit_sha

SDL=https://github.com/libsdl-org/SDL.git:release-3.2.14
SDL_image=https://github.com/libsdl-org/SDL_image.git:release-3.2.4
SDL_ttf=https://github.com/libsdl-org/SDL_ttf.git:release-3.2.2
#SDL_mixer=https://github.com/libsdl-org/SDL_mixer.git:release-3.2.0
Crzgames_Libraries=https://github.com/CrzGames/Crzgames_Libraries.git:8e5ab015e20c8c93c388d63f7b1a20e29369aaf4
Crzgames_RCENet=https://github.com/CrzGames/Crzgames_RCENet.git:54612ae2790ae4d3a9a9262cbe86867d1c451e93
cimgui=https://github.com/cimgui/cimgui.git:094a55523a40fdb309f48b971a583ef02aeb56ab
luajit=https://luajit.org/git/luajit.git:f9140a622a0c44a99efb391cc1c2358bc8098ab7
```

<br /><br /><br /><br />


## 🔄 Updating Dependencies

Pour mettre à jour une dépendance :
1. Modifiez la référence dans `dependencies.txt`
2. Réexécutez le script :

```bash
cmake -P cmake/setup_dependencies.cmake
```

<br /><br /><br /><br />


## 🔄 Cycle Development
1. Open favorite IDE.
2. Edit or add files in 'src' or 'include' folder
  
<br /><br /><br /><br />


## Production
### ⚙️➡️ Automatic Distribution Process (CI / CD)
#### Si c'est un nouveau projet suivez les instructions : 
1. Ajoutées les SECRETS_GITHUB pour :
   - O2SWITCH_FTP_HOST
   - O2SWITCH_FTP_PASSWORD
   - O2SWITCH_FTP_PORT
   - O2SWITCH_FTP_USERNAME
   - PAT (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))

<br /><br />

### ✋ Manual Distribution Process
1. Generate librarie RC2DCore for Release and Debug, run command :
```bash
# Linux
./build-scripts/generate-lib/linux.sh

# macOS
./build-scripts/generate-lib/macos.sh

# Windows
sh .\build-scripts\generate-lib\windows.sh

# Android (run in Unix)
./build-scripts/generate-lib/android.sh

# Android (run in Windows)
sh .\build-scripts\generate-lib\android.sh

# iOS (run in macOS)
./build-scripts/generate-lib/ios.sh
```
2. Get librarie RC2DCore, steps for different systems :
```bash
# Windows
1. Go directory 'dist/lib/windows/'
2. Go in directory 'Release' OR 'Debug'
3. Get librarie RC2D : rc2d.lib

# Linux
1. Go directory 'dist/lib/linux/'
2. Go in directory 'Release' OR 'Debug'
3. Get librarie RC2D : librc2d.a

# macOS
1. Go directory 'dist/lib/macos/'
2. Go in directory 'Release' OR 'Debug'
3. Get librarie RC2D : librc2d.a

# Android
1. Go directory 'dist/lib/android/'
2. Go in directory 'Release' OR 'Debug'
3. Get librarie RC2D : librc2d.so

# iOS
1. Go directory 'dist/lib/ios/arm64-arm64e/iphoneos/' for sdk iphoneos (arm64,arm64e) OR 'dist/lib/ios/x86_64-arm64/iphonesimulator/' for sdk iphonesimulator (x86_64,arm64)
2. Go in directory 'Release' OR 'Debug'
3. Next directory for get librarie RC2D : librc2d.a
```
