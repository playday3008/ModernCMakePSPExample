# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
  "${ModernCMakePSPExample_SOURCE_DIR}/include/modern/*.hpp"
  "${ModernCMakePSPExample_SOURCE_DIR}/include/modern/*.hxx"
  "${ModernCMakePSPExample_SOURCE_DIR}/include/modern/*.h"
)
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
  "${ModernCMakePSPExample_SOURCE_DIR}/src/*.cpp"
  "${ModernCMakePSPExample_SOURCE_DIR}/src/*.cxx"
  "${ModernCMakePSPExample_SOURCE_DIR}/src/*.c"
)

# Make an automatic library - will be static or dynamic based on user setting
add_library(modern_library ${SOURCE_LIST} ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(modern_library PUBLIC ../include)

# This depends on ArduinoJson
target_link_libraries(modern_library PRIVATE ArduinoJson)

# PSP Libraries
target_link_libraries(modern_library PRIVATE
  pspdebug
  pspdisplay
  pspge
)

# All users of this library will need at least C++23
target_compile_features(modern_library PUBLIC cxx_std_23)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST}
)
