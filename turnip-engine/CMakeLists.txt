project(turnip-engine LANGUAGES CXX)

# Core Sources:
file(GLOB_RECURSE TUR_ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
file(GLOB_RECURSE TUR_ENGINE_HEADERS CONFIGURE_DEPENDS src/*.hpp src/.h)

list(FILTER TUR_ENGINE_SOURCES EXCLUDE REGEX "src/platform/")
list(FILTER TUR_ENGINE_HEADERS EXCLUDE REGEX "src/platform/")

list(FILTER TUR_ENGINE_SOURCES EXCLUDE REGEX "src/rhi/")
list(FILTER TUR_ENGINE_HEADERS EXCLUDE REGEX "src/rhi/")

# Core:
add_library(turnip-core STATIC ${TUR_ENGINE_HEADERS})
add_library(turnip::core ALIAS turnip-core)
set_target_properties("turnip-core" PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(turnip-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(turnip-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/core")

target_link_libraries(turnip-core PUBLIC
    imgui::imgui
    stb
    # imguizmo::imguizmo
    glm::glm-header-only
    EnTT::EnTT
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    lua::lua
    sol2::sol2
    pfd
    spirv-cross-glsl
    miniaudio
    box2d::box2d
    box2dinclude
    tinygltf
    freetype
    yaml-cpp::yaml-cpp
)

# Platform & Graphics specific:
add_subdirectory(src/platform)
add_subdirectory(src/rhi)

# Engine:
add_library(${PROJECT_NAME} STATIC ${TUR_ENGINE_SOURCES})
add_library(turnip::engine ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
    "turnip::core"
    "turnip::platform"
    "turnip::rhi"
)

