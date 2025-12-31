#include <stdlib.h>
#include <string.h>

#include "../node_internal.h"
#include <nanotui/widgets/label.h>

static void label_render(Node* self, RenderBuffer* rb);
static void label_measure(Node* self);

typedef struct {
    char* text;
} LabelData;

static BorderConfig label_default_border(Node* self) {
    (void)self;
    return (BorderConfig){
        .mask = BORDER_NONE,
        .style = node_border_style_ascii(),
    };
}

void label_set_text(Node* label, const char* text) {
    if (!label)
        return;

    LabelData* d = label->impl;
    if (!d)
        return;

    free(d->text);
    d->text = strdup(text ? text : "");
}

Node* label_create(const char* text) {
    Node* n = calloc(1, sizeof(Node));
    if (!n)
        return NULL;

    node_init(n);

    LabelData* d = malloc(sizeof(LabelData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->text = strdup(text ? text : "");
    if (!d->text) {
        free(d);
        free(n);
        return NULL;
    }

    n->measure = label_measure;
    n->render = label_render;
    n->impl = d;
    n->default_border = label_default_border;

    return n;
}

static void label_measure(Node* self) {
    if (!self)
        return;

    LabelData* d = self->impl;
    if (!d || !d->text)
        return;

    int text_len = (int)strlen(d->text);

    BorderMask border_mask = node_border_get(self).mask;

    int pref_w = text_len;
    int pref_h = 1;

    if (border_mask & BORDER_LEFT)
        pref_w += 1;
    if (border_mask & BORDER_RIGHT)
        pref_w += 1;
    if (border_mask & BORDER_TOP)
        pref_h += 1;
    if (border_mask & BORDER_BOTTOM)
        pref_h += 1;

    self->width_hint.min = 0;
    self->width_hint.pref = pref_w;
    self->width_hint.max = -1;

    self->height_hint.min = 1;
    self->height_hint.pref = pref_h;
    self->height_hint.max = pref_h;
}

static void label_render(Node* self, RenderBuffer* rb) {
    LabelData* d = self->impl;

    BorderMask border_mask = node_border_get(self).mask;

    int inner_x = self->x + ((border_mask & BORDER_LEFT) ? 1 : 0);
    int inner_y = self->y + ((border_mask & BORDER_TOP) ? 1 : 0);
    int inner_w = self->width
        - ((border_mask & BORDER_LEFT) ? 1 : 0)
        - ((border_mask & BORDER_RIGHT) ? 1 : 0);
    int inner_h = self->height
        - ((border_mask & BORDER_TOP) ? 1 : 0)
        - ((border_mask & BORDER_BOTTOM) ? 1 : 0);

    if (inner_w <= 0 || inner_h <= 0)
        return;

    for (int i = 0; d->text[i] && i < inner_w; i++) {
        render_buffer_put(rb, inner_x + i, inner_y, d->text[i]);
    }
}
