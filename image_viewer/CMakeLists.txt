include(FetchContent)

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3-stable
)
FetchContent_MakeAvailable(glfw)
add_definitions(-DGL_SILENCE_DEPRECATION)

# Img UI
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG
)
FetchContent_MakeAvailable(imgui)

# stb_image
FetchContent_Declare(
        stb_image
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb_image)


# ImGui sources
SET(IMGUI_SOURCES ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )

add_executable(image_viewer
        src/main.cpp
        include/vertex.h include/fragment.h
        src/image_io.cpp include/image_io.h
        src/glfw_support.cpp include/glfw_support.h
        ${IMGUI_SOURCES}
        )


target_include_directories(
        image_viewer
        PRIVATE
        include
        ${stb_image_SOURCE_DIR}
        ${glfw3_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
)

target_link_libraries(
        image_viewer
        PUBLIC
        Common
        glfw
)