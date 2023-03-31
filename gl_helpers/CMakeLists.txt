find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

# GLM
option(GLM_TEST_ENABLE "Disable" OFF)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS NAMES glm REQUIRED
)
FetchContent_MakeAvailable(glm)

add_library(GLHelpers
        SHARED
        src/shader.cc include/shader.h
        )

target_include_directories(GLHelpers
        PRIVATE
        ${spdlog_SOURCE_DIR}/include
        PUBLIC
        include
        )

target_link_libraries(GLHelpers
        PUBLIC
        ${OPENGL_LIBRARIES}
        glm
        )
