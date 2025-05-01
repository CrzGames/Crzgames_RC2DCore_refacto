# Options configurables :
set(ARTIFACT_URL "https://github.com/libsdl-org/SDL_shadercross/suites/22530712448/artifacts/1297172006" CACHE STRING "URL to the artifact zip")
set(ARTIFACT_NAME "SDL3_shadercross-VC-x64" CACHE STRING "Name of the artifact")
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/vendored/SDL_shadercross" CACHE STRING "Output directory for SDL_shadercross headers/libs")
set(TOOLS_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/examples/shaders/tools" CACHE STRING "Output directory for runtime binaries")

file(MAKE_DIRECTORY "${OUTPUT_DIR}")
file(MAKE_DIRECTORY "${TOOLS_OUTPUT_DIR}")

set(DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/${ARTIFACT_NAME}.zip")

message(STATUS "Downloading artifact from ${ARTIFACT_URL}...")
file(DOWNLOAD
    ${ARTIFACT_URL}
    ${DOWNLOAD_PATH}
    STATUS DOWNLOAD_STATUS
    SHOW_PROGRESS
)
list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)
if(NOT DOWNLOAD_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to download artifact: ${DOWNLOAD_STATUS}")
endif()

message(STATUS "Extracting artifact...")
file(ARCHIVE_EXTRACT
    INPUT ${DOWNLOAD_PATH}
    DESTINATION ${CMAKE_BINARY_DIR}/shadercross_tmp
)

# Copy includes
file(COPY "${CMAKE_BINARY_DIR}/shadercross_tmp/include"
     DESTINATION "${OUTPUT_DIR}")

# Copy libraries
file(COPY "${CMAKE_BINARY_DIR}/shadercross_tmp/lib"
     DESTINATION "${OUTPUT_DIR}")

# Copy binaries to tools folder
foreach(FILENAME IN LISTS
    "shadercross.exe"
    "dxcompiler.dll"
    "dxil.dll"
    "SDL3.dll"
    "SDL3_shadercross.dll"
    "spirv-cross-c-shared.dll"
)
    file(COPY "${CMAKE_BINARY_DIR}/shadercross_tmp/bin/${FILENAME}"
         DESTINATION "${TOOLS_OUTPUT_DIR}")
endforeach()

message(STATUS "SDL_shadercross artifact successfully installed.")
