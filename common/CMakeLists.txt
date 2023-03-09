add_library(Common SHARED
        src/float_image.cpp  include/float_image.h
        )

target_include_directories(Common
        PUBLIC
        include
        )