#include "../node_internal.h"
#include <nanotui/widgets/button.h>
#include <nanotui/render.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

typedef struct {
    char* text;
    ButtonCallback on_press;
    void* user_data;
} ButtonData;

static void button_measure(Node* self) {
    ButtonData* d = self->impl;
    int len = (d && d->text) ? (int)strlen(d->text) : 0;

    self->height_hint.min = 1;
    self->height_hint.pref = 1;
    self->height_hint.max = 1;

    self->width_hint.min = len + 2;   // [text]
    self->width_hint.pref = len + 2;
    self->width_hint.max = -1;
}

static void button_render(Node* self, RenderBuffer* rb) {
    ButtonData* d = self->impl;
    const char* t = (d && d->text) ? d->text : "";

    char l = '[';
    char r = ']';
    uint32_t attr = 0;

    if (node_has_focus(self)) {
        attr = COLOR_PAIR(1) | A_BOLD;
    }

    int x = self->x;
    int y = self->y;
    int w = self->width;
    if (w <= 0 || self->height <= 0) return;

    // draw left bracket
    render_buffer_put_attr(rb, x, y, l, attr);

    // draw text (clipped)
    int tx = x + 1;
    int max = w - 2;
    for (int i = 0; i < max && t[i]; i++) {
        render_buffer_put_attr(rb, tx + i, y, t[i], attr);
    }

    // draw right bracket if fits
    if (w >= 2) render_buffer_put_attr(rb, x + w - 1, y, r, attr);
}

static int button_on_key(Node* self, int key) {
    ButtonData* d = self->impl;
    if (!d) return 0;

    if (key == '\n' || key == ' ') {
        if (d->on_press) d->on_press(self, d->user_data);
        return 1;
    }
    return 0;
}

Node* button_create(const char* text, ButtonCallback on_press, void* user_data) {
    Node* n = calloc(1, sizeof(Node));
    if (!n) return NULL;

    node_init(n);
    

    ButtonData* d = calloc(1, sizeof(ButtonData));
    if (!d) { 
        free(n); 
        return NULL; 
    }

    if (text) {
        d->text = strdup(text);
        if (!d->text) { free(d); free(n); return NULL; }
    }
    d->on_press = on_press;
    d->user_data = user_data;

    n->impl = d;
    n->render = button_render;
    n->measure = button_measure;
    n->on_key = button_on_key;

    node_set_focusable(n, 1);

    return n;
}

void button_set_text(Node* button, const char* text) {
    if (!button) return;
    ButtonData* d = button->impl;
    if (!d) return;

    free(d->text);
    d->text = text ? strdup(text) : NULL;
}

char* button_get_text(Node* button) {
    if (!button) return NULL;
    ButtonData* d = button->impl;
    if (!d) return NULL;
    return d->text;
}