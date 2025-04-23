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

# SDL3, SDL3_ttf, SDL3_image : 
# Permet de dire à CMake de ne pas chercher les dépendances dans le système,
# mais de les utiliser à partir du répertoire "vendored", dont également les submodules
# des repositories de SDL3_ttf, SDL3_mixer et SDL3_image
set(SDLTTF_VENDORED ON)
set(SDLIMAGE_VENDORED ON)
#set(SDLMIXER_VENDORED ON)

# SDL3_image : Désactiver explicitement tous les formats qui sont activé par défault, sauf PNG et SVG
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

# Sources du projet
file(GLOB_RECURSE SOURCES
  "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Choisir entre bibliothèque partagée ou statique
if(RC2D_BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${SOURCES})
  # Rajouter le suffixe "_static" au nom de la bibliothèque statique, pour éviter les conflits
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_static")
endif()

# Inclure les répertoires d'en-tête (headers)
target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/include/external"
)

# OpenSSL
if(APPLE)
  # OpenSSL via XCFramework : macOS, iOS, tvOS
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

  target_link_libraries(${PROJECT_NAME} PRIVATE "${OPENSSL_XCFRAMEWORK_DIR}")
elseif(WIN32)
  # Windows => Détection dynamique architecture (x64 ou ARM64)
  include(CheckCCompilerFlag)
  string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" GENERATOR_PLATFORM_LOWER)
  
  if(GENERATOR_PLATFORM_LOWER STREQUAL "arm64")
    set(WINDOWS_ARCH_DIR "arm64")
  else()
    # Par défaut, x64
    set(WINDOWS_ARCH_DIR "x64")
  endif()

  # Librairies OpenSSL précompilées statiques pour Windows
  target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/include")
  target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/lib/${WINDOWS_ARCH_DIR}")
  
  # Link avec les libs statiques
  target_link_libraries(${PROJECT_NAME} PRIVATE libcrypto_static libssl_static Crypt32)
elseif(ANDROID)
  target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Android/include")
  target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Android/lib")
  target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)
elseif(UNIX AND NOT APPLE)
  target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Linux/include")
  target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Linux/lib")
  target_link_libraries(${PROJECT_NAME} PRIVATE libssl_static libcrypto_static Crypt32)
endif()

# Link les dépendances (librairies communes à tous les plateformes)
target_link_libraries(${PROJECT_NAME} PRIVATE
  SDL3_image::SDL3_image
  #SDL3_mixer::SDL3_mixer -> FIXME: SDL3_mixer n'ai pas encore disponible
  SDL3_ttf::SDL3_ttf
  SDL3::SDL3
)

# Génération des sorties
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

# Pour les exemples
if(RC2D_BUILD_EXAMPLES)
  # Ajouter le fichier de l'exemple
  set(RC2D_EXAMPLE_SOURCES "${PROJECT_SOURCE_DIR}/examples/main.c")

  # Créer un exécutable pour l'exemple
  add_executable(rc2d_example ${RC2D_EXAMPLE_SOURCES})

  # Inclure les headers du projet rc2d et external
  target_include_directories(rc2d_example PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/external"
  )

  # Linker les dépendances SDL3, SDL3_ttf, SDL3_image, SDL3_mixer et RC2D
  target_link_libraries(rc2d_example PRIVATE
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3::SDL3
    ${PROJECT_NAME} # RC2D
  )

  # OpenSSL
  if(APPLE)
    # OpenSSL via XCFramework : macOS, iOS, tvOS
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

    target_link_libraries(${PROJECT_NAME} PRIVATE "${OPENSSL_XCFRAMEWORK_DIR}")
  elseif(WIN32)
    # Windows => Détection dynamique architecture (x64 ou ARM64)
    include(CheckCCompilerFlag)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" GENERATOR_PLATFORM_LOWER)
    
    if(GENERATOR_PLATFORM_LOWER STREQUAL "arm64")
      set(WINDOWS_ARCH_DIR "arm64")
    else()
      # Par défaut, x64
      set(WINDOWS_ARCH_DIR "x64")
    endif()

    # Librairies OpenSSL précompilées statiques pour Windows
    target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Windows/lib/openssl/x64/Release")
    
    # Link avec les libs statiques
    target_link_libraries(${PROJECT_NAME} PRIVATE libcrypto_static libssl_static Crypt32)
  elseif(ANDROID)
    target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Android/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Android/lib")
    target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)
  elseif(UNIX AND NOT APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Linux/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendored/Crzgames_LibCpp_Linux/lib")
    target_link_libraries(${PROJECT_NAME} PRIVATE libssl_static libcrypto_static Crypt32)
  endif()
endif()

# Pour les tests unitaires
if(RC2D_BUILD_TESTS)
  enable_testing()

  # Ajouter les fichiers des tests unitaires
  # Cela inclut tous les fichiers .c dans le répertoire tests et ses sous-répertoires
  file(GLOB_RECURSE RC2D_TEST_SOURCES
    "${PROJECT_SOURCE_DIR}/tests/*.c"
  )

  # Créer un exécutable pour les tests
  add_executable(rc2d_tests ${RC2D_TEST_SOURCES})

  # Inclure les headers du projet rc2d dans les tests
  # Cela permet d'accéder aux fichiers d'en-tête de la bibliothèque RC2D
  target_include_directories(rc2d_tests PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
  )

  # Linker la lib RC2D avec le binaire de test
  target_link_libraries(rc2d_tests PRIVATE ${PROJECT_NAME})

  # Linker Criterion selon l'OS
  if(UNIX)
    find_library(CRITERION_LIB criterion REQUIRED)
    target_link_libraries(rc2d_tests PRIVATE ${CRITERION_LIB})
  elseif(WIN32)
    # Sous Windows : on suppose l’utilisation de vcpkg ou autre gestionnaire
    # Sinon, il faudra builder Criterion et linker manuellement
    find_package(Criterion REQUIRED)
    target_link_libraries(rc2d_tests PRIVATE Criterion::Criterion)
  endif()

  # Permet de lancer les tests avec la commande "ctest" intégrée dans CMake
  add_test(NAME RC2D_AllTests COMMAND rc2d_tests)
endif()