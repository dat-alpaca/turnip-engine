project(turnip-platform)

add_library(${PROJECT_NAME} STATIC "platform.hpp")
add_library(turnip::platform ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ".")
target_link_libraries(${PROJECT_NAME} PRIVATE "turnip::core")

if(${TUR_USE_GLFW})
    add_subdirectory(glfw)
    target_link_libraries(${PROJECT_NAME} PUBLIC "turnip::glfw")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "TUR_WINDOWING_GLFW")
endif()
