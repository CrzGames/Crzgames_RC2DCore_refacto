# CMake 3.25.0 est la version minimale requise pour ce projet
cmake_minimum_required(VERSION 3.25.0)

# Projet 
project(rc2d LANGUAGES C)

# Découper la version en parties
#string(REPLACE "." ";" VERSION_LIST ${APP_VERSION})
#list(GET VERSION_LIST 0 APP_VERSION_MAJOR)
#list(GET VERSION_LIST 1 APP_VERSION_MINOR)
#list(GET VERSION_LIST 2 APP_VERSION_PATCH)

# Spécifier la version C standard
set(CMAKE_C_STANDARD 99)
# Cela signifie que le compilateur doit respecter la version C standard spécifiée
set(CMAKE_C_STANDARD_REQUIRED True)

# Option pour activer la compilation des shaders en ligne (hot reload) pour SDL3_shadercross
option(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED "Enable hot reload shaders with SDL3_shadercross" ON)

# Option pour activer le module RC2D_data
option(RC2D_DATA_MODULE_ENABLED "Enable RC2D_data module" ON)

# Option pour activer le module RC2D_net (via RCEnet, fork de ENet)
option(RC2D_NET_MODULE_ENABLED "Enable RC2D_net module" ON)

# Option pour activer le module RC2D_onnx (via ONNX Runtime)
option(RC2D_ONNX_MODULE_ENABLED "Enable RC2D_onnx module" ON)

# Option pour le niveau d'assertion
option(RC2D_ASSERT_LEVEL "Niveau des assertions (0=none, 1=release, 2=debug, 3=paranoid)" 3)

# Option Tests unitaires avec Criterion
option(RC2D_BUILD_TESTS "Build unit tests with Criterion" OFF)

# Option pour construire les exemples
option(RC2D_BUILD_EXAMPLES "Build examples" ON)

# Option pour choisir entre statique et dynamique
option(RC2D_BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Dossier de sortie
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Configuration multi-plateforme avec versions minimales
if(WIN32)
  set(CMAKE_SYSTEM_VERSION 10.0.10240 CACHE STRING "Windows 10 Minimum Version" FORCE)
elseif(APPLE)
  if(CMAKE_OSX_SYSROOT MATCHES "macosx")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "macOS Deployment Target" FORCE)
  elseif(CMAKE_OSX_SYSROOT MATCHES "iphoneos")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "18.0" CACHE STRING "iOS Deployment Target" FORCE)
  endif()
elseif (UNIX AND NOT APPLE)
  # Linux-LIKE utilisera Vulkan
elseif(ANDROID)
  set(CMAKE_ANDROID_API 28 CACHE STRING "Android API Level" FORCE)
endif()

# Configuration des drapeaux du compilateur
if(MSVC)
  # /W3 : Niveau d'avertissement 3 (equivalent a -Wall)
  # /W4 : Niveau d'avertissement 4 (equivalent a -Wextra)
  # /permissive- : Spécifie un mode de compilation strict pour suivre les standards C++ 
  # /wd4100 : Désactive l'avertissement pour les paramètres de fonction non utilisés (C4100)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /W4 /permissive- /wd4100")
else()
  # -Wall : Active tous les avertissements de base
  # -Wextra : Active des avertissements supplémentaires
  # -pedantic : Enforce strictement les standards C++
  # -Wno-unused-parameter : Désactive les avertissements pour les paramètres de fonction non utilisés
  # -Wno-error=strict-prototypes : Désactive les erreurs pour les prototypes de fonction stricts
  # -Wno-newline-eof : Désactive les avertissements pour les nouvelles lignes à la fin du fichier
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-error=strict-prototypes -Wno-newline-eof")
endif()

# SDL3_ttf, SDL3_image, SDL3_mixer : 
# Permet de dire à CMake de ne pas chercher les dépendances dans le système,
# mais de les utiliser à partir du répertoire "dependencies", dont également les submodules
# des repositories de SDL3_ttf, SDL3_mixer, SDL3_image
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)
#set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)

# SDL3_image : Désactiver explicitement tous les formats d'image qui sont activé par défault.
# Il restera ces formats activé par défault : STB, PNG et SVG
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_BMP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_GIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JPG OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_LBM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PCX OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_QOI OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TGA OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XCF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XPM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XV OFF CACHE BOOL "" FORCE)

# SDL3, SDL3_ttf, SDL3_image, SDL3_mixer : 
# Permet de dire à CMake de ne pas chercher les dépendances dans le système,
# mais de les utiliser à partir du répertoire "dependencies", dont également les submodules
# des repositories de SDL3, SDL3_ttf, SDL3_mixer, SDL3_image
add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)
add_subdirectory(dependencies/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(dependencies/SDL_image EXCLUDE_FROM_ALL)
#add_subdirectory(dependencies/SDL_mixer EXCLUDE_FROM_ALL)

# Determiner l'architecture de la plateforme
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|x86_64)$")
  set(RC2D_ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|aarch64|arm64)$")
  set(RC2D_ARCH "arm64")
else()
  set(RC2D_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Chemin vers les dépendances
set(DEPENDENCIES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
set(CRZGAMES_LIBRARIES "${DEPENDENCIES_PATH}/Crzgames_Libraries")

function(rc2d_configure_shadercross target_name)
  if (APPLE AND CMAKE_OSX_SYSROOT MATCHES "macosx")
    # Repertoire cible des binaires de shadercross pour "examples/shaders/tools"
    set(SHADERCROSS_BINARY_DIR "${CRZGAMES_LIBRARIES}/macos/bin/SDL3_shadercross/x64_arm64")

    # Répertoire cible pour les outils de shaders, pour la compilation hors ligne des shaders
    set(SHADER_TOOLS_DIR "${PROJECT_SOURCE_DIR}/examples/shaders/tools")

    # Crée le dossier "examples/shaders/tools" AVANT toute copie
    add_custom_command(TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_TOOLS_DIR}"
    )

    # Copier les binaires de shadercross dans le répertoire "examples/shaders/tools"
    file(GLOB_RECURSE SHADERCROSS_BINARY
      "${SHADERCROSS_BINARY_DIR}/*"
    )
    foreach(FILE IN LISTS SHADERCROSS_BINARY)
      get_filename_component(FILE_NAME "${FILE}" NAME)
      add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE}"
                "${SHADER_TOOLS_DIR}/${FILE_NAME}"
      )
    endforeach()

    # Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est activé, on link la lib shadercross
    if(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED)
      # Link le dossier include de shadercross à la target
      target_include_directories(${target_name} PRIVATE
        "${CRZGAMES_LIBRARIES}/macos/include"
      )

      # Récupérer le dossier de la lib shadercross
      target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/macos/lib/SDL3_shadercross/x64_arm64")

      # Link la lib shadercross
      target_link_libraries(${target_name} PRIVATE
        SDL3_shadercross
      )

      # Copier aussi les DLLs dans le répertoire de l'exécutable
      file(GLOB_RECURSE SHADERCROSS_DLLS
        "${CRZGAMES_LIBRARIES}/macos/lib/SDL3_shadercross/x64_arm64/*"
      )
      foreach(FILE IN LISTS SHADERCROSS_DLLS)
        get_filename_component(FILE_NAME "${FILE}" NAME)
        add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${FILE}"
                  "$<TARGET_FILE_DIR:${target_name}>/${FILE_NAME}"
        )
      endforeach()
    endif()
  elseif(WIN32)
    # Repertoire cible des dll + binaire de shadercross pour "examples/shaders/tools"
    set(SHADERCROSS_DLL_BINARY_DIR "${CRZGAMES_LIBRARIES}/windows/bin/SDL3_shadercross/${RC2D_ARCH}")

    # Répertoire cible pour les outils de shaders, pour la compilation hors ligne des shaders
    set(SHADER_TOOLS_DIR "${PROJECT_SOURCE_DIR}/examples/shaders/tools")

    # Crée le dossier "examples/shaders/tools" AVANT toute copie
    add_custom_command(TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_TOOLS_DIR}"
    )

    # Copier les DLLs et binaires de shadercross dans le répertoire "examples/shaders/tools"
    file(GLOB_RECURSE SHADERCROSS_DLLS_BINARY
      "${SHADERCROSS_DLL_BINARY_DIR}/*.dll"
      "${SHADERCROSS_DLL_BINARY_DIR}/*.exe"
    )
    foreach(FILE IN LISTS SHADERCROSS_DLLS_BINARY)
      get_filename_component(FILE_NAME "${FILE}" NAME)
      add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE}"
                "${SHADER_TOOLS_DIR}/${FILE_NAME}"
      )
    endforeach()

    # Si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est activé, on link la lib shadercross
    if(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED)
      # Link le dossier include de shadercross à la target
      target_include_directories(${target_name} PRIVATE
        "${CRZGAMES_LIBRARIES}/windows/include"
      )

      # Récupérer le dossier de la lib shadercross
      target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/windows/lib/SDL3_shadercross/${RC2D_ARCH}")

      # Link la lib shadercross
      target_link_libraries(${target_name} PRIVATE
        SDL3_shadercross
      )

      # Copier aussi les DLLs dans le répertoire de l'exécutable
      file(GLOB_RECURSE SHADERCROSS_DLLS
        "${CRZGAMES_LIBRARIES}/windows/bin/SDL3_shadercross/${RC2D_ARCH}/*.dll"
      )
      foreach(FILE IN LISTS SHADERCROSS_DLLS)
        get_filename_component(FILE_NAME "${FILE}" NAME)
        add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${FILE}"
                  "$<TARGET_FILE_DIR:${target_name}>/${FILE_NAME}"
        )
      endforeach()
    endif()
  elseif(UNIX AND NOT APPLE AND RC2D_PLATFORM STREQUAL "SLR3-SNIPER") # Steam Deck / Steam Linux
    # Repertoire cible du binaire de shadercross pour "examples/shaders/tools"
    set(SHADERCROSS_BINARY_DIR "${CRZGAMES_LIBRARIES}/slr3-sniper/bin/SDL3_shadercross/${RC2D_ARCH}")
    
    # Repertoire cible des dll + binaire de shadercross pour "examples/shaders/tools"
    set(SHADER_TOOLS_DIR "${PROJECT_SOURCE_DIR}/examples/shaders/tools")

    # Crée le dossier "examples/shaders/tools" AVANT toute copie
    add_custom_command(TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_TOOLS_DIR}"
    )

    file(GLOB_RECURSE SHADERCROSS_BINARY
      "${SHADERCROSS_BINARY_DIR}/*"
    )
    foreach(FILE IN LISTS SHADERCROSS_BINARY)
      get_filename_component(FILE_NAME "${FILE}" NAME)
      add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE}"
                "${SHADER_TOOLS_DIR}/${FILE_NAME}"
      )
    endforeach()

    if(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED)
      target_include_directories(${target_name} PRIVATE
        "${CRZGAMES_LIBRARIES}/slr3-sniper/include"
      )

      # Récupérer le dossier de la lib shadercross
      target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/slr3-sniper/lib/SDL3_shadercross/${RC2D_ARCH}")

      # Link la lib shadercross
      target_link_libraries(${target_name} PRIVATE
        SDL3_shadercross
      )

      file(GLOB_RECURSE SHADERCROSS_DLLS
        "${CRZGAMES_LIBRARIES}/slr3-sniper/lib/SDL3_shadercross/${RC2D_ARCH}/*"
      )
      foreach(FILE IN LISTS SHADERCROSS_DLLS)
        get_filename_component(FILE_NAME "${FILE}" NAME)
        add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${FILE}"
                  "$<TARGET_FILE_DIR:${target_name}>/${FILE_NAME}"
        )
      endforeach()
    endif() 
  endif()
endfunction()

# Fonction pour OpenSSL :
# - Sur les plateformes Apple (macOS, iOS, tvOS) : utilise XCFramework, on télécharge et extrait OpenSSL.xcframework
# - Sur Windows : utilise les librairies précompilées statiques de Crzgames_LibCpp_Windows
# - Sur Android : à faire
# - Sur Linux : à faire
# Puis lors de l'appel de la fonction, on link les librairies OpenSSL avec le target_name
function(rc2d_configure_openssl target_name)
  if(APPLE)
    # Link le dossier include de OpenSSL à la target
    target_include_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/macos/include")

    # Rajouter le répertoire de la lib OpenSSL (libssl_static et libcrypto_static), pour le linker ensuite
    target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/macos/lib/OpenSSL/X64_arm64")

    # Link les librairies OpenSSL
    target_link_libraries(${target_name} PRIVATE
      libssl    # OpenSSL
      libcrypto # OpenSSL
    )
  elseif(WIN32)
    # Link le dossier include de OpenSSL à la target
    target_include_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/windows/include")

    # Rajouter le répertoire de la lib OpenSSL (libssl_static et libcrypto_static), pour le linker ensuite
    target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/windows/lib/OpenSSL/${RC2D_ARCH}")

    # Link les librairies OpenSSL
    target_link_libraries(${target_name} PRIVATE
      libssl    # OpenSSL
      libcrypto # OpenSSL
      Crypt32   # OpenSSL CryptoAPI (Librarie native de Windows, obligatoire)
      ws2_32    # OpenSSL Winsock2  (Librarie native de Windows, obligatoire)
    )
  elseif (UNIX AND NOT APPLE)
    # Link le dossier include de OpenSSL à la target
    target_include_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/linux/include")

    # Rajouter le répertoire de la lib OpenSSL (libssl_static et libcrypto_static), pour le linker ensuite
    target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/linux/lib/OpenSSL/${RC2D_ARCH}")

    # Link les librairies OpenSSL
    target_link_libraries(${target_name} PRIVATE
      libssl    # OpenSSL
      libcrypto # OpenSSL
    )
  elseif(UNIX AND NOT APPLE AND RC2D_PLATFORM STREQUAL "SLR3-SNIPER")
    # Link le dossier include de OpenSSL à la target
    target_include_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/slr3-sniper/include")

    # Rajouter le répertoire de la lib OpenSSL (libssl_static et libcrypto_static), pour le linker ensuite
    target_link_directories(${target_name} PRIVATE "${CRZGAMES_LIBRARIES}/slr3-sniper/lib/OpenSSL/${RC2D_ARCH}")

    # Link les librairies OpenSSL
    target_link_libraries(${target_name} PRIVATE
      libssl    # OpenSSL
      libcrypto # OpenSSL
    )
  endif()
endfunction()

# Headers de RC2D + les lib qui sont dans le répertoire "external"
function(rc2d_include_headers target_name)
  target_include_directories(${target_name} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/external"
  )
endfunction()

# Fonction pour les target_compile_definitions par target
function(rc2d_target_compile_definitions target_name)
  # Définitions de compilation pour le rechargement à chaud des shaders
  if(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED)
    target_compile_definitions(${target_name} PRIVATE RC2D_GPU_SHADER_HOT_RELOAD_ENABLED=1)
  else()
    target_compile_definitions(${target_name} PRIVATE RC2D_GPU_SHADER_HOT_RELOAD_ENABLED=0)
  endif()

  # Définitions de compilation pour le niveau d'assertion
  target_compile_definitions(${target_name} PRIVATE RC2D_ASSERT_LEVEL=${RC2D_ASSERT_LEVEL})

  # Les définitions de compilation pour les modules RC2D
  if(RC2D_DATA_MODULE_ENABLED)
    target_compile_definitions(${target_name} PRIVATE RC2D_DATA_MODULE_ENABLED=1)
  else()
    target_compile_definitions(${target_name} PRIVATE RC2D_DATA_MODULE_ENABLED=0)
  endif()

  if(RC2D_NET_MODULE_ENABLED)
    target_compile_definitions(${target_name} PRIVATE RC2D_NET_MODULE_ENABLED=1)
  else()
    target_compile_definitions(${target_name} PRIVATE RC2D_NET_MODULE_ENABLED=0)
  endif()

  if(RC2D_ONNX_MODULE_ENABLED)
    target_compile_definitions(${target_name} PRIVATE RC2D_ONNX_MODULE_ENABLED=1)
  else()
    target_compile_definitions(${target_name} PRIVATE RC2D_ONNX_MODULE_ENABLED=0)
  endif()
endfunction()

# Sources du projet RC2D
file(GLOB_RECURSE SOURCES
  "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Choisir entre bibliothèque dynamique/partagées ou statique pour RC2D
if(ANDROID)
  # Pour Android, il faut obligatoirement mettre SHARED
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
  if(RC2D_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
  else()
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
    # Rajouter le suffixe "_static" au nom de la bibliothèque statique, pour éviter les conflits
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_static")
  endif()
endif()

# Compiler les définitions pour la target RC2D
rc2d_target_compile_definitions(${PROJECT_NAME})

# Inclure les répertoires d'en-tête (headers)
rc2d_include_headers(${PROJECT_NAME})

# Link les dépendances communes à toutes les plateformes
target_link_libraries(${PROJECT_NAME} PRIVATE
  #SDL3_mixer::SDL3_mixer -> FIXME: SDL3_mixer n'ai pas encore disponible
  SDL3_image::SDL3_image
  SDL3_ttf::SDL3_ttf
  SDL3::SDL3
)

# Linker la dépendance OpenSSL non commune à toutes les plateformes
# Cela dépend de la plateforme, donc on le fait dans la fonction rc2d_configure_openssl
rc2d_configure_openssl(${PROJECT_NAME})

# Linker SDL3_shadercross selon la plateforme
rc2d_configure_shadercross(${PROJECT_NAME})

# Génération des sorties pour RC2D
if(APPLE)
  # Pour Apple (macOS, iOS, tvOS) générer un bundle cross-platform : XCFramework
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/xcframework
    COMMAND xcodebuild -create-xcframework
      -library $<TARGET_FILE:${PROJECT_NAME}>
      -headers ${PROJECT_SOURCE_DIR}/include
      -output ${CMAKE_BINARY_DIR}/xcframework/${PROJECT_NAME}.xcframework
  )
endif()

# Pour l'exemple RC2D
if(RC2D_BUILD_EXAMPLES)
  # Ajouter les fichiers source de l'exemple
  # Cela inclut tous les fichiers .c dans le répertoire "examples/src" et ses sous-répertoires
  file(GLOB_RECURSE RC2D_EXAMPLE_SOURCES
    "${PROJECT_SOURCE_DIR}/examples/src/*.c"
  )

  # Créer un exécutable pour l'exemple
  add_executable(rc2d_example ${RC2D_EXAMPLE_SOURCES})

  # Définir des flags de compilation pour le mode Release (NDEBUG = No Debug, donc Release)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")

  # Compiler les définitions pour la target rc2d_example
  rc2d_target_compile_definitions(rc2d_example)

  # Les fichiers d'en-tête (headers) de l'exemple sont dans le répertoire "examples/include"
  target_include_directories(rc2d_example PRIVATE
    "${PROJECT_SOURCE_DIR}/examples/include"
  )

  # Inclure les répertoires d'en-tête (headers)
  rc2d_include_headers(rc2d_example)

  # Linker la dépendance OpenSSL non commune à toutes les plateformes
  # Cela dépend de la plateforme, donc on le fait dans la fonction rc2d_configure_openssl
  rc2d_configure_openssl(rc2d_example)

  # Linker SDL3_shadercross selon la plateforme
  rc2d_configure_shadercross(rc2d_example)

  # Link les dépendances communes à toutes les plateformes + on link la lib RC2D pour finir
  target_link_libraries(rc2d_example PRIVATE
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3::SDL3
    ${PROJECT_NAME} # RC2D
  )
endif()

# Pour les tests unitaires RC2D
if(RC2D_BUILD_TESTS)
  enable_testing()

  # Ajouter les fichiers source des tests unitaires
  # Cela inclut tous les fichiers .c dans le répertoire "tests" et ses sous-répertoires
  file(GLOB_RECURSE RC2D_TEST_SOURCES
    "${PROJECT_SOURCE_DIR}/tests/*.c"
  )

  # Créer un exécutable pour les tests
  add_executable(rc2d_tests ${RC2D_TEST_SOURCES})

  # Compiler les définitions pour la target rc2d_tests
  rc2d_target_compile_definitions(rc2d_tests)

  # Inclure les répertoires d'en-tête (headers)
  rc2d_include_headers(rc2d_tests)

  # Linker SDL3_shadercross selon la plateforme
  rc2d_configure_shadercross(rc2d_tests)

  # Linker la dépendance OpenSSL non commune à toutes les plateformes
  # Cela dépend de la plateforme, donc on le fait dans la fonction rc2d_configure_openssl
  rc2d_configure_openssl(rc2d_tests)

  # Linker Criterion selon la plateforme
  if(APPLE)
  elseif(WIN32)
  
  elseif(LINUX)

  endif()

  # Link les dépendances communes à toutes les plateformes + on link la lib RC2D pour finir
  target_link_libraries(rc2d_tests PRIVATE
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3::SDL3
    ${PROJECT_NAME} # RC2D
  )

  # Permet de lancer les tests avec la commande "ctest" intégrée dans CMake
  add_test(NAME RC2D_AllTests COMMAND rc2d_tests)
endif()