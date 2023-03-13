add_library(Xform SHARED
        src/xform-graph.cpp include/xform-graph.h
        src/xform.cpp include/xform.h
        src/xform-port.cpp include/xform-port.h
        src/load-file-xform.cpp include/load-file-xform.h
        src/split-channel-xform.cpp include/split-channel-xform.h
        src/save-file-xform.cpp include/save-file-xform.h
        include/xform-config.h
        )

target_include_directories(Xform
        PUBLIC
        include
        )

# Integration Test
add_executable(graph_test
        tests/graph-integration-test.cpp
        )

target_link_libraries(graph_test
        PRIVATE
        Xform
        )

