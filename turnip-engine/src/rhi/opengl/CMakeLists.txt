project(turnip-gl)

file(GLOB_RECURSE TURNIP_GL_SOURCES CONFIGURE_DEPENDS "./*.hpp" "./*.h" "./*.cpp" "./*.c")

add_library(${PROJECT_NAME} STATIC ${TURNIP_GL_SOURCES})
add_library(turnip::gl ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
    "turnip::core"
    "OpenGL::GL"
    "glad"
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
