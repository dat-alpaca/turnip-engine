project(turnip-rhi)

add_library(${PROJECT_NAME} STATIC "rhi.hpp")
add_library(turnip::rhi ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_link_libraries(${PROJECT_NAME} PRIVATE "turnip::core")

if(${TUR_USE_OPENGL})
    add_subdirectory(opengl)
    target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::gl")
elseif(${TUR_USE_VULKAN})
    add_subdirectory(vulkan)
    target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::vulkan")
endif()
