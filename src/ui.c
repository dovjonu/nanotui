#include <stdlib.h>
#include <ncurses.h>

#include <nanotui/ui.h>
#include <nanotui/render.h>

#include <nanotui/widget.h>
#include <nanotui/widgets/label.h>


/* forward declaration */
void backend_ncurses_flush(RenderBuffer* rb);

struct UI {
    int running;
};

UI* ui_create(void) {
    UI* ui = malloc(sizeof(UI));
    if (!ui) return NULL;
    ui->running = 0;
    return ui;
}

void ui_run(UI* ui) {
    if (!ui) return;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int w, h;
    getmaxyx(stdscr, h, w);

    RenderBuffer* rb = render_buffer_create(w, h);
    render_buffer_clear(rb);

    /* ---- DEMO DRAW: simple box ---- */
    for (int x = 0; x < w; x++) {
        render_buffer_put(rb, x, 0, '-');
        render_buffer_put(rb, x, h - 1, '-');
    }
    for (int y = 0; y < h; y++) {
        render_buffer_put(rb, 0, y, '|');
        render_buffer_put(rb, w - 1, y, '|');
    }
    render_buffer_put(rb, 0, 0, '+');
    render_buffer_put(rb, w - 1, 0, '+');
    render_buffer_put(rb, 0, h - 1, '+');
    render_buffer_put(rb, w - 1, h - 1, '+');

    /* test label */
    Widget* label = label_create("Hello from Label!", 10, 10);
    widget_render(label, rb);

    backend_ncurses_flush(rb);

    getch(); /* wait for key */

    label_destroy(label);
    render_buffer_destroy(rb);
    endwin();
}

void ui_destroy(UI* ui) {
    if (!ui) return;
    free(ui);
}
