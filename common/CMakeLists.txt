# GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        0.9.9.8
        GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(glm)

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
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(Common
        SHARED
        src/float_image.cpp  include/float_image.h
        src/shader.cpp include/shader.h
        src/gl_utils.cpp include/gl_utils.h
        src/string_utils.cpp include/string_utils.h
        )

target_include_directories(Common
        PUBLIC
        include
        ${glm_SOURCE_DIR}
        )

target_link_libraries(Common
        PUBLIC
        ${EXTRA_LIBS}
        )