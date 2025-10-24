project(turnip-glfw)

file(GLOB TURNIP_GLFW CONFIGURE_DEPENDS "./*.hpp" "./*.h" "./*.cpp" "./*.c")

add_library(${PROJECT_NAME} STATIC ${TURNIP_GLFW})
add_library(turnip::glfw ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::core" "glfw" "glm::glm")

if(${TUR_USE_OPENGL})
    add_subdirectory(opengl)
    target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::glfw-gl")
elseif(${TUR_USE_VULKAN})
    add_subdirectory(vulkan)
    target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::glfw-vulkan")
endif()
