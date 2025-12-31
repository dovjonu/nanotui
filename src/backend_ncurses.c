#include <ncurses.h>

#include <nanotui/render.h>

void backend_ncurses_flush(RenderBuffer* rb) {
    if (!rb) return;

    for (int y = 0; y < rb->height; y++) {
        for (int x = 0; x < rb->width; x++) {
            Cell* c = &rb->cells[y * rb->width + x];
            mvaddch(y, x, c->ch | c->attr);
        }
    }
    refresh();
}
