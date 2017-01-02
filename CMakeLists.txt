cmake_minimum_required(VERSION 3.6)
project(DiscordPP_lib)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES discordpp.hh bot.hh bot.cc)
add_library(DiscordPP_lib SHARED ${SOURCE_FILES})

#add_subdirectory(lib/curlpp)

find_package(Boost REQUIRED system)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp pthread ssl crypto)
