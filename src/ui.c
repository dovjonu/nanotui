#include <stdlib.h>
#include <ncurses.h>

#include <nanotui/ui.h>
#include <nanotui/render.h>
#include <nanotui/node.h>


/* forward declaration */
void backend_ncurses_flush(RenderBuffer* rb);

struct UI {
    int running;
    Node* root;
};

UI* ui_create(void) {
    UI* ui = malloc(sizeof(UI));
    if (!ui) return NULL;
    ui->running = 0;
    ui->root = NULL;
    return ui;
}

void ui_run(UI* ui) {
    if (!ui) return;
    if (!ui->root) return;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int w, h;
    getmaxyx(stdscr, h, w);

    RenderBuffer* rb = render_buffer_create(w, h);
    render_buffer_clear(rb);

    node_set_rect(ui->root, 0, 0, w, h);

    node_render(ui->root, rb);

    backend_ncurses_flush(rb);

    getch(); /* wait for key */

    render_buffer_destroy(rb);
    endwin();
}

void ui_set_root(UI* ui, Node* root) {
    if (!ui) return;
    ui->root = root;
}

void ui_destroy(UI* ui) {
    if (!ui) return;
    free(ui);
}
