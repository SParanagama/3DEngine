cmake_minimum_required (VERSION 3.0)

project(3DEngine)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)

add_subdirectory(3rdparty/SDL)
include_directories(3rdparty/glad/include)
include_directories(3rdparty/SDL/include)
include_directories(3rdparty/stb/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/glm)

set(SOURCE_FILES
        source/src/main.cpp
        3rdparty/glad/src/glad.c
        3rdparty/stb/src/stb_image_impl.cpp
        source/src/Shader.cpp
        source/src/Texture.cpp
        )

include_directories(source/inc)

set(SHADERS
    shaders/shader.vs
    shaders/shader.fs
)

add_executable(3DEngine ${SOURCE_FILES})
target_link_libraries(3DEngine SDL2main SDL2-static ${OPENGL_LIBRARIES} ${GLU_LIBRARIES})

install (TARGETS 3DEngine DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install (FILES ${SHADERS} DESTINATION ${PROJECT_SOURCE_DIR}/bin)