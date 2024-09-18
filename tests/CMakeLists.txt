# Testing library
include(../cmake/Catch2.cmake)
# Adds Catch2::Catch2

# Set definitions for Catch2
target_compile_definitions(
  Catch2
  PUBLIC
  # POSIX isn't fully supported on PSP
  CATCH_CONFIG_NO_POSIX_SIGNALS
)

# Tests need to be added as executables first
add_executable(testlib testlib.cpp)

# I'm using C++17 in the test
target_compile_features(testlib PRIVATE cxx_std_23)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(testlib PRIVATE modern_library Catch2::Catch2WithMain)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibtest COMMAND testlib) # Command can be a target

set(BUILD_PRX ON)       

create_pbp_file(
  TARGET testlib
  TITLE "${CMAKE_PROJECT_NAME}"
  #ICON_PATH
  #BACKGROUND_PATH
  #PREVIEW_PATH
)
