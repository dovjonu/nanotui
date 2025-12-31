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
    Node* focused;
    int locked;
};

UI* ui_create(void) {
    UI* ui = malloc(sizeof(UI));
    if (!ui) return NULL;
    ui->running = 0;
    ui->root = NULL;
    ui->focused = NULL;
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
    start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);

    ui->running = 1;

    int w, h;
    getmaxyx(stdscr, h, w);

    RenderBuffer* rb = render_buffer_create(w, h);

    // pick initial focus
    ui->focused = node_focus_next(ui->root, NULL);
    if (ui->focused) node_set_focus(ui->focused, 1);

    while (ui->running) {
        getmaxyx(stdscr, h, w);

        render_buffer_clear(rb);
        node_set_rect(ui->root, 0, 0, w, h);
        node_render(ui->root, rb);
        backend_ncurses_flush(rb);

        int key = getch();

        // global keys
        if (!ui->locked && (key == 'q' || key == 27)) { // q or ESC
            ui->running = 0;
            break;
        }

        if (!ui->locked && key == KEY_RIGHT) { // Tab cycles focus
            if (ui->focused) node_set_focus(ui->focused, 0);
            ui->focused = node_focus_next(ui->root, ui->focused);
            if (ui->focused) node_set_focus(ui->focused, 1);
            continue;
        }

        if (!ui->locked && key == KEY_LEFT) {
            if (ui->focused) node_set_focus(ui->focused, 0);
            ui->focused = node_focus_prev(ui->root, ui->focused);
            if (ui->focused) node_set_focus(ui->focused, 1);
            continue;
        }

        // dispatch key to focused node
        if (ui->focused) {
            node_handle_key(ui->focused, key);
        }
    }

    render_buffer_destroy(rb);
    endwin();
}

void ui_set_root(UI* ui, Node* root) {
    if (!ui) return;
    ui->root = root;
    node_set_ui(root, ui);
}

Node* ui_get_root(const UI* ui) {
    if (!ui) return NULL;
    return ui->root;
}

void ui_lock(UI* ui) {
    if (!ui) return;
    ui->locked = 1;
}

void ui_unlock(UI* ui) {
    if (!ui) return;
    ui->locked = 0;
}

int ui_get_locked(const UI* ui) {
    if (!ui) return 0;
    return ui->locked;
}

void ui_destroy(UI* ui) {
    if (!ui) return;
    free(ui);
}
