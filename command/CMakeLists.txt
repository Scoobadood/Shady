add_library(Command
        SHARED
        src/command.cpp include/command-root.h
        src/command-delete.cpp include/command-delete.h
        src/command-connect.cpp include/command-connect.h
        src/command-add.cpp include/command-add.h
        src/command-list.cpp include/command-list.h
        src/command-configure.cpp include/command-configure.h
        src/command-disconnect.cpp include/command-disconnect.h
        src/command-load.cpp include/command-load.h
        src/command-run.cpp include/command-run.h
        src/command-quit.cpp include/command-quit.h
        src/command-save.cpp include/command-save.h
        src/command-show.cpp include/command-show.h
        )

target_include_directories(Command
        PUBLIC
        include
)

target_link_libraries(Command
        PUBLIC
        Xform
        )