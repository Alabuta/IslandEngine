cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

if(NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION 1.0.0)
endif()

project(IslandEngine VERSION ${PROJECT_VERSION})

include(build/conanbuildinfo.cmake)
conan_basic_setup()

configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/config.h"
)

find_package(OpenGL REQUIRED)
#find_package(glm REQUIRED)
#find_package(glfw3 3.2.1 REQUIRED)

if(NOT WIN32)
	find_package(X11 REQUIRED)
endif()

#find_package(Boost REQUIRED COMPONENTS
#    filesystem
#)

include_directories(SYSTEM include)
include_directories(src)

set(SOURCE_FILES
    src/input/InputManager.cxx
    src/input/MouseInput.cxx

    src/platform/window.cxx
    
    src/GAPI/procedures.cxx

    src/main.cxx
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}

    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED NO
    CXX_EXTENSIONS OFF

    POSITION_INDEPENDENT_CODE YES

    DEBUG_POSTFIX .d
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
        -Wall
)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    OpenGL::GL
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    -Wl,-no-undefined
    -Wl,-no-allow-shlib-undefined
    -Wl,-unresolved-symbols=report-all

    #dl
    #X11
	#Xxf86vm
	#Xcursor
	#Xi
	#Xrandr
	#Xinerama
	#rt
	#m

    #pthread
	
    #stdc++fs

    OpenGL::GL
    # OpenGL::OpenGL

    #glm
    #glfw3

	 ${CONAN_LIBS}

	#CONAN_PKG::glm
	#CONAN_PKG::boost
)
