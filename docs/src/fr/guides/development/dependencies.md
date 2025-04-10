
# Managing Dependencies

In the development of RC2D, managing dependencies is crucial for ensuring that all components of the engine work seamlessly together. This document outlines the libraries and tools required by RC2D, their versions, and how they are integrated within the engine. Additionally, it provides a guide on updating these dependencies to maintain the engine's efficiency and security.

## Updating Dependencies
1. Utilisez la commande pour mettre à jour le sous-module à sa dernière version disponible dans le dépôt distant. Cela téléchargera les dernières modifications du sous-module depuis son dépôt distant et les incorporera dans votre répertoire principal :
   ```bash
   git submodule update --remote

   git add .
   git commit -m "chore: update all dependencies"
   git push
   ```

<br /><br />

## Dependencies and Versions

### Libraries
Below is a list of dependencies required by RC2D, their versions, whether the original library was modified for RC2D, and their primary purpose in the engine.

| Dependency   | Version | Modified for RC2D | Purpose          | Language  |
|--------------|---------|-------------------|------------------|-----------|
| Enet         | 1.3.17  |                   | Networking       | C
| nlohmann     | 3.11.13 |                   | JSON Parsing     | C/C++     |
| SDL2         | 2.30.0  |                   | Core Framework   | C         |
| SDL2_image   | 2.8.2   |                   | Image Loading    | C         |
| SDL2_ttf     | 2.22.0  |                   | Font Rendering   | C         |
| SDL2_mixer   | 2.8.0   |                   | Audio Management | C         |
| Base64       | 1.0.0   |                   | Encoding         | C         |
| LZ4          | 1.9.4   |                   | Compression      | C         |
| OpenSSL      | 3.2.0   |                   | Encryption / DTLS | C        |
| Monocypher   | 4.0.2   |                   | Encryption       | C         |
| AES          | 1.0.0   |                   | Encryption       | C         |
| doctest      | 2.4.11  |                   | Unit Testing     | C/C++     |
| rres         | 1.2     | ✓                 | Resource Management | C      |
| spine-c      | 4.1.00  | ✓                 | Rigging and Animation | C    |
| spine-sdl-c  | 4.1.00  | ✓                 | Rigging and Animation SDL Integration | C |

Legend:
- ✓: Indicates the library was modified for integration with RC2D.

<br />

### Tools
Below is a list of dependencies required by RC2D, concerning the tools used within the engine.

| Dependency   | Version |
|--------------|---------|
| rrespacker   | 1.5.0   |

<br /><br />


## Library Dependencies

The following table outlines the dependencies between the various libraries used in RC2D, indicating which libraries are reliant on others within the framework.

| Library      | Depends on      |
|--------------|-----------------|
| SDL2_image   | SDL2            |
| SDL2_ttf     | SDL2            |
| SDL2_mixer   | SDL2            |
| spine-sdl-c  | spine-c         |
| rres         | AES, Monocypher, LZ4 |

This structure ensures that the core functionalities provided by these libraries are seamlessly integrated and operate cohesively within the RC2D game engine ecosystem.

<br /><br />


## API documentation

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
- **Base64** - Encoding library: [Base64 GitHub](https://github.com/tobiaslocker/base64)
- **Nlohmann JSON** - JSON parsing library: [Nlohmann JSON GitHub](https://github.com/nlohmann/json)

<br />

### Tools

- **rrespacker** - Resources rres : [rrespacker User Guide](https://raylibtech.itch.io/rrespacker)