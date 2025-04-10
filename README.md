# Crzgames - RC2DCore

## 🛠 Tech Stack
- C (Language)
- A C99 Compiler
- CI / CD (Github Actions)
- CMake (Build script)
- Compiler (GCC, CL, Clang, Ninja, NDK)

<br /><br /><br /><br />


## ⚙️ Setup Environment
1. Clone the project repository and initialize all submodules using the following commands :
  ```bash
  git clone --recurse-submodules git@github.com:corentin35000/Crzgames_RC2DCore.git
  ```
2. (Optional) Download and Install Node.js >= 18.0.0 to run website documentation for GameEngine
3. Steps by Platform :
  ```bash  
  # Windows :
  1. Requirements : Windows >= 10
  1. Download and Install Visual Studio == 2022 (MSVC >= v143 + Windows SDK >= 10) : https://visualstudio.microsoft.com/fr/downloads/
  2. Download and Install CMake >= 3.28 : https://cmake.org/download/ and add PATH ENVIRONMENT.

  
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
  
<br /><br /><br /><br />


## 🔄 Updating Dependencies
1. Utilisez la commande pour mettre à jour les sous-module à sa dernière version disponible dans le dépôt distant. Cela téléchargera les dernières modifications du sous-module depuis son dépôt distant et les incorporera dans votre répertoire principal :
   ```bash
   git submodule update --remote

   git add .
   git commit -m "chore: updating dependencies"
   git push
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
