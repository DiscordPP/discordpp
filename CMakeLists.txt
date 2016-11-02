cmake_minimum_required(VERSION 3.3)
project(discordpp)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc discordpp.hh Bot.cpp Bot.h)
add_executable(discordpp ${SOURCE_FILES})

add_subdirectory(lib/curlpp)

find_package(Boost REQUIRED system filesystem)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp pthread ssl crypto)

foreach(DATFILE softcommands.dat token.dat) #login.dat
    #file(REMOVE ${CMAKE_BINARY_DIR}/${DATFILE})
    ##if(EXISTS $<TARGET_FILE_DIR:discordpp>/${DATFILE})
    ##    file(REMOVE $<TARGET_FILE_DIR:discordpp>/${DATFILE})
    ##endif()
    #if(EXISTS ${CMAKE_SOURCE_DIR}/${DATFILE})
    #    add_custom_command(TARGET discordpp PRE_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy
    #            ${CMAKE_SOURCE_DIR}/${DATFILE} ${CMAKE_BINARY_DIR})
    #endif()
    if(EXISTS ${DATFILE})
        configure_file(${DATFILE} ${DATFILE} COPYONLY)
    else()
        file(REMOVE ${CMAKE_BINARY_DIR}/${DATFILE})
    endif()
endforeach()