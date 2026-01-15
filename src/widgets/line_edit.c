#include "../node_internal.h"
#include <nanotui/widgets/line_edit.h>
#include <nanotui/render.h>
#include <nanotui/ui.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

typedef struct {
    int length;              // the physical user setable width of the line edit
    int cursor_pos;         //  position of the cursor
    int scroll_offset;     //  offset for scrolling text if it exceeds length
    int editing;          //   1 if we are in the editing mode
    int cursor_blink;    //    1 if the cursor is currently visible (for blinking)
    long last_blink_ms; //     last time the cursor blink state changed
    char* text;        //      the text content
} LineEditData;

static void line_edit_measure(Node* self) {
    LineEditData* d = self->impl;
    int len = (d && d->length) ? d->length : 0;

    self->height_hint.min = 1;
    self->height_hint.pref = 1;
    self->height_hint.max = 1;

    self->width_hint.min = len;
    self->width_hint.pref = len;
    self->width_hint.max = len;
}

static void line_edit_render(Node* self, RenderBuffer* rb) {
    LineEditData* d = self->impl;
    const char* t = (d && d->text) ? d->text : "";
    long now = ui_now_ms();
    if (d->editing && now - d->last_blink_ms >= 500) {
        d->cursor_blink = !d->cursor_blink;
        d->last_blink_ms = now;
    }

    uint32_t attr = A_STANDOUT;

    if (node_has_focus(self)) {
        attr = COLOR_PAIR(1) | A_BOLD | A_STANDOUT;
    }

    if (!node_is_enabled(self)) {
        attr |= A_DIM;
    }

    int x = self->x;
    int y = self->y;
    int w = self->width;
    if (w <= 0 || self->height <= 0) return;

    int visible = w;

    for (int i = 0; i < visible; i++) {
        int ti = d->scroll_offset + i;

        wchar_t ch = L' ';
        if (ti < (int)strlen(t)) {
            ch = t[ti];
        }

        render_buffer_put_attr(rb, x + i, y, ch, attr);
    }


    if (d->editing && d->cursor_blink) {
        int cx = d->cursor_pos - d->scroll_offset;
        if (cx >= 0 && cx < w) {
            attr_t cattr = COLOR_PAIR(1) | A_BOLD | A_STANDOUT | A_UNDERLINE;

            wchar_t ch = L' ';
            if (d->cursor_pos < (int)strlen(t))
                ch = t[d->cursor_pos];

            render_buffer_put_attr(rb, x + cx, y, ch, cattr);
        }
    }

}

static void line_edit_ensure_cursor_visible(Node* self) {
    LineEditData* d = self->impl;
    if (!d) return;

    int w = self->width;
    if (w <= 0) w = 1;

    int len = (int)strlen(d->text);

    /* Clamp cursor */
    if (d->cursor_pos < 0) d->cursor_pos = 0;
    if (d->cursor_pos > len) d->cursor_pos = len;

    /* Reserve one column for cursor when editing */
    int text_width = w;
    if (d->editing && text_width > 1)
        text_width = w - 1;

    /* Keep cursor visible */
    if (d->cursor_pos < d->scroll_offset) {
        d->scroll_offset = d->cursor_pos;
    } else if (d->cursor_pos > d->scroll_offset + text_width) {
        d->scroll_offset = d->cursor_pos - text_width;
    }

    /* Clamp scroll_offset so text can shrink back */
    int max_scroll = len - text_width;
    if (max_scroll < 0) max_scroll = 0;

    if (d->scroll_offset > max_scroll)
        d->scroll_offset = max_scroll;
    if (d->scroll_offset < 0)
        d->scroll_offset = 0;
}

static int line_edit_on_key(Node* self, int key) {
    LineEditData* d = self->impl;
    if (!d) return 0;

    UI* ui = node_get_ui(self);

    // Enter to lock/start editing
    if (node_is_enabled(self) && (key == '\n' || key == KEY_ENTER)) {
        if (ui) ui_lock(ui);
        d->editing = 1;
        d->last_blink_ms = ui_now_ms();
        d->cursor_blink = 1;
        line_edit_ensure_cursor_visible(self);
        return 1;
    }

    // ESC to unlock/stop editing
    if (key == 27) {
        if (ui) ui_unlock(ui);
        d->editing = 0;
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
            line_edit_ensure_cursor_visible(self);
            return 1;
        }

        if (key == KEY_DC) { // Delete key
            int len = (int)strlen(d->text);
            if (d->cursor_pos < len) {
                memmove(&d->text[d->cursor_pos], &d->text[d->cursor_pos + 1], len - d->cursor_pos);
            }
            line_edit_ensure_cursor_visible(self);
            return 1;
        }

        if (key >= 32 && key <= 126) { // printable characters
            int len = (int)strlen(d->text);
            d->text = realloc(d->text, len + 2); // +1 for new char, +1 for null terminator
            memmove(&d->text[d->cursor_pos + 1], &d->text[d->cursor_pos], len - d->cursor_pos + 1);
            d->text[d->cursor_pos] = (char)key;
            d->cursor_pos++;
            line_edit_ensure_cursor_visible(self);
            return 1;
        }

        if (key == KEY_LEFT) {
            if (d->cursor_pos > 0) {
                d->cursor_pos--;
            }
            line_edit_ensure_cursor_visible(self);
            return 1;
        }

        if (key == KEY_RIGHT) {
            if (d->cursor_pos < (int)strlen(d->text)) {
                d->cursor_pos++;
            }
            line_edit_ensure_cursor_visible(self);
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
    d->scroll_offset = 0;

    n->impl = d;
    n->render = line_edit_render; 
    n->measure = line_edit_measure;
    n->on_key = line_edit_on_key;
    n->enabled = 1;
    n->focusable = 1;

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