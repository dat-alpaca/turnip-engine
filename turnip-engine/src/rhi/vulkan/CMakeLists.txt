project(turnip-vulkan)

file(GLOB_RECURSE TURNIP_VULKAN_SOURCES CONFIGURE_DEPENDS "./*.hpp" "./*.h" "./*.cpp" "./*.c")

add_library(${PROJECT_NAME} STATIC ${TURNIP_VULKAN_SOURCES})
add_library(turnip::vulkan ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
    "turnip::core"
    "Vulkan::Vulkan"
    "GPUOpen::VulkanMemoryAllocator"
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
