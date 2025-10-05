project(turnip-glfw-gl)

file(GLOB TURNIP_GLFW_GL_SOURCES CONFIGURE_DEPENDS "./*.hpp" "./*.h" "./*.cpp" "./*.c")

add_library(${PROJECT_NAME} STATIC ${TURNIP_GLFW_GL_SOURCES})
add_library(turnip::glfw-gl ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    "turnip::core"
    "OpenGL::GL"
    "glfw"
    "glad"
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
