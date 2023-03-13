# stb_image
FetchContent_Declare(
        stb_image
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb_image)

add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(Xform SHARED
        src/xform-graph.cpp include/xform-graph.h
        src/xform.cpp include/xform.h
        src/xform-port.cpp include/xform-port.h
        src/load-file-xform.cpp include/load-file-xform.h
        src/split-channel-xform.cpp include/split-channel-xform.h
        src/save-file-xform.cpp include/save-file-xform.h
        src/xform-config.cpp include/xform-config.h

        src/image_io.cpp include/image_io.h
        )

target_include_directories(Xform
        PUBLIC
        include
        ${stb_image_SOURCE_DIR}
        )

target_link_libraries(Xform
        PUBLIC
        Common
        )

# Integration Test
add_executable(xform_int_test
        tests/graph-integration-test.cpp
        )

target_link_libraries(xform_int_test
        PRIVATE
        Xform
        )


# Unit tests
add_executable(xform_unit_tests
        tests/test_main.cpp
        tests/test_ports.cpp tests/test_ports.h
        tests/test_base_xform.cpp tests/test_base_xform.h
        tests/test_config.cpp
        )

target_link_libraries(xform_unit_tests
        PRIVATE
        Xform
        gtest
        gmock
        )
