#include "../node_internal.h"
#include <nanotui/layouts/vbox.h>
#include <stdlib.h>

/* VBox private data */
typedef struct {
    int spacing;
} VBoxData;

static BorderConfig vbox_default_border(Node* self) {
    (void)self;
    return (BorderConfig){
        .mask = BORDER_NONE,
        .style = node_border_style_ascii(),
        //.style = node_border_style_ascii_thick(),
        .title = (BorderTitle){ .text = NULL, .position = BORDER_TITLE_LEFT },
    };
}

static void vbox_measure(Node* self) {
    if (!self)
        return;

    VBoxData* d = self->impl;
    if (!d)
        return;

    int pref_w = 0;
    int pref_h = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        if (c->width_hint.pref > pref_w)
            pref_w = c->width_hint.pref;
        pref_h += c->height_hint.pref;
    }

    if (self->child_count > 1)
        pref_h += d->spacing * (self->child_count - 1);

    BorderMask border_mask = node_border_get(self).mask;
    if (border_mask & BORDER_LEFT)
        pref_w += 1;
    if (border_mask & BORDER_RIGHT)
        pref_w += 1;
    if (border_mask & BORDER_TOP)
        pref_h += 1;
    if (border_mask & BORDER_BOTTOM)
        pref_h += 1;

    if (pref_w < 1)
        pref_w = 1;
    if (pref_h < 1)
        pref_h = 1;

    self->width_hint.pref = pref_w;
    self->height_hint.pref = pref_h;
}

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

    BorderMask border_mask = node_border_get(self).mask;

    if (border_mask & BORDER_LEFT) {
        inner_x += 1;
        inner_w -= 1;
    }
    if (border_mask & BORDER_RIGHT) {
        inner_w -= 1;
    }
    if (border_mask & BORDER_TOP) {
        inner_y += 1;
        inner_h -= 1;
    }
    if (border_mask & BORDER_BOTTOM) {
        inner_h -= 1;
    }

    /* Guard against negative or zero space */
    if (inner_w <= 0 || inner_h <= 0)
        return;

    /* No children → nothing to layout */
    if (self->child_count == 0)
        return;


    /* Compute total preferred height (main axis) */
    int total_pref = 0;
    int total_flex = 0;
    int child_count = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;
        child_count++;
        total_pref += c->height_hint.pref;
        if (c->flex > 0)
            total_flex += c->flex;
    }

    if (child_count == 0)
        return;

    int total_spacing = d->spacing * (child_count - 1);
    int available_height = inner_h - total_spacing;
    if (available_height <= 0)
        return;

    int extra = available_height - total_pref;
    if (extra < 0)
        extra = 0;

    /* Assign geometry to children (sequential, pref-sized; flex consumes extra) */
    int y = inner_y;
    int remaining = inner_h;
    int remaining_extra = extra;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        /* spacing before child (except first visible child) */
        if (y != inner_y) {
            if (remaining < d->spacing)
                break;
            y += d->spacing;
            remaining -= d->spacing;
        }

        int flex_share = 0;
        if (c->flex > 0 && total_flex > 0 && remaining_extra > 0) {
            flex_share = (extra * c->flex) / total_flex;
            if (flex_share > remaining_extra)
                flex_share = remaining_extra;
            remaining_extra -= flex_share;
        }

        int desired_h = c->height_hint.pref + flex_share;
        if (desired_h < 0)
            desired_h = 0;
        if (desired_h > remaining)
            desired_h = remaining;

        node_set_rect(c, inner_x, y, inner_w, desired_h);

        y += desired_h;
        remaining -= desired_h;
        if (remaining <= 0)
            break;
    }
}

/* VBox constructor */
Node* vbox_create(int spacing) {
    Node* n = calloc(1, sizeof(Node));
    if (!n)
        return NULL;

    node_init(n);

    VBoxData* d = malloc(sizeof(VBoxData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->spacing = spacing;
    //d->layout_mode = layout_mode;

    n->layout = vbox_layout;
    n->measure = vbox_measure;
    n->render = NULL;   /* layout nodes do not draw */
    n->impl = d;

    /* Defaults */
    n->default_border = vbox_default_border;

    return n;
}
