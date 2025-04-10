# ===========================================================================
# CMAKE PRINCIPAL APPELÉ PAR GRADLE
# ===========================================================================

cmake_minimum_required(VERSION 3.22.1)

project(rc2d)

# armeabi-v7a requires cpufeatures library
include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

# Spécifier la version C standard
set(CMAKE_C_STANDARD 99) # ou 90, 99, 11, 17, 18, 23 selon la version du standard C que vous souhaitez utiliser
set(CMAKE_C_STANDARD_REQUIRED True)

# Configuration des drapeaux du compilateur
# -Wall : Active tous les avertissements de base
# -Wextra : Active des avertissements supplémentaires
# -pedantic : Enforce strictement les standards C
# -Wno-unused-parameter : Désactive les avertissements pour les paramètres de fonction non utilisés
# -Wno-error=strict-prototypes : Désactive les erreurs pour les prototypes de fonction stricts
# -Wno-newline-eof : Désactive les avertissements pour les nouvelles lignes à la fin du fichier
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-error=strict-prototypes -Wno-newline-eof")

# Chemin vers les dépendences
set(CRZGAMES_LIBCPP_ANDROID_PATH "${PROJECT_SOURCE_DIR}/../../../dependencies/Crzgames_LibCpp_Android")

# Ajouts des librairies
# RCENet Library
add_library(rcenet STATIC IMPORTED)
set_target_properties(rcenet PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/rcenet/${ANDROID_ABI}/librcenet.a)

# SDL2 Library
add_subdirectory(SDL) # SDL sources are in a subfolder named "SDL"
find_library(SDL2 SDL2)

# SDL2_image Library
add_library(SDL2_image SHARED IMPORTED)
set_target_properties(SDL2_image PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_image/${ANDROID_ABI}/libSDL2_image.so)

# SDL2_ttf Library
add_library(SDL2_ttf SHARED IMPORTED)
set_target_properties(SDL2_ttf PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_ttf/${ANDROID_ABI}/libSDL2_ttf.so)

# Freetype Library (for SDL2_ttf)
add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_ttf/${ANDROID_ABI}/libfreetype.a)

# Harfbuzz Library (for SDL2_ttf)
add_library(harfbuzz STATIC IMPORTED)
set_target_properties(harfbuzz PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_ttf/${ANDROID_ABI}/libharfbuzz.a)

# SDL2_mixer Library
add_library(SDL2_mixer SHARED IMPORTED)
set_target_properties(SDL2_mixer PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_mixer/${ANDROID_ABI}/libSDL2_mixer.so)

# Wavpack Library (for SDL2_mixer)
add_library(wavpack STATIC IMPORTED)
set_target_properties(wavpack PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/SDL2_mixer/${ANDROID_ABI}/libwavpack.a)

# OpenSSL_Crypto Library
add_library(openssl_crypto SHARED IMPORTED)
set_target_properties(openssl_crypto PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/openssl/${ANDROID_ABI}/libcrypto.so)

# OpenSSL Library
add_library(openssl SHARED IMPORTED)
set_target_properties(openssl PROPERTIES IMPORTED_LOCATION ${CRZGAMES_LIBCPP_ANDROID_PATH}/lib/Release/openssl/${ANDROID_ABI}/libssl.so)

# Ajouter les fichiers sources du moteur de jeu ainsi que les librairies aes, lz4, spine-c, spine-sdl-c et monocypher
file(GLOB_RECURSE SOURCES 
  "${PROJECT_SOURCE_DIR}/../../../src/*.c"
  "${PROJECT_SOURCE_DIR}/../../../src/external/monocypher/*.c"
  "${PROJECT_SOURCE_DIR}/../../../src/external/aes/*.c"
  "${PROJECT_SOURCE_DIR}/../../../src/external/lz4/*.c"
  "${PROJECT_SOURCE_DIR}/../../../src/external/spine-c/*.c"
  "${PROJECT_SOURCE_DIR}/../../../src/external/spine-c/*.h" # Exceptionnellement pour un fichier include de Spine qui doit être present
)

# Main library for Android
add_library(${PROJECT_NAME} 
  SHARED # Shared library (.so) -> Obligatoire pour la contrainte du wrapper SDL Java JNI
  ${SOURCES}
)

# Include directories all
include_directories(
  ${PROJECT_SOURCE_DIR}/../../../include # RC2DCore include directory
  ${PROJECT_SOURCE_DIR}/../../../include/external # Lib externes integrer au moteur
  ${CRZGAMES_LIBCPP_ANDROID_PATH}/include
  ${CRZGAMES_LIBCPP_ANDROID_PATH}/include/SDL2_image
  ${CRZGAMES_LIBCPP_ANDROID_PATH}/include/SDL2_ttf
  ${CRZGAMES_LIBCPP_ANDROID_PATH}/include/SDL2_mixer
)

# Link all libraries (.so/.a)
target_link_libraries(${PROJECT_NAME}
  harfbuzz
  freetype
  wavpack
  SDL2_mixer
  SDL2_ttf
  SDL2_image
  SDL2
  rcenet
  openssl
  openssl_crypto
)