project(turnip-glfw-vulkan)

file(GLOB TURNIP_GLFW_VULKAN_SOURCES CONFIGURE_DEPENDS "./*.hpp" "./*.h" "./*.cpp" "./*.c")

add_library(${PROJECT_NAME} STATIC ${TURNIP_GLFW_VULKAN_SOURCES})
add_library(turnip::glfw-vulkan ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    "turnip::core"
    "Vulkan::Vulkan"
    "glfw"
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
