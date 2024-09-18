include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1
  GIT_PROGRESS   TRUE
  FIND_PACKAGE_ARGS
)

message(STATUS "Fetching Catch2")
FetchContent_MakeAvailable(Catch2)
message(STATUS "Fetching Catch2 - done")
