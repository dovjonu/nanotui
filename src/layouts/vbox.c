#include "../node_internal.h"
#include <nanotui/layouts/vbox.h>
#include <stdlib.h>

/* VBox private data */
typedef struct {
    int spacing;
} VBoxData;

/* VBox layout function */
static void vbox_layout(Node* self) {
    if (!self)
        return;

    VBoxData* d = self->impl;
    if (!d)
        return;

    /* Compute inner (content) box after borders */
    int inner_x = self->x;
    int inner_y = self->y;
    int inner_w = self->width;
    int inner_h = self->height;

    if (self->border.mask & BORDER_LEFT) {
        inner_x += 1;
        inner_w -= 1;
    }
    if (self->border.mask & BORDER_RIGHT) {
        inner_w -= 1;
    }
    if (self->border.mask & BORDER_TOP) {
        inner_y += 1;
        inner_h -= 1;
    }
    if (self->border.mask & BORDER_BOTTOM) {
        inner_h -= 1;
    }

    /* Guard against negative or zero space */
    if (inner_w <= 0 || inner_h <= 0)
        return;

    /* No children → nothing to layout */
    if (self->child_count == 0)
        return;

    /* Compute child heights */
    int total_spacing = d->spacing * (self->child_count - 1);
    int available_height = inner_h - total_spacing;

    if (available_height <= 0)
        return;

    int child_height = available_height / self->child_count;

    /* Assign geometry to children */
    int y = inner_y;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        c->x = inner_x;
        c->y = y;
        c->width = inner_w;
        c->height = child_height;

        y += child_height + d->spacing;
    }
}

/* VBox constructor */
Node* vbox_create(int spacing) {
    Node* n = calloc(1, sizeof(Node));
    if (!n)
        return NULL;

    VBoxData* d = malloc(sizeof(VBoxData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->spacing = spacing;

    n->layout = vbox_layout;
    n->render = NULL;   /* layout nodes do not draw */
    n->impl = d;

    /* Defaults */
    n->border.mask = BORDER_NONE;

    return n;
}
