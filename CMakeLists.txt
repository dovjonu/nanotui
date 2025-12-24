cmake_minimum_required(VERSION 3.16)
project(nanotui C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)

include_directories(include)

add_library(nanotui
    src/ui.c
    src/node.c
    src/layout.c
    src/render.c
    src/backend_ncurses.c
    src/widgets/label.c
    src/layouts/vbox.c
)

target_link_libraries(nanotui ${CURSES_LIBRARIES})

add_executable(hello examples/hello.c)
target_link_libraries(hello nanotui)