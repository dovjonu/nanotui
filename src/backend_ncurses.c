#define _XOPEN_SOURCE_EXTENDED 1

#include <locale.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>

#include <nanotui/render.h>

void backend_ncurses_flush(RenderBuffer* rb) {
    if (!rb) return;

    for (int y = 0; y < rb->height; y++) {
        for (int x = 0; x < rb->width; x++) {
            Cell* c = &rb->cells[y * rb->width + x];

            wchar_t wch = (wchar_t)c->ch;
            cchar_t cc;

            setcchar(&cc, &wch, c->attr, c->color_pair, NULL);
            mvadd_wch(y, x, &cc);
        }
    }
    refresh();
}
