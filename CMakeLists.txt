cmake_minimum_required(VERSION 3.3)
project(discordpp)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc discordpp.hh)
add_executable(discordpp ${SOURCE_FILES})

add_subdirectory(lib/curlpp)

#find_library(CURLPP REQUIRED curlpp)
#find_library(BOOST REQUIRED boost)
find_package(Boost REQUIRED system)

#INCLUDE(FindPkgConfig)

##PKG_SEARCH_MODULE(CURLPP REQUIRED curlpp)
#PKG_SEARCH_MODULE(BOOST REQUIRED libboost)
##PKG_SEARCH_MODULE(JSONC REQUIRED json-c)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} /opt/include ) # ${JSONC_INCLUDE_DIRS}
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp pthread ssl crypto) # ${JSONC_LIBRARIES}

add_custom_command(TARGET discordpp PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/login.dat $<TARGET_FILE_DIR:discordpp>)
add_custom_command(TARGET discordpp PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/softcommands.dat $<TARGET_FILE_DIR:discordpp>)