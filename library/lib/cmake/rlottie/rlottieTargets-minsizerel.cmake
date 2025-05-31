#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "rlottie::rlottie" for configuration "MinSizeRel"
set_property(TARGET rlottie::rlottie APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(rlottie::rlottie PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "/home/parallels/rlottie/build/out/lib/librlottie.so.0.2"
  IMPORTED_SONAME_MINSIZEREL "librlottie.so.0"
  )

list(APPEND _cmake_import_check_targets rlottie::rlottie )
list(APPEND _cmake_import_check_files_for_rlottie::rlottie "/home/parallels/rlottie/build/out/lib/librlottie.so.0.2" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
