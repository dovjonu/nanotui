#include "../node_internal.h"
#include <nanotui/widgets/line_edit.h>
#include <nanotui/render.h>
#include <nanotui/ui.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

typedef struct {
    int length; // the physical user setable width of the line edit
    int cursor_pos;
    char* text;
} LineEditData;

static void line_edit_measure(Node* self) {
    LineEditData* d = self->impl;
    int len = (d && d->length) ? d->length : 0;

    self->height_hint.min = 1;
    self->height_hint.pref = 1;
    self->height_hint.max = 1;

    self->width_hint.min = len;
    self->width_hint.pref = len;
    self->width_hint.max = -1;
}

static void line_edit_render(Node* self, RenderBuffer* rb) {
    LineEditData* d = self->impl;
    const char* t = (d && d->text) ? d->text : "";

    uint32_t attr = A_STANDOUT;

    if (node_has_focus(self)) {
        attr = COLOR_PAIR(1) | A_BOLD | A_STANDOUT;
    }

    int x = self->x;
    int y = self->y;
    int w = self->width;
    if (w <= 0 || self->height <= 0) return;

    // draw text
    int tx = x;
    int max = w;
    for (int i = 0; i < max && t[i]; i++) {
        render_buffer_put_attr(rb, tx + i, y, t[i], attr);
    }
    if (d->length > (int)strlen(t)) {
        for (int i = (int)strlen(t); i < d->length && (tx + i) < (x + w); i++) {
            render_buffer_put_attr(rb, tx + i, y, ' ', attr);
        }
    }
}

static int line_edit_on_key(Node* self, int key) {
    LineEditData* d = self->impl;
    if (!d) return 0;

    UI* ui = node_get_ui(self);

    // Enter to lock/start editing
    if (key == '\n' || key == KEY_ENTER) {
        if (ui) ui_lock(ui);
        return 1;
    }

    // ESC to unlock/stop editing
    if (key == 27) {
        if (ui) ui_unlock(ui);
        return 1;
    }

    // If UI is locked (we're editing), accept input
    if (ui && ui_get_locked(ui)) {
        if ((key == KEY_BACKSPACE || key == 127)) {
            int len = (int)strlen(d->text);
            if (len > 0 && d->cursor_pos > 0) {
                memmove(&d->text[d->cursor_pos - 1], &d->text[d->cursor_pos], len - d->cursor_pos + 1);
                d->cursor_pos--;
            }
            return 1;
        }

        if (key >= 32 && key <= 126) { // printable characters
            int len = (int)strlen(d->text);
            d->text = realloc(d->text, len + 2); // +1 for new char, +1 for null terminator
            memmove(&d->text[d->cursor_pos + 1], &d->text[d->cursor_pos], len - d->cursor_pos + 1);
            d->text[d->cursor_pos] = (char)key;
            d->cursor_pos++;
            return 1;
        }
    }
    
    return 0;
}

Node* line_edit_create(int length) {
    Node* n = calloc(1, sizeof(Node));
    if (!n) return NULL;

    node_init(n);

    LineEditData* d = calloc(1, sizeof(LineEditData));
    if (!d) { 
        free(n); 
        return NULL; 
    }

    d->length = length;
    d->cursor_pos = 0;
    d->text = strdup("");

    n->impl = d;
    n->render = line_edit_render; 
    n->measure = line_edit_measure;
    n->on_key = line_edit_on_key;

    node_set_focusable(n, 1);

    return n;
}

void  line_edit_set_text(Node* line_edit, const char* text) {
    if (!line_edit)
        return;
    
    LineEditData* d = line_edit->impl;
    if (!d)
        return;

    free(d->text);
    d->text = strdup(text ? text : "");
}

char* line_edit_get_text(Node* line_edit) {
    if (!line_edit)
        return NULL;
    
    LineEditData* d = line_edit->impl;
    if (!d)
        return NULL;

    return d->text;
}