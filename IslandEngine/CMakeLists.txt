cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION 1.0.0)
endif()

project(IslandEngine VERSION ${PROJECT_VERSION})

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

if(NOT WIN32)
	find_package(X11 REQUIRED)
endif()

find_package(Boost REQUIRED COMPONENTS
    filesystem
)

include_directories(SYSTEM include)

set(SOURCE_FILES
    src/Input/InputManager.hxx              src/Input/InputManager.cxx
    src/Input/MouseInput.hxx                src/Input/MouseInput.cxx

    src/Renderer/oglContext.hxx
                                            src/Renderer/oglProcedures.cxx

    src/main.hxx                            src/main.cxx
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}

    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED NO
    CXX_EXTENSIONS OFF

    POSITION_INDEPENDENT_CODE YES

    DEBUG_POSTFIX _d
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic
        -Wall
        -Wextra
		-Wold-style-cast
		-Wnon-virtual-dtor
		-Wcast-align
		-Wunused
		-Wconversion
		-Wsign-conversion
		-Wnull-dereference
		-Wdouble-promotion
		-Wformat=2
		-Wduplicated-cond
		-Wduplicated-branches
		-Wlogical-op
		-Wuseless-cast
        # -Wlifetime

        #-fsanitize=thread -fsanitize=address
)

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic
        -Wall
        -Wextra
		-Wold-style-cast
		-Wnon-virtual-dtor
		-Wcast-align
		-Wunused
		-Wconversion
		-Wsign-conversion
		-Wnull-dereference
		-Wdouble-promotion
		-Wformat=2

        #-fsanitize=thread -fsanitize=address
)

elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic
        -Wall
        -Wextra
)

endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    OpenGL::GL
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    -Wl,-no-undefined
    -Wl,-no-allow-shlib-undefined
    -Wl,-unresolved-symbols=report-all

    dl
    X11
	Xxf86vm
	Xcursor
	Xi
	Xrandr
	Xinerama
	rt
	m

    pthread

    stdc++fs

    OpenGL::GL
    # OpenGL::OpenGL

    glm
    glfw3
)