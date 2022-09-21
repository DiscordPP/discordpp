cmake_minimum_required(VERSION 3.6)
project(discordpp)

if (CMAKE_CXX_STANDARD LESS 14)
    error(${PROJECT_NAME} requires at least C++14)
endif ()

###################

option(DISCORDPP_USE_BOOST "set ON to use Boost.Asio instead of standalone Asio" OFF)

set(ALL_DISCORDPP_LIBRARIES "discordpp" PARENT_SCOPE)
set(ALL_DISCORDPP_PLUGINS "" PARENT_SCOPE)
set(ALL_DISCORDPP_INCLUDES "#include <discordpp/bot.hh>\n" PARENT_SCOPE)
set(JSON_MultipleHeaders OFF)

add_subdirectory(lib/nlohmannjson)

include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

if (NOT ${DISCORDPP_USE_BOOST})
    find_package(Threads REQUIRED)

    add_library(asio INTERFACE)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_include_directories(asio INTERFACE lib/asio/asio/include)
    target_link_libraries(asio INTERFACE Threads::Threads)
    set(TARGET_ASIO asio)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE
        nlohmann_json::nlohmann_json
        fmt::fmt ${TARGET_ASIO})

function(CREATE_DISCORDPP_INCLUDE)
    set(DISCORDPP_INCLUDE_GENERATED ${CMAKE_BINARY_DIR}/include PARENT_SCOPE)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
    file(WRITE ${CMAKE_BINARY_DIR}/include/discordpp.hh ${ALL_DISCORDPP_INCLUDES})
endfunction()

function(CREATE_DISCORDPP_DEFINITIONS)
    list(JOIN ALL_DISCORDPP_PLUGINS , ALL_DISCORDPP_PLUGINS_JOINED)
    add_compile_definitions(ALL_DISCORDPP_PLUGINS=${ALL_DISCORDPP_PLUGINS_JOINED})
endfunction()
