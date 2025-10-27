cmake_minimum_required(VERSION 3.20)
project(turnip LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Options:
option(TUR_BUILD_SANDBOX "Builds the turnip sandbox program" ON)
option(TUR_BUILD_ENGINE "Builds the turnip engine library" ON)

option(TUR_USE_GLFW "Uses the GLFW windowing implementation" ON)
option(TUR_USE_OPENGL "Uses the OpenGL backend" OFF)
option(TUR_USE_VULKAN "Uses the Vulkan backend" ON)

# Core Dependencies:
add_subdirectory(extern/lua)
add_subdirectory(extern/entt)
add_subdirectory(extern/glm)
add_subdirectory(extern/json)
add_subdirectory(extern/pfd)
add_subdirectory(extern/sol2)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/SPIRV-Cross)
add_subdirectory(extern/miniaudio)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_SOURCE_DIR}/extern/stb")

add_library(pfd INTERFACE)
target_include_directories(pfd INTERFACE "${CMAKE_SOURCE_DIR}/extern/pfd")

include(scripts/fetch_imgui.cmake)

# Platform  Options:
if(${TUR_USE_GLFW})
    add_subdirectory(extern/glfw)
    add_compile_definitions(GLFW_INCLUDE_NONE)
    add_compile_definitions(GLFW_STATIC)
    add_compile_definitions(GLFW_VULKAN_STATIC)
    add_compile_definitions(TUR_WINDOWING_GLFW)
endif()

# Graphics Options:
if(${TUR_USE_OPENGL})
    find_package(OpenGL REQUIRED)
    add_subdirectory(extern/glad/cmake)
    glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.6)
    target_include_directories(glad PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/gladsources/glad/include)

    add_compile_definitions(TUR_GRAPHICS_GL)
elseif(${TUR_USE_VULKAN})
    find_package(Vulkan REQUIRED)
    add_subdirectory(extern/vma)

    add_compile_definitions(TUR_GRAPHICS_VK)
    add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS)
endif()

# Projects
if(${TUR_BUILD_ENGINE})
    add_subdirectory(turnip-engine)
endif()

if(${TUR_BUILD_SANDBOX})
    add_subdirectory(turnip-sandbox)
endif()
