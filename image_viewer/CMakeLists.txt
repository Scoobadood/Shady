include(FetchContent)

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3-stable
)
FetchContent_MakeAvailable(glfw)
find_package(glfw3 3.2 REQUIRED)

# OpenGL
IF (APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIR})
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(GLUT_LIBRARY GLUT)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    MARK_AS_ADVANCED(COCOA_LIBRARY
            GLUT_LIBRARY
            OpenGL_LIBRARY)
    SET(EXTRA_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ENDIF (APPLE)

add_executable(image_viewer
        src/gl_utils.cpp include/gl_utils.h
        src/main.cpp
        )


target_include_directories(
        image_viewer
        PRIVATE
        include
        ${glfw3_SOURCE_DIR}/include
)

target_link_libraries(
        image_viewer
        PUBLIC
        Common
        glfw
        ${EXTRA_LIBS}
)