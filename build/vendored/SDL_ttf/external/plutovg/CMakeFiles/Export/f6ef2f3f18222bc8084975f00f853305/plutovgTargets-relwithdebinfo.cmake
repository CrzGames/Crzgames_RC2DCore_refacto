#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "plutovg::plutovg" for configuration "RelWithDebInfo"
set_property(TARGET plutovg::plutovg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(plutovg::plutovg PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/plutovg.lib"
  )

list(APPEND _cmake_import_check_targets plutovg::plutovg )
list(APPEND _cmake_import_check_files_for_plutovg::plutovg "${_IMPORT_PREFIX}/lib/plutovg.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
