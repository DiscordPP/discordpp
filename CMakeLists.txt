cmake_minimum_required(VERSION 3.3)
project(discordpp)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc discordpp.hh resource.hh)
add_executable(discordpp ${SOURCE_FILES})

add_subdirectory(lib/curlpp)

find_package(Boost REQUIRED system filesystem)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}) # ${JSONC_INCLUDE_DIRS}
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp pthread ssl crypto) # ${JSONC_LIBRARIES}

foreach(DATFILE login.dat softcommands.dat token.dat)
    if(EXISTS ${CMAKE_SOURCE_DIR}/${DATFILE})
        add_custom_command(TARGET discordpp PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/${DATFILE} $<TARGET_FILE_DIR:discordpp>)
    elseif(EXISTS $<TARGET_FILE_DIR:discordpp>/${DATFILE})
        file(REMOVE $<TARGET_FILE_DIR:discordpp>/${DATFILE})
    endif()
endforeach()