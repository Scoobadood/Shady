# OpenGL
find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(Common
        SHARED
        src/float_image.cpp  include/float_image.h
        src/gl_utils.cpp include/gl_utils.h
        src/string_utils.cpp include/string_utils.h
        src/file_utils.cpp include/file_utils.h
        )

target_include_directories(Common
        PUBLIC
        include
        )

target_link_libraries(Common
        PUBLIC
        ${OPENGL_LIBRARIES}
        )