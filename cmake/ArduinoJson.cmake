include(FetchContent)
FetchContent_Declare(
    ArduinoJson
    GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
    GIT_TAG        v7.1.0
    GIT_PROGRESS   TRUE
)

message(STATUS "Fetching ArduinoJson")
FetchContent_MakeAvailable(ArduinoJson)
message(STATUS "Fetching ArduinoJson - done")