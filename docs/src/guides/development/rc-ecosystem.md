
# 🚀 RC Ecosystem

In the development of RC2D, managing dependencies is crucial for ensuring that all components of the engine work seamlessly together. This document outlines the libraries and tools required by RC2D, their versions, and how they are integrated within the engine. 

## 📚 RC Documentation
| Package      | WebSite / Repository |
|--------------|----------------------|
| RCENet       | https://github.com/corentin35000/Crzgames_RCENet |
| RC2DCore     | https://github.com/corentin35000/Crzgames_RC2DCore |
| RC2DFramework | https://librc2d.crzcommon2.com |
| RCNETCore     | https://github.com/corentin35000/Crzgames_RCNETCore |
| RCNETFramework | https://librcnet.crzcommon2.com |

<br /><br />


## 📦 RC Packages
| Package      | Version | Purpose          | Language  |
|--------------|---------|------------------|-----------|
| RCENet       | 1.5.1   | Networking       | C         |                
| RC2DCore     | 1.4.0   | Core Client      | C         |                
| RC2DFramework | 1.0.0  | Framework Game   | C++       |
| RCNETCore     | 1.0.0  | Core Server      | C         |
| RCNETFramework | 1.0.0 | Framework Server | C++       |

<br /><br />


## 📦 RC Packages Distribution
### Library RCENet
| Platform | Format | Compiler | Type |
|----------|-----------|-------|------|
| **Windows-MSVC** | `.lib / .dll` | CL | static / shared |
| **Windows-MinGW** | `.a` | GCC | static |
| **macOS** | `.a / .dylib` | Clang | static / shared |
| **visionOS** | `.a` | Clang | static |
| **tvOS** | `.a` | Clang | static |
| **iOS** | `.a` | Clang | static |
| **Android** | `.a / .so` | NDK (Clang) | static / shared |
| **Linux** | `.a / .so` | GCC | static / shared |
| **BSD** | `.a / .so` | Clang | static / shared |
| **Haiku** | `.a / .so` | Clang | static / shared |
| **Solaris** | `? / ?` | Clang | ? / ? |
| **Xbox** |  |  |
| **Nintendo Switch** |  |  |
| **Playstation** |  |  |

### Library RC2DCore
| Platform | Format | Compiler | Type |
|----------|-----------|-------|------|
| **Windows** | `.lib` | CL | static |
| **macOS** | `.a` | Clang | static |
| **Linux** | `.a` | GCC | static |
| **Android** | `.so` | NDK (Clang) | dynamic |
| **iOS** | `.a` | Clang | static |
| **HTML5** | `.a` | Clang | static |
| **Xbox** |  |  |
| **Nintendo Switch** |  |  |
| **Playstation** |  |  |

### Library RCNETCore
| Platform | Format | Compiler | Type |
|----------|-----------|-------|------|
| **Linux** | `.a` | GCC | static |

<br /><br />


## ✨ RCENet - Dependencies and Versions
There are absolutely no dependencies for RCENet

<br /><br />


## ✨ RC2D - Dependencies and Versions
### 🕹️ RC2DCore
Below is a list of dependencies required by RC2DCore, their versions, whether the original library was modified for RC2D, and their primary purpose in the engine.

| Dependency   | Version | Modified for RC2DCore | Purpose          | Language  | Source Integrated in RC2DCore | Depends on |
|--------------|---------|-------------------|------------------|-----------|-------------------|------------|
| RCENet       | 1.5.1   |                    | Networking       | C         |                   |            |         
| SDL2         | 2.30.0  |                   | Core Framework   | C         |                   | SDL2       |
| SDL2_image   | 2.8.2   |                   | Image Loading    | C         |                   | SDL2       |
| SDL2_ttf     | 2.22.0  |                   | Font Rendering   | C         |                   | SDL2       |
| SDL2_mixer   | 2.8.0   |                   | Audio Management | C         |                   |            |
| LZ4          | 1.10.0  |                   | Compression      | C         | ✓                 |            |
| OpenSSL      | 3.3.1   |                   | Encryption / DTLS | C        |                   |            |
| Monocypher   | 4.0.2   |                   | Encryption       | C         | ✓                 |            |
| AES          | 1.0.0   |                   | Encryption       | C         | ✓                 |            |
| rres         | 1.2.0   | ✓                 | Resource Management | C      | ✓                 | AES, Monocypher, LZ4 |
| spine-c      | 4.1.0   | ✓                 | Rigging and Animation | C    | ✓                 |            |
| spine-sdl-c  | 4.1.0   | ✓                 | Rigging and Animation SDL Integration | C | ✓    | spine-c    | 

### 📦 RC2DFramework
Below is a list of dependencies required by RC2DFramework, their versions, whether the original library was modified for RC2D, and their primary purpose in the engine.

| Dependency   | Version | Dependency Optional | Purpose          | Language  |
|--------------|---------|-------------------|------------------|-----------|
| cpp-httplib  | 0.16.0  | ✓                 | HTTP             | C++       |   
| nlohmann     | 3.11.13 | ✓                 | JSON Parsing     | C++       |   
| doctest      | 2.4.11  | ✓                 | Unit Testing     | C++       |   
| SDL2         | 2.30.0  |                   | Core Framework   | C         |   
| SDL2_image   | 2.8.2   |                   | Image Loading    | C         |   
| SDL2_ttf     | 2.22.0  |                   | Font Rendering   | C         |     
| SDL2_mixer   | 2.8.0   |                   | Audio Management | C         |   
| OpenSSL      | 3.3.1   |                   | Encryption / DTLS | C        |    
| RC2DCore     | 1.4.0   |                   | Core Client      | C         | 
| RCENet       | 1.5.1   |                   | Networking       | C         |                   

### 🔧 RC2DFramework Tools
Below is a list of dependencies required by RC2DFramework, concerning the tools used within the Framework.

| Dependency   | Version | 
|--------------|---------|
| rrespacker   | 1.5.0   |

<br /><br />


## ✨ RCNET - Dependencies and Versions
### 🌐 RCNETCore
Below is a list of dependencies required by RCNETCore, their versions, whether the original library was modified for RCNET, and their primary purpose in the engine.

| Dependency   | Version | Modified for RCNETCore | Purpose          | Language | Source Integrated in RCNETCore |
|--------------|---------|-------------------|------------------|-----------|--------------------
| RCENet       | 1.5.1   |                   | Networking       | C         |                   |               
| LZ4          | 1.10.0  |                   | Compression      | C         | ✓                 |
| AES          | 1.0.0   |                   | Encryption      | C         | ✓                 |
| Monocypher   | 4.0.2   |                   | Encryption      | C         | ✓                 |
| OpenSSL      | 3.3.1   |                   | Encryption / DTLS | C        |                   |
| NATS         | 3.8.2   |                   | Broker Message   | C         |                   |

### 📦 RCNETFramework
Below is a list of dependencies required by RCNETFramework, their versions, whether the original library was modified for RCNET, and their primary purpose in the engine.

| Dependency   | Version | Dependency Optional | Purpose         | Language  |
|--------------|---------|---------------------|------------------|-----------|
| nlohmann     | 3.11.13 | ✓                 | JSON Parsing     | C++       |
| doctest      | 2.4.11  | ✓                 | Unit Testing     | C++       |
| jwt-cpp      | 0.7.0   | ✓                 | Token            | C++       |
| cpp-httplib  | 0.16.0  | ✓                 | HTTP             | C++       |   
| picojson     | ?       | ✓                 | JSON             | C++       |
| RCENet       | 1.5.1   |                   | Networking        | C         |               
| OpenSSL      | 3.3.1   |                   | Encryption / DTLS | C        |
| NATS         | 3.8.2   |                   | Broker Message   | C         |
| RCNETCore    | 1.0.0   |                   | Core Server      | C         |

<br /><br />


## 📖 API Documentation

Below is a list of external libraries and tools the engine relies on, along with links to their documentation or home pages for further details.

### Libraries

- **Enet** - Networking library: [Enet User Guide](http://enet.bespin.org/usergroup0.html)
- **SDL2** - Core framework: [SDL2 API by Category](https://wiki.libsdl.org/SDL2/APIByCategory)
- **SDL2_image** - Image loading library: [SDL2_image CategoryAPI](https://wiki.libsdl.org/SDL_image/CategoryAPI)
- **SDL2_ttf** - Font rendering library: [SDL2_ttf CategoryAPI](https://wiki.libsdl.org/SDL_ttf/CategoryAPI)
- **SDL2_mixer** - Audio management library: [SDL2_mixer CategoryAPI](https://wiki.libsdl.org/SDL2_mixer/CategoryAPI)
- **LZ4** - Compression library: [LZ4 GitHub](https://github.com/lz4/lz4)
- **OpenSSL** - Encryption library: [OpenSSL Documentation](https://www.openssl.org/docs/)
- **spine-c** - Rigging and animation library: [Spine-c Documentation](https://fr.esotericsoftware.com/spine-c)
- **spine-sdl-c** - Spine integration with SDL: [Spine-sdl Documentation](https://fr.esotericsoftware.com/spine-sdl)
- **Rres** - Resource management tool: [Rres GitHub](https://github.com/raysan5/rres)
- **Doctest** - Testing framework: [Doctest GitHub](https://github.com/doctest/doctest?tab=readme-ov-file)
- **AES** - Encryption library: [Tiny AES in C](https://github.com/kokke/tiny-AES-c)
- **Monocypher** - Encryption library: [Monocypher](https://monocypher.org/)
- **Nlohmann JSON** - JSON parsing library: [Nlohmann JSON GitHub](https://github.com/nlohmann/json)

<br />

### Tools

- **rrespacker** - Resources rres : [rrespacker User Guide](https://raylibtech.itch.io/rrespacker)