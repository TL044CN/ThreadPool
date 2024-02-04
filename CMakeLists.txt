cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

#find_program(SCCACHE sccache)
#if(SCCACHE)
#    message("sccache found")
#    set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE})
#    set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE})
#    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
#else()
#    message("sccache not found. No caching.")
#endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(documentation
        COMMAND "doxygen"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        COMMENT "Generating Doxygen Documentation"
        VERBARIM
    )
else()
    message("Doxygen required to build Doxygen Documentation")
endif()

project(ThreadPool
    VERSION 0.0.1
    DESCRIPTION "Threadpool Library"
    LANGUAGES CXX
)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMODULE_RESULT
        )
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}")
        endif()
    endif()
endif()

add_library(${PROJECT_NAME} source/ThreadPool.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# tests:
enable_testing()
add_subdirectory(tests)