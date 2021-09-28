cmake_minimum_required(VERSION 3.6)
project(discordpp)

set(CMAKE_CXX_STANDARD 14)

###################

set(ALL_DISCORDPP_LIBRARIES "discordpp" PARENT_SCOPE)
set(ALL_DISCORDPP_PLUGINS "" PARENT_SCOPE)

add_subdirectory(lib/nlohmannjson)

include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json fmt::fmt)
