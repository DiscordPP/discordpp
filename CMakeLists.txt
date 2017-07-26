cmake_minimum_required(VERSION 3.6)
project(discordpp)

set(CMAKE_CXX_STANDARD 14)

###################

add_subdirectory(lib/nlohmannjson)

add_library(${PROJECT_NAME} INTERFACE)
# target_sources(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/discordpp")

INCLUDE_DIRECTORIES(${discordpp_SOURCE_DIR} ${json_SOURCE_DIR})

# INCLUDE_DIRECTORIES(${json_SOURCE_DIR})
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE json)

#[[
set(SOURCE_FILES discordpp/discordpp.hh discordpp/bot.hh discordpp/restmodule.hh discordpp/websocketmodule.hh src/discordpp.cc src/bot.cc) # src/discordpp.cc src/bot.cc
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# target_sources(${PROJECT_NAME} INTERFACE )

# message("discordpp CMAKE_CURRENT_SOURCE_DIR " ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories (${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp/include/)

target_include_directories (${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED system)

set(nlohmann_json_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmannjson/src/)
set(nlohmann_json_include_dir ${nlohmann_json_include_dir} PARENT_SCOPE)
message(${nlohmann_json_include_dir})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${nlohmann_json_include_dir})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES}) # curlpp curl # pthread ssl crypto
]]