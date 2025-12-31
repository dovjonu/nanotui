cmake_minimum_required(VERSION 3.16)
project(nanotui C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)

include_directories(include)

add_library(nanotui
    src/ui.c
    src/node.c
    src/render.c
    src/backend_ncurses.c
    src/widgets/label.c
    src/widgets/button.c
    src/widgets/checkbox.c
    src/widgets/line_edit.c
    src/layouts/vbox.c
    src/layouts/hbox.c
)

target_link_libraries(nanotui ${CURSES_LIBRARIES})

add_executable(button_example examples/button_example.c)
target_link_libraries(button_example nanotui)
add_executable(test2 examples/test2.c)
target_link_libraries(test2 nanotui)