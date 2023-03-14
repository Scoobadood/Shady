# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3-stable
)
FetchContent_MakeAvailable(glfw)

# Img UI
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG
)
FetchContent_MakeAvailable(imgui)

# Command line tool. Still requires GLFW to establish OpenGL context
add_executable(iv
        src/xxmain.cpp
        include/graph.h src/graph.cpp
)

target_include_directories(iv
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
        )

target_link_libraries(iv
        PRIVATE
        Xform
        glfw
        Common
        )


# ImGui Main
SET(IMGUI_SOURCES ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )

add_executable(toy
        src/main_ui.cpp
        include/graph.h src/graph.cpp
        ${IMGUI_SOURCES}
        )

target_include_directories(toy
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        )

target_link_libraries(toy
        PRIVATE
        Xform
        glfw
        Common
        )
