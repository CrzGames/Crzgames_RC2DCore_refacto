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

# Active la compilation des shaders en ligne (hot reload) pour SDL3_shadercross
option(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED "Enable hot reload with SDL3_shadercross" ON)

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
  # Windows 10+ pour D3D12 (features level 11_1)
  set(CMAKE_SYSTEM_VERSION 10.0.10240 CACHE STRING "Windows 10 Minimum Version" FORCE)
elseif(APPLE)
  # macOS 10.14+ et iOS/tvOS 13.0+ pour Metal
  message(STATUS "Detected Apple platform with SYSROOT: ${CMAKE_OSX_SYSROOT}")

  if(CMAKE_OSX_SYSROOT MATCHES "macosx")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "macOS Deployment Target" FORCE)
  elseif(CMAKE_OSX_SYSROOT MATCHES "iphoneos")
    set(CMAKE_OSX_ARCHITECTURES "arm64;arm64e" CACHE STRING "iOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "iOS Deployment Target" FORCE)
  elseif(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "iOS Simulator Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "iOS Simulator Deployment Target" FORCE)
  elseif(CMAKE_OSX_SYSROOT MATCHES "appletvos")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "tvOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "tvOS Deployment Target" FORCE)
  elseif(CMAKE_OSX_SYSROOT MATCHES "appletvsimulator")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "tvOS Simulator Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "tvOS Simulator Deployment Target" FORCE)
endif()
elseif (UNIX AND NOT APPLE)
  # Linux-LIKE utilisera Vulkan
elseif(ANDROID)
  # API 24 (Android 7.0+) pour Vulkan
  set(CMAKE_ANDROID_API 24 CACHE STRING "Android API Level" FORCE)
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
# mais de les utiliser à partir du répertoire "vendored", dont également les submodules
# des repositories de SDL3_ttf, SDL3_mixer et SDL3_image
set(SDLTTF_VENDORED ON)
set(SDLIMAGE_VENDORED ON)
#set(SDLMIXER_VENDORED ON)

# SDL3_image : Désactiver explicitement tous les formats qui sont activé par défault, sauf : PNG et SVG
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
# mais de les utiliser à partir du répertoire "vendored", dont également les submodules
# des repositories de SDL3, SDL3_ttf, SDL3_mixer et SDL3_image
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
#add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)

# Désactiver le hot reload des shaders en ligne pour certaines plateformes
if(APPLE)
  if(CMAKE_OSX_SYSROOT MATCHES "iphoneos" OR
     CMAKE_OSX_SYSROOT MATCHES "appletvos" OR
     CMAKE_OSX_SYSROOT MATCHES "iphonesimulator" OR
     CMAKE_OSX_SYSROOT MATCHES "appletvsimulator")
    set(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED OFF CACHE BOOL "Hot reload is not supported on mobile Apple platforms" FORCE)
  endif()
elseif(ANDROID)
  set(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED OFF CACHE BOOL "Hot reload is not supported on Android" FORCE)
endif()

# Fonction pour configurer SDL3_shadercross
function(rc2d_configure_shadercross target_name)
  if(WIN32)
    # Le répertoire de Shadercross est dans le répertoire "vendored" du projet
    set(SHADERCROSS_ROOT "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows")

    # TODO: Il faudra detecter la version de Windows (x64 ou ARM64) et choisir le bon répertoire
    # En attendant, on force le répertoire x64
    set(SHADERCROSS_ARCH "x64")

    # Link include et lib de Shadercross
    target_include_directories(${target_name} PRIVATE "${SHADERCROSS_ROOT}/include")
    target_link_directories(${target_name} PRIVATE "${SHADERCROSS_ROOT}/lib/SDL3_shadercross/${SHADERCROSS_ARCH}")
    target_link_libraries(${target_name} PRIVATE SDL3_shadercross.lib)

    # Le répertoire de sortie pour les fichiers de Shadercross pour la compilation hors ligne
    set(TOOLS_OUT "${PROJECT_SOURCE_DIR}/examples/shaders/tools")
    # Créer si le répertoire n'existe pas
    file(MAKE_DIRECTORY "${TOOLS_OUT}")

    # Tous les fichiers à copier dans examples/shaders/tools/ pour la compilation hors ligne
    set(SHADERCROSS_TOOLS_ALL
      "shadercross.exe"
      "dxcompiler.dll"
      "dxil.dll"
      "SDL3.dll"
      "SDL3_shadercross.dll"
      "spirv-cross-c-shared.dll"
    )

    # Fichiers à copier aussi dans le dossier de build, uniquement pour le hot reload de la compilation en ligne
    set(SHADERCROSS_TO_RUNTIME
      "dxcompiler.dll"
      "dxil.dll"
      "SDL3_shadercross.dll"
      "spirv-cross-c-shared.dll"
    )

    # Copie dans examples/shaders/tools/ tous les fichiers de Shadercross
    foreach(F IN LISTS SHADERCROSS_TOOLS_ALL)
      file(COPY "${SHADERCROSS_ROOT}/bin/SDL3_shadercross/${F}" DESTINATION "${TOOLS_OUT}")
    endforeach()

    if(RC2D_GPU_SHADER_HOT_RELOAD_ENABLED)
      # Copie dans le dossier de build tous les fichiers de Shadercross pour la compilation en ligne
      foreach(F IN LISTS SHADERCROSS_TO_RUNTIME)
        add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADERCROSS_ROOT}/bin/SDL3_shadercross/${F}"
            "$<TARGET_FILE_DIR:${target_name}>/${F}"
        )
      endforeach()
    endif()
  elseif(APPLE)
  
  elseif (UNIX AND NOT APPLE)

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
    if(NOT DEFINED OPENSSL_APPLE_VERSION)
      set(OPENSSL_APPLE_VERSION "3.3.3001" CACHE STRING "OpenSSL version" FORCE)
    endif()

    set(OPENSSL_XCFRAMEWORK_URL "https://github.com/krzyzanowskim/OpenSSL/releases/download/${OPENSSL_APPLE_VERSION}/OpenSSL.xcframework.zip")
    set(OPENSSL_XCFRAMEWORK_ZIP "${CMAKE_BINARY_DIR}/OpenSSL.xcframework.zip")
    set(OPENSSL_XCFRAMEWORK_DIR "${CMAKE_BINARY_DIR}/OpenSSL.xcframework")

    if(NOT EXISTS "${OPENSSL_XCFRAMEWORK_DIR}")
        file(DOWNLOAD "${OPENSSL_XCFRAMEWORK_URL}" "${OPENSSL_XCFRAMEWORK_ZIP}"
          SHOW_PROGRESS
          STATUS OPENSSL_DOWNLOAD_STATUS)
        list(GET OPENSSL_DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download OpenSSL.xcframework.zip: ${OPENSSL_DOWNLOAD_STATUS}")
        endif()
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${OPENSSL_XCFRAMEWORK_ZIP}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            RESULT_VARIABLE EXTRACT_RESULT
        )
        if(NOT EXTRACT_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to extract OpenSSL.xcframework.zip")
        endif()
    endif()

    target_link_libraries(${target_name} PRIVATE "${OPENSSL_XCFRAMEWORK_DIR}")
  elseif(WIN32)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" GENERATOR_PLATFORM_LOWER)

    # Détection dynamique architecture (x64 ou ARM64)
    if(GENERATOR_PLATFORM_LOWER STREQUAL "arm64")
      set(WINDOWS_ARCH_DIR "arm64")
    else()
      set(WINDOWS_ARCH_DIR "x64")
    endif()

    # Librairies OpenSSL précompilées statiques pour Windows
    target_include_directories(${target_name} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/include")

    # Rajouter le répertoire de la lib OpenSSL (libssl_static et libcrypto_static), pour le linker ensuite
    target_link_directories(${target_name} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/lib/openssl/${WINDOWS_ARCH_DIR}/Release")

    target_link_libraries(${target_name} PRIVATE
      libssl_static    # OpenSSL
      libcrypto_static # OpenSSL
      Crypt32          # OpenSSL Windows CryptoAPI
      ws2_32           # OpenSSL Windows Winsock2
    )
  elseif(ANDROID)

  elseif(UNIX AND NOT APPLE)

  endif()
endfunction()

# Headers de RC2D + les lib qui sont dans le répertoire "external"
function(rc2d_include_headers target_name)
  target_include_directories(${target_name} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/external"
  )
endfunction()

# Sources du projet RC2D
file(GLOB_RECURSE SOURCES
  "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Choisir entre bibliothèque dynamique/partagées ou statique pour RC2D
if(RC2D_BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${SOURCES})
  # Rajouter le suffixe "_static" au nom de la bibliothèque statique, pour éviter les conflits
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_static")
endif()

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
  add_executable(rc2d_example WIN32 ${RC2D_EXAMPLE_SOURCES})

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