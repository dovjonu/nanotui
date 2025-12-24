#include <stdlib.h>
#include <string.h>

#include "../node_internal.h"
#include <nanotui/widgets/label.h>

static void label_render(Node* self, RenderBuffer* rb);

typedef struct {
    char* text;
} LabelData;

Node* label_create(const char* text) {
    Node* n = calloc(1, sizeof(Node));

    LabelData* d = malloc(sizeof(LabelData));
    d->text = strdup(text);

    n->render = label_render;
    n->impl = d;

    return n;
}

static void label_render(Node* self, RenderBuffer* rb) {
    LabelData* d = self->impl;

    for (int i = 0; d->text[i]; i++) {
        render_buffer_put(rb, self->x + i, self->y, d->text[i]);
    }
}
