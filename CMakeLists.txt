cmake_minimum_required(VERSION 3.8..3.18)

project(discordpp
    DESCRIPTION "A modularized C++ Library for the Discord API"
    LANGUAGES CXX
)

add_subdirectory(lib/nlohmannjson)

add_library(discordpp INTERFACE)
add_library(discordpp::discordpp ALIAS discordpp)


include(GNUInstallDirs)
target_include_directories(discordpp
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/discordpp>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/discordpp>
)
target_link_libraries(discordpp INTERFACE nhlomannjson::nhlomannjson)
target_compile_features(discordpp INTERFACE cxx_std_14)

install(TARGETS discordpp
    EXPORT discordpp-export
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/discordpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/discordpp"
)

install(EXPORT discordpp-export
	FILE discordppTargets.cmake
	NAMESPACE discordpp::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/discordpp"
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/discordppConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/discordpp"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/discordppConfig.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/discordpp"
)
