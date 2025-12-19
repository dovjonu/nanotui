#include <stdlib.h>
#include <string.h>

#include "../widget_internal.h"
#include <nanotui/widget.h>

static void label_render(Widget* w, RenderBuffer* rb);

typedef struct {
    char* text;
} LabelData;

Widget* label_create(const char* text, int x, int y) {
    Widget* w = malloc(sizeof(Widget));
    w->render = label_render;
    w->x = x;
    w->y = y;

    LabelData* d = malloc(sizeof(LabelData));
    d->text = strdup(text);
    w->impl = d;

    return w;
}

static void label_render(Widget* w, RenderBuffer* rb) {
    LabelData* d = w->impl;
    for (int i = 0; d->text[i]; i++)
        render_buffer_put(rb, w->x + i, w->y, d->text[i]);
}

void label_destroy(Widget* w) {
    if (!w) return;
    LabelData* d = w->impl;
    free(d->text);
    free(d);
    free(w);
}