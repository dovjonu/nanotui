#include <stdlib.h>
#include <ncurses.h>

#include <nanotui/ui.h>
#include <nanotui/render.h>

#include <nanotui/widgets/label.h>
#include <nanotui/layouts/vbox.h>
#include <nanotui/node.h>

#include "node_internal.h"


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

    Node* root = vbox_create(1);

    root->border.mask =
        BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT;

    root->border.style = (BorderStyle){
        .top = '-',
        .bottom = '-',
        .left = '|',
        .right = '|',
        .top_left = '+',
        .top_right = '+',
        .bottom_left = '+',
        .bottom_right = '+'
    };

    node_add_child(root, label_create("Hello"));
    node_add_child(root, label_create("World"));
    node_add_child(root, label_create("!"));

    Node* second = vbox_create(1);

    second->border.mask =
        BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT;

    second->border.style = (BorderStyle){
        .top = '-',
        .bottom = '-',
        .left = '|',
        .right = '|',
        .top_left = '+',
        .top_right = '+',
        .bottom_left = '+',
        .bottom_right = '+'
    };

    node_add_child(second, label_create("Nested"));
    node_add_child(second, label_create("VBox"));
    node_add_child(root, second);


    ui_set_root(ui, root);
    root->x = 0;
    root->y = 0;
    root->width = w;
    root->height = h;

    node_render(root, rb);

    backend_ncurses_flush(rb);

    getch(); /* wait for key */

    render_buffer_destroy(rb);
    endwin();
}

void ui_set_root(UI* ui, Node* root) {
    ui->root = root;
}

void ui_destroy(UI* ui) {
    if (!ui) return;
    free(ui);
}
