# stb_image
FetchContent_Declare(
        stb_image
        GIT_REPOSITORY https://github.com/nothings/stb.git
)

# Nlohman JSON
FetchContent_Declare(
        nlohman_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(stb_image nlohman_json)

add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(Xform SHARED
        # Core graph
        src/xform-graph.cpp include/xform-graph.h
        src/xforms/xform-port.cpp include/xforms/xform-port.h
        src/xforms/xform-config.cpp include/xforms/xform-config.h
        src/xform-exceptions.cc include/xform-exceptions.h

        # Graph IO
        src/xform-io.cpp include/xform-io.h
        src/xform-factory.cpp include/xform-factory.h

        # Specific functions
        src/xforms/xform.cpp include/xforms/xform.h
        src/xforms/render-xform.cpp include/xforms/render-xform.h
        src/xforms/single-io-shader-xform.cpp include/xforms/single-io-shader-xform.h
        src/image_io.cpp include/image_io.h
        src/xforms/load-file-xform.cpp include/xforms/load-file-xform.h
        src/xforms/split-channel-xform.cpp include/xforms/split-channel-xform.h
        src/xforms/merge-channel-xform.cpp include/xforms/merge-channel-xform.h
        src/xforms/add-channel-xform.cpp include/xforms/add-channel-xform.h
        src/xforms/save-file-xform.cpp include/xforms/save-file-xform.h
        src/xforms/brightness-xform.cpp include/xforms/brightness-xform.h
        src/xforms/gaussian-blur-xform.cpp include/xforms/gaussian-blur-xform.h
        )

target_include_directories(Xform
        PUBLIC
        include
        ${stb_image_SOURCE_DIR}
        )

target_link_libraries(Xform
        PUBLIC
        Common
        PRIVATE
        nlohmann_json::nlohmann_json
        GLHelpers
        )
