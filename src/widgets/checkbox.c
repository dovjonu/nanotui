#include "../node_internal.h"
#include <nanotui/widgets/checkbox.h>
#include <nanotui/render.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

typedef struct {
    char* text;
    int checked;
    CheckboxCallback on_toggle;
    void* user_data;
} CheckboxData;

static void checkbox_measure(Node* self) {
    CheckboxData* d = self->impl;
    int len = (d && d->text) ? (int)strlen(d->text) : 0;

    self->height_hint.min = 1;
    self->height_hint.pref = 1;
    self->height_hint.max = 1;

    self->width_hint.min = len + 3;   // [ ]text
    self->width_hint.pref = len + 3;
    self->width_hint.max = -1;
}

static void checkbox_render(Node* self, RenderBuffer* rb) {
    CheckboxData* d = self->impl;
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

    char checkmark = ' ';
    if (d->checked)
        checkmark = 'X';


    // draw left bracket
    render_buffer_put_attr(rb, x, y, l, attr);
    render_buffer_put_attr(rb, x+1, y, checkmark, attr);
    render_buffer_put_attr(rb, x+2, y, r, attr);

    // draw text
    int tx = x + 3;
    int max = w - 3;
    for (int i = 0; i < max && t[i]; i++) {
        render_buffer_put_attr(rb, tx + i, y, t[i], attr);
    }
}

static int checkbox_on_key(Node* self, int key) {
    CheckboxData* d = self->impl;
    if (!d) return 0;

    if (key == '\n' || key == ' ') {
        d->checked = !d->checked;
        if (d->on_toggle) d->on_toggle(self, d->user_data);
        return 1;
    }
    return 0;
}

Node* checkbox_create(const char* text, CheckboxCallback on_toggle, void* user_data){
    Node* n = calloc(1, sizeof(Node));
    if (!n) return NULL;

    node_init(n);
    

    CheckboxData* d = calloc(1, sizeof(CheckboxData));
    if (!d) { 
        free(n); 
        return NULL; 
    }

    if (text) {
        d->text = strdup(text);
        if (!d->text) { free(d); free(n); return NULL; }
    }
    d->on_toggle = on_toggle;
    d->user_data = user_data;
    d->checked = 0;

    n->impl = d;
    n->render = checkbox_render;
    n->measure = checkbox_measure;
    n->on_key = checkbox_on_key;

    node_set_focusable(n, 1);

    return n;
}

void  checkbox_set_text(Node* checkbox, const char* text){
    if (!checkbox)
        return;
    
    CheckboxData* d = checkbox->impl;
    if (!d)
        return;

    free(d->text);
    d->text = strdup(text ? text : "");
}

char* checkbox_get_text(Node* checkbox) {
    if (!checkbox)
        return NULL;
    
    CheckboxData* d = checkbox->impl;
    if (!d)
        return NULL;

    return d->text;
}

int  checkbox_is_checked(Node* checkbox) {
    if (!checkbox)
        return 0;
    
    CheckboxData* d = checkbox->impl;
    if (!d)
        return 0;

    return d->checked;
}

void  checkbox_set_checked(Node* checkbox, int checked) {
    if (!checkbox)
        return;
    
    CheckboxData* d = checkbox->impl;
    if (!d)
        return;

    d->checked = checked ? 1 : 0;
}