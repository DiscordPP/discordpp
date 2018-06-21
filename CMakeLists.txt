cmake_minimum_required(VERSION 3.6)
project(discordpp)

set(CMAKE_CXX_STANDARD 14)

###################

add_subdirectory(lib/nlohmannjson)

add_library(${PROJECT_NAME} INTERFACE)

INCLUDE_DIRECTORIES(${discordpp_SOURCE_DIR} ${json_SOURCE_DIR})
