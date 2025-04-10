# CMake 3.25.0 est la version minimale requise pour ce projet
cmake_minimum_required(VERSION 3.30.0)

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

# Option pour choisir entre statique et dynamique
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

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
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "macOS Deployment Target")
  elseif(CMAKE_OSX_SYSROOT MATCHES "iphoneos")
    set(CMAKE_OSX_ARCHITECTURES "arm64;arm64e" CACHE STRING "iOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "iOS Deployment Target")
  elseif(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "iOS Simulator Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "iOS Simulator Deployment Target")
  elseif(CMAKE_OSX_SYSROOT MATCHES "appletvos")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "tvOS Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "tvOS Deployment Target")
  elseif(CMAKE_OSX_SYSROOT MATCHES "appletvsimulator")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "tvOS Simulator Architectures" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "tvOS Simulator Deployment Target")
endif()
elseif (UNIX AND NOT APPLE)
  # Linux-LIKE utilisera Vulkan
elseif(ANDROID)
  # API 24 (Android 7.0+) pour Vulkan
  set(CMAKE_ANDROID_API 24 CACHE STRING "Android API Level")
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

# Use vendored libs
set(SDLTTF_VENDORED ON)
set(SDLIMAGE_VENDORED ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)


# Télécharger OpenSSL XCFramework pour Apple
if(APPLE)
  if(NOT DEFINED OPENSSL_APPLE_VERSION)
    set(OPENSSL_APPLE_VERSION "3.3.3001")
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
endif()

# Sources du projet
file(GLOB_RECURSE SOURCES
  "${PROJECT_SOURCE_DIR}/src/*.c"
  "${PROJECT_SOURCE_DIR}/external/src/*.c"
)

# Choisir entre bibliothèque partagée ou statique
if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

# Inclure les répertoires d'en-tête (headers)
target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/external/include"
)

# Link les dépendances (librairies communes à tous les plateformes)
target_link_libraries(${PROJECT_NAME} PRIVATE
  SDL3_image::SDL3_image
  #SDL3_mixer::SDL3_mixer -> FIXME: SDL3_mixer n'ai pas encore disponible
  SDL3_ttf::SDL3_ttf
  SDL3::SDL3
)

# Link la dépendance OpenSSL par rapport à la plateforme
if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "${OPENSSL_XCFRAMEWORK_DIR}")
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
  )
endif()

# Link les dépendances spécifiques à chaque plateforme
if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Crypt32 # Bibliothèque native à Windows (pour la librairie OpenSSL)
  )
endif()

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
elseif(ANDROID)
  # Pour .aar, utilise un script Gradle ou un plugin (à configurer séparément)
endif()