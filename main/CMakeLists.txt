# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3-stable
)

# Img UI
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG
)
FetchContent_MakeAvailable(glfw imgui)

add_definitions(-DGL_SILENCE_DEPRECATION)


# ImGui Main
SET(IMGUI_SOURCES ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )

add_executable(shady
        src/main_ui.cpp
        src/ui_menu.cpp include/ui_menu.h
        src/ui_theme.cpp include/ui_theme.h
        src/ui_graph.cpp include/ui_graph.h
        src/ui_xform_config.cpp include/ui_xform_config.h
        include/ui_state.h
        ${IMGUI_SOURCES}
        )

target_include_directories(shady
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        )

target_link_libraries(shady
        PRIVATE
        Xform
        glfw
        Common
        )


# Command line tool. Still requires GLFW to establish OpenGL context
add_executable(shady_cli
        src/main_cli.cpp
        )


target_include_directories(shady_cli
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
        )

target_link_libraries(shady_cli
        PRIVATE
        Xform
        glfw
        Common
        Command
        )

