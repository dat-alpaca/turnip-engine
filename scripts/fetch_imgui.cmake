if(${TUR_USE_GLFW})
    set(IMGUI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/extern/imgui")
    file(GLOB IMGUI_SOURCES "${IMGUI_SOURCE_DIR}/*.cpp" "${IMGUI_SOURCE_DIR}/*.h")

    list(APPEND
        IMGUI_SOURCES
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h
    )

    if(${TUR_USE_GL})
        list(APPEND
            IMGUI_SOURCES
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        )

    elseif(${TUR_USE_VULKAN})
        list(APPEND
            IMGUI_SOURCES
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
            ${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.h
        )
    endif()

    add_library("imgui" STATIC ${IMGUI_SOURCES})
    target_include_directories("imgui" PUBLIC ${IMGUI_SOURCE_DIR})

    add_library("imgui::imgui" ALIAS "imgui")
endif()
