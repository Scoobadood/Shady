include(FetchContent)

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3-stable
)
FetchContent_MakeAvailable(glfw)
add_definitions(-DGL_SILENCE_DEPRECATION)

# stb_image
FetchContent_Declare(
        stb_image
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb_image)

add_executable(image_viewer
        src/main.cpp
        include/vertex.h include/fragment.h
        src/image_io.cpp include/image_io.h
        )


target_include_directories(
        image_viewer
        PRIVATE
        include
        ${stb_image_SOURCE_DIR}
        ${glfw3_SOURCE_DIR}/include
)

target_link_libraries(
        image_viewer
        PUBLIC
        Common
        glfw
)