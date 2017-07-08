cmake_minimum_required(VERSION 3.6)
project(discordpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES discordpp/discordpp.hh src/discordpp.cc discordpp/bot.hh src/bot.cc)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

add_subdirectory(lib/curlpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp/include/)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
	set (Boost_USE_MULTITHREADED TRUE)
	set (Boost_USE_STATIC_LIBS TRUE)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
else ()
	set (Boost_USE_MULTITHREADED FALSE)
	set (Boost_USE_STATIC_LIBS FALSE)
endif ()

find_package(Boost REQUIRED system regex date_time random)
find_package(OpenSSL REQUIRED)
find_package(curl REQUIRED)

if (NOT MSVC)
	add_library(pthread STATIC)
endif()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} curlpp)
