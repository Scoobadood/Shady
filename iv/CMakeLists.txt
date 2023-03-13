# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3-stable
)
FetchContent_MakeAvailable(glfw)

add_executable(iv
        src/xxmain.cpp
)

target_include_directories(iv
        PUBLIC
        ${glfw3_SOURCE_DIR}/include
        )

target_link_libraries(iv
        PUBLIC
        Xform
        glfw
        Common
        )