add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_23)

target_link_libraries(app PRIVATE modern_library)

set(BUILD_PRX ON)       

create_pbp_file(
  TARGET app
  TITLE "${CMAKE_PROJECT_NAME}"
  #ICON_PATH
  #BACKGROUND_PATH
  #PREVIEW_PATH
)
