cmake_minimum_required(VERSION 3.6)
project(discordpp)

set(CMAKE_CXX_STANDARD 14)

###################

add_subdirectory(lib/nlohmannjson)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)
