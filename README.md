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
├── 📁 .github
├── 📁 android-project
├── 📁 build-scripts
├── 📁 cmake
│   └── 📄 setup_dependencies.cmake
├── 📁 dependencies (git ignored)
│   ├── 📁 Crzgames_Libraries
│   ├── 📁 Crzgames_RCENet
│   ├── 📁 SDL
│   ├── 📁 SDL_image
│   ├── 📁 SDL_ttf
│   ├── 📁 SDL_mixer
│   ├── 📁 SDL_shadercross
├── 📁 docs
├── 📁 example
├── 📁 include
├── 📁 src
├── 📁 tests
├── 📄 .gitignore
├── 📄 CHANGELOG.md
├── 📄 CMakeLists.txt
├── 📄 dependencies.txt
├── 📄 README.md
├── 📄 release-please-config.json
├── 📄 version.txt
```

<br /><br /><br /><br />


## 📋 Plateformes supportées
| Platform | Architectures | System Version | Compatible |
|----------|---------------|----------------|------------|
| **Windows** | x64 / arm64 | Windows 10+   | ✓          |
| **macOS** | Intel x64 / Apple Silicon arm64 | macOS 15.0+ | ✓ |
| **iOS/iPadOS** | arm64 (iphoneos) - not iphonesimulator | iOS/iPadOS 18.0+ | ✓ |
| **Android** | arm64-v8a / armeabi-v7a | Android 9.0+ | ✓ |
| **Linux** | x64 / arm64 | glibc 2.35+ | ✓ |
| **Steam Linux** | x64 / arm64 | Steam Linux Runtime 3.0 (Sniper) | ✓ |
| **Steam Deck** | x64 | Steam Linux Runtime 3.0 (Sniper) | ✓ |
| **Xbox** | x64 | Xbox Série X/S+ |  |
| **Nintendo Switch** | arm64 | Nintendo Switch 1+ |  |
| **Playstation** | x64 | Playstation5+ |  |

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


## 🕹️ Plateformes spécifiques : Steam Linux et Steam Deck

### Documentation officiel de Valve
#### Steam Linux Runtime
- Documentation général : https://gitlab.steamos.cloud/steamrt/steam-runtime-tools/-/tree/main/docs
- Documentation du SDK : https://gitlab.steamos.cloud/steamrt/sniper/sdk

#### Steam Deck
- Documentation général : https://partner.steamgames.com/doc/steamdeck/loadgames
<br />

### Prérequis matériel - Steam Deck
- Le kit de devéloppement que Valve fournis à ces paternaires sont des Steam Deck LCD. Ces exemplaires sont les mêmes que ceux qui sont envoyés à la clientèle. Ils ne comportent aucun composant ou logiciel de développement supplémentaire.
- Si nous avons déjà un Steam Deck LCD/OLED, cela marchera de la même façon.
<br />

### Prérequis techniques pour Steam Linux Runtime 3.0 (Sniper) - Steam Linux / Steam Deck
- **Runtime recommandé** : `Steam Linux Runtime 3.0 (Sniper)` est la version stable à utiliser en 2025 pour **Steam Linux** et **Steam Deck**. Les versions `Steam Linux Runtime 4.0 / 5.0` existent mais sont expérimentales et ne doivent pas être utilisées pour la production. Cependant, surveillez les mises à jour, car elles pourraient devenir les versions recommandées à l'avenir.
- **Système de base** : Steam Linux Runtime 3.0 (Sniper) est basé sur **Debian 11** et utilise **glibc 2.31** sous le capot.
- **Compilateur recommandé** : Utilisez **GCC 14** (ou une version spécifique cohérente) pour compiler **toutes** les dépendances et le binaire du jeu. Il est crucial de maintenir le même compilateur et la même version du compilateur pour éviter des incompatibilités.
  - **Note importante** : Steam Linux Runtime 3.0 (Sniper) utilise par défaut **GCC 10**.
  - À partir de **GCC 12** (et versions ultérieures), les options `-static-libgcc` et `-static-libstdc++` sont appliquées par défaut pour éviter des mises à jour inattendues de la bibliothèque C++ standard. Cela garantit une meilleure stabilité pour la distribution.
  - Installation de GCC 12 ou 14 (Experimental pour ces versions) :
  ```bash
  # GCC 12
  apt-get install gcc-12-monolithic

  # GCC 14
  apt-get install gcc-14-monolithic
  ```
<br />

### Compilation - Steam Linux / Steam Deck
- **Images Docker recommandées** :
  - **Pour arm64 (Steam Linux)** : Utilisez l'image Docker : `registry.gitlab.steamos.cloud/steamrt/sniper/sdk/arm64:3.0.20250408.124536`. Le tag `latest` pour `arm64` n'est pas encore disponible et doit être évité, car le SDK arm64 est encore en version bêta/expérimentale.
  - **Pour x64 (Steam Linux / Steam Deck)** : Utilisez l'image Docker avec le tag : `registry.gitlab.steamos.cloud/steamrt/sniper/sdk:3.0.20250408.124536`. Le tag `latest` est `stable pour x64`, mais il est recommandé de figer une version spécifique pour éviter des changements imprévus.
  - **Listes des tags disponibles pour Docker** : https://repo.steampowered.com/steamrt3/images/

- **Conseils pour les bibliothèques partagées/dynamique** :
  - Évitez d'intégrer des bibliothèques partagées à votre jeu si elles sont également disponibles dans Steam Linux Runtime 3.0 (Sniper). Cela peut entraîner des problèmes de compatibilité.
  - Si vous chargez une bibliothèque partagées, assurez-vous d'utiliser son nom `SONAME versionné`, tel que libvulkan.so.1 ou libgtk-3.so.0, comme nom à rechercher. Évitez d'utiliser un lien symbolique de développement tel que libvulkan.soou libgtk-3.so, qui ne sera pas disponible dans le conteneur Steam Linux Runtime 3.0 (Sniper) et ne garantit pas la compatibilité ABI, même s'il fonctionne. Évitez également d'utiliser un nom entièrement versionné tel que libvulkan.so.1.2.189
ou libgtk-3.so.0.2404.26, car il ne fonctionnera plus si la bibliothèque est mise à niveau vers une version plus récente et compatible.
  - Utilisez les versions des bibliothèques partagées incluses dans Steam Linux Runtime, si possible.
  - (Recommandé) Si vous devez inclure une bibliothèque dans votre jeu, envisagez d'utiliser la liaison statique. Si vous utilisez la liaison statique, l'utilisation de : `-Wl,-Bsymbolic` comme option de compilation peut éviter les problèmes de compatibilité (pour éviter les conflits de symboles).
  
- **Conseils pour la compatibilité** :
  - **Évitez les mises à jour automatiques** : Les images Docker doivent être figées sur des versions spécifiques (comme indiqué ci-dessus) pour éviter des changements imprévus dans l'environnement de compilation.
  - **Dépendances** : Toutes les dépendances listées dans la section **Dépendances principales** (SDL3, SDL3_image, etc.) doivent être construire/compilées avec le même compilateur (GCC par exemple) et la même version du compilateur, ainsi que les mêmes versions des images Docker du SDK. Toute les librairies doivent être obligatoirement construit depuis les sources pour être en phase avec le runtime : Steam Linux Runtime 3.0 (Sniper). Et pour finir lors de la compilation du jeu (dépendences + binaire du jeu) il faut également faire cela dans l'image Docker du SDK de Sniper.
<br />

### Execution - Steam Linux / Steam Deck
#### Explications
Pour Steam Linux et Steam Deck, tous les jeux exécutés sous ***Steam Linux Runtime 3.0 (Sniper)*** (et d'autres versions comme Soldier ou Scout dans certains cas) sont lancés dans un conteneur géré par : ***pressure-vessel***.
- Pressure-vessel est l'outil de conteneurisation utilisé par Steam pour exécuter les jeux dans un environnement isolé, que ce soit sur Steam Linux (distributions Linux classiques) ou sur Steam Deck (qui utilise SteamOS, basé sur Linux).
- Tous les jeux sous Steam Linux Runtime 3.0 (Sniper), qu'ils soient natifs Linux ou Windows (via Proton), sont exécutés dans un conteneur créé par pressure-vessel. Cela inclut :
  - Les jeux natifs Linux ciblant Steam Linux Runtime 3.0 (Sniper).
  - Les jeux Windows exécutés via Proton (Proton 8.0+ utilise Steam Linux Runtime 3.0 (Sniper), versions antérieures utilisent Steam Linux Runtime 2.0 (Soldier)).
  - Les jeux natifs Linux ciblant Steam Linux Runtime 1.0 (Scout) lorsqu'ils sont exécutés via le mode "scout-on-soldier", qui utilise également un conteneur.
- Rôle de ***pressure-vessel*** :
  - Il isole le jeu dans un environnement prévisible, basé sur les bibliothèques du runtime (par exemple, Debian 11 pour Steam Linux Runtime 3.0 (Sniper)).
  - Il intègre les pilotes graphiques du système hôte (GPU) pour des performances optimales.
  - Il gère l'accès aux fichiers (comme le répertoire personnel privé) et aux périphériques (comme les contrôleurs).

#### Tester un jeu non-Steam avec pressure-vessel - Steam Linux
Pour tester un jeu encore non distribué sur Steam dans un conteneur pressure-vessel, afin de valider sa compatibilité avec l'environnement Steam Linux Runtime 3.0 (Sniper) qui est l'environnement d'execution de Steam Linux et du Steam Deck.
1. Il faut un PC de développement Linux.
2. Télécharger et Installer Steam.
3. Téléchargez le runtime Steam Linux Runtime 3.0 (Sniper) via Steam :
```bash
# Sur Windows mettres cela dans un onglet de navigateur est faire "ENTER"
steam steam://install/1628350
```
Cela installe Sniper dans votre bibliothèque Steam, généralement dans : `~/.steam/steam/steamapps/common/SteamLinuxRuntime_sniper/`. <br />
4. Lancer le jeu avec le script run de Steam Linux Runtime 3.0 (Sniper) :
```bash
cd /chemin/vers/votre/jeu
~/.steam/steam/steamapps/common/SteamLinuxRuntime_sniper/run \
    -- \
    ./votre_jeu_executable
```
- Le `--` sépare les options de pressure-vessel des arguments du jeu. Vous pouvez ajouter des options comme :
- `--shell=instead` : Pour ouvrir un shell interactif dans le conteneur avant de lancer le jeu.
- `--devel` : Pour activer le mode développeur (utile pour le débogage).
- `--terminal=tty` : Pour connecter l'entrée standard au terminal.

#### Tester un jeu non-Steam avec pressure-vessel - Steam Deck
Pour tester un jeu encore non distribué sur Steam dans un conteneur pressure-vessel, afin de valider sa compatibilité avec l'environnement Steam Linux Runtime 3.0 (Sniper) qui est l'environnement d'execution de Steam Linux et du Steam Deck. <br />

Configuration du client (sur le PC de développement) : <br />
1. Il faut un PC de développement sois Linux ou Windows.
2. Télécharger et Installer Steam.
3. Téléchargez et Installer `SteamOS Devkit Client` via Steam :
```bash
# Sur Windows mettres cela dans un onglet de navigateur est faire "ENTER"
steam://install/943760

# Sur Linux, exécutez dans un terminal :
steam steam://install/943760
```
4. Vous pouvez lancer l'outil (SteamOS Devkit Client) depuis votre bibliothèque Steam, dans la catégorie « Logiciels ». <br />
   Si vous préférez l'exécuter sous forme de ligne de commande, suivez ces étapes :
   ```bash
   # Windows
   windows-client/devkit-gui.exe
   
   # Linux (Dans le répertoire linux-client)
   python3.9 ./devkit-gui-cp39.pyz
   ```

Configuration du serveur (sur Steam Deck) : <br />
1. Activer le mode développeur : Appuyer sur le boutton "Steam" sur la console puis "Paramètres" à l'écran -> Système -> "Activer le mode développement" cocher le.
2. Mettre à jour le système SteamOS avec la dernière version : Appuyer sur le boutton "Steam" sur la console puis "Paramètres" à l'écran -> Système -> Rechercher une mise à jour.
3. Il faut impérativement que `le PC de développement ET le Steam Deck` sois connecté sur le même réseau local.
4. Récupérer l'IP du Steam Deck : Appuyer sur le boutton "Steam" sur la console puis "Paramètres" à l'écran -> Internet -> Cliquer sur le réseau ou nous somme actuellement connecté, puis dans adresse IPV4 il y a "Adresse IP", noté là pour la rentrée dans l'outil SteamOS Devkit Client côté PC.
5. Appuyer sur le boutton "Steam" sur la console puis "Développement" tout en bas de la console, cliquer sur "Association à la machine de développement", puis côté PC dans l'outil SteamOS Devkit Client il faut déjà avoir préremplie l'IP du Steam Deck et laisser le port 32000 par défault, puis cliquer sur "Connect", la on reçois un message sur le Steam Deck il faut confirmer pour appareiler le PC de développement et le Steam Deck ensembles.
6. Dans l'outil SteamOS Devkit Client on devrais être synchroniser à présent avec le Steam Deck, il faut penser aussi à cochées dans l'onglet `DevKits` :
   - RenderDoc captures enabled (lors de l'utilisation de Vulkan on peux debug nos shaders etc)
   - Draw performance overlay
7. Aller dans l'onglet `Title Upload`, pour déployer son jeu.
8. Configuration recommandée :
   - Name: nom_du_jeu (sans espaces ou caractère spécial)
   - Local Folder: Le dossier ou ce trouve l'executable du jeu
   - Clean upload:
     - Coché la case "Delete extranenous remote files" : Cela garantit que seuls les fichiers de la nouvelle version de votre jeu sont présents sur le Steam Deck, évitant les interférences avec des fichiers obsolètes ou inutiles précédemment déployer.
     - Coché la case "Verify checksums" : Cela garantit que les fichiers transférés sur le Steam Deck sont identiques à ceux sur votre PC, en comparant leurs sommes de contrôle (généralement via un algorithme comme MD5 ou SHA). Si un fichier est corrompu ou modifié pendant le transfert, l'outil le détectera et pourra le retransférer.
    - Start Command: rc2d_example.exe (le binaire du jeu)
    - Steam Play : cochez cette case si vous chargez des exécutables binaires Windows qui doivent être lancés par Steam Play via Proton.
    - Auto upload: Coché la case `Auto upload upon build success notification`, est une fonctionnalité avancée qui permet de déclencher automatiquement le déploiement de votre jeu sur le Steam Deck dès que votre build est réussie.

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
Apple Silicon :
- Tous les modèles macOS Apple Silicon (M1, M2, M3, M4) et plus récent

Intel :
- MacBook Pro 2018+
- MacBook Air 2020 Retina
- iMac 2019+
- Mac mini 2018+
- Mac Pro 2019
- iMac Pro 2017

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


  # macOS :
  1. Requirements : MacOS X >= 14.0
  2. Download and Install xCode >= 15.2.0
  3. Download and Install Command Line Tools : xcode-select --install
  4. Download and Install CMake >= 3.28 : brew install cmake


  # Android (run in Windows) :
  1. Download and Install : Android Studio 2023.1.1 or newer
  2. Add environment variable: ANDROID_HOME for path SDK Android (SDK Manager path)
  3. Download and Install CMake >= 3.28 : https://cmake.org/download/ and add PATH ENVIRONMENT.
  4. Download and Install Java JDK LTS (Oracle) == 17.0.10


  # iOS (only macOS) :
  1. Requirements : MacOS X >= 14.0
  2. Download and Install Command Line Tools : xcode-select --install
  3. Download and Install xCode >= 15.2.0
  4. Download and Install SDK iOS >= 17.0.0
  5. Download and Install brew : /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  6. Download and Install cmake : brew install cmake 
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

SDL=https://github.com/libsdl-org/SDL.git:release-3.2.12
SDL_image=https://github.com/libsdl-org/SDL_image.git:release-3.2.4
SDL_ttf=https://github.com/libsdl-org/SDL_ttf.git:release-3.2.2
#SDL_mixer=https://github.com/libsdl-org/SDL_mixer.git:release-3.2.0
SDL_shadercross=https://github.com/libsdl-org/SDL_shadercross.git:378f742d3023f6be7d9278908d0e47bf33fac361
Crzgames_LibCpp_Windows=https://github.com/CrzGames/Crzgames_LibCpp_Windows.git:16be49eb0b8ea63abc5b22558f660135e0948627
Crzgames_LibCpp_Linux=https://github.com/CrzGames/Crzgames_LibCpp_Linux.git:393760fa85ee0c46ff5a5a3d894748ecafca4ff9
Crzgames_LibCpp_Android=https://github.com/CrzGames/Crzgames_LibCpp_Android.git:688c153891bfd8eb8adc449f7886ee22711dc4c6
#OpenSSL_Apple=https://github.com/krzyzanowskim/OpenSSL.git:3.3.3001
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
